/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: TraitRendererComponent.h
 * Created: 2022-01-21
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#pragma once

#include "CoreMinimal.h"
#include "Containers/Map.h"
#include "Components/InstancedStaticMeshComponent.h"

#include "Machine.h"

#include "TraitRendererComponent.generated.h"


/**
 * Basic class for render subjects. To set up which subjects you want to render,
 * please, initialize the TraitType variable.
 */
UCLASS(ClassGroup = Apparatist, meta = (BlueprintSpawnableComponent),
	   Blueprintable, Category = "TraitRenderer")
class APPARATISTRUNTIME_API UTraitRendererComponent
  : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()

  private:

	/**
	 * All of the current transforms.
	 */
	TArray<FTransform> Transforms;

	/**
	 * Transforms that are actually used.
	 */
	FBitMask ValidTransforms;

	/**
	 * Transforms available for reuse.
	 */
	TArray<int32> FreeTransforms;

	FCriticalSection TransformsCS;

	bool bFirstUpdate = true;

	class FAsyncRenderStateUpdateTask
	  : public FNonAbandonableTask
	{
		friend class FAsyncTask<FAsyncRenderStateUpdateTask>;

		UTraitRendererComponent* Owner = nullptr;

		FAsyncRenderStateUpdateTask(UTraitRendererComponent* InOwner)
		  : Owner(InOwner)
		{}

		void DoWork()
		{
			check(Owner);
			Owner->UpdateRenderState();
		}

		FORCEINLINE TStatId
		GetStatId() const
		{
			RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncRenderStateUpdateTask, STATGROUP_ThreadPoolAsyncTasks);
		}
	};

	FAsyncTask<FAsyncRenderStateUpdateTask>* AsyncRenderStateUpdateTask = nullptr;

	static TMap<UScriptStruct*, UTraitRendererComponent*> InstancesByTraitTypes;

  protected:

	/**
	 * The type of the trait that should trigger its instance to be rendered.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, NoClear,
			  Category = "TraitRenderer")
	UScriptStruct* TraitType = nullptr;

	/**
	 * The global scale for the rendered instances.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, 
			  Category = "TraitRenderer")
	FVector Scale = {1.0f, 1.0f, 1.0f};

	/**
	 * Should teleport be enabled during the render-state update.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rendering")
	bool bUpdateViaTeleport = false;

	/**
	 * Manually update the state.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Workflow")
	bool bManualUpdate = false;

	/**
	 * Manually batch-update the transforms.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Workflow")
	bool bManualRenderStateUpdate = false;

	void
	BeginPlay() override;

	void
	EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void
	TickComponent(float DeltaTime, enum ELevelTick TickType,
				  FActorComponentTickFunction* ThisTickFunction) override;

  public:

	UTraitRendererComponent();

	void Update();

	void UpdateRenderState();

	void UpdateRenderStateAsync()
	{
		if (AsyncRenderStateUpdateTask != nullptr)
		{
			delete AsyncRenderStateUpdateTask;
		}
		AsyncRenderStateUpdateTask = new FAsyncTask<FAsyncRenderStateUpdateTask>(this);
		AsyncRenderStateUpdateTask->StartBackgroundTask();
	}

	void WaitForAsyncRenderStateUpdateCompletion()
	{
		if (AsyncRenderStateUpdateTask != nullptr)
		{
			AsyncRenderStateUpdateTask->EnsureCompletion();
		}
	}

	static UTraitRendererComponent*
	GetInstance(UScriptStruct* const TraitType)
	{
		if (TraitType == nullptr)
		{
			return nullptr;
		}
		const auto InstancePtr = InstancesByTraitTypes.Find(TraitType);
		if (InstancePtr == nullptr)
		{
			return nullptr;
		}
		return *InstancePtr;
	}
}; //-class UTraitRendererComponent
