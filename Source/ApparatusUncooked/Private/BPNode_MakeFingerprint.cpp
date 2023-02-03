/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_MakeFingerprint.cpp
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

#include "BPNode_MakeFingerprint.h"

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
#include "ApparatusUncookedUtils.h"
#include "ApparatusFunctionLibrary.h"
#include "Machine.h"
#include "Detail.h"

#define LOCTEXT_NAMESPACE "UBPNode_MakeFingerprint"

const FString UBPNode_MakeFingerprint::DetailClassPinNamePrefix(TEXT("DetailType_"));

const FString UBPNode_MakeFingerprint::TraitTypePinNamePrefix(TEXT("TraitType_"));

const FName UBPNode_MakeFingerprint::FingerprintPinName(TEXT("Fingerprint"));

const FName UBPNode_MakeFingerprint::FlagmarkPinName(TEXT("Flagmark"));


FText UBPNode_MakeFingerprint::GetNodeTitle(
	ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Title", "Make Fingerprint");
}

FText UBPNode_MakeFingerprint::GetTooltipText() const
{
	return LOCTEXT("TooltipText",
					 "Make a fingerprint from a list of detail types.");
}

bool UBPNode_MakeFingerprint::IsNodePure() const { return true; }

bool UBPNode_MakeFingerprint::SyncPinNames()
{
	bool bChanged = false;

	int32 DetailClassIndex = 0;
	for (UEdGraphPin *Pin : Pins)
	{
		if (IsDetailClassPin(Pin))
		{
			const FName NewName = MakeDetailClassPinName(DetailClassIndex++);
			if (Pin->PinName != NewName)
			{
				Pin->PinName = NewName;
				bChanged = true;
			}
		}
	}

	int32 TraitTypeIndex = 0;
	for (UEdGraphPin *Pin : Pins)
	{
		if (IsTraitTypePin(Pin))
		{
			const FName NewName = MakeTraitTypePinName(TraitTypeIndex++);
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

void UBPNode_MakeFingerprint::UpdateFlagmarkPinUI(UEdGraphPin* FlagmarkPin)
{
	auto Schema = GetSchema();
	check(Schema);

	int32 DefaultValue = 0;
	FDefaultValueHelper::ParseInt(FlagmarkPin->DefaultValue, DefaultValue);
	if ((FlagmarkPin->LinkedTo.Num() == 0) && (DefaultValue != FM_None))
	{
		FString Tooltip = "Must include all of: ";
		Tooltip += ToString((EFlagmark)DefaultValue);
		Schema->ConstructBasicPinTooltip(
			*FlagmarkPin,
			FText::FromString(Tooltip),
			FlagmarkPin->PinToolTip);
		FlagmarkPin->PinFriendlyName = FText::FromString(TEXT(FLAGMARK_SHORTCUT_STR));
	}
	else
	{
		Schema->ConstructBasicPinTooltip(
			*FlagmarkPin,
			LOCTEXT("FlagmarkPinTooltip", "The flagmark specification of the subject."),
			FlagmarkPin->PinToolTip);
		FlagmarkPin->PinFriendlyName = FText::GetEmpty();
	}
}

void UBPNode_MakeFingerprint::UpdateTraitTypePinUI(UEdGraphPin *TraitTypePin)
{
	checkSlow(IsTraitTypePin(TraitTypePin));
	auto Schema = GetSchema();
	check(Schema);
	Schema->ConstructBasicPinTooltip(
		*TraitTypePin, LOCTEXT("TraitTypePinTooltip", "The type of traits to include."),
		TraitTypePin->PinToolTip);

	auto Type = GetTraitTypePinType(TraitTypePin);

	if (Type)
	{
		TraitTypePin->PinFriendlyName = FText::FromString(TEXT(TRAIT_SHORTCUT_STR));
	}
	else
	{
		TraitTypePin->PinFriendlyName = FText::GetEmpty();
	}
}

void UBPNode_MakeFingerprint::UpdateDetailClassPinUI(UEdGraphPin *DetailClassPin)
{
	checkSlow(IsDetailClassPin(DetailClassPin));
	auto Schema = GetSchema();
	check(Schema);
	Schema->ConstructBasicPinTooltip(
		*DetailClassPin, LOCTEXT("DetailClassPinTooltip", "The class of details to include."),
		DetailClassPin->PinToolTip);

	auto Class = GetDetailClassPinClass(DetailClassPin);

	if (Class && Class != UDetail::StaticClass())
	{
		DetailClassPin->PinFriendlyName = FText::FromString(TEXT(DETAIL_SHORTCUT_STR));
	}
	else
	{
		DetailClassPin->PinFriendlyName = FText::GetEmpty();
	}
}

void UBPNode_MakeFingerprint::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Backward-compatibility migration:
	if (NumTypes != 0)
	{
		DetailsCount = NumTypes;
		NumTypes = 0;
	}
	else if (TypesCount != 0)
	{
		DetailsCount = TypesCount;
		TypesCount = 0;
	}

	const UEdGraphSchema* Schema = GetSchema();
	check(Schema);

	if (HasFlagmark())
	{
		auto* const FlagmarkPin =
			CreatePin(EGPD_Input,
					UEdGraphSchema_K2::PC_Int,
					UEdGraphSchema_K2::PSC_Bitmask,
					GetFlagmarkClass(),
					FlagmarkPinName);
		FlagmarkPin->DefaultValue = ToString(FM_None);
		UpdateFlagmarkPinUI(FlagmarkPin);
	}

	UEdGraphPin *FingerprintPin =
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct,
				  FFingerprint::StaticStruct(), FingerprintPinName);

	Schema->ConstructBasicPinTooltip(
		*FingerprintPin,
		LOCTEXT("FingerprintPinTooltip",
				  "The resulting fingerprint object."),
		FingerprintPin->PinToolTip);

	for (int32 i = 0; i < TraitsCount; ++i)
	{
		const FName NewTraitTypePinName(MakeTraitTypePinName(i));

		auto TraitTypePin =
			CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object,
					  /*SubCategory=*/UScriptStruct::StaticClass(), NewTraitTypePinName);

		UpdateTraitTypePinUI(TraitTypePin);
	}

	for (int32 i = 0; i < DetailsCount; ++i)
	{
		const FName NewTypePinName(MakeDetailClassPinName(i));

		auto DetailClassPin =
			CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Class,
					  /*SubCategory=*/UDetail::StaticClass(), NewTypePinName);

		UpdateDetailClassPinUI(DetailClassPin);
	}
}

