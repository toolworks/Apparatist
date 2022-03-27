// 2021 Vladislav Dmitrievich Turbanov

#pragma once

#include "CoreMinimal.h"

#include "Oriented.generated.h"


/**
 * Struct representing actor/projectile that have an orientation in the world.
 */
USTRUCT(BlueprintType, Category = "Basic")
struct APPARATISTRUNTIME_API FOriented
{
	GENERATED_BODY()

  public:

	/* Look forward orientation of the object. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	FVector Orientation = FVector::ZeroVector;

	/* Default constructor. */
	FOriented() {}

	///* Constructor with all arguments. */
	//FORCEINLINE FOriented(const FTransform& InTransform)
	//  : FOriented(InTransform.GetLocation())
	//{}

	/* Construct the trait by 2D-location. */
	FORCEINLINE FOriented(const FVector& InOrientation)
	  : Orientation(InOrientation)
	{}

	/* Simple get Orientation pure function. */
	FORCEINLINE const FVector&
	GetOrientation() const
	{
		return Orientation;
	}

	/* Simple set Orientation pure function. */
	FORCEINLINE void
	SetOrientation(const FVector& InOrientation)
	{
		Orientation = InOrientation;
	}
};