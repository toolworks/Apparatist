/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_GenericTraitFunction.cpp
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

#include "BPNode_GenericTraitFunction.h"

#include "KismetCompiler.h"
#include "ScopedTransaction.h"
#include "ToolMenusEditor.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_FunctionEntry.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "ApparatusFunctionLibrary.h"
#include "ApparatusUncookedUtils.h"

#define LOCTEXT_NAMESPACE "UBPNode_GenericTraitFunction"


FName
UBPNode_GenericTraitFunction::GetTraitTypePinName() const
{
	auto* const Prop = FindTraitTypeProperty();
	if (!Prop)
	{
		return NAME_None;
	}
	return Prop->NamePrivate;
}

FName
UBPNode_GenericTraitFunction::GetTraitDataPinName() const
{
	auto* const Prop = FindTraitDataProperty();
	if (!Prop)
	{
		return NAME_None;
	}
	return Prop->NamePrivate;
}

FName
UBPNode_GenericTraitFunction::GetSubjectPinName() const
{
	auto* const Prop = FindSubjectProperty();
	if (!Prop)
	{
		return NAME_None;
	}
	return Prop->NamePrivate;
}

FName
UBPNode_GenericTraitFunction::GetSubjectivePinName() const
{
	auto* const Prop = FindSubjectiveProperty();
	if (!Prop)
	{
		return NAME_None;
	}
	return Prop->NamePrivate;
}

UBPNode_GenericTraitFunction::UBPNode_GenericTraitFunction(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{}

bool
UBPNode_GenericTraitFunction::IsTraitPin(UEdGraphPin* Pin,
										 const bool   bExact/*=true*/) const
{
	if (!Pin) return false;
	if (bExact && Pin->PinName != GetTraitDataPinName()) return false;
	return (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct || 
		    Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard);
}

bool
UBPNode_GenericTraitFunction::IsTraitTypePin(UEdGraphPin* Pin,
											 const bool   bExact/*=true*/) const
{
	if (!Pin) return false;
	if (bExact && Pin->PinName != GetTraitTypePinName()) return false;
	return (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object) &&
		   (Pin->PinType.PinSubCategoryObject == UScriptStruct::StaticClass());
}

UEdGraphPin*
UBPNode_GenericTraitFunction::GetTraitDataPin() const
{
	return FindPinChecked(GetTraitDataPinName(), GetTraitPinDirection());
}

UScriptStruct*
UBPNode_GenericTraitFunction::GetTraitType(UEdGraphPin* TraitPin,
										   const bool   bRecursive) const
{
	check(TraitPin);

	UScriptStruct* Type = nullptr;
	if (IsTraitTypePin(TraitPin, bRecursive))
	{
		if (bRecursive && (TraitPin->LinkedTo.Num() > 0)) // HasAnyConnections() is too smart.
		{
			auto* ConnectedPin = TraitPin->LinkedTo[0];
			if (ConnectedPin)
			{
				Type = GetTraitType(ConnectedPin, false);
			}
		}
		if (!Type)
		{
			Type = Cast<UScriptStruct>(TraitPin->DefaultObject);
		}
	}
	else if (IsTraitPin(TraitPin, bRecursive))
	{
		if (bRecursive && TraitPin->LinkedTo.Num() > 0) // HasAnyConnections() is too smart.
		{
			auto* ConnectedPin = TraitPin->LinkedTo[0];
			if (ConnectedPin)
			{
				Type = GetTraitType(ConnectedPin, false);
			}
		}
		if (!Type && (TraitPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct))
		{
			Type = Cast<UScriptStruct>(TraitPin->PinType.PinSubCategoryObject);
		}
	}
	return Type;
}

UScriptStruct*
UBPNode_GenericTraitFunction::GetTraitType() const
{
	UEdGraphPin* Pin = nullptr;
	Pin = GetTraitDataPin();
	if (Pin->LinkedTo.Num() == 0) // HasAnyConnections() is too smart.
	{
		Pin = GetTraitTypePin();
	}
	return GetTraitType(Pin);
}

UEdGraphPin*
UBPNode_GenericTraitFunction::GetSubjectPin() const
{
	check(!IsSubjectiveBased());
	return FindPin(GetSubjectPinName(), EGPD_Input);
}

UEdGraphPin*
UBPNode_GenericTraitFunction::GetTraitTypePin() const
{
	return FindPinChecked(GetTraitTypePinName(), EGPD_Input);
}

FStructProperty*
UBPNode_GenericTraitFunction::FindTraitDataProperty() const
{
	if (TraitDataPropertyCache) return TraitDataPropertyCache;
	UFunction* const Function = GetTargetFunction();
	return TraitDataPropertyCache = FindParameterOfType(Function, FGenericStruct::StaticStruct());
}

FObjectProperty*
UBPNode_GenericTraitFunction::FindTraitTypeProperty() const
{
	if (TraitTypePropertyCache) return TraitTypePropertyCache;
	UFunction* const Function = GetTargetFunction();
	auto* Prop = FindParameterOfClass(Function, UScriptStruct::StaticClass());
	return TraitTypePropertyCache = CastFieldChecked<FObjectProperty>(Prop);
}

FStructProperty*
UBPNode_GenericTraitFunction::FindSubjectProperty() const
{
	if (SubjectPropertyCache) return SubjectPropertyCache;
	UFunction* const Function = GetTargetFunction();
	return SubjectPropertyCache = FindParameterOfType(Function, FSubjectHandle::StaticStruct());
}

FInterfaceProperty*
UBPNode_GenericTraitFunction::FindSubjectiveProperty() const
{
	if (SubjectivePropertyCache) return SubjectivePropertyCache;
	UFunction* const Function = GetTargetFunction();
	auto* SubjectiveProp = FindParameterOfClass(Function, USubjective::StaticClass());
	return SubjectivePropertyCache = CastField<FInterfaceProperty>(SubjectiveProp);
}

bool
UBPNode_GenericTraitFunction::IsSubjectiveBased() const
{
	return FindSubjectiveProperty() != nullptr;
}

bool
UBPNode_GenericTraitFunction::IsOutputting() const
{
	auto* Property = FindTraitDataProperty();
	check(Property);
	return (Property->PropertyFlags & CPF_ConstParm) == CPF_None;
}

UEdGraphPin*
UBPNode_GenericTraitFunction::GetSubjectivePin() const
{
	check(IsSubjectiveBased());
	return FindPinChecked(GetSubjectivePinName(), EGPD_Input);
}

UEdGraphPin*
UBPNode_GenericTraitFunction::GetExecPin() const
{
	check(!IsNodePure());
	return FindPinChecked(UEdGraphSchema_K2::PN_Execute, EGPD_Input);
}

void
UBPNode_GenericTraitFunction::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
}

