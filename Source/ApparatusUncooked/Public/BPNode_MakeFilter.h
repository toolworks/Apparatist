/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_MakeFilter.h
 * Created: Friday, 23rd October 2020 7:00:48 pm
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2022, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#pragma once

#include "K2Node.h"
#include "K2Node_AddPinInterface.h"

#include "BPNode_MakeFingerprint.h"

#include "BPNode_MakeFilter.generated.h"

/**
 * A user-friendly filter construction node.
 */
UCLASS(BlueprintType, Blueprintable)
class APPARATUSUNCOOKED_API UBPNode_MakeFilter
	: public UBPNode_MakeFingerprint
{
	GENERATED_BODY()

	//+DEPRECATED

	UPROPERTY()
	int32 NumNotTypes = 0;

	UPROPERTY()
	int32 ExcludedTypesCount = 0;

	//-DEPRECATED

protected:

	/**
	 * The number of excluded trait pins to generate for this node.
	 */
	UPROPERTY()
	int32 ExcludedTraitsCount = 0;

	/**
	 * The number of excluded detail pins to generate for this node.
	 */
	UPROPERTY()
	int32 ExcludedDetailsCount = 0;

	/**
	 * Is the negative flagmark filtering active.
	 */
	UPROPERTY()
	bool bExcludingFlagmarkActive = false;

	/**
	 * The excluded trait type pin name prefix.
	 */
	static const FString ExcludedTraitTypePinNamePrefix;

	/**
	 * The excluded detail class pin name prefix.
	 */
	static const FString ExcludedDetailClassPinNamePrefix;

	/**
	 * The output fingerprint pin name.
	 */
	static const FName FilterPinName;

	/**
	 * The excluding flagmark input pin name.
	 */
	static const FName ExcludingFlagmarkPinName;

	// BEGIN - UEdGraphNode
	virtual void AllocateDefaultPins() override;

	virtual void PostReconstructNode() override;

	virtual FLinearColor GetNodeTitleColor() const override;

	virtual void
	EarlyValidation(class FCompilerResultsLog &MessageLog) const override;

	// Needed for correct handling of the next method.
	FORCEINLINE
	virtual void ExpandToArrayNodes(class FKismetCompilerContext& CompilerContext,
									UEdGraph* SourceGraph,
									UEdGraphPin*& TraitsArrayPin,
									UEdGraphPin*& DetailsArrayPin) override
	{
		Super::ExpandToArrayNodes(CompilerContext, SourceGraph,
								  TraitsArrayPin, DetailsArrayPin);
	}

	virtual void
	ExpandToArrayNodes(class FKismetCompilerContext& CompilerContext,
					   UEdGraph* SourceGraph,
					   UEdGraphPin*& TraitsArrayPin,
					   UEdGraphPin*& DetailsArrayPin,
					   UEdGraphPin*& ExcludedTraitsArrayPin,
					   UEdGraphPin*& ExcludedDetailsArrayPin);

	virtual class UK2Node_CallFunction*
	ExpandToMakeFilterNode(
		class FKismetCompilerContext &CompilerContext,
		UEdGraph *SourceGraph);

	virtual UEdGraphPin*
	ExpandToFilterPin(
		class FKismetCompilerContext &CompilerContext,
		UEdGraph *SourceGraph);

	virtual void
	ExpandNode(class FKismetCompilerContext &CompilerContext,
			   UEdGraph *SourceGraph) override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FText GetTooltipText() const override;

	virtual void PinConnectionListChanged(UEdGraphPin *ChangedPin) override;

	virtual void PinDefaultValueChanged(UEdGraphPin *ChangedPin) override;

	// END - UEdGraphNode.

	// BEGIN - UK2Node interface
	virtual FText GetMenuCategory() const override;

	virtual void GetNodeContextMenuActions(
		class UToolMenu *Menu,
		class UGraphNodeContextMenuContext *Context) const override;

	virtual bool IsNodePure() const override;
	//-UK2Node interface.

	//+EXCLUDING FLAGMARK
	virtual void
	AddExcludingFlagmarkPin();

	virtual void
	InteractiveAddExcludingFlagmarkPin();

	virtual void
	RemoveExcludingFlagmarkPin();

	virtual void
	InteractiveRemoveExcludingFlagmarkPin();
	//-EXCLUDING FLAGMARK

	//+TRAIT EXCLUSION
	virtual void
	AddExcludedTraitTypePin();

	virtual void
	InteractiveAddExcludedTraitTypePin();

	virtual void
	RemoveExcludedTraitTypePin(UEdGraphPin *Pin);

	virtual void
	InteractiveRemoveExcludedTraitTypePin(UEdGraphPin *ExcludedTypePin);
	//-TRAIT EXCLUSION

	//+DETAIL EXCLUSION
	virtual void
	AddExcludedDetailClassPin();

	virtual void
	InteractiveAddExcludedDetailClassPin();

	virtual void
	RemoveExcludedDetailClassPin(UEdGraphPin *Pin);

	virtual void
	InteractiveRemoveExcludedDetailClassPin(UEdGraphPin *ExcludedTypePin);
	//-DETAIL EXCLUSION

	/**
	 * Get the output filter pin.
	 */
	virtual UEdGraphPin* GetFilterPin() const;

	virtual UEdGraphPin* GetExcludingFlagmarkPin() const;

	virtual UEdGraphPin* GetExcludedTraitTypePinGivenIndex(const int32 Index) const;

	virtual UEdGraphPin* GetExcludedDetailClassPinGivenIndex(const int32 Index) const;

	virtual UScriptStruct* GetExcludedTraitTypePinType(UEdGraphPin *Pin) const;

	virtual UClass* GetExcludedDetailClassPinClass(UEdGraphPin *Pin) const;

	virtual FName MakeExcludedTraitTypePinName(const int32 Index) const;

	virtual FName MakeExcludedDetailClassPinName(const int32 Index) const;

	virtual int32 GetExcludedTraitTypePinIndex(const UEdGraphPin *Pin) const;

	virtual int32 GetExcludedDetailClassPinIndex(const UEdGraphPin *Pin) const;

	virtual bool IsExcludingFlagmarkPin(const UEdGraphPin* Pin) const;

	virtual bool IsExcludedTraitTypePin(const UEdGraphPin* Pin) const;

	virtual bool IsExcludedDetailClassPin(const UEdGraphPin* Pin) const;

	virtual void OnExcludingFlagmarkPinChanged(UEdGraphPin* ChangedPin);

	virtual void OnExcludedTraitTypePinChanged(UEdGraphPin* ChangedPin);

	virtual void OnExcludedDetailClassPinChanged(UEdGraphPin* ChangedPin);

	/**
	 * Synchronize the internal pin names to the actual state.
	 *
	 * @return Were actually any renames performed?
	 */
	virtual bool SyncPinNames();

	/**
	 * Update the excluding flagmark pin UI according to its state.
	 */
	virtual void UpdateExcludingFlagmarkPinUI(UEdGraphPin *Pin);

	/**
	 * Update the excluded trait type pin UI according to its state.
	 */
	virtual void UpdateExcludedTraitTypePinUI(UEdGraphPin *Pin);

	/**
	 * Update the excluded detail class pin UI according to its state.
	 */
	virtual void UpdateExcludedDetailClassPinUI(UEdGraphPin *Pin);

  public:

	/**
	 * Is the excluding flagmark active on the node?
	 */
	FORCEINLINE bool
	HasExcludingFlagmark() const
	{
		return bExcludingFlagmarkActive;
	}

	/**
	 * Get the number of excluded traits.
	 */
	FORCEINLINE int32
	ExcludedTraitsNum() const
	{
		return ExcludedTraitsCount;
	}

	/**
	 * Get the number of excluded details.
	 */
	FORCEINLINE int32
	ExcludedDetailsNum() const
	{
		return ExcludedDetailsCount;
	}

	/**
	 * Can the filter be cached statically?
	 */
	bool
	CanBeCached() const;
}; // class UBPNode_MakeFilter
