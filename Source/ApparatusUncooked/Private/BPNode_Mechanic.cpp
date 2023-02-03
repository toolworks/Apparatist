/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_Mechanic.cpp
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

#include "BPNode_Mechanic.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Components/ActorComponent.h"
#include "EdGraphSchema_K2_Actions.h"
#include "EditorCategoryUtils.h"
#include "K2Node_TemporaryVariable.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_CallFunction.h"
#include "K2Node_DynamicCast.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_MacroInstance.h"
#include "K2Node_MakeArray.h"
#include "K2Node_PureAssignmentStatement.h"
#include "K2Node_BreakStruct.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "ToolMenusEditor.h"
#include "ScopedTransaction.h"

#include "ApparatusFunctionLibrary.h"
#include "ApparatusRuntime.h"
#include "ApparatusUncookedUtils.h"
#include "Detail.h"
#include "Machine.h"
#include "SubjectiveActorComponent.h"
#include "MechanicalGameModeBase.h"
#include "MechanicalActor.h"

#define LOCTEXT_NAMESPACE "UBPNode_Mechanic"

const FString UBPNode_Mechanic::DetailPinNamePrefix(TEXT("Detail_"));

const FString UBPNode_Mechanic::TraitPinNamePrefix(TEXT("Trait_"));

const FName UBPNode_Mechanic::MechanismPinName(TEXT("Mechanism"));

const FName UBPNode_Mechanic::EvaluationBodyPinName(TEXT("EvaluationBody"));

const FName UBPNode_Mechanic::OperatePinName(TEXT("Operate"));

const FName UBPNode_Mechanic::CompletedPinName(TEXT("Completed"));

const FName UBPNode_Mechanic::SubjectPinName(TEXT("Subject"));

const FName UBPNode_Mechanic::SubjectivePinName(TEXT("Subjective"));

bool UBPNode_Mechanic::IsNodePure() const { return false; }

FText UBPNode_Mechanic::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (IsBooting())
	{
		return NSLOCTEXT(LOCTEXT_NAMESPACE,
						 "BootingTitle",
						 "Booting Mechanic");
	}
	else if (IsSteady())
	{
		return NSLOCTEXT(LOCTEXT_NAMESPACE,
						 "SteadyTitle",
						 "Steady Mechanic");
	}
	else
	{
		return NSLOCTEXT(LOCTEXT_NAMESPACE,
						 "Title",
						 "Mechanic");
	}
}

FText UBPNode_Mechanic::GetTooltipText() const
{
	if (IsBooting())
	{
		return LOCTEXT("BootTooltipText",
						 "Boot all of the matching halted subjects.");
	}
	else
	{
		return LOCTEXT("TooltipText",
						 "Process all of the matching subjects.");
	}
}

bool UBPNode_Mechanic::IsConnectedToEvent(const FName EventName) const
{
	auto ExecPin = GetExecPin();
	if (!ExecPin)
		return false;
	if (!ExecPin->LinkedTo.Num())
		return false;

	TSet<UEdGraphNode*> TestedNodes; // Remember these to lessen infinite loops.
	const int32 MaxIterations = 16; // May still be an infinite cycle there.
	for (int32 i = 0; ExecPin && (i < MaxIterations); ++i)
	{
		if (!ExecPin->LinkedTo.Num())
		{
			return false;
		}

		for (auto Link : ExecPin->LinkedTo)
		{
			auto Node = Link->GetOwningNodeUnchecked();
			if (!Node) continue;
			auto EventNode = Cast<UK2Node_Event>(Node);
			if (!EventNode) continue;

			if (EventNode->EventReference.GetMemberName() == EventName)
			{
				return true;
			}
		}

		{
			const auto ExecPinSave = ExecPin;
			ExecPin = nullptr;
			for (auto Link : ExecPinSave->LinkedTo)
			{
				auto Node = Link->GetOwningNodeUnchecked();
				if (!Node) continue;
				if (TestedNodes.Contains(Node)) continue;
				ExecPin = Node->FindPin(UEdGraphSchema_K2::PN_Execute);
				if (!ExecPin) continue;
			}
		}
	}
	return false;
}