FLinearColor UBPNode_MakeFingerprint::GetNodeTitleColor() const
{
	return Super::GetNodeTitleColor();
}

FText UBPNode_MakeFingerprint::GetMenuCategory() const
{
	return FText::FromString("Apparatus|Fingerprint");
}

void UBPNode_MakeFingerprint::GetMenuActions(
	FBlueprintActionDatabaseRegistrar &ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that
	// actions might have to be updated (or deleted) if their object-key is
	// mutated (or removed)... here we use the node's class (so if the node
	// type disappears, then the action should go with it)
	auto ActionKey = GetClass();
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		auto NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UBPNode_MakeFingerprint::EarlyValidation(
	class FCompilerResultsLog &MessageLog) const
{
	Super::EarlyValidation(MessageLog);

	// Validate detail pins:
	for (int32 i = 0; i < DetailsCount; ++i)
	{
		UEdGraphPin* Pin = GetDetailClassPinGivenIndex(i);
		UClass* Class = GetDetailClassPinClass(Pin);
		if (Class == nullptr ||
			Class == UObject::StaticClass() ||
			Class == UDetail::StaticClass())
		{
			MessageLog.Error(
				*FText::Format(NSLOCTEXT(LOCTEXT_NAMESPACE,
										 "UnspecifiedDetailClassPin",
										 "The detail pin #{0} @@ is added "
										 "but not specified in @@"),
							   i)
					 .ToString(),
				Pin, this);
		}
		// Check for duplicates:
		for (int32 j = 0; j < i; ++j)
		{
			UEdGraphPin* OtherPin = GetDetailClassPinGivenIndex(j);
			UClass* OtherClass = GetDetailClassPinClass(OtherPin);
			if (OtherClass && OtherClass == Class)
			{
				MessageLog.Error(
					*FText::Format(NSLOCTEXT(LOCTEXT_NAMESPACE,
											 "DuplicateDetailClassPin",
											 "The detail pin #{0} @@ is a "
											 "duplicate of #{1} in @@."),
								i, j)
						.ToString(),
					Pin, this);
			}
		}
	}

	// Validate trait pins:
	for (int32 i = 0; i < TraitsCount; ++i)
	{
		UEdGraphPin* Pin = GetTraitTypePinGivenIndex(i);
		UScriptStruct* Type = GetTraitTypePinType(Pin);
		if (Type == nullptr)
		{
			MessageLog.Error(
				*FText::Format(NSLOCTEXT(LOCTEXT_NAMESPACE,
										 "UnspecifiedTraitTypePin",
										 "The trait pin #{0} @@ is added "
										 "but not specified in @@"),
							   i)
					 .ToString(),
				Pin, this);
		}
		// Check for duplicates:
		for (int32 j = 0; j < i; ++j)
		{
			UEdGraphPin *OtherPin = GetTraitTypePinGivenIndex(j);
			UScriptStruct* OtherType = GetTraitTypePinType(OtherPin);
			if (OtherType && OtherType == Type)
			{
				MessageLog.Error(
					*FText::Format(NSLOCTEXT(LOCTEXT_NAMESPACE,
											"DuplicateTraitTypePin",
											"The trait pin #{0} @@ is a "
											"duplicate of #{1} in @@."),
								i, j)
						.ToString(),
					Pin, this);
			}
		}
	}
}

UEdGraphPin* UBPNode_MakeFingerprint::GetFlagmarkPin() const
{
	if (!HasFlagmark()) return nullptr;
	return FindPinChecked(FlagmarkPinName, EGPD_Input);
}

void UBPNode_MakeFingerprint::ExpandToArrayNodes(
	class FKismetCompilerContext& CompilerContext,
	UEdGraph*                     SourceGraph,
	UEdGraphPin*&                 TraitsArrayPin,
	UEdGraphPin*&                 DetailsArrayPin)
{
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	check(Schema);

	const auto TraitsArray =
		CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>(this,
																 SourceGraph);
	const auto DetailsArray =
		CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>(this,
																 SourceGraph);

	TraitsArray->AllocateDefaultPins();
	DetailsArray->AllocateDefaultPins();

	// Using this temporary trait type variable to initialize array type:
	const auto TypePlaceholder = CompilerContext.SpawnInternalVariable(
		this, UEdGraphSchema_K2::PC_Object, NAME_None, UScriptStruct::StaticClass());
	// Using this temporary detail class variable to initialize array type:
	const auto ClassPlaceholder = CompilerContext.SpawnInternalVariable(
		this, UEdGraphSchema_K2::PC_Class, NAME_None, UDetail::StaticClass());

	// BEGIN - Allocate array pins.
	while (TraitsArray->Pins.Num() < TraitsCount + 1)
	{
		TraitsArray->AddInputPin();
	}
	while (DetailsArray->Pins.Num() < DetailsCount + 1)
	{
		DetailsArray->AddInputPin();
	}
	// END - Allocate array pins.

	// BEGIN - Define arrays types.
	for (int32 i = 0; i < TraitsCount; ++i)
	{
		auto DstPin = TraitsArray->GetPinAt(i + 1);
		MAKE_LINK(TypePlaceholder->GetVariablePin(), DstPin);
	}
	for (int32 i = 0; i < DetailsCount; ++i)
	{
		auto DstPin = DetailsArray->GetPinAt(i + 1);
		MAKE_LINK(ClassPlaceholder->GetVariablePin(), DstPin);
	}

	TraitsArray->PostReconstructNode();
	DetailsArray->PostReconstructNode();
	// END - Define array types.

	// BEGIN - Link type arrays.
	for (int32 i = 0; i < TraitsCount; ++i)
	{
		auto TraitTypePin = GetTraitTypePinGivenIndex(i);
		auto DstPin = TraitsArray->GetPinAt(i + 1);

		Schema->BreakSinglePinLink(TypePlaceholder->GetVariablePin(), DstPin);

		if (TraitTypePin->LinkedTo.Num() == 0)
		{
			if (TraitTypePin->DefaultObject != nullptr)
			{
				auto Type = GetTraitTypePinType(TraitTypePin);
				DstPin->DefaultObject = TraitTypePin->DefaultObject;
			}
		}
		else
		{
			verify(!CompilerContext.MovePinLinksToIntermediate(*TraitTypePin, *DstPin)
					.IsFatal());
		}
	}

	for (int32 i = 0; i < DetailsCount; ++i)
	{
		auto DetailClassPin = GetDetailClassPinGivenIndex(i);
		auto DstPin = DetailsArray->GetPinAt(i + 1);

		Schema->BreakSinglePinLink(ClassPlaceholder->GetVariablePin(), DstPin);

		if (DetailClassPin->LinkedTo.Num() == 0)
		{
			if (DetailClassPin->DefaultObject != nullptr)
			{
				auto Type = GetDetailClassPinClass(DetailClassPin);
				DstPin->DefaultObject = DetailClassPin->DefaultObject;
			}
		}
		else
		{
			verify(!CompilerContext.MovePinLinksToIntermediate(*DetailClassPin, *DstPin)
					.IsFatal());
		}
	}
	// END - Link type arrays.

	// Assign the results...
	DetailsArrayPin = DetailsArray->GetOutputPin();
	TraitsArrayPin = TraitsArray->GetOutputPin();
}

void UBPNode_MakeFingerprint::Expand_CopyOrMoveLink(class FKismetCompilerContext& CompilerContext,
													UEdGraph* SourceGraph,
													UEdGraphPin* SrcPin,
													UEdGraphPin* DstPin)
{
	check(SrcPin);
	check(DstPin);
	if (SrcPin->LinkedTo.Num() > 0)
	{
		CompilerContext.MovePinLinksToIntermediate(*SrcPin, *DstPin);
	}
	else
	{
		DstPin->DefaultValue  = SrcPin->DefaultValue;
		DstPin->DefaultObject = SrcPin->DefaultObject;
	}
}

void
UBPNode_MakeFingerprint::ExpandNode(
	class FKismetCompilerContext& CompilerContext,
	UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	const auto Schema = CompilerContext.GetSchema();
	check(Schema);

	static const FName MakeFingerprintName =
		GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, MakeFingerprint);

	auto MakeFingerprintNode =
		CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(
			this, SourceGraph);
	MakeFingerprintNode->FunctionReference.SetExternalMember(
		MakeFingerprintName, UApparatusFunctionLibrary::StaticClass());
	MakeFingerprintNode->AllocateDefaultPins();

	if (HasFlagmark())
	{
		auto* const TargetFlagmarkPin =
			MakeFingerprintNode->FindPinChecked(TEXT("Flagmark"));

		auto* const FlagmarkPin = GetFlagmarkPin();
		Expand_CopyOrMoveLink(CompilerContext, SourceGraph,
							  FlagmarkPin, TargetFlagmarkPin);
	}

	auto ResultFingerprintPin = MakeFingerprintNode->GetReturnValuePin();
	check(ResultFingerprintPin);
	UEdGraphPin *FingerprintPin = GetFingerprintPin();

	CompilerContext.MovePinLinksToIntermediate(*FingerprintPin,
											   *ResultFingerprintPin);

	UEdGraphPin* TraitTypesPin = nullptr;
	UEdGraphPin* DetailClassesPin = nullptr;
	ExpandToArrayNodes(CompilerContext, SourceGraph,
					   TraitTypesPin, DetailClassesPin);

	MAKE_LINK(
		   TraitTypesPin,
		   MakeFingerprintNode->FindPinChecked(TEXT("Traits"), EGPD_Input));
	MAKE_LINK(
		   DetailClassesPin,
		   MakeFingerprintNode->FindPinChecked(TEXT("Details"), EGPD_Input));

	BreakAllNodeLinks();
}

