/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_MakeFilter.cpp
 * Created: Friday, 23rd October 2020 7:00:48 pm
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#include "BPNode_MakeFilter.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Components/ActorComponent.h"
#include "EdGraphSchema_K2_Actions.h"
#include "EditorCategoryUtils.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_CallFunction.h"
#include "K2Node_DynamicCast.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_MacroInstance.h"
#include "K2Node_MakeArray.h"
#include "K2Node_PureAssignmentStatement.h"
#include "K2Node_TemporaryVariable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "ScopedTransaction.h"
#include "ToolMenusEditor.h"
#include "Misc/DefaultValueHelper.h"

#include "ApparatusRuntime.h"
#include "ApparatusFunctionLibrary.h"
#include "ApparatusUncookedUtils.h"
#include "Machine.h"
#include "Detail.h"


#define LOCTEXT_NAMESPACE "UBPNode_MakeFilter"

const FString
UBPNode_MakeFilter::ExcludedTraitTypePinNamePrefix(TEXT("ExcludedTraitType_"));

const FString
UBPNode_MakeFilter::ExcludedDetailClassPinNamePrefix(TEXT("ExcludedDetailType_"));

const FName
UBPNode_MakeFilter::ExcludingFlagmarkPinName(TEXT("ExcludingFlagmark"));

const FName
UBPNode_MakeFilter::FilterPinName(TEXT("Filter"));

FText UBPNode_MakeFilter::GetNodeTitle(
	ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Title", "Make Filter");
}

FText UBPNode_MakeFilter::GetTooltipText() const
{
	return LOCTEXT("TooltipText",
					 "Make a filter from a list of detail types.");
}

bool UBPNode_MakeFilter::IsNodePure() const { return true; }

bool UBPNode_MakeFilter::SyncPinNames()
{
	bool bChanged = Super::SyncPinNames();

	int32 ExcludedTypeIndex = 0;
	for (UEdGraphPin* Pin : Pins)
	{
		if (IsExcludedTraitTypePin(Pin))
		{
			const FName NewName = MakeExcludedTraitTypePinName(ExcludedTypeIndex++);
			if (Pin->PinName != NewName)
			{
				Pin->PinName = NewName;
				bChanged = true;
			}
		}
	}

	int32 ExcludedClassIndex = 0;
	for (UEdGraphPin* Pin : Pins)
	{
		if (IsExcludedDetailClassPin(Pin))
		{
			const FName NewName = MakeExcludedDetailClassPinName(ExcludedClassIndex++);
			if (Pin->PinName != NewName)
			{
				Pin->PinName = NewName;
				bChanged = true;
			}
		}
	}

	if (bChanged)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(
			GetBlueprint());
	}

	return bChanged;
}

void UBPNode_MakeFilter::UpdateExcludingFlagmarkPinUI(UEdGraphPin* ExcludingFlagmarkPin)
{
	auto Schema = GetSchema();
	check(Schema);

	int32 DefaultValue = 0;
	FDefaultValueHelper::ParseInt(ExcludingFlagmarkPin->DefaultValue, DefaultValue);
	if ((ExcludingFlagmarkPin->LinkedTo.Num() == 0) && (DefaultValue != FM_None))
	{
		FString Tooltip = "Must not include any of: ";
		Tooltip += ToString((EFlagmark)DefaultValue);
		Schema->ConstructBasicPinTooltip(
			*ExcludingFlagmarkPin,
			FText::FromString(Tooltip),
			ExcludingFlagmarkPin->PinToolTip);
		ExcludingFlagmarkPin->PinFriendlyName = FText::FromString(TEXT(NEGATIVE_FLAGMARK_SHORTCUT_STR));
	}
	else
	{
		Schema->ConstructBasicPinTooltip(
			*ExcludingFlagmarkPin,
			LOCTEXT("ExcludingFlagmarkPinTooltip", "The excluding flagmark specification of the subject."),
			ExcludingFlagmarkPin->PinToolTip);
		ExcludingFlagmarkPin->PinFriendlyName = FText::GetEmpty();
	}
}

void UBPNode_MakeFilter::UpdateExcludedTraitTypePinUI(UEdGraphPin *ExcludedTraitTypePin)
{
	checkSlow(IsExcludedTraitTypePin(ExcludedTraitTypePin));
	auto Schema = GetSchema();
	check(Schema);
	Schema->ConstructBasicPinTooltip(
		*ExcludedTraitTypePin,
		LOCTEXT("ExcludedTraitPinTooltip", "The type of traits to exclude."),
		ExcludedTraitTypePin->PinToolTip);

	auto Type = GetExcludedTraitTypePinType(ExcludedTraitTypePin);

	if (Type)
	{
		ExcludedTraitTypePin->PinFriendlyName = FText::FromString(TEXT(NEGATIVE_TRAIT_SHORTCUT_STR));
	}
	else
	{
		ExcludedTraitTypePin->PinFriendlyName = FText::GetEmpty();
	}
}

