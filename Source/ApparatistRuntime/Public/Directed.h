/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: BubbleCageCell.h
 * Created: 2022-03-27
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

#include "Directed.generated.h"


/**
 * Struct representing actor/projectile that have an orientation in the world.
 */
USTRUCT(BlueprintType, Category = "Basic")
struct APPARATISTRUNTIME_API FDirected
{
	GENERATED_BODY()

  public:

	/**
	 * The forward looking vector.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	FVector Direction = FVector::ForwardVector;

	/**
	 * Default-initialize the direction.
	 */
	FDirected()
	{}

	/**
	 * Initialize with a vector direction.
	 */
	FORCEINLINE
	FDirected(const FVector& InDirection)
	  : Direction(InDirection)
	{}
};