void UBPNode_MakeFingerprint::PostReconstructNode()
{
	Super::PostReconstructNode();

	// Have to update the UI here, cause this is launched
	// after the serialization.

	if (HasFlagmark())
	{
		UpdateFlagmarkPinUI(GetFlagmarkPin());
	}
	for (int32 i = 0; i < TraitsCount; ++i)
	{
		UEdGraphPin* TraitTypePin = GetTraitTypePinGivenIndex(i);
		UpdateTraitTypePinUI(TraitTypePin);
	}
	for (int32 i = 0; i < DetailsCount; ++i)
	{
		UEdGraphPin* DetailClassPin = GetDetailClassPinGivenIndex(i);
		UpdateDetailClassPinUI(DetailClassPin);
	}
}

FName UBPNode_MakeFingerprint::MakeDetailClassPinName(const int32 Index) const
{
	return *(DetailClassPinNamePrefix + FString::FormatAsNumber(Index));
}

FName UBPNode_MakeFingerprint::MakeTraitTypePinName(const int32 Index) const
{
	return *(TraitTypePinNamePrefix + FString::FormatAsNumber(Index));
}

void UBPNode_MakeFingerprint::AddInputPin(UEdGraphPin*& AddedPin)
{
	AddedPin = nullptr;
	if ((DetailsCount == 0) && (TraitsCount > 0))
	{
		AddedPin = InteractiveAddTraitTypePin();
	}
	else
	{
		AddedPin = InteractiveAddDetailClassPin();
	}
}