void UBPNode_MakeFilter::UpdateExcludedDetailClassPinUI(UEdGraphPin *ExcludedDetailClassPin)
{
	checkSlow(IsExcludedDetailClassPin(ExcludedDetailClassPin));
	auto Schema = GetSchema();
	check(Schema);
	Schema->ConstructBasicPinTooltip(
		*ExcludedDetailClassPin,
		LOCTEXT("ExcludedDetailPinTooltip", "The class of details to exclude."),
		ExcludedDetailClassPin->PinToolTip);

	auto Class = GetExcludedDetailClassPinClass(ExcludedDetailClassPin);

	if (Class && Class != UDetail::StaticClass())
	{
		ExcludedDetailClassPin->PinFriendlyName = FText::FromString(TEXT(NEGATIVE_DETAIL_SHORTCUT_STR));
	}
	else
	{
		ExcludedDetailClassPin->PinFriendlyName = FText::GetEmpty();
	}
}

void UBPNode_MakeFilter::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	GetFingerprintPin()->SafeSetHidden(true);

	//+BACKWARD-COMPATIBILITY
	if (NumNotTypes != 0)
	{
		ExcludedDetailsCount = NumNotTypes;
		NumNotTypes = 0;
	}
	if (ExcludedTypesCount != 0)
	{
		ExcludedDetailsCount = ExcludedTypesCount;
		ExcludedTypesCount = 0;
	}
	//-BACKWARD-COMPATIBILITY

	const UEdGraphSchema* Schema = GetSchema();
	check(Schema);

	UEdGraphPin *FilterPin =
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct,
				  FFilter::StaticStruct(), FilterPinName);

	Schema->ConstructBasicPinTooltip(
		*FilterPin,
		LOCTEXT("FilterPinTooltip",
				"The resulting filter object."),
		FilterPin->PinToolTip);

	if (HasFlagmark())
	{
		auto* const FlagmarkPin = GetFlagmarkPin();
		FlagmarkPin->DefaultValue = FString::FromInt(FFilter::DefaultFlagmark); 

		Schema->ConstructBasicPinTooltip(
			*FlagmarkPin,
			LOCTEXT("FlagmarkPinTooltip",
					"The positive flag-based filtering of subjects. "
					"All of the flags must be present."),
			FlagmarkPin->PinToolTip);
	}

	if (HasExcludingFlagmark())
	{
		auto* const ExcludingFlagmarkPin =
			CreatePin(EGPD_Input,
					  UEdGraphSchema_K2::PC_Int,
					  UEdGraphSchema_K2::PSC_Bitmask,
					  GetFlagmarkClass(),
					  ExcludingFlagmarkPinName);
		ExcludingFlagmarkPin->DefaultValue = FString::FromInt(FFilter::DefaultExcludingFlagmark); 
		UpdateExcludingFlagmarkPinUI(ExcludingFlagmarkPin);
	}

	for (int32 i = 0; i < ExcludedTraitsCount; ++i)
	{
		const FName Name(MakeExcludedTraitTypePinName(i));

		auto ExcludedTraitTypePin =
			CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object,
					  /*SubCategory=*/UScriptStruct::StaticClass(), Name);

		UpdateExcludedTraitTypePinUI(ExcludedTraitTypePin);
	}
	for (int32 i = 0; i < ExcludedDetailsCount; ++i)
	{
		const FName Name(MakeExcludedDetailClassPinName(i));

		auto ExcludedDetailClassPin =
			CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Class,
					  /*SubCategory=*/UDetail::StaticClass(), Name);

		UpdateExcludedDetailClassPinUI(ExcludedDetailClassPin);
	}
}

void UBPNode_MakeFilter::PostReconstructNode()
{
	Super::PostReconstructNode();

	if (HasExcludingFlagmark())
	{
		UpdateExcludingFlagmarkPinUI(GetExcludingFlagmarkPin());
	}
	for (int32 i = 0; i < ExcludedTraitsCount; ++i)
	{
		UEdGraphPin* ExcludedTraitTypePin = GetExcludedTraitTypePinGivenIndex(i);
		UpdateExcludedTraitTypePinUI(ExcludedTraitTypePin);
	}
	for (int32 i = 0; i < ExcludedDetailsCount; ++i)
	{
		UEdGraphPin* ExcludedDetailClassPin = GetExcludedDetailClassPinGivenIndex(i);
		UpdateExcludedDetailClassPinUI(ExcludedDetailClassPin);
	}
}

FLinearColor UBPNode_MakeFilter::GetNodeTitleColor() const
{
	return Super::GetNodeTitleColor();
}

FText UBPNode_MakeFilter::GetMenuCategory() const
{
	return FText::FromString("Apparatus|Filter");
}

void UBPNode_MakeFilter::EarlyValidation(
	class FCompilerResultsLog& MessageLog) const
{
	Super::EarlyValidation(MessageLog);