UK2Node::ERedirectType
UBPNode_GenericTraitFunction::DoPinsMatchForReconstruction(
	const UEdGraphPin* NewPin, int32 NewPinIndex,
	const UEdGraphPin* OldPin, int32 OldPinIndex) const
{
	ERedirectType Result = Super::DoPinsMatchForReconstruction(
								NewPin, NewPinIndex,
								OldPin, OldPinIndex);
	
	if (OldPin->Direction != NewPin->Direction) return Result;
	if (OldPin->PinName == NewPin->PinName) return Result;
	if (OldPin->SubPins.Num() > 0) return Result;

	// TODO: Remove backward compatibility.
	if (OldPin->PinName == FName(TEXT("Subject")) &&
		NewPin->PinName == GetSubjectPinName())
	{
		return ERedirectType::ERedirectType_Name;
	}
	
	return Result;
}

void
UBPNode_GenericTraitFunction::EarlyValidation(
	class FCompilerResultsLog& MessageLog) const
{
	UScriptStruct* Type = GetTraitType();

	if (!Type)
	{
		MessageLog.Error(
			*LOCTEXT("InvalidTraitTypePin",
						"Invalid or missing trait type @@ "
						"in @@").ToString(),
			GetTraitTypePin(), this);
	}

	if (IsSubjectiveBased())
	{
		auto* SubjectivePin = GetSubjectivePin();
		if (!SubjectivePin->HasAnyConnections())
		{
			MessageLog.Error(
				*LOCTEXT("InvalidSubjectivePin",
						 "The mandatory subjective pin @@ "
						 "is not connected in @@").ToString(),
				SubjectivePin, this);
		}
	}
	else
	{
		auto* SubjectPin = GetSubjectPin();
		if ((SubjectPin != nullptr) && !SubjectPin->LinkedTo.Num())
		{
			MessageLog.Error(
				*LOCTEXT("InvalidSubjectPin",
						 "The mandatory subject pin @@ "
						 "is not connected in @@").ToString(),
				SubjectPin, this);
		}
	}