void UBPNode_MakeFingerprint::AddInputPin()
{
	UEdGraphPin* AddedPin = nullptr;
	AddInputPin(AddedPin);
	ensure(AddedPin);
}

UEdGraphPin*
UBPNode_MakeFingerprint::AddFlagmarkPin()
{
	if (!CanAddPin()) return nullptr;
	if (HasFlagmark()) return GetFlagmarkPin();

	Modify();

	auto Schema = GetDefault<UEdGraphSchema_K2>();
	check(Schema);

	auto* const FlagmarkPin =
		CreatePin(EGPD_Input,
				  UEdGraphSchema_K2::PC_Int,
				  UEdGraphSchema_K2::PSC_Bitmask,
				  GetFlagmarkClass(),
				  FlagmarkPinName);
	FlagmarkPin->DefaultValue = FString::FromInt(FM_None);
	Schema->ConstructBasicPinTooltip(
		*FlagmarkPin,
		LOCTEXT("FlagmarkPinTooltip",
				"The flag-based state of the subject."),
		FlagmarkPin->PinToolTip);

	bFlagmarkActive = true;

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	return FlagmarkPin;
}

UEdGraphPin*
UBPNode_MakeFingerprint::AddTraitTypePin()
{
	if (!CanAddPin()) return nullptr;

	Modify();

	auto Schema = GetDefault<UEdGraphSchema_K2>();
	check(Schema);

	auto TraitTypePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object,
								  /*SubCategory=*/UScriptStruct::StaticClass(),
								  MakeTraitTypePinName(TraitsCount));
	Schema->SetPinAutogeneratedDefaultValueBasedOnType(TraitTypePin);

	UpdateTraitTypePinUI(TraitTypePin);

	TraitsCount += 1;

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	return TraitTypePin;
}