	for (int32 i = 0; i < ExcludedTraitsCount; ++i)
	{
		UEdGraphPin* ExcludedTraitPin = GetExcludedTraitTypePinGivenIndex(i);
		UScriptStruct* ExcludedTraitPinType = GetExcludedTraitTypePinType(ExcludedTraitPin);
		if (ExcludedTraitPinType == nullptr)
		{
			MessageLog.Error(
				*FText::Format(
					 LOCTEXT("ExcludedTraitPinUnspecified",
							   "The detail negation pin #{0} @@ is added "
							   "but not specified in @@"),
					 i)
					 .ToString(),
				ExcludedTraitPin, this);
			continue;
		}
		// Check for conflict with trait inclusions...
		for (int32 j = 0; j < TraitsCount; ++j)
		{
			UEdGraphPin* TraitPin = GetTraitTypePinGivenIndex(j);
			UScriptStruct* TraitPinType = GetTraitTypePinType(TraitPin);
			if (ExcludedTraitPinType == TraitPinType)
			{
				MessageLog.Error(
					*LOCTEXT("ConflictingExcludedTraitPin",
							   "The detail negation (!) @@ conflicts "
							   "with the detail specification @@ in @@")
						 .ToString(),
					ExcludedTraitPin, TraitPin, this);
			}
		}
	}

	for (int32 i = 0; i < ExcludedDetailsCount; ++i)
	{
		UEdGraphPin* ExcludedDetailPin = GetExcludedDetailClassPinGivenIndex(i);
		UClass* ExcludedClassPinType = GetExcludedDetailClassPinClass(ExcludedDetailPin);
		if (ExcludedClassPinType == nullptr ||
			ExcludedClassPinType == UObject::StaticClass() ||
			ExcludedClassPinType == UDetail::StaticClass())
		{
			MessageLog.Error(
				*FText::Format(
					 LOCTEXT("ExcludedDetailPinUnspecified",
							   "The detail negation pin #{0} @@ is added "
							   "but not specified in @@"),
					 i)
					 .ToString(),
				ExcludedDetailPin, this);
			continue;
		}
		// Check for conflicts with detail inclusions...
		for (int32 j = 0; j < DetailsCount; ++j)
		{
			UEdGraphPin* DetailPin = GetDetailClassPinGivenIndex(j);
			UClass* DetailPinClass = GetDetailClassPinClass(DetailPin);
			if ((ExcludedClassPinType == DetailPinClass) ||
				DetailPinClass->IsChildOf(ExcludedClassPinType))
			{
				MessageLog.Error(
					*LOCTEXT("ConflictingExcludedDetailPin",
							   "The detail negation (!) @@ conflicts "
							   "with the detail specification @@ in @@")
						 .ToString(),
					ExcludedDetailPin, DetailPin, this);
			}
		}
	}
}

void UBPNode_MakeFilter::ExpandToArrayNodes(
	class FKismetCompilerContext& CompilerContext,
	UEdGraph* SourceGraph,
	UEdGraphPin*& TraitsArrayPin,
	UEdGraphPin*& DetailsArrayPin,
	UEdGraphPin*& ExcludedTraitsArrayPin,
	UEdGraphPin*& ExcludedDetailsArrayPin)
{
	ExpandToArrayNodes(CompilerContext, SourceGraph,
					   TraitsArrayPin, DetailsArrayPin);

	const auto Schema = CompilerContext.GetSchema();
	check(Schema);

	// Make array from excluded input traits...
	auto* MakeExcludedTraitsArrayNode =
		CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>(this,
																 SourceGraph);
	// Make array from excluded input details...
	auto* MakeExcludedDetailsArrayNode =
		CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>(this,
																 SourceGraph);

	MakeExcludedTraitsArrayNode->AllocateDefaultPins();
	MakeExcludedDetailsArrayNode->AllocateDefaultPins();

	// Placeholder variable for traits initialization:
	auto* TypePlaceholder = CompilerContext.SpawnInternalVariable(
		this, UEdGraphSchema_K2::PC_Object, NAME_None, UScriptStruct::StaticClass());

	// Placeholder variable for details initialization:
	auto* ClassPlaceholder = CompilerContext.SpawnInternalVariable(
		this, UEdGraphSchema_K2::PC_Class, NAME_None, UDetail::StaticClass());

	//-Allocate array pins.
	while (MakeExcludedTraitsArrayNode->Pins.Num() < ExcludedTraitsCount + 1)
	{
		MakeExcludedTraitsArrayNode->AddInputPin();
	}
	while (MakeExcludedDetailsArrayNode->Pins.Num() < ExcludedDetailsCount + 1)
	{
		MakeExcludedDetailsArrayNode->AddInputPin();
	}
	//+Allocate array pins.

	//+Define array types.
	for (int32 i = 0; i < ExcludedTraitsCount; ++i)
	{
		auto DstPin = MakeExcludedTraitsArrayNode->GetPinAt(i + 1);
		MAKE_LINK(TypePlaceholder->GetVariablePin(), DstPin);
	}
	MakeExcludedTraitsArrayNode->PostReconstructNode();
	for (int32 i = 0; i < ExcludedDetailsCount; ++i)
	{
		auto DstPin = MakeExcludedDetailsArrayNode->GetPinAt(i + 1);
		MAKE_LINK(ClassPlaceholder->GetVariablePin(), DstPin);
	}

	MakeExcludedTraitsArrayNode->PostReconstructNode();
	MakeExcludedDetailsArrayNode->PostReconstructNode();
	//-Define array types.

	//+Link array classes.
	for (int32 i = 0; i < ExcludedTraitsCount; ++i)
	{
		auto* ExcludedTraitTypePin = GetExcludedTraitTypePinGivenIndex(i);
		auto* DstPin = MakeExcludedTraitsArrayNode->GetPinAt(i + 1);

		Schema->BreakSinglePinLink(TypePlaceholder->GetVariablePin(), DstPin);

		if (ExcludedTraitTypePin->LinkedTo.Num() == 0)
		{
			if (ExcludedTraitTypePin->DefaultObject != nullptr)
			{
				DstPin->DefaultObject = ExcludedTraitTypePin->DefaultObject;
			}
		}
		else
		{
			verify(!CompilerContext.MovePinLinksToIntermediate(*ExcludedTraitTypePin, *DstPin)
					.IsFatal());
		}
	}
	for (int32 i = 0; i < ExcludedDetailsCount; ++i)
	{
		auto* ExcludedDetailClassPin = GetExcludedDetailClassPinGivenIndex(i);
		auto* DstPin = MakeExcludedDetailsArrayNode->GetPinAt(i + 1);

		Schema->BreakSinglePinLink(ClassPlaceholder->GetVariablePin(), DstPin);

		if (ExcludedDetailClassPin->LinkedTo.Num() == 0)
		{
			if (ExcludedDetailClassPin->DefaultObject != nullptr)
			{
				DstPin->DefaultObject = ExcludedDetailClassPin->DefaultObject;
			}
		}
		else
		{
			verify(!CompilerContext.MovePinLinksToIntermediate(*ExcludedDetailClassPin, *DstPin)
					.IsFatal());
		}
	}
	//-Link type arrays.

	ExcludedTraitsArrayPin = MakeExcludedTraitsArrayNode->GetOutputPin();
	ExcludedDetailsArrayPin = MakeExcludedDetailsArrayNode->GetOutputPin();
}

