/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: BubbleCageCell.h
 * Created: 2022-21-01
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

#include "BubbleSphere.generated.h"


/**
 * @brief The sphere-based collider.
 * 
 * @note The ::FScaled trait is not respected for efficiency reasons.
 * You should thereby always scale the radius of this sphere manually.
 */
USTRUCT(BlueprintType, Category = "BubbleCage")
struct APPARATISTRUNTIME_API FBubbleSphere
{
	GENERATED_BODY()

  public:

	/// The radius of the sphere.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BubbleCage")
	float Radius = 1;

	/**
	 * The decoupling strength.
	 * 
	 * This is a bit like a reciprocal of a mass.
	 * The more this value is, the more
	 * the sphere is decoupled from its neighbors
	 * in proportion to their correcponding values.
	 * 
	 * Set to 0 to suppress the decoupling for this sphere.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BubbleCage",
			  Meta = (ClampMin="0"))
	float DecoupleProportion = 1.0f;

	/// The accumulated decoupling force.
	FVector AccumulatedDecouple = FVector::ZeroVector;

	/// The number of accumulated decouples.
	int32 AccumulatedDecoupleCount = 0;

	/* Default constructor. */
	FBubbleSphere() {}

	/* Constructor with radius argument. */
	FBubbleSphere(const float InRadius) : Radius(InRadius) {}
};