bool UBPNode_Mechanic::IsBooting() const
{
	static const FName BootName =
		GET_FUNCTION_NAME_CHECKED(IMechanical, ReceiveBoot);

	return IsConnectedToEvent(BootName);
}

bool UBPNode_Mechanic::IsSteady() const
{
	static const FName SteadyName =
		GET_FUNCTION_NAME_CHECKED(IMechanical, ReceiveSteadyTick);

	return IsConnectedToEvent(SteadyName);
}

bool UBPNode_Mechanic::SyncPinNames()
{
	bool bChanged = Super::SyncPinNames();;

	int32 DetailIndex = 0;
	int32 TraitIndex = 0;

	for (UEdGraphPin *Pin : Pins)
	{
		if (IsDetailPin(Pin))
		{
			const FName NewName = MakeDetailPinName(DetailIndex++);
			if (Pin->PinName != NewName)
			{
				Pin->PinName = NewName;
				bChanged = true;
			}
		}
		else if (IsTraitPin(Pin)) 
		{
			const FName NewName = MakeTraitPinName(TraitIndex++);
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

UClass* UBPNode_Mechanic::GetDetailPinClass(UEdGraphPin* Pin) const
{
	if (IsDetailClassPin(Pin))
	{
		return GetDetailClassPinClass(Pin);
	}
	else if (IsDetailPin(Pin))
	{
		const int32 Index = IndexOfDetailPin(Pin);
		return GetDetailClassPinClass(GetDetailClassPinGivenIndex(Index));
	}
	return nullptr;
}

UScriptStruct* UBPNode_Mechanic::GetTraitPinType(UEdGraphPin* Pin) const
{
	if (IsTraitTypePin(Pin))
	{
		return GetTraitTypePinType(Pin);
	}
	else if (IsTraitPin(Pin))
	{
		const int32 Index = IndexOfTraitPin(Pin);
		return GetTraitTypePinType(GetTraitTypePinGivenIndex(Index));
	}
	return nullptr;
}

void UBPNode_Mechanic::UpdateDetailPinUI(UEdGraphPin *DetailPin)
{
	checkSlow(IsDetailPin(DetailPin));
	auto Schema = GetSchema();
	check(Schema);
	Schema->ConstructBasicPinTooltip(
		*DetailPin,
		LOCTEXT("DetailPinTooltip",
				"The macthing detail instance of the subject."),
		DetailPin->PinToolTip);
	auto Class = GetDetailPinClass(DetailPin);
	check(Class);
	DetailPin->PinFriendlyName = Class->GetDisplayNameText();
}

void UBPNode_Mechanic::UpdateTraitPinUI(UEdGraphPin *TraitPin)
{
	checkSlow(IsTraitPin(TraitPin));
	auto Schema = GetSchema();
	check(Schema);
	Schema->ConstructBasicPinTooltip(
		*TraitPin,
		LOCTEXT("TraitPinTooltip",
				"The macthing trait instance of the subject."),
		TraitPin->PinToolTip);
	UScriptStruct* Type = GetTraitPinType(TraitPin);
	if (Type)
	{
		TraitPin->SafeSetHidden(false);
		TraitPin->PinFriendlyName = Type->GetDisplayNameText();
	}
	else
	{
		TraitPin->SafeSetHidden(true);
		TraitPin->PinFriendlyName = FText::GetEmpty();
	}
}

UK2Node::ERedirectType
UBPNode_Mechanic::DoPinsMatchForReconstruction(
	const UEdGraphPin* NewPin, int32 NewPinIndex,
	const UEdGraphPin* OldPin, int32 OldPinIndex) const
{
	ERedirectType Result = Super::DoPinsMatchForReconstruction(
		NewPin, NewPinIndex, OldPin, OldPinIndex);
	if (NewPin->PinName == OperatePinName && OldPin->PinName == EvaluationBodyPinName)
	{
		return ERedirectType::ERedirectType_Name;
	}
	return Result;
}

void UBPNode_Mechanic::MigrateMissingTypes(TArray<UEdGraphPin*>& OldPins)
{
	for (UEdGraphPin* OldPin : OldPins)
	{
		if (IsTraitTypePin(OldPin))
		{
			const int32 Index = FindTraitTypePinIndex(OldPin);
			if (Index != -1)
			{
				UEdGraphPin* NewTraitTypePin = GetTraitTypePinGivenIndex(Index);
				UScriptStruct* NewTraitType = GetTraitTypePinType(NewTraitTypePin);
				UScriptStruct* OldTraitType = GetTraitTypePinType(OldPin);
				if (!NewTraitType && OldTraitType)
				{
					// This is very important during the Editor loading.
					// Otherwise, the trait type may be semi-loaded and
					// fail to split correctly:
					OldTraitType->RecursivelyPreload();

					NewTraitTypePin->DefaultObject = OldTraitType;
					UEdGraphPin* NewTraitPin = GetTraitPinGivenIndex(Index);
					NewTraitPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
					NewTraitPin->PinType.PinSubCategoryObject = OldTraitType;
					UpdateTraitPinUI(NewTraitPin);
				}
			}
		}
	}
}

void
UBPNode_Mechanic::ReallocatePinsDuringReconstruction(
	TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);
	MigrateMissingTypes(OldPins);
	RestoreSplitPins(OldPins);
}

void UBPNode_Mechanic::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	UEdGraphPin* ExecPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec,
									 UEdGraphSchema_K2::PN_Execute);

	GetFilterPin()->SafeSetHidden(true);

	const UEdGraphSchema* Schema = GetSchema();
	check(Schema);

	UEdGraphPin* MechanismPin = CreatePin(
		EGPD_Input, UEdGraphSchema_K2::PC_Object,
		AMechanism::StaticClass(), MechanismPinName);
	MechanismPin->bAdvancedView = true;

	if (AdvancedPinDisplay == ENodeAdvancedPins::NoPins)
	{
		AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
	}

	Schema->ConstructBasicPinTooltip(
		*MechanismPin,
		LOCTEXT("MechanismPinTooltip",
				"The explicit mechanism to use. If not specified, will be using the world's default mechanism."),
		MechanismPin->PinToolTip);

	UEdGraphPin* OperatePin = CreatePin(
		EGPD_Output, UEdGraphSchema_K2::PC_Exec, OperatePinName);

	UEdGraphPin* CompletedPin =
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, CompletedPinName);
	UEdGraphPin* SubjectPin =
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct,
				  FSubjectHandle::StaticStruct(), SubjectPinName);
	UEdGraphPin* SubjectivePin =
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Interface,
				  USubjective::StaticClass(), SubjectivePinName);

	Schema->ConstructBasicPinTooltip(
		*SubjectivePin,
		LOCTEXT("SubjectPinTooltip",
				"The current subjective to evaluate in the body."),
		SubjectivePin->PinToolTip);

	if (HasFlagmark())
	{
		auto* const FlagmarkPin = GetFlagmarkPin();
		FlagmarkPin->DefaultValue = FString::FromInt(FM_None); 
	}

	for (int32 i = 0; i < TraitsCount; ++i)
	{
		const FName NewTraitPinName = MakeTraitPinName(i);
		auto TraitTypePin = GetTraitTypePinGivenIndex(i);
		check(TraitTypePin);
		UScriptStruct* TraitType = GetTraitTypePinType(TraitTypePin);
		UEdGraphPin* TraitPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct,
										  /*SubCategory=*/TraitType,
										  NewTraitPinName);
		UpdateTraitPinUI(TraitPin);
	}

	for (int32 i = 0; i < DetailsCount; ++i)
	{
		const FName NewDetailPinName = MakeDetailPinName(i);
		auto DetailClassPin = GetDetailClassPinGivenIndex(i);
		check(DetailClassPin);
		auto DetailClass = GetDetailClassPinClass(DetailClassPin);
		check(DetailClass);

		auto DetailPin =
			CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object,
					  /*SubCategory=*/DetailClass, NewDetailPinName);

		UpdateDetailPinUI(DetailPin);
	}
}

