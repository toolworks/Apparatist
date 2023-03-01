/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: BubbleCage.h
 * Created: 2023-02-18 15:00:11
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#pragma once

#include <atomic>

#include "Containers/UnrealString.h"
#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"

#include "BubbleCageComponent.h"

#include "BubbleCage.generated.h"


#define BUBBLE_DEBUG 0

/**
 * A simple and performant collision detection and decoupling for spheres.
 */
UCLASS(Category = "BubbleCage")
class APPARATISTRUNTIME_API ABubbleCage
  : public AActor
{
	GENERATED_BODY()

  private:

	/**
	 * The singleton instance of the cage.
	 */
	static ABubbleCage* Instance;

	/**
	 * The main bubble cage component.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bubble Cage", Meta = (AllowPrivateAccess))
	UBubbleCageComponent* BubbleCageComponent = nullptr;

  protected:

	void
	BeginDestroy() override
	{
		if (LIKELY(Instance == this))
		{
			Instance = nullptr;
		}
		
		Super::BeginDestroy();
	}

	void
	BeginPlay() override
	{
		Super::BeginPlay();

		Instance = this;
	}

  public:

	/**
	 * Get the global instance of the cage.
	 */
	static FORCEINLINE ABubbleCage*
	GetInstance()
	{
		return Instance;
	}

	/**
	 * Get the actual bubble cage component.
	 */
	auto
	GetComponent()
	{
		return BubbleCageComponent;
	}

	ABubbleCage();

	/**
	 * Get overlapping spheres for a specific location.
	 */
	static int32
	GetOverlapping(const FVector&          Location,
				   TArray<FSubjectHandle>& OutOverlappers)
	{
		if (LIKELY(Instance != nullptr && Instance->BubbleCageComponent != nullptr))
		{
			return Instance->BubbleCageComponent->GetOverlapping(Location, OutOverlappers);
		}
		OutOverlappers.Reset();
		return 0;
	}

	/**
	 * Get overlapping spheres for a specific location and a filter.
	 */
	static int32
	GetOverlapping(const FVector&          Location,
				   const FFilter&          Filter,
				   TArray<FSubjectHandle>& OutOverlappers)
	{
		if (LIKELY(Instance != nullptr && Instance->BubbleCageComponent != nullptr))
		{
			return Instance->BubbleCageComponent->GetOverlapping(Location, Filter, OutOverlappers);
		}
		OutOverlappers.Reset();
		return 0;
	}

	/**
	 * Get overlapping spheres for a specific location and a filter.
	 */
	static TArray<FSubjectHandle>
	GetOverlapping(const FVector& Location,
				   const float    Radius = 0.0f)
	{
		if (LIKELY(Instance != nullptr && Instance->BubbleCageComponent != nullptr))
		{
			return Instance->BubbleCageComponent->GetOverlapping(Location, Radius);
		}
		return TArray<FSubjectHandle>();
	}

	/**
	 * Get overlapping spheres for a specific location and a filter.
	 */
	static TArray<FSubjectHandle>
	GetOverlapping(const FVector& Location,
				   const float    Radius,
				   const FFilter& Filter)
	{
		if (LIKELY(Instance != nullptr && Instance->BubbleCageComponent != nullptr))
		{
			return Instance->BubbleCageComponent->GetOverlapping(Location, Radius, Filter);
		}
		return TArray<FSubjectHandle>();
	}

	/**
	 * Get overlapping spheres for the specified location.
	 */
	static int32
	GetOverlapping(const FVector&          Location,
				   const float             Radius,
				   TArray<FSubjectHandle>& OutOverlappers)
	{
		if (LIKELY(Instance != nullptr && Instance->BubbleCageComponent != nullptr))
		{
			return Instance->BubbleCageComponent->GetOverlapping(Location, Radius, OutOverlappers);
		}
		OutOverlappers.Reset();
		return 0;
	}

	/**
	 * Get overlapping spheres for the specified location.
	 */
	UFUNCTION(BlueprintCallable)
	static int32
	GetOverlapping(const FVector&          Location,
				   const float             Radius,
				   const FFilter&          Filter,
				   TArray<FSubjectHandle>& OutOverlappers)
	{
		if (LIKELY(Instance != nullptr && Instance->BubbleCageComponent != nullptr))
		{
			return Instance->BubbleCageComponent->GetOverlapping(Location, Radius, Filter, OutOverlappers);
		}
		OutOverlappers.Reset();
		return 0;
	}

	/**
	 * Re-fill the cage with bubbles.
	 */
	UFUNCTION(BlueprintCallable)
	static void
	Update()
	{
		if (UNLIKELY(Instance == nullptr)) return;
		Instance->BubbleCageComponent->Update();
	}

	/**
	 * Decouple the bubbles within the cage.
	 */
	UFUNCTION(BlueprintCallable)
	static void
	Decouple()
	{
		if (UNLIKELY(Instance == nullptr)) return;
		Instance->BubbleCageComponent->Decouple();
	}

	/**
	 * Re-register and decouple the bubbles.
	 */
	UFUNCTION(BlueprintCallable)
	static void
	Evaluate()
	{
		if (UNLIKELY(Instance == nullptr)) return;
		Instance->BubbleCageComponent->Evaluate();
	}
};