UEdGraphPin*
UBPNode_MakeFingerprint::AddDetailClassPin()
{
	if (!CanAddPin()) return nullptr;

	Modify();

	auto Schema = GetDefault<UEdGraphSchema_K2>();
	check(Schema);

	auto DetailClassPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Class,
							 /*SubCategory=*/UDetail::StaticClass(),
							 MakeDetailClassPinName(DetailsCount));

	Schema->SetPinAutogeneratedDefaultValueBasedOnType(DetailClassPin);

	UpdateDetailClassPinUI(DetailClassPin);

	DetailsCount += 1;

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	return DetailClassPin;
}

UEdGraphPin*
UBPNode_MakeFingerprint::InteractiveAddFlagmarkPin()
{
	FScopedTransaction Transaction(
		LOCTEXT("AddFlagmarkPinTx", "Add Flagmark Pin"));
	return AddFlagmarkPin();
}

UEdGraphPin*
UBPNode_MakeFingerprint::InteractiveAddTraitTypePin()
{
	FScopedTransaction Transaction(
		LOCTEXT("AddTraitTypePinTx", "Add Trait Pin"));
	return AddTraitTypePin();
}

UEdGraphPin*
UBPNode_MakeFingerprint::InteractiveAddDetailClassPin()
{
	FScopedTransaction Transaction(
		LOCTEXT("AddDetailClassPinTx", "Add Detail Pin"));
	return AddDetailClassPin();
}