FLinearColor UBPNode_Mechanic::GetNodeTitleColor() const
{
	return Super::GetNodeTitleColor();
}

FText UBPNode_Mechanic::GetMenuCategory() const
{
	return FText::FromString("Apparatus|Evaluation");
}

void UBPNode_Mechanic::GetMenuActions(
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

void UBPNode_Mechanic::EarlyValidation(
	class FCompilerResultsLog &MessageLog) const
{
	Super::EarlyValidation(MessageLog);
}

bool
UBPNode_Mechanic::IsBeltBasedIterating() const
{
	// Only the details should be considered here,
	// since the excluded ones may actually be processed
	// within chunk-based iterating.
	return DetailsNum() > 0;
}

bool
UBPNode_Mechanic::IsChunkBasedIterating() const
{
	return !IsBeltBasedIterating();
}

void
UBPNode_Mechanic::ExpandNode(class FKismetCompilerContext& CompilerContext,
							 UEdGraph*                     SourceGraph)
{
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	check(Schema);

	FName EnchainName = 
		GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, MechanismEnchain);
	static const FName BeginOrAdvanceChainName =
		GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, BeginOrAdvanceChain);
	static const FName GetChainSubjectName =
		GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, GetChainSubject);
	static const FName GetChainSubjectiveName =
		GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, GetChainSubjective);
	static const FName GetChainTraitName =
		GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, GetChainTrait);
	static const FName GetChainTraitHintedName =
		GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, GetChainTraitHinted);
	static const FName GetChainDetailName =
		GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, GetChainDetail);
	static const FName GetChainDetailHintedName =
		GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, GetChainDetailHinted);

	// Set the boot filter used...
	if (IsBooting())
	{
		EnchainName = 
			GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, MechanismEnchainHalted);
	}
	else
	{
		EnchainName = 
			GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, MechanismEnchainBooted);
	}