bool
UBPNode_MakeFilter::CanBeCached() const
{
	if (TraitsNum()          <= 8 &&
		DetailsNum()         <= 8 &&
		ExcludedTraitsNum()  <= 8 &&
		ExcludedDetailsNum() <= 8)
	{
		for (int32 i = 0; i < TraitsNum(); ++i)
		{
			auto* TraitPin = GetTraitTypePinGivenIndex(i);
			if (!TraitPin) return false;
			if (TraitPin->HasAnyConnections()) return false;
		}
		for (int32 i = 0; i < DetailsNum(); ++i)
		{
			auto* DetailPin = GetDetailClassPinGivenIndex(i);
			if (!DetailPin) return false;
			if (DetailPin->HasAnyConnections()) return false;
		}
		for (int32 i = 0; i < ExcludedTraitsNum(); ++i)
		{
			auto* ExcludedTraitPin = GetExcludedTraitTypePinGivenIndex(i);
			if (!ExcludedTraitPin) return false;
			if (ExcludedTraitPin->HasAnyConnections()) return false;
		}
		for (int32 i = 0; i < ExcludedDetailsNum(); ++i)
		{
			auto* ExcludedDetailPin = GetExcludedDetailClassPinGivenIndex(i);
			if (!ExcludedDetailPin) return false;
			if (ExcludedDetailPin->HasAnyConnections()) return false;
		}
	}
	return false;
}

UEdGraphPin*
UBPNode_MakeFilter::GetExcludingFlagmarkPin() const
{
	if (!HasExcludingFlagmark()) return nullptr;
	return FindPinChecked(ExcludingFlagmarkPinName, EGPD_Input);
}