void UBPNode_MakeFingerprint::RemoveFlagmarkPin()
{
	if (!HasFlagmark())
		return;

	Modify();

	auto Schema = GetSchema();
	check(Schema);

	const auto FlagmarkPin = GetFlagmarkPin();

	FlagmarkPin->BreakAllPinLinks();

	RemovePin(FlagmarkPin);

	bFlagmarkActive = false;

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(
		GetBlueprint());
}

void UBPNode_MakeFingerprint::RemoveTraitTypePin(const int32 Index)
{
	if (!TraitsCount)
		return;
	if (Index == -1)
		return;

	Modify();

	auto Schema = GetSchema();
	check(Schema);

	auto TraitTypePin = GetTraitTypePinGivenIndex(Index);

	TraitTypePin->BreakAllPinLinks();

	RemovePin(TraitTypePin);

	TraitsCount -= 1;

	if (!SyncPinNames())
	{
		// Still mark the blueprint as changed.
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(
			GetBlueprint());
	}
}

void UBPNode_MakeFingerprint::RemoveDetailClassPin(const int32 Index)
{
	if (!DetailsCount)
		return;
	if (Index == -1)
		return;

	Modify();

	auto Schema = GetSchema();
	check(Schema);

	auto DetailClassPin = GetDetailClassPinGivenIndex(Index);

	DetailClassPin->BreakAllPinLinks();

	RemovePin(DetailClassPin);

	DetailsCount -= 1;

	if (!SyncPinNames())
	{
		// Still mark the blueprint as changed.
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(
			GetBlueprint());
	}
}

void UBPNode_MakeFingerprint::RemoveTraitTypePin(UEdGraphPin *TypePin)
{
	auto Index = FindTraitTypePinIndex(TypePin);
	if (Index != -1)
	{
		RemoveTraitTypePin(Index);
	}
}

void UBPNode_MakeFingerprint::RemoveDetailClassPin(UEdGraphPin *TypePin)
{
	auto Index = FindDetailClassPinIndex(TypePin);
	if (Index != -1)
	{
		RemoveDetailClassPin(Index);
	}
}

void UBPNode_MakeFingerprint::InteractiveRemoveFlagmarkPin()
{
	FScopedTransaction Transaction(
		LOCTEXT("RemoveFlagmarkPinTx", "Remove Flagmark Pin"));
	RemoveFlagmarkPin();
}


void UBPNode_MakeFingerprint::InteractiveRemoveTraitTypePin(UEdGraphPin *TraitTypePin)
{
	FScopedTransaction Transaction(
		LOCTEXT("RemoveTraitTypePinTx", "Remove Trait Pin"));
	RemoveTraitTypePin(TraitTypePin);
}

void UBPNode_MakeFingerprint::InteractiveRemoveDetailClassPin(UEdGraphPin *DetailClassPin)
{
	FScopedTransaction Transaction(
		LOCTEXT("RemoveDetailClassPinTx", "Remove Detail Pin"));
	RemoveDetailClassPin(DetailClassPin);
}