#pragma region Mechanism Proxy

	UEdGraphPin* MechanismPin = nullptr;
	UK2Node_AssignmentStatement* AssignMechanismNode = nullptr;
	if (GetMechanismPin()->LinkedTo.Num() > 0)
	{
		const auto MechanismNode = CompilerContext.SpawnInternalVariable(
			this, UEdGraphSchema_K2::PC_Object, NAME_None, AMechanism::StaticClass());
		
		AssignMechanismNode = CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(this, SourceGraph);
		AssignMechanismNode->AllocateDefaultPins();
		MOVE_LINK(this->GetExecPin(), AssignMechanismNode->GetExecPin());
		MOVE_LINK(this->GetMechanismPin(), AssignMechanismNode->GetValuePin());
		MAKE_LINK(MechanismNode->GetVariablePin(), AssignMechanismNode->GetVariablePin());
		MechanismPin = MechanismNode->GetVariablePin();
	}

#pragma endregion Mechanism Proxy

	const auto ActiveFilterPin = ExpandToFilterPin(CompilerContext, SourceGraph);
	check(ActiveFilterPin);

#pragma region Enchain

	const auto EnchainNode =
		CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(
			this, SourceGraph);
	EnchainNode->FunctionReference.SetExternalMember(
		EnchainName, UApparatusFunctionLibrary::StaticClass());
	EnchainNode->AllocateDefaultPins();

	// Connect filter to enchain with:
	MAKE_LINK(ActiveFilterPin,
			  EnchainNode->FindPinChecked(TEXT("Filter"), EGPD_Input));
	if (AssignMechanismNode)
	{
		MAKE_LINK(AssignMechanismNode->GetThenPin(), EnchainNode->GetExecPin());
	}
	else
	{
		MOVE_LINK(this->GetExecPin(), EnchainNode->GetExecPin());
	}
	const auto ChainPin = EnchainNode->FindPinChecked(TEXT("OutChainId"), EGPD_Output);
	if (MechanismPin)
	{
		MAKE_LINK(MechanismPin, EnchainNode->FindPinChecked(TEXT("Mechanism"), EGPD_Input));
	}

#pragma endregion Enchain

