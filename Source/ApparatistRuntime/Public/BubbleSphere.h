// 2022 Vladislav Dmitrievich Turbanov

#pragma once

#include "CoreMinimal.h"

#include "BubbleSphere.generated.h"


/**
 * Struct representing object positioned in BubbleCage.
 */
USTRUCT(BlueprintType, Category = "BubbleCage")
struct APPARATISTRUNTIME_API FBubbleSphere
{
	GENERATED_BODY()

  public:

	/* The radius of the circle. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BubbleCage")
	float Radius = 1;

	/* The index in the cage. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BubbleCage")
	int32 CellIndex = -1;

	/**
	 * Accumulated decoupling force.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "BubbleCage")
	FVector AccumulatedDecouple = FVector::ZeroVector;

	/**
	 * Number of accumulated decouples.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "BubbleCage")
	int32 AccumulatedDecoupleCount = 0;

	/* Default constructor. */
	FBubbleSphere() {}

	/* Constructor with radius argument. */
	FBubbleSphere(const float InRadius) : Radius(InRadius) {}
};
