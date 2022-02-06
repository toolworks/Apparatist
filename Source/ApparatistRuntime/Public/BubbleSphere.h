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

	/**
	 * Accumulated decoupling force.
	 */
	FVector AccumulatedDecouple = FVector::ZeroVector;

	/**
	 * Number of accumulated decouples.
	 */
	int32 AccumulatedDecoupleCount = 0;

	/* Default constructor. */
	FBubbleSphere() {}

	/* Constructor with radius argument. */
	FBubbleSphere(const float InRadius) : Radius(InRadius) {}
};
