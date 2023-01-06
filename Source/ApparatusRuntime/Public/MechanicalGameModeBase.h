/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: MechanicalGameModeBase.h
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

#include "CoreMinimal.h"
#include "UObject/WeakInterfacePtr.h"
#include "GameFramework/GameModeBase.h"

#include "Machine.h"

#include "MechanicalGameModeBase.generated.h"


/**
 * The base game mode mechanism facility.
 */
UCLASS()
class APPARATUSRUNTIME_API AMechanicalGameModeBase
  : public AGameModeBase
  , public IMechanical
{
	GENERATED_BODY()

	/**
	 * The steady update time interval.
	 */
	UPROPERTY(EditAnywhere, Category = Mechanism,
			  Meta = (AllowPrivateAccess = "true"))
	float SteadyDeltaTime = IMechanical_DefaultSteadyDeltaTime;

	friend class UDetail;
	friend class UBelt;
	friend class ISubjective;

  public:
	/**
	 * Construct a new mechanism.
	 */
	AMechanicalGameModeBase();

	/**
	 * Destroy the mechanism.
	 */
	~AMechanicalGameModeBase();

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
		return GetGameTimeSinceCreation();
	}

	virtual void Tick(float DeltaTime) override;

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
}; // class AMechanicalGameModeBase

FORCEINLINE AMechanicalGameModeBase::AMechanicalGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

FORCEINLINE AMechanicalGameModeBase::~AMechanicalGameModeBase()
{
}

FORCEINLINE void AMechanicalGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	DoRegister();
}

FORCEINLINE void AMechanicalGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DoUnregister();
	Super::EndPlay(EndPlayReason);
}

FORCEINLINE void AMechanicalGameModeBase::Tick(float DeltaTime)
{
	DoTick(GetGameTimeSinceCreation(),
		   DeltaTime,
		   SteadyDeltaTime);
}