UK2Node_CallFunction*
UBPNode_MakeFilter::ExpandToMakeFilterNode(
	class FKismetCompilerContext& CompilerContext,
	UEdGraph* SourceGraph)
{
	const auto* Schema = CompilerContext.GetSchema();
	check(Schema);

	static const FName MakeFilterName =
		GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, MakeFilter);

	static const FName MakeCachedFilter8Name =
		GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, MakeCachedFilter8);

	UK2Node_CallFunction* MakeFilterNode =
			CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(
				this, SourceGraph);
	FName MakeFunctionName;
	const bool IsCached = CanBeCached();
	if (IsCached)
	{
		const int32 Max = FMath::Max(TArray<int32>({TraitsNum(), DetailsNum(), ExcludedTraitsNum(), ExcludedDetailsNum()}));
		const int32 Count = FMath::CeilLogTwo(Max);
		MakeFunctionName = FName(FString(TEXT("MakeCachedFilter")) + FString::FromInt(Max));
	}
	else
	{
		MakeFunctionName = MakeFilterName;
	}

	MakeFilterNode->FunctionReference.SetExternalMember(
			MakeFunctionName, UApparatusFunctionLibrary::StaticClass());
	MakeFilterNode->AllocateDefaultPins();

	if (IsCached)
	{
		auto* KeyPin = MakeFilterNode->FindPinChecked(TEXT("Key"), EGPD_Input);
		KeyPin->DefaultValue = this->GetPathName();
		for (int32 i = 0; i < TraitsNum(); ++i)
		{
			auto* TraitPin = GetTraitTypePinGivenIndex(i);
			if (!TraitPin) continue;
			const auto DstName = FString(TEXT("Trait_")) + FString::FromInt(i);
			auto* DstPin = MakeFilterNode->FindPinChecked(DstName, EGPD_Input);
			Expand_CopyOrMoveLink(CompilerContext, SourceGraph,
								  TraitPin, DstPin);
		}
		for (int32 i = 0; i < DetailsNum(); ++i)
		{
			auto* DetailPin = GetDetailClassPinGivenIndex(i);
			if (!DetailPin) continue;
			const auto DstName = FString(TEXT("Detail_")) + FString::FromInt(i);
			auto* DstPin = MakeFilterNode->FindPinChecked(DstName, EGPD_Input);
			Expand_CopyOrMoveLink(CompilerContext, SourceGraph,
								  DetailPin, DstPin);
		}
		for (int32 i = 0; i < ExcludedTraitsNum(); ++i)
		{
			auto* ExcludedTraitPin = GetExcludedTraitTypePinGivenIndex(i);
			if (!ExcludedTraitPin) continue;
			const auto DstName = FString(TEXT("ExcludedTrait_")) + FString::FromInt(i);
			auto* DstPin = MakeFilterNode->FindPinChecked(DstName, EGPD_Input);
			Expand_CopyOrMoveLink(CompilerContext, SourceGraph,
								  ExcludedTraitPin, DstPin);
		}
		for (int32 i = 0; i < ExcludedDetailsNum(); ++i)
		{
			auto* ExcludedDetailPin = GetExcludedDetailClassPinGivenIndex(i);
			if (!ExcludedDetailPin) continue;
			const auto DstName = FString(TEXT("ExcludedDetail_")) + FString::FromInt(i);
			auto* DstPin = MakeFilterNode->FindPinChecked(DstName, EGPD_Input);
			Expand_CopyOrMoveLink(CompilerContext, SourceGraph,
								  ExcludedDetailPin, DstPin);
		}
	}
	else
	{
		UEdGraphPin* TraitTypesPin            = nullptr;
		UEdGraphPin* DetailClassesPin         = nullptr;
		UEdGraphPin* ExcludedTraitTypesPin    = nullptr;
		UEdGraphPin* ExcludedDetailClassesPin = nullptr;
		ExpandToArrayNodes(CompilerContext, SourceGraph,
						   TraitTypesPin, DetailClassesPin, 
						   ExcludedTraitTypesPin, ExcludedDetailClassesPin);

		MAKE_LINK(
				TraitTypesPin,
				MakeFilterNode->FindPinChecked(TEXT("Traits"), EGPD_Input));
		MAKE_LINK(
				DetailClassesPin,
				MakeFilterNode->FindPinChecked(TEXT("Details"), EGPD_Input));
		MAKE_LINK(
				ExcludedTraitTypesPin,
				MakeFilterNode->FindPinChecked(TEXT("ExcludedTraits"), EGPD_Input));
		MAKE_LINK(
				ExcludedDetailClassesPin,
				MakeFilterNode->FindPinChecked(TEXT("ExcludedDetails"), EGPD_Input));
	}

	if (HasFlagmark())
	{
		auto* const TargetFlagmarkPin =
			MakeFilterNode->FindPinChecked(TEXT("Flagmark"));
		auto* const FlagmarkPin = GetFlagmarkPin();
		Expand_CopyOrMoveLink(CompilerContext, SourceGraph,
							FlagmarkPin, TargetFlagmarkPin);
	}

	if (HasExcludingFlagmark())
	{
		auto* const TargetExcludingFlagmarkPin =
			MakeFilterNode->FindPinChecked(TEXT("ExcludingFlagmark"));
		auto* const ExcludingFlagmarkPin = GetExcludingFlagmarkPin();
		Expand_CopyOrMoveLink(CompilerContext, SourceGraph,
							ExcludingFlagmarkPin, TargetExcludingFlagmarkPin);
	}

	return MakeFilterNode;
}

UEdGraphPin*
UBPNode_MakeFilter::ExpandToFilterPin(
	class FKismetCompilerContext& CompilerContext,
	UEdGraph* SourceGraph)
{
	
	auto* MakeFilterNode = ExpandToMakeFilterNode(CompilerContext, SourceGraph);
	UEdGraphPin* ResultFilterPin = MakeFilterNode->GetReturnValuePin();
	check(ResultFilterPin);
	return ResultFilterPin;
}

void UBPNode_MakeFilter::ExpandNode(
	class FKismetCompilerContext& CompilerContext,
	UEdGraph* SourceGraph)
{
	const auto* Schema = CompilerContext.GetSchema();
	check(Schema);

	UEdGraphPin* FilterPin = GetFilterPin();
	check(FilterPin);
	UEdGraphPin* ResultFilterPin = ExpandToFilterPin(CompilerContext, SourceGraph);
	check(ResultFilterPin);

	CompilerContext.MovePinLinksToIntermediate(*FilterPin,
											   *ResultFilterPin);

	BreakAllNodeLinks();
}

FName UBPNode_MakeFilter::MakeExcludedTraitTypePinName(const int32 Index) const
{
	return *(ExcludedTraitTypePinNamePrefix + FString::FormatAsNumber(Index));
}

