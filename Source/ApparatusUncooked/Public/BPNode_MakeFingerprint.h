/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_MakeFingerprint.h
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

#include "BPNode_MakeFingerprint.generated.h"

/**
 * A user-friendly fingerprint construction node.
 */
UCLASS(BlueprintType, Blueprintable)
class APPARATUSUNCOOKED_API UBPNode_MakeFingerprint
	: public UK2Node,
	  public IK2Node_AddPinInterface
{
	GENERATED_BODY()

  private:

	//+DEPRECATED
	UPROPERTY()
	int32 NumTypes = 0;

	UPROPERTY()
	int32 TypesCount = 0;
	//-DEPRECATED

  protected:

	/**
	 * The number of detail pins to generate for this node.
	 */
	UPROPERTY()
	int32 DetailsCount = 0;

	/**
	 * The number of trait pins to generate for this node.
	 */
	UPROPERTY()
	int32 TraitsCount = 0;

	/**
	 * Is the flagmark specification active.
	 */
	UPROPERTY()
	bool bFlagmarkActive = false;

	/**
	 * The detail class pin name prefix.
	 */
	static const FString DetailClassPinNamePrefix;

	/**
	 * The trait type pin name prefix.
	 */
	static const FString TraitTypePinNamePrefix;

	/**
	 * The output fingerprint pin name.
	 */
	static const FName FingerprintPinName;

	/**
	 * The flagmark input pin name.
	 */
	static const FName FlagmarkPinName;

	//+UEdGraphNode
	virtual void AllocateDefaultPins() override;

	virtual FLinearColor GetNodeTitleColor() const override;

	virtual void
	EarlyValidation(class FCompilerResultsLog &MessageLog) const override;

	virtual void ExpandNode(class FKismetCompilerContext &CompilerContext,
							UEdGraph *SourceGraph) override;

	virtual FText
	GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FText
	GetTooltipText() const override;

	virtual void
	PinConnectionListChanged(UEdGraphPin *ChangedPin) override;

	virtual void
	PinDefaultValueChanged(UEdGraphPin *ChangedPin) override;

	virtual void
	PostReconstructNode() override;
	//-UEdGraphNode.

	// BEGIN - UK2Node interface
	virtual FText GetMenuCategory() const override;

	virtual void GetMenuActions(
		FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

	virtual void GetNodeContextMenuActions(
		class UToolMenu *Menu,
		class UGraphNodeContextMenuContext* Context) const override;

	virtual bool IsNodePure() const override;
	// END - UK2Node interface.

	// BEGIN - IK2Node_AddPinInterface
	virtual void AddInputPin() override;
	// END - IK2Node_AddPinInterface

	/**
	 * Add a pin returning it via an argument.
	 */
	virtual void AddInputPin(UEdGraphPin*& AddedPin);

	virtual UEdGraphPin* AddFlagmarkPin();

	virtual UEdGraphPin* InteractiveAddFlagmarkPin();

	virtual void MenuAddFlagmarkPin()
	{
		InteractiveAddFlagmarkPin();
	}

	virtual UEdGraphPin* AddTraitTypePin();
	
	virtual UEdGraphPin* InteractiveAddTraitTypePin();

	virtual void MenuAddTraitTypePin()
	{
		InteractiveAddTraitTypePin();
	}

	virtual UEdGraphPin* AddDetailClassPin();

	virtual UEdGraphPin* InteractiveAddDetailClassPin();

	virtual void MenuAddDetailClassPin()
	{
		InteractiveAddDetailClassPin();
	}

	virtual void RemoveFlagmarkPin();

	virtual void RemoveTraitTypePin(const int32 Index);

	virtual void RemoveTraitTypePin(UEdGraphPin *Pin);

	virtual void RemoveDetailClassPin(const int32 Index);

	virtual void RemoveDetailClassPin(UEdGraphPin *Pin);

	virtual void InteractiveRemoveFlagmarkPin();

	virtual void InteractiveRemoveTraitTypePin(UEdGraphPin* TypePin);

	virtual void InteractiveRemoveDetailClassPin(UEdGraphPin* ClassPin);

	virtual void MenuRemoveFlagmarkPin()
	{
		InteractiveRemoveFlagmarkPin();
	}

	/**
	 * Get the output fingerprint pin.
	 */
	virtual UEdGraphPin* GetFingerprintPin() const;

	/**
	 * Get the input flagmark pin (if exists).
	 */
	virtual UEdGraphPin* GetFlagmarkPin() const;

	virtual UEdGraphPin* GetTraitTypePinGivenIndex(const int32 Index) const;

	virtual UEdGraphPin* GetDetailClassPinGivenIndex(const int32 Index) const;

	virtual UScriptStruct* GetTraitTypePinType(UEdGraphPin* Pin) const;

	virtual UClass* GetDetailClassPinClass(UEdGraphPin* Pin) const;

	virtual FName MakeTraitTypePinName(const int32 Index) const;

	virtual FName MakeDetailClassPinName(const int32 Index) const;

	virtual int32 FindTraitTypePinIndex(const UEdGraphPin* Pin) const;

	virtual int32 FindDetailClassPinIndex(const UEdGraphPin* Pin) const;

	virtual bool IsFlagmarkPin(const UEdGraphPin* Pin) const;

	virtual bool IsTraitTypePin(const UEdGraphPin* Pin) const;

	virtual bool IsDetailClassPin(const UEdGraphPin* Pin) const;

	virtual void OnFlagmarkPinChanged(UEdGraphPin* ChangedPin);

	virtual void OnTraitTypePinChanged(UEdGraphPin* ChangedPin);

	virtual void OnDetailClassPinChanged(UEdGraphPin* ChangedPin);

	/**
	 * Synchronize the internal pin names to the actual state.
	 *
	 * @return Were actually any renames performed?
	 */
	virtual bool SyncPinNames();

	virtual void ExpandToArrayNodes(class FKismetCompilerContext& CompilerContext,
									UEdGraph* SourceGraph,
									UEdGraphPin*& TraitsArrayPin,
									UEdGraphPin*& DetailsArrayPin);

	virtual void Expand_CopyOrMoveLink(class FKismetCompilerContext& CompilerContext,
									   UEdGraph* SourceGraph,
									   UEdGraphPin* SrcPin,
									   UEdGraphPin* DstPin);

	virtual void UpdateFlagmarkPinUI(UEdGraphPin* Pin);

	virtual void UpdateTraitTypePinUI(UEdGraphPin* Pin);

	virtual void UpdateDetailClassPinUI(UEdGraphPin* Pin);

  public:

	/**
	 * Is flagmark specification active?
	 */
	FORCEINLINE bool
	HasFlagmark() const
	{
		return bFlagmarkActive;
	}

	/**
	 * Get the number of traits in the fingerprint.
	 */
	FORCEINLINE int32
	TraitsNum() const
	{
		return TraitsCount;
	}

	/**
	 * Get the number of details in the fingerprint.
	 */
	FORCEINLINE int32
	DetailsNum() const
	{
		return DetailsCount;
	}

}; // class UBPNode_MakeFingerprint