#pragma region Begin or Advance

	const auto BeginOrAdvanceChainNode =
		CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(
			this, SourceGraph);
	BeginOrAdvanceChainNode->FunctionReference.SetExternalMember(
		BeginOrAdvanceChainName, UApparatusFunctionLibrary::StaticClass());
	BeginOrAdvanceChainNode->AllocateDefaultPins();
	MAKE_LINK(ChainPin, BeginOrAdvanceChainNode->FindPinChecked(TEXT("ChainId"), EGPD_Input));
	MAKE_LINK(EnchainNode->GetThenPin(), BeginOrAdvanceChainNode->GetExecPin());
	if (MechanismPin)
	{
		MAKE_LINK(MechanismPin, BeginOrAdvanceChainNode->FindPinChecked(TEXT("Mechanism"), EGPD_Input));
	}

#pragma endregion Begin or Advance

#pragma region Iterating Continuation/Completion State Branch

	const auto IteratingBranchNode = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(
		this, SourceGraph);
	IteratingBranchNode->AllocateDefaultPins();
	MAKE_LINK(BeginOrAdvanceChainNode->GetThenPin(), IteratingBranchNode->GetExecPin());
	MAKE_LINK(BeginOrAdvanceChainNode->GetReturnValuePin(), IteratingBranchNode->GetConditionPin());
	MOVE_LINK(GetCompletedPin(), IteratingBranchNode->GetElsePin());

#pragma endregion Iterating Continuation/Completion State Branch

#pragma region Evaluating Sequence

	const auto Sequence =
		CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(
			this, SourceGraph);
	Sequence->AllocateDefaultPins();
	MAKE_LINK(IteratingBranchNode->GetThenPin(), Sequence->GetExecPin());
	MAKE_LINK(Sequence->GetThenPinGivenIndex(1), BeginOrAdvanceChainNode->GetExecPin());
	MOVE_LINK(GetEvaluationBodyPin(), Sequence->GetThenPinGivenIndex(0));