FName UBPNode_MakeFilter::MakeExcludedDetailClassPinName(const int32 Index) const
{
	return *(ExcludedDetailClassPinNamePrefix + FString::FormatAsNumber(Index));
}

void UBPNode_MakeFilter::AddExcludingFlagmarkPin()
{
	if (!CanAddPin())
		return;
	if (HasExcludingFlagmark())
		return;

	Modify();

	auto Schema = GetDefault<UEdGraphSchema_K2>();
	check(Schema);

	auto* const ExcludingFlagmarkPin =
		CreatePin(EGPD_Input,
				UEdGraphSchema_K2::PC_Int,
				UEdGraphSchema_K2::PSC_Bitmask,
				GetFlagmarkClass(),
				ExcludingFlagmarkPinName);
	ExcludingFlagmarkPin->DefaultValue = FString::FromInt(FFilter::DefaultExcludingFlagmark); 

	Schema->ConstructBasicPinTooltip(
		*ExcludingFlagmarkPin,
		LOCTEXT("ExcludingFlagmarkPinTooltip",
				"The negative flag-based filtering of subjects."),
		ExcludingFlagmarkPin->PinToolTip);

	bExcludingFlagmarkActive = true;

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

void UBPNode_MakeFilter::AddExcludedTraitTypePin()
{
	if (!CanAddPin())
		return;

	Modify();

	auto Schema = GetDefault<UEdGraphSchema_K2>();
	check(Schema);

	auto ExcludedTraitTypePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object,
										  /*SubCategory=*/UScriptStruct::StaticClass(),
										  MakeExcludedTraitTypePinName(ExcludedTraitsCount));
	Schema->SetPinAutogeneratedDefaultValueBasedOnType(ExcludedTraitTypePin);

	UpdateExcludedTraitTypePinUI(ExcludedTraitTypePin);

	ExcludedTraitsCount += 1;

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

void UBPNode_MakeFilter::AddExcludedDetailClassPin()
{
	if (!CanAddPin())
		return;

	Modify();

	auto Schema = GetDefault<UEdGraphSchema_K2>();
	check(Schema);

	auto ExcludedDetailClassPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Class,
								/*SubCategory=*/UDetail::StaticClass(),
								MakeExcludedDetailClassPinName(ExcludedDetailsCount));
	Schema->SetPinAutogeneratedDefaultValueBasedOnType(ExcludedDetailClassPin);

	UpdateExcludedDetailClassPinUI(ExcludedDetailClassPin);

	ExcludedDetailsCount += 1;

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

void UBPNode_MakeFilter::InteractiveAddExcludingFlagmarkPin()
{
	FScopedTransaction Transaction(NSLOCTEXT(LOCTEXT_NAMESPACE,
											 "AddExcludingFlagmarkPinTx",
											 "Add Excluding Flagmark"));
	AddExcludingFlagmarkPin();
}

void UBPNode_MakeFilter::InteractiveAddExcludedTraitTypePin()
{
	FScopedTransaction Transaction(NSLOCTEXT(LOCTEXT_NAMESPACE,
											 "AddExcludedTraitPinTx",
											 "Add Excluded Trait"));
	AddExcludedTraitTypePin();
}

void UBPNode_MakeFilter::InteractiveAddExcludedDetailClassPin()
{
	FScopedTransaction Transaction(NSLOCTEXT(LOCTEXT_NAMESPACE,
											 "AddExcludedDetailPinTx",
											 "Add Excluded Detail"));
	AddExcludedDetailClassPin();
}

void UBPNode_MakeFilter::RemoveExcludingFlagmarkPin()
{
	if (!HasExcludingFlagmark())
		return;

	Modify();

	auto Schema = GetSchema();
	check(Schema);

	const auto ExcludingFlagmarkPin = GetExcludingFlagmarkPin();
	ExcludingFlagmarkPin->BreakAllPinLinks();

	RemovePin(ExcludingFlagmarkPin);

	bExcludingFlagmarkActive = false;

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(
		GetBlueprint());
}

void UBPNode_MakeFilter::RemoveExcludedTraitTypePin(UEdGraphPin *ExcludedTraitPin)
{
	if (!ExcludedTraitsCount)
		return;
	if (!ExcludedTraitPin)
		return;

	Modify();

	auto Schema = GetSchema();
	check(Schema);

	ExcludedTraitPin->BreakAllPinLinks();

	RemovePin(ExcludedTraitPin);

	ExcludedTraitsCount -= 1;

	if (!SyncPinNames())
	{
		// Still mark as modified. Otherwise, will be marked automatically.
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(
			GetBlueprint());
	}
}

void UBPNode_MakeFilter::RemoveExcludedDetailClassPin(UEdGraphPin *ExcludedDetailPin)
{
	if (!ExcludedDetailsCount)
		return;
	if (!ExcludedDetailPin)
		return;

	Modify();

	auto Schema = GetSchema();
	check(Schema);

	ExcludedDetailPin->BreakAllPinLinks();

	RemovePin(ExcludedDetailPin);

	ExcludedDetailsCount -= 1;

	if (!SyncPinNames())
	{
		// Still mark as modified. Otherwise, will be marked automatically.
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(
			GetBlueprint());
	}
}

void UBPNode_MakeFilter::InteractiveRemoveExcludingFlagmarkPin()
{
	FScopedTransaction Transaction(LOCTEXT("RemoveExcludingFlagmarkTx",
										   "Remove Excluding Flagmark"));
	RemoveExcludingFlagmarkPin();
}

void UBPNode_MakeFilter::InteractiveRemoveExcludedTraitTypePin(UEdGraphPin *ExcludedTraitPin)
{
	FScopedTransaction Transaction(LOCTEXT("RemoveExcludedTraitPinTx",
										   "Remove Excluded Trait"));
	RemoveExcludedTraitTypePin(ExcludedTraitPin);
}

void UBPNode_MakeFilter::InteractiveRemoveExcludedDetailClassPin(UEdGraphPin *ExcludedDetailPin)
{
	FScopedTransaction Transaction(LOCTEXT("RemoveExcludedDetailPinTx",
										   "Remove Excluded Detail"));
	RemoveExcludedDetailClassPin(ExcludedDetailPin);
}

void UBPNode_MakeFilter::GetNodeContextMenuActions(
	UToolMenu *Menu, UGraphNodeContextMenuContext *Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);

	// No additions during debugging:
	if (Context->bIsDebugging) return;
	
	FToolMenuSection& Section =
		Menu->AddSection("BPNodeMakeFilter",
						 LOCTEXT("ContextMenuHeader", "Filter"));

	if (HasExcludingFlagmark())
	{
		Section.AddMenuEntry(
			TEXT("RemoveExcludingFlagmarkPin"),
			LOCTEXT("RemoveExcludingFlagmarkPin",
					"Remove Excluding Flagmark"),
			LOCTEXT("RemoveExcludingFlagmarkPinTooltip",
					"Remove the excluding flagmark from the filter."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateUObject(
				const_cast<UBPNode_MakeFilter*>(this),
				&UBPNode_MakeFilter::InteractiveRemoveExcludingFlagmarkPin)));
	}
	else
	{
		Section.AddMenuEntry(
			TEXT("AddExcludingFlagmarkPin"),
			LOCTEXT("AddExcludingFlagmarkPin",
					"Add Excluding Flagmark"),
			LOCTEXT("AddExcludingFlagmarkPinTooltip",
					"Add an excluding flagmark to the filter."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateUObject(
				const_cast<UBPNode_MakeFilter*>(this),
				&UBPNode_MakeFilter::InteractiveAddExcludingFlagmarkPin)));
	}

	if (Context->Pin != nullptr)
	{
		if (IsExcludedTraitTypePin(Context->Pin))
		{
			Section.AddMenuEntry(
				TEXT("RemoveExcludedTraitTypePin"),
				LOCTEXT("RemoveExcludedTraitTypePin",
						  "Remove Excluded Trait"),
				LOCTEXT("RemoveExcludedTraitTypePinTooltip",
						  "Remove the excluded trait type from the filter."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateUObject(
					const_cast<UBPNode_MakeFilter*>(this),
					&UBPNode_MakeFilter::InteractiveRemoveExcludedTraitTypePin,
					const_cast<UEdGraphPin *>(Context->Pin))));
		}
	}
	else
	{
		Section.AddMenuEntry(
			TEXT("AddExcludedTraitTypePin"),
			LOCTEXT("AddExcludedTraitTypePin",
					  "Add Excluded Trait"),
			LOCTEXT("AddExcludedTraitTypePinTooltip",
					  "Add a trait type exclusion to the filter."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateUObject(
				const_cast<UBPNode_MakeFilter*>(this),
				&UBPNode_MakeFilter::InteractiveAddExcludedTraitTypePin)));
	}
	if (Context->Pin != nullptr)
	{
		if (IsExcludedDetailClassPin(Context->Pin))
		{
			Section.AddMenuEntry(
				TEXT("RemoveExcludedDetailClassPin"),
				LOCTEXT("RemoveExcludedDetailClassPin",
						  "Remove Excluded Detail"),
				LOCTEXT("RemoveExcludedDetailClassPinTooltip",
						  "Remove the excluded detail class from the filter."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateUObject(
					const_cast<UBPNode_MakeFilter*>(this),
					&UBPNode_MakeFilter::InteractiveRemoveExcludedDetailClassPin,
					const_cast<UEdGraphPin *>(Context->Pin))));
		}
	}
	else
	{
		Section.AddMenuEntry(
			TEXT("AddExcludedDetailClassPin"),
			LOCTEXT("AddExcludedDetailClassPin",
					  "Add Excluded Detail"),
			LOCTEXT("AddExcludedDetailClassPinTooltip",
					  "Add a detail class exclusion to the filter."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateUObject(
				const_cast<UBPNode_MakeFilter*>(this),
				&UBPNode_MakeFilter::InteractiveAddExcludedDetailClassPin)));
	}
}