void UBPNode_MakeFingerprint::GetNodeContextMenuActions(
	UToolMenu *Menu, UGraphNodeContextMenuContext *Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);

	// No context menu during the debugging process:
	if (Context->bIsDebugging) return;
	
	FToolMenuSection& Section =
		Menu->AddSection("BPNodeMakeFingerprint",
						 LOCTEXT("ContextMenuHeader", "Fingerprint"));

	if (Context->Pin != nullptr)
	{
		if (IsDetailClassPin(Context->Pin))
		{
			Section.AddMenuEntry(
				TEXT("RemoveDetailClassPin"),
				LOCTEXT("RemoveDetailClassPin", "Remove a detail pin."),
				LOCTEXT("RemoveDetailClassPinTooltip","Remove the detail pin."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateUObject(
					const_cast<UBPNode_MakeFingerprint*>(this),
					&UBPNode_MakeFingerprint::InteractiveRemoveDetailClassPin,
					const_cast<UEdGraphPin *>(Context->Pin))));
		} else if (IsTraitTypePin(Context->Pin)) {
			Section.AddMenuEntry(
				TEXT("RemoveTraitTypePin"),
				LOCTEXT("RemoveTraitTypePin", "Remove a trait pin."),
				LOCTEXT("RemoveTraitTypePinTooltip", "Remove the trait pin."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateUObject(
					const_cast<UBPNode_MakeFingerprint*>(this),
					&UBPNode_MakeFingerprint::InteractiveRemoveTraitTypePin,
					const_cast<UEdGraphPin *>(Context->Pin))));
		}
	}

	if (!HasFlagmark())
	{
		Section.AddMenuEntry(
			TEXT("AddFlagmarkPin"),
			LOCTEXT("AddFlagmarkPin", "Add Flagmark Pin"),
			LOCTEXT("AddFlagmarkPinTooltip", "Add a flagmark specification pin."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateUObject(
				const_cast<UBPNode_MakeFingerprint*>(this),
				&UBPNode_MakeFingerprint::MenuAddFlagmarkPin)));
	}
	else
	{
		Section.AddMenuEntry(
			TEXT("RemoveFlagmarkPin"),
			LOCTEXT("RemoveFlagmarkPin", "Remove Flagmark Pin"),
			LOCTEXT("RemoveFlagmarkPinTooltip", "Remove a flagmark specification pin."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateUObject(
				const_cast<UBPNode_MakeFingerprint*>(this),
				&UBPNode_MakeFingerprint::MenuRemoveFlagmarkPin)));
	}
	Section.AddMenuEntry(
		TEXT("AddTraitTypePin"),
		LOCTEXT("AddTraitTypePin", "Add Trait Pin"),
		LOCTEXT("AddTraitTypePinTooltip", "Add another trait pin."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateUObject(
			const_cast<UBPNode_MakeFingerprint*>(this),
			&UBPNode_MakeFingerprint::MenuAddTraitTypePin)));
	Section.AddMenuEntry(
		TEXT("AddDetailClassPin"),
		LOCTEXT("AddDetailClassPin", "Add Detail Pin"),
		LOCTEXT("AddDetailClassPinTooltip", "Add another detail pin."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateUObject(
			const_cast<UBPNode_MakeFingerprint*>(this),
			&UBPNode_MakeFingerprint::MenuAddDetailClassPin)));
}

bool UBPNode_MakeFingerprint::IsFlagmarkPin(const UEdGraphPin *Pin) const
{
	return Pin != nullptr && Pin->Direction == EGPD_Input &&
		   Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Int &&
		   Pin->PinType.PinSubCategory == UEdGraphSchema_K2::PSC_Bitmask &&
		   Pin->PinName == FlagmarkPinName;
}

bool UBPNode_MakeFingerprint::IsTraitTypePin(const UEdGraphPin *Pin) const
{
	return Pin != nullptr && Pin->Direction == EGPD_Input &&
		   Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object &&
		   Pin->PinType.PinSubCategoryObject == UScriptStruct::StaticClass() &&
		   Pin->PinName.ToString().StartsWith(TraitTypePinNamePrefix);
}

bool UBPNode_MakeFingerprint::IsDetailClassPin(const UEdGraphPin *Pin) const
{
	return Pin != nullptr && Pin->Direction == EGPD_Input &&
		   Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Class &&
		   Pin->PinName.ToString().StartsWith(DetailClassPinNamePrefix);
}

int32 UBPNode_MakeFingerprint::FindDetailClassPinIndex(const UEdGraphPin *Pin) const
{
	if (Pin == nullptr)
	{
		return -1;
	}

	for (int32 i = 0; i < DetailsCount; ++i)
	{
		const FName Name = MakeDetailClassPinName(i);
		const UEdGraphPin* const FoundPin = FindPin(Name);
		if (!IsDetailClassPin(FoundPin)) continue;
		if (FoundPin->PinName == Pin->PinName)
		{
			return i;
		}
	}
	return -1;
}

int32 UBPNode_MakeFingerprint::FindTraitTypePinIndex(const UEdGraphPin* Pin) const
{
	if (Pin == nullptr)
	{
		return -1;
	}

	for (int32 i = 0; i < TraitsCount; ++i)
	{
		const FName Name = MakeTraitTypePinName(i);
		const UEdGraphPin* const FoundPin = FindPin(Name);
		if (!IsTraitTypePin(FoundPin)) continue;
		if (FoundPin->PinName == Pin->PinName)
		{
			return i;
		}
	}
	return -1;
}