#pragma endregion Evaluating Sequence

	//+SUBJECT PIN
	const auto GetSubjectNode =
		CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(
			this, SourceGraph);
	GetSubjectNode->FunctionReference.SetExternalMember(
		GetChainSubjectName, UApparatusFunctionLibrary::StaticClass());
	GetSubjectNode->AllocateDefaultPins();
	MAKE_LINK(GetSubjectNode->FindPinChecked(TEXT("ChainId"), EGPD_Input),
			  ChainPin);
	MOVE_LINK(this->GetSubjectPin(), GetSubjectNode->GetReturnValuePin());
	if (MechanismPin)
	{
		MAKE_LINK(MechanismPin, GetSubjectNode->FindPinChecked(TEXT("Mechanism"), EGPD_Input));
	}
	//-SUBJECT PIN

	//+SUBJECTIVE PIN
	auto* GetSubjectiveNode =
		CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(
			this, SourceGraph);
	GetSubjectiveNode->FunctionReference.SetExternalMember(
		GetChainSubjectiveName, UApparatusFunctionLibrary::StaticClass());
	GetSubjectiveNode->AllocateDefaultPins();
	MAKE_LINK(GetSubjectiveNode->FindPinChecked(TEXT("ChainId"), EGPD_Input),
			  ChainPin);
	MOVE_LINK(GetSubjectivePin(), GetSubjectiveNode->GetReturnValuePin());
	if (MechanismPin)
	{
		MAKE_LINK(MechanismPin, GetSubjectiveNode->FindPinChecked(TEXT("Mechanism"), EGPD_Input));
	}
	//-SUBJECTIVE PIN

	const auto bBeltBasedIterating = IsBeltBasedIterating();
	for (int32 i = 0; i < DetailsCount; ++i)
	{
		const auto GetDetailNode =
			CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(
				this, SourceGraph);
		GetDetailNode->FunctionReference.SetExternalMember(
			bBeltBasedIterating ? GetChainDetailHintedName : GetChainDetailName,
			UApparatusFunctionLibrary::StaticClass());
		GetDetailNode->AllocateDefaultPins();
		MAKE_LINK(GetDetailNode->FindPinChecked(TEXT("ChainId"), EGPD_Input), ChainPin);
		if (bBeltBasedIterating)
		{
			GetDetailNode->FindPinChecked(TEXT("DetailIndexHint"), EGPD_Input)
				->DefaultValue = FString::FromInt(i);
		}

		// Set the class of the detail...
		auto DetailClassPin = GetDetailClassPinGivenIndex(i);
		auto DetailClass = GetDetailClassPinClass(DetailClassPin);
		auto DstDetailClassPin =
			GetDetailNode->FindPinChecked(TEXT("DetailClass"), EGPD_Input);
		DstDetailClassPin->DefaultObject = DetailClass;
		GetDetailNode->PostReconstructNode();

		MOVE_LINK(GetDetailPinGivenIndex(i), GetDetailNode->GetReturnValuePin());

		if (MechanismPin)
		{
			MAKE_LINK(MechanismPin, GetDetailNode->FindPinChecked(TEXT("Mechanism"), EGPD_Input));
		}
	}
	for (int32 i = 0; i < TraitsCount; ++i)
	{
		const auto GetTraitNode =
			CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(
				this, SourceGraph);
		GetTraitNode->FunctionReference.SetExternalMember(
			bBeltBasedIterating ? GetChainTraitName : GetChainTraitHintedName,
			UApparatusFunctionLibrary::StaticClass());
		GetTraitNode->AllocateDefaultPins();
		MAKE_LINK(GetTraitNode->FindPinChecked(TEXT("ChainId"), EGPD_Input), ChainPin);

		// Set the type of the trait...
		auto TraitTypePin = GetTraitTypePinGivenIndex(i);
		auto TraitType = GetTraitTypePinType(TraitTypePin);
		auto DstTraitTypePin =
			GetTraitNode->FindPinChecked(TEXT("TraitType"), EGPD_Input);
		DstTraitTypePin->DefaultObject = TraitType;
		if (!bBeltBasedIterating)
		{
			GetTraitNode->FindPinChecked(TEXT("TraitIndex"), EGPD_Input)
				->DefaultValue = FString::FromInt(i);
		}
		// Can't use it here: GetTraitNode->PostReconstructNode();
		MOVE_SPLIT_LINK(GetTraitPinGivenIndex(i), GetTraitNode->FindPinChecked(TEXT("OutTraitData"), EGPD_Output));

		if (MechanismPin)
		{
			MAKE_LINK(MechanismPin, GetTraitNode->FindPinChecked(TEXT("Mechanism"), EGPD_Input));
		}
	}

	BreakAllNodeLinks();
}

void UBPNode_Mechanic::PostReconstructNode()
{
	Super::PostReconstructNode();

	for (int32 i = 0; i < DetailsCount; ++i)
	{
		auto TypePin = GetDetailClassPinGivenIndex(i);

		auto Type = GetDetailClassPinClass(TypePin);
		check(Type);

		auto DetailPin = GetDetailPinGivenIndex(i);
		check(DetailPin);
		DetailPin->PinType.PinSubCategoryObject = Type;
		UpdateDetailPinUI(DetailPin);
	}

	for (int32 i = 0; i < TraitsCount; ++i)
	{
		auto TraitTypePin = GetTraitTypePinGivenIndex(i);

		auto Type = GetTraitTypePinType(TraitTypePin);

		auto TraitPin = GetTraitPinGivenIndex(i);
		check(TraitPin);
		TraitPin->PinType.PinSubCategoryObject = Type;
		UpdateTraitPinUI(TraitPin);
	}
}

FName UBPNode_Mechanic::MakeDetailPinName(int32 Index) const
{
	return *(DetailPinNamePrefix + FString::FormatAsNumber(Index));
}

FName UBPNode_Mechanic::MakeTraitPinName(int32 Index) const
{
	return *(TraitPinNamePrefix + FString::FormatAsNumber(Index));
}