	Super::EarlyValidation(MessageLog);
}

void
UBPNode_GenericTraitFunction::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);

	UFunction* Function = GetTargetFunction();
	if (Function)
	{
		if (Function->HasMetaData(FBlueprintMetadata::MD_UnsafeForConstructionScripts))
		{
			const auto* const Graph = GetGraph();
			bool bInConstructionScript = UEdGraphSchema_K2::IsConstructionScript(Graph);

			if (!bInConstructionScript)
			{
				// Safety for clones...
				TArray<const UK2Node_FunctionEntry*> EntryPoints;
				Graph->GetNodesOfClass(EntryPoints);

				if (EntryPoints.Num() == 1)
				{
					auto const* const Node = EntryPoints[0];
					if (Node)
					{
						auto* const SignatureFunction = 
							Node->FunctionReference.ResolveMember<UFunction>(
								Node->GetBlueprintClassFromNode());
						bInConstructionScript =
							SignatureFunction && 
							(SignatureFunction->GetFName() == UEdGraphSchema_K2::FN_UserConstructionScript);
					}
				}
			}

			if (bInConstructionScript)
			{
				MessageLog.Warning(*LOCTEXT("UnsafeDuringConstruction",
											"Function '@@' is unsafe to call in a construction script.").ToString(), this);
			}
		}
	}
}

void UBPNode_GenericTraitFunction::UpdateTraitPin(UScriptStruct* TraitType)
{
	auto TraitPin = GetTraitDataPin();
	if (TraitType)
	{
		if (TraitPin->PinType.PinSubCategoryObject != TraitType)
		{
			if (TraitPin->PinType.PinSubCategoryObject != nullptr)
			{
				auto* Schema = GetSchema();
				if (Schema)
				{
					Schema->RecombinePin(TraitPin);
				}
				TraitPin->BreakAllPinLinks();
			}
			TraitPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
			TraitPin->PinType.PinSubCategoryObject = TraitType;
			UpdateTraitDataPinUI(TraitPin);
		}
	}
	else
	{
		if (TraitPin->PinType.PinSubCategoryObject != nullptr)
		{
			auto* Schema = GetSchema();
			if (Schema)
			{
				Schema->RecombinePin(TraitPin);
			}
			TraitPin->BreakAllPinLinks();
			TraitPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
			TraitPin->PinType.PinSubCategoryObject = nullptr;
			UpdateTraitDataPinUI(TraitPin);
		}
	}
}

void UBPNode_GenericTraitFunction::UpdateTraitPin()
{
	auto TraitPin = GetTraitDataPin();
	UScriptStruct* TraitType = GetTraitType();
	UpdateTraitPin(TraitType);
}

void
UBPNode_GenericTraitFunction::UpdateTraitDataPinUI(UEdGraphPin* TraitPin)
{
	check(TraitPin);
	check(IsTraitPin(TraitPin));
	auto Schema = GetSchema();
	check(Schema);

	auto* TraitDataProp = FindTraitDataProperty();
	check(TraitDataProp);
	
	const FString& DefaultDisplayName = TraitDataProp->GetMetaData(FBlueprintMetadata::MD_DisplayName);
	UScriptStruct* TraitType = GetTraitType();
	if (TraitType)
	{
		TraitPin->PinFriendlyName = TraitType->GetDisplayNameText();
	}
	else
	{
		TraitPin->PinFriendlyName = FText::FromString(DefaultDisplayName);
	}

	Schema->ConstructBasicPinTooltip(
		*TraitPin,
		LOCTEXT("TraitDataPinTooltip",
				"The trait in question."),
		TraitPin->PinToolTip);
}

void
UBPNode_GenericTraitFunction::UpdateTraitTypePinUI(UEdGraphPin* TraitTypePin)
{
	check(TraitTypePin);
	check(IsTraitTypePin(TraitTypePin));
	auto Schema = GetSchema();
	check(Schema);

	auto* TraitTypeProp = FindTraitTypeProperty();
	check(TraitTypeProp);
	
	const FString& DefaultDisplayName = TraitTypeProp->GetMetaData(FBlueprintMetadata::MD_DisplayName);
	UScriptStruct* TraitType = GetTraitType();
	if (TraitType)
	{
		// Replace with a shortcut, since the drop-down is already descriptive.
		TraitTypePin->PinFriendlyName = FText::FromString(TEXT(TRAIT_SHORTCUT_STR));
	}
	else
	{
		TraitTypePin->PinFriendlyName = FText::FromString(DefaultDisplayName);
	}

	Schema->ConstructBasicPinTooltip(
		*TraitTypePin,
		LOCTEXT("TraitTypePinTooltip",
				"The type of the trait."),
		TraitTypePin->PinToolTip);
}

