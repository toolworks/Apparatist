/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: MechanicalActorComponent.h
 * Created: 2021-06-10 21:58:48
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

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Machine.h"

#include "MechanicalActorComponent.generated.h"


/**
 * The mechanical actor component entity.
 */
UCLASS(ClassGroup = Apparatus, meta = (BlueprintSpawnableComponent), Blueprintable)
class APPARATUSRUNTIME_API UMechanicalActorComponent
  : public UActorComponent
  , public IMechanical
{
	GENERATED_BODY()

	/**
	 * The steady update time interval.
	 */
	UPROPERTY(EditAnywhere, Category = Mechanism,
			  Meta = (AllowPrivateAccess = "true"))
	float SteadyDeltaTime = IMechanical_DefaultSteadyDeltaTime;

  public:

	/**
	 * Construct a new mechanism.
	 */
	UMechanicalActorComponent();

	/**
	 * Destroy the mechanism.
	 */
	~UMechanicalActorComponent();

  protected:

	/**
	 * Begin executing the mechanism.
	 */
	virtual void BeginPlay() override;

	/**
	 * End executing the mechanism.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  public:

	virtual float
	GetSteadyDeltaTime() const override
	{
		return SteadyDeltaTime;
	}

	virtual float
	GetOwnTime() const override
	{
		AActor* Owner = GetOwner();
		if (LIKELY(Owner))
		{
			return Owner->GetGameTimeSinceCreation();
		}
		return NAN;
	}

	virtual void
	TickComponent(
		float                        DeltaTime,
		enum ELevelTick              TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Get the time of the last processed steady frame.
	 */
	FORCEINLINE float
	GetProcessedSteadyTime() const
	{
		return IMechanical::GetProcessedSteadyTime_Implementation();
	}

	/**
	 * The current ratio within the steady frame.
	 * 
	 * This is in relation between the previous steady
	 * frame and the current next one.
	 * Should be used for interframe interpolation.
	 */
	FORCEINLINE float
	CalcSteadyFrameRatio() const
	{
		return IMechanical::CalcSteadyFrameRatio_Implementation();
	}

	/**
	 * The current steady frame.
	 */
	FORCEINLINE int64
	GetSteadyFrame() const
	{
		return SteadyFrame;
	}

	/**
	 * The total steady time elapsed.
	 */
	FORCEINLINE float
	GetSteadyTime() const
	{
		return IMechanical::GetSteadyTime_Implementation();
	}

	/**
	 * The current steady future factor.
	 * 
	 * This is in relation between the previous change time
	 * delta to the next steady frame change delta time.
	 */
	FORCEINLINE float
	CalcSteadyFutureFactor() const
	{
		return IMechanical::CalcSteadyFutureFactor_Implementation();
	}
}; //-class UMechanicalActorComponent

FORCEINLINE
UMechanicalActorComponent::UMechanicalActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

FORCEINLINE
UMechanicalActorComponent::~UMechanicalActorComponent() {}


FORCEINLINE void
UMechanicalActorComponent::BeginPlay()
{
	Super::BeginPlay();
	DoRegister();
}

FORCEINLINE void
UMechanicalActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DoUnregister();
	Super::EndPlay(EndPlayReason);
}

FORCEINLINE void
UMechanicalActorComponent::TickComponent(
	float                        DeltaTime,
	enum ELevelTick              TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	AActor* Owner = GetOwner();
	if (LIKELY(Owner))
	{
		DoTick(Owner->GetGameTimeSinceCreation(),
			   DeltaTime,
			   SteadyDeltaTime);
	}
}