void UBPNode_Mechanic::AddInputPin(UEdGraphPin*& AddedPin)
{
	Super::AddInputPin(AddedPin);
	if (!AddedPin) return;

	// Modify() is called by the parent's method.

	auto Schema = GetDefault<UEdGraphSchema_K2>();
	check(Schema);

	if (IsDetailClassPin(AddedPin))
	{
		const int32 NewPinIndex = FindDetailClassPinIndex(AddedPin);
		check(NewPinIndex >= 0);
		auto DetailPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object,
								   /*SubCategory=*/GetDetailClassPinClass(AddedPin),
								   MakeDetailPinName(NewPinIndex));
		Schema->SetPinAutogeneratedDefaultValueBasedOnType(DetailPin);
		UpdateDetailPinUI(DetailPin);
	}
	else if (IsTraitTypePin(AddedPin))
	{
		const int32 NewTraitIndex = FindTraitTypePinIndex(AddedPin);
		check(NewTraitIndex >= 0);
		auto TraitPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct,
								  /*SubCategory=*/GetTraitTypePinType(AddedPin),
								  MakeTraitPinName(NewTraitIndex));
		Schema->SetPinAutogeneratedDefaultValueBasedOnType(TraitPin);
		UpdateTraitPinUI(TraitPin);
	}

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

UEdGraphPin* UBPNode_Mechanic::AddDetailClassPin()
{
	UEdGraphPin* DetailClassPin = Super::AddDetailClassPin();
	if (!DetailClassPin) return nullptr;

	// Modify() is called by the parent's method.

	auto Schema = GetDefault<UEdGraphSchema_K2>();
	check(Schema);

	const int32 NewPinIndex = FindDetailClassPinIndex(DetailClassPin);
	check(NewPinIndex >= 0);
	auto DetailPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object,
							/*SubCategory=*/GetDetailClassPinClass(DetailClassPin),
							MakeDetailPinName(NewPinIndex));
	Schema->SetPinAutogeneratedDefaultValueBasedOnType(DetailPin);

	UpdateDetailPinUI(DetailPin);

	return DetailClassPin;
}

UEdGraphPin* UBPNode_Mechanic::AddTraitTypePin()
{
	UEdGraphPin* TraitTypePin = Super::AddTraitTypePin();
	if (!TraitTypePin) return nullptr;

	// Modify() is called by the parent's method.

	auto Schema = GetDefault<UEdGraphSchema_K2>();
	check(Schema);

	const int32 NewTraitIndex = FindTraitTypePinIndex(TraitTypePin);
	check(NewTraitIndex >= 0);
	auto TraitPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct,
							  /*SubCategory=*/GetTraitTypePinType(TraitTypePin),
							  MakeTraitPinName(NewTraitIndex));
	Schema->SetPinAutogeneratedDefaultValueBasedOnType(TraitPin);

	UpdateTraitPinUI(TraitPin);

	return TraitTypePin;
}

void UBPNode_Mechanic::RemoveDetailClassPin(const int32 Index)
{
	if (!DetailsCount)
		return;
	if (Index == -1)
		return;

	Super::RemoveDetailClassPin(Index);

	auto DetailPin = GetDetailPinGivenIndex(Index);
	check(DetailPin);

	DetailPin->BreakAllPinLinks();

	RemovePin(DetailPin);

	if (!SyncPinNames())
	{
		// Still mark the blueprint as changed.
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(
			GetBlueprint());
	}
}

void UBPNode_Mechanic::RemoveTraitTypePin(const int32 Index)
{
	if (!TraitsCount)
		return;
	if (Index == -1)
		return;

	Super::RemoveTraitTypePin(Index);

	auto TraitPin = GetTraitPinGivenIndex(Index);
	check(TraitPin);

	auto* Schema = GetSchema();
	if (Schema)
	{
		Schema->RecombinePin(TraitPin);
	}
	TraitPin->BreakAllPinLinks();

	RemovePin(TraitPin);

	if (!SyncPinNames())
	{
		// Still mark the blueprint as changed.
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(
			GetBlueprint());
	}
}

bool UBPNode_Mechanic::IsDetailPin(const UEdGraphPin *Pin) const
{
	return Pin != nullptr && Pin->Direction == EGPD_Output &&
		   Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object &&
		   Pin->PinName.ToString().StartsWith(DetailPinNamePrefix);
}

