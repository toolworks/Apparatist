// 2022 Vladislav Dmitrievich Turbanov

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
	 * Get overlapping spheres for the specified location.
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
	 * Get overlapping spheres for the specified location.
	 */
	static TArray<FSubjectHandle>
	GetOverlapping(const FVector& Location,
				   const float    Radius)
	{
		if (LIKELY(Instance != nullptr && Instance->BubbleCageComponent != nullptr))
		{
			return Instance->BubbleCageComponent->GetOverlapping(Location, Radius);
		}
		return TArray<FSubjectHandle>();
	}

	/**
	 * Get overlapping spheres for the specified location.
	 */
	UFUNCTION(BlueprintCallable)
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