void UBPNode_MakeFingerprint::PinConnectionListChanged(UEdGraphPin *ChangedPin)
{
	Super::PinConnectionListChanged(ChangedPin);
	if (IsDetailClassPin(ChangedPin))
	{
		OnDetailClassPinChanged(ChangedPin);
	}
	else if (IsTraitTypePin(ChangedPin))
	{
		OnTraitTypePinChanged(ChangedPin);
	}
	else if (IsFlagmarkPin(ChangedPin))
	{
		OnFlagmarkPinChanged(ChangedPin);
	}
}

void UBPNode_MakeFingerprint::PinDefaultValueChanged(UEdGraphPin *ChangedPin)
{
	Super::PinDefaultValueChanged(ChangedPin);
	if (IsDetailClassPin(ChangedPin))
	{
		OnDetailClassPinChanged(ChangedPin);
	}
	else if (IsTraitTypePin(ChangedPin))
	{
		OnTraitTypePinChanged(ChangedPin);
	}
	else if (IsFlagmarkPin(ChangedPin))
	{
		OnFlagmarkPinChanged(ChangedPin);
	}
}

UScriptStruct* UBPNode_MakeFingerprint::GetTraitTypePinType(UEdGraphPin *Pin) const
{
	check(Pin);

	UScriptStruct* Type = nullptr;
	if (Pin != nullptr)
	{
		if (Pin->LinkedTo.Num() == 0)
		{
			if (Pin->DefaultObject)
			{
				Type = Cast<UScriptStruct>(Pin->DefaultObject);
			}
		}
		else if (auto LinkedPin = Pin->LinkedTo[0])
		{
			Type = Cast<UScriptStruct>(LinkedPin->DefaultObject);
		}
	}
	return Type;
}

UClass* UBPNode_MakeFingerprint::GetDetailClassPinClass(UEdGraphPin *Pin) const
{
	check(Pin);

	UClass* Class = UDetail::StaticClass();
	if (Pin != nullptr)
	{
		if (Pin->LinkedTo.Num() == 0)
		{
			if (Pin->DefaultObject != nullptr)
			{
				Class = Cast<UClass>(Pin->DefaultObject);
			}
		}
		else if (auto LinkedPin = Pin->LinkedTo[0])
		{
			Class = Cast<UClass>(LinkedPin->DefaultObject);
		}
	}
	if (Class == nullptr)
	{
		Class = UDetail::StaticClass();
	}
	return Class;
}

UEdGraphPin*
UBPNode_MakeFingerprint::GetDetailClassPinGivenIndex(const int32 Index) const
{
	return FindPinChecked(MakeDetailClassPinName(Index), EGPD_Input);
}

UEdGraphPin*
UBPNode_MakeFingerprint::GetTraitTypePinGivenIndex(const int32 Index) const
{
	return FindPinChecked(MakeTraitTypePinName(Index), EGPD_Input);
}

UEdGraphPin*
UBPNode_MakeFingerprint::GetFingerprintPin() const
{
	return FindPinChecked(FingerprintPinName, EGPD_Output);
}

void UBPNode_MakeFingerprint::OnFlagmarkPinChanged(UEdGraphPin* ChangedPin)
{
	Modify();

	UpdateFlagmarkPinUI(ChangedPin);

	if (auto Graph = GetGraph())
	{
		Graph->NotifyGraphChanged();
	}
}

void UBPNode_MakeFingerprint::OnTraitTypePinChanged(UEdGraphPin* ChangedPin)
{
	Modify();

	const auto Index = FindTraitTypePinIndex(ChangedPin);

	if (Index == -1)
		return;

	UpdateTraitTypePinUI(ChangedPin);

	if (auto Graph = GetGraph())
	{
		Graph->NotifyGraphChanged();
	}
}

void UBPNode_MakeFingerprint::OnDetailClassPinChanged(UEdGraphPin *ChangedPin)
{
	Modify();

	const auto Index = FindDetailClassPinIndex(ChangedPin);

	if (Index == -1)
		return;

	UpdateDetailClassPinUI(ChangedPin);

	if (auto Graph = GetGraph())
	{
		Graph->NotifyGraphChanged();
	}
}

#undef LOCTEXT_NAMESPACE
