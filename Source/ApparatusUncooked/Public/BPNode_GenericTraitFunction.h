/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_GenericTraitFunction.h
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

#pragma once

#include "K2Node.h"
#include "Engine/MemberReference.h"
#include "K2Node_CallFunction.h"

#include "BPNode_GenericTraitFunction.generated.h"


/**
 * Generic trait-based node.
 * 
 * A smarter version of the call function designed
 * specifically for the struct traits.
 */
UCLASS(Abstract, MinimalAPI)
class UBPNode_GenericTraitFunction
  : public UK2Node_CallFunction
{
	GENERATED_BODY()

  private:

	mutable FStructProperty* TraitDataPropertyCache = nullptr;

	mutable FObjectProperty* TraitTypePropertyCache = nullptr;

	mutable FStructProperty* SubjectPropertyCache = nullptr;

	mutable FInterfaceProperty* SubjectivePropertyCache = nullptr;

  public:

	/**
	 * The name of the trait type pin.
	 */
	FName GetTraitTypePinName() const;

	/**
	 * The name of the trait data pin.
	 */
	FName GetTraitDataPinName() const;

	/**
	 * Subject handle pin name.
	 */
	FName GetSubjectPinName() const;

	/**
	 * Subjective pin name.
	 */
	FName GetSubjectivePinName() const;


	//+UK2Node

	virtual void
	AllocateDefaultPins() override;

	virtual void
	EarlyValidation(class FCompilerResultsLog &MessageLog) const override;

	void
	ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;

	virtual void 
	GetMenuActions(
		FBlueprintActionDatabaseRegistrar &ActionRegistrar) const override;

	virtual void
	ExpandNode(class FKismetCompilerContext& CompilerContext,
			   UEdGraph*                     SourceGraph) override;

	virtual void
	PostReconstructNode() override;

	virtual ERedirectType
	DoPinsMatchForReconstruction(
		const UEdGraphPin* NewPin,
		int32 NewPinIndex,
		const UEdGraphPin* OldPin,
		int32 OldPinIndex) const override;

	virtual void
	ReallocatePinsDuringReconstruction(
		TArray<UEdGraphPin*>& OldPins) override;

	virtual void
	PinConnectionListChanged(UEdGraphPin* ChangedPin) override;

	virtual void
	PinDefaultValueChanged(UEdGraphPin* ChangedPin) override;
	//-UK2Node

	/**
	 * Check if the pin is a trait data pin.
	 */
	virtual bool
	IsTraitPin(UEdGraphPin* Pin, const bool bExact = true) const;

	/**
	 * Check if the pin is a trait type pin.
	 */
	virtual bool
	IsTraitTypePin(UEdGraphPin* Pin, const bool bExact = true) const;

	/**
	 * Is the trait data actually an output to write to?
	 */
	virtual bool
	IsOutputting() const;

	/**
	 * Update the trait data pin's type
	 * according to an explicitly supplied type.
	 */
	virtual void
	UpdateTraitPin(UScriptStruct* TraitType);

	/**
	 * Update the trait data pin's type
	 * according to an actual state.
	 */
	virtual void
	UpdateTraitPin();

	/**
	 * Update the trait pin user interface.
	 */
	virtual void
	UpdateTraitDataPinUI(UEdGraphPin* TraitPin);

	/**
	 * Update the trait type pin user interface.
	 */
	virtual void
	UpdateTraitTypePinUI(UEdGraphPin* TraitTypePin);

	/**
	 * Get the trait data pin direction.
	 */
	virtual EEdGraphPinDirection
	GetTraitPinDirection() const
	{
		return IsOutputting() ? EGPD_Output : EGPD_Input;
	}

	/**
	 * Is this a subjective-based node?
	 */
	virtual bool
	IsSubjectiveBased() const;

	/**
	 * Get the subject pin.
	 */
	virtual UEdGraphPin*
	GetSubjectPin() const;

	/**
	 * Get the trait type pin.
	 */
	virtual UEdGraphPin*
	GetTraitTypePin() const;

	/**
	 * Get the subjective pin.
	 */
	virtual UEdGraphPin*
	GetSubjectivePin() const;

	/**
	 * Get the trait data pin. May be either an output
	 * or an input pin.
	 */
	virtual UEdGraphPin*
	GetTraitDataPin() const;

	/**
	 * Get the type of the trait at a pin.
	 */
	virtual UScriptStruct*
	GetTraitType(UEdGraphPin* TraitPin, const bool bRecursive = true) const;

	/**
	 * Get the type of the trait.
	 */
	virtual UScriptStruct*
	GetTraitType() const;

	virtual UEdGraphPin*
	GetExecPin() const;

	/**
	 * Find the trait data property.
	 */
	virtual FStructProperty*
	FindTraitDataProperty() const;

	/**
	 * Find the trait type property.
	 */
	virtual FObjectProperty*
	FindTraitTypeProperty() const;

	/**
	 * Find the subject property.
	 */
	virtual FStructProperty*
	FindSubjectProperty() const;

	/**
	 * Find the subjective property.
	 */
	virtual FInterfaceProperty*
	FindSubjectiveProperty() const;

	UBPNode_GenericTraitFunction(
		const FObjectInitializer& ObjectInitializer);

};//-class UBPNode_GenericTraitFunction
