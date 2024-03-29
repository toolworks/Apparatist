/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: BubbleCageCell.h
 * Created: 2022-05-11
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

#include "Scaled.generated.h"


/**
 * A general scaling provider.
 */
USTRUCT(BlueprintType, Category = "Basic")
struct APPARATISTRUNTIME_API FScaled
{
	GENERATED_BODY()

  public:

	/// The scaling factors to apply.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	FVector Factors = FVector::OneVector;

	/// Default constructor.
	FScaled() {}

	/// Construct with a factor.
	FORCEINLINE
	FScaled(const float InFactor)
	  : Factors(InFactor)
	{}

	/// Construct with factors vector.
	FORCEINLINE
	FScaled(const FVector InFactors)
	  : Factors(InFactors)
	{}

	/// Construct copying from a transform.
	FORCEINLINE
	FScaled(const FTransform& InTransform)
	  : Factors(InTransform.GetScale3D())
	{}
};
