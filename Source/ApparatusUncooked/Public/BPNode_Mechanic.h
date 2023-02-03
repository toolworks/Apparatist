/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_Mechanic.h
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
#include "K2Node_AddPinInterface.h"

#include "BPNode_MakeFilter.h"

#include "BPNode_Mechanic.generated.h"


/**
 * The main mechanic evaluation node.
 */
UCLASS(BlueprintType, Blueprintable)
class APPARATUSUNCOOKED_API UBPNode_Mechanic final
  : public UBPNode_MakeFilter
{
	GENERATED_BODY()

  protected:

	/**
	 * The explicit mechanism pin name.
	 */
	static const FName MechanismPinName;

	/**
	 * The modulation body pin name (Deprecated).
	 */
	static const FName EvaluationBodyPinName;

	/**
	 * The modulation body pin name.
	 */
	static const FName OperatePinName;

	/**
	 * The completed pin name.
	 */
	static const FName CompletedPinName;

	/**
	 * The subject pin name.
	 */
	static const FName SubjectPinName;

	/**
	 * The subjective pin name.
	 */
	static const FName SubjectivePinName;

	/**
	 * The output detail pin name prefix.
	 */
	static const FString DetailPinNamePrefix;

	/**
	 * The output trait pin name prefix.
	 */
	static const FString TraitPinNamePrefix;

	/**
	 * Migrate the missing types from old pins.
	 * 
	 * @param OldPins The old pins to migrate.
	 */
	virtual void MigrateMissingTypes(TArray<UEdGraphPin*>& OldPins);
	
	//+ UEdGraphNode

	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;

	virtual void AllocateDefaultPins() override;

	virtual FLinearColor GetNodeTitleColor() const override;

	virtual void
	EarlyValidation(class FCompilerResultsLog& MessageLog) const override;

	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext,
							UEdGraph* SourceGraph) override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FText GetTooltipText() const override;

	virtual void PostReconstructNode() override;
	//- UEdGraphNode

	//+UK2Node
	virtual FText
	GetMenuCategory() const override;

	virtual void
	GetMenuActions(
		FBlueprintActionDatabaseRegistrar &ActionRegistrar) const override;


	virtual ERedirectType
	DoPinsMatchForReconstruction(
		const UEdGraphPin* NewPin,
		int32 NewPinIndex,
		const UEdGraphPin* OldPin,
		int32 OldPinIndex) const override;
	//-UK2Node

	virtual void AddInputPin(UEdGraphPin*& AddedPin) override;

	virtual UEdGraphPin* AddDetailClassPin() override;

	virtual UEdGraphPin* AddTraitTypePin() override;

	virtual void RemoveDetailClassPin(const int32 Index) override;

	virtual void RemoveTraitTypePin(const int32 Index) override;

	virtual bool IsConnectedToEvent(const FName EventName) const;

	virtual bool IsBooting() const;

	virtual bool IsSteady() const;

	virtual bool IsNodePure() const override;

	virtual UEdGraphPin* GetMechanismPin() const;

	virtual UEdGraphPin* GetEvaluationBodyPin() const;

	virtual UEdGraphPin* GetCompletedPin() const;

	virtual UEdGraphPin* GetSubjectPin() const;

	virtual UEdGraphPin* GetSubjectivePin() const;

	virtual UEdGraphPin* GetDetailPinGivenIndex(const int32 Index) const;

	virtual UEdGraphPin* GetTraitPinGivenIndex(const int32 Index) const;

	virtual FName MakeDetailPinName(const int32 Index) const;

	virtual FName MakeTraitPinName(const int32 Index) const;

	virtual int32 IndexOfDetailPin(const UEdGraphPin *Pin) const;

	virtual int32 IndexOfTraitPin(const UEdGraphPin *Pin) const;

	virtual bool IsDetailPin(const UEdGraphPin *Pin) const;

	virtual bool IsTraitPin(const UEdGraphPin *Pin) const;

	/**
	 * Check if we should iterate in a belt-based way.
	 */
	virtual bool IsBeltBasedIterating() const;

	/**
	 * Check if we should iterate in a chunk-based way.
	 */
	virtual bool IsChunkBasedIterating() const;

	virtual UClass* GetDetailPinClass(UEdGraphPin* Pin) const;

	virtual UScriptStruct* GetTraitPinType(UEdGraphPin* Pin) const;

	virtual void OnDetailClassPinChanged(UEdGraphPin *ChangedPin) override;

	virtual void OnTraitTypePinChanged(UEdGraphPin *ChangedPin) override;

	/**
	 * Synchronize the internal pin names to the actual state.
	 *
	 * @return Were actually any renames performed?
	 */
	virtual bool SyncPinNames();

	virtual void UpdateDetailPinUI(UEdGraphPin *Pin);

	virtual void UpdateTraitPinUI(UEdGraphPin *TraitPin);
}; //- UBPNode_Mechanic