void
UBPNode_GenericTraitFunction::PinConnectionListChanged(UEdGraphPin* ChangedPin)
{
	if (IsTraitPin(ChangedPin))
	{
		UScriptStruct* TraitType = GetTraitType(ChangedPin);
		if (TraitType)
		{
			// Be guided by the trait data pin...
			auto* TraitTypePin = GetTraitTypePin();
			TraitTypePin->DefaultObject = TraitType;
			UpdateTraitTypePinUI(TraitTypePin);
		}
	}
	UpdateTraitPin();
}

void
UBPNode_GenericTraitFunction::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
	Super::PinDefaultValueChanged(ChangedPin);
	if (IsTraitTypePin(ChangedPin))
	{
		UpdateTraitTypePinUI(ChangedPin);

		UScriptStruct* TraitType = GetTraitType(ChangedPin);
		auto* TraitPin = GetTraitDataPin();
		UScriptStruct* FactualTraitType = GetTraitType(TraitPin);
		if (FactualTraitType != TraitType)
		{
			if (!FactualTraitType)
			{
				auto* Schema = GetSchema();
				if (Schema)
				{
					Schema->RecombinePin(TraitPin);
				}
				TraitPin->BreakAllPinLinks();
			}
			UpdateTraitPin(TraitType);
		}
	}
}

void
UBPNode_GenericTraitFunction::ReallocatePinsDuringReconstruction(
	TArray<UEdGraphPin*>& OldPins)
{
	AllocateDefaultPins();

	UScriptStruct* TraitType = nullptr;

	// Search for trait type within the type pin...
	for (auto* OldPin : OldPins)
	{
		if (IsTraitTypePin(OldPin))
		{
			TraitType = GetTraitType(OldPin);
			break;
		}
	}

	if (!TraitType)
	{
		// Search for the trait type within the data pin...
		for (auto* OldPin : OldPins)
		{
			if (IsTraitPin(OldPin))
			{
				TraitType = GetTraitType(OldPin);
				break;
			}
		}
	}

	UEdGraphPin* TraitPin = GetTraitDataPin();
	if (TraitType)
	{
		// The type was reconstructed so set it:
		if (TraitPin->PinType.PinSubCategoryObject != TraitType)
		{
			TraitPin->PinType.PinCategory          = UEdGraphSchema_K2::PC_Struct;
			TraitPin->PinType.PinSubCategoryObject = TraitType;
		}
		auto* TraitTypePin = GetTraitTypePin();
		TraitTypePin->DefaultObject = TraitType;
		UpdateTraitTypePinUI(TraitTypePin);
	}
	UpdateTraitDataPinUI(TraitPin);

	RestoreSplitPins(OldPins);
}

void
UBPNode_GenericTraitFunction::PostReconstructNode()
{
	Super::PostReconstructNode();

	UScriptStruct* TraitType = GetTraitType();
	UEdGraphPin*   TraitPin  = GetTraitDataPin();
	if (TraitType)
	{
		TraitPin->PinType.PinCategory          = UEdGraphSchema_K2::PC_Struct;
		TraitPin->PinType.PinSubCategoryObject = TraitType;
	}
	else
	{
		TraitPin->PinType.PinCategory          = UEdGraphSchema_K2::PC_Wildcard;
		TraitPin->PinType.PinSubCategoryObject = nullptr;
	}
	UpdateTraitDataPinUI(TraitPin);
}

void
UBPNode_GenericTraitFunction::GetMenuActions(
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

void
UBPNode_GenericTraitFunction::ExpandNode(
	FKismetCompilerContext& CompilerContext,
	UEdGraph*               SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	auto* DstTraitTypePin = GetTraitTypePin();
	UScriptStruct* TraitType = GetTraitType();
	DstTraitTypePin->DefaultObject = TraitType;
}

#undef LOCTEXT_NAMESPACE