bool UBPNode_MakeFilter::IsExcludingFlagmarkPin(const UEdGraphPin* Pin) const
{
	return Pin != nullptr && Pin->Direction == EGPD_Input &&
		   Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Int &&
		   Pin->PinType.PinSubCategory == UEdGraphSchema_K2::PSC_Bitmask &&
		   Pin->PinName == ExcludingFlagmarkPinName;
}

bool UBPNode_MakeFilter::IsExcludedTraitTypePin(const UEdGraphPin *Pin) const
{
	return Pin != nullptr && Pin->Direction == EGPD_Input &&
		   Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object &&
		   Pin->PinType.PinSubCategoryObject == UScriptStruct::StaticClass() &&
		   Pin->PinName.ToString().StartsWith(ExcludedTraitTypePinNamePrefix);
}

bool UBPNode_MakeFilter::IsExcludedDetailClassPin(const UEdGraphPin *Pin) const
{
	return Pin != nullptr && Pin->Direction == EGPD_Input &&
		   Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Class &&
		   Pin->PinType.PinSubCategoryObject == UDetail::StaticClass() &&
		   Pin->PinName.ToString().StartsWith(ExcludedDetailClassPinNamePrefix);
}

int32 UBPNode_MakeFilter::GetExcludedTraitTypePinIndex(const UEdGraphPin *Pin) const
{
	if (Pin == nullptr)
	{
		return -1;
	}

	for (int32 i = 0; i < ExcludedDetailsCount; ++i)
	{
		const auto Name = MakeExcludedTraitTypePinName(i);
		if (FindPin(Name) == Pin)
		{
			return i;
		}
	}
	return -1;
}