bool UBPNode_Mechanic::IsTraitPin(const UEdGraphPin *Pin) const
{
	return Pin != nullptr && Pin->Direction == EGPD_Output &&
		   Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct &&
		   Pin->PinName.ToString().StartsWith(TraitPinNamePrefix);
}

int32 UBPNode_Mechanic::IndexOfDetailPin(const UEdGraphPin* Pin) const
{
	if (Pin == nullptr)
	{
		return -1;
	}
	for (int32 i = 0; i < DetailsCount; ++i)
	{
		const auto Name = MakeDetailPinName(i);
		if (Pin->PinName == Name)
		{
			return i;
		}
	}
	return -1;
}

int32 UBPNode_Mechanic::IndexOfTraitPin(const UEdGraphPin* Pin) const
{
	if (Pin == nullptr)
	{
		return -1;
	}
	for (int32 i = 0; i < TraitsCount; ++i)
	{
		const auto Name = MakeTraitPinName(i);
		if (Pin->PinName == Name)
		{
			return i;
		}
	}
	return -1;
}

UEdGraphPin *UBPNode_Mechanic::GetDetailPinGivenIndex(const int32 Index) const
{
	return FindPinChecked(MakeDetailPinName(Index), EGPD_Output);
}

UEdGraphPin *UBPNode_Mechanic::GetTraitPinGivenIndex(const int32 Index) const
{
	return FindPinChecked(MakeTraitPinName(Index), EGPD_Output);
}

UEdGraphPin *UBPNode_Mechanic::GetCompletedPin() const
{
	return FindPinChecked(CompletedPinName, EGPD_Output);
}

UEdGraphPin* UBPNode_Mechanic::GetMechanismPin() const
{
	return FindPinChecked(MechanismPinName, EGPD_Input);
}

UEdGraphPin* UBPNode_Mechanic::GetEvaluationBodyPin() const
{
	return FindPinChecked(OperatePinName, EGPD_Output);
}

UEdGraphPin* UBPNode_Mechanic::GetSubjectPin() const
{
	return FindPinChecked(SubjectPinName, EGPD_Output);
}

UEdGraphPin* UBPNode_Mechanic::GetSubjectivePin() const
{
	return FindPinChecked(SubjectivePinName, EGPD_Output);
}

void UBPNode_Mechanic::OnDetailClassPinChanged(UEdGraphPin* ChangedPin)
{
	Super::OnDetailClassPinChanged(ChangedPin);
	Modify();

	const auto Index = FindDetailClassPinIndex(ChangedPin);

	if (Index == -1)
		return;

	const auto NewClass = GetDetailClassPinClass(ChangedPin);

	auto DetailPin = GetDetailPinGivenIndex(Index);
	check(DetailPin);

	auto OldClass = CastChecked<UClass>(DetailPin->PinType.PinSubCategoryObject);
	if (!NewClass->IsChildOf(OldClass))
	{
		DetailPin->BreakAllPinLinks();
	}

	DetailPin->PinType.PinSubCategoryObject = NewClass;

	UpdateDetailPinUI(DetailPin);

	if (auto Graph = GetGraph())
	{
		Graph->NotifyGraphChanged();
	}
}

void UBPNode_Mechanic::OnTraitTypePinChanged(UEdGraphPin* ChangedPin)
{
	Super::OnTraitTypePinChanged(ChangedPin);
	Modify();

	const auto Index = FindTraitTypePinIndex(ChangedPin);

	if (Index == -1)
		return;

	const auto NewType = GetTraitTypePinType(ChangedPin);

	auto TraitPin = GetTraitPinGivenIndex(Index);
	check(TraitPin);

	auto* Schema = GetSchema();
	if (Schema)
	{
		Schema->RecombinePin(TraitPin);
	}
	TraitPin->BreakAllPinLinks();

	TraitPin->PinType.PinSubCategoryObject = NewType;

	UpdateTraitPinUI(TraitPin);

	if (auto Graph = GetGraph())
	{
		Graph->NotifyGraphChanged();
	}
}

#undef LOCTEXT_NAMESPACE