int32 UBPNode_MakeFilter::GetExcludedDetailClassPinIndex(const UEdGraphPin *Pin) const
{
	if (Pin == nullptr)
	{
		return -1;
	}

	for (int32 i = 0; i < ExcludedDetailsCount; ++i)
	{
		const auto Name = MakeExcludedDetailClassPinName(i);
		if (FindPin(Name) == Pin)
		{
			return i;
		}
	}
	return -1;
}

void UBPNode_MakeFilter::PinConnectionListChanged(UEdGraphPin* ChangedPin)
{
	Super::PinConnectionListChanged(ChangedPin);

	if (IsExcludedDetailClassPin(ChangedPin))
	{
		OnExcludedDetailClassPinChanged(ChangedPin);
	}
	else if (IsExcludedTraitTypePin(ChangedPin))
	{
		OnExcludedTraitTypePinChanged(ChangedPin);
	}
	else if (IsExcludingFlagmarkPin(ChangedPin))
	{
		OnExcludingFlagmarkPinChanged(ChangedPin);
	}
}

void UBPNode_MakeFilter::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
	Super::PinDefaultValueChanged(ChangedPin);

	if (IsExcludedDetailClassPin(ChangedPin))
	{
		OnExcludedDetailClassPinChanged(ChangedPin);
	}
	else if (IsExcludedTraitTypePin(ChangedPin))
	{
		OnExcludedTraitTypePinChanged(ChangedPin);
	}
	else if (IsExcludingFlagmarkPin(ChangedPin))
	{
		OnExcludingFlagmarkPinChanged(ChangedPin);
	}
}

UScriptStruct*
UBPNode_MakeFilter::GetExcludedTraitTypePinType(UEdGraphPin *Pin) const
{
	return GetTraitTypePinType(Pin);
}

UClass*
UBPNode_MakeFilter::GetExcludedDetailClassPinClass(UEdGraphPin *Pin) const
{
	return GetDetailClassPinClass(Pin);
}

UEdGraphPin*
UBPNode_MakeFilter::GetExcludedTraitTypePinGivenIndex(const int32 Index) const
{
	return FindPinChecked(MakeExcludedTraitTypePinName(Index), EGPD_Input);
}

UEdGraphPin*
UBPNode_MakeFilter::GetExcludedDetailClassPinGivenIndex(const int32 Index) const
{
	return FindPinChecked(MakeExcludedDetailClassPinName(Index), EGPD_Input);
}

UEdGraphPin *UBPNode_MakeFilter::GetFilterPin() const
{
	return FindPinChecked(FilterPinName, EGPD_Output);
}

void UBPNode_MakeFilter::OnExcludingFlagmarkPinChanged(UEdGraphPin *ChangedPin)
{
	Modify();

	UpdateExcludingFlagmarkPinUI(ChangedPin);

	if (auto Graph = GetGraph())
	{
		Graph->NotifyGraphChanged();
	}
}

void UBPNode_MakeFilter::OnExcludedTraitTypePinChanged(UEdGraphPin *ChangedPin)
{
	Modify();

	UpdateExcludedTraitTypePinUI(ChangedPin);

	if (auto Graph = GetGraph())
	{
		Graph->NotifyGraphChanged();
	}
}

void UBPNode_MakeFilter::OnExcludedDetailClassPinChanged(UEdGraphPin *ChangedPin)
{
	Modify();

	UpdateExcludedDetailClassPinUI(ChangedPin);

	if (auto Graph = GetGraph())
	{
		Graph->NotifyGraphChanged();
	}
}

#undef LOCTEXT_NAMESPACE
