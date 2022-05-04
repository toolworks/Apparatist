// 2021 Vladislav Dmitrievich Turbanov

#pragma once

#include "CoreMinimal.h"

#include "Located.generated.h"


/**
 * Struct representing actor/projectile that have a position in the world.
 */
USTRUCT(BlueprintType, Category = "Basic")
struct APPARATISTRUNTIME_API FLocated
{
	GENERATED_BODY()

  public:

	/* Location of the object. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	FVector Location = FVector::ZeroVector;

	/* Default constructor. */
	FLocated() {}

	/* Constructor with all arguments. */
	FORCEINLINE FLocated(const FTransform& InTransform)
	  : Location(InTransform.GetLocation())
	{}

	/* Construct the trait by 2D-location. */
	FORCEINLINE FLocated(const FVector& InLocation)
	  : Location(InLocation)
	{}

	/* Simple get Location pure function. */
	FORCEINLINE const FVector&
	GetLocation() const
	{
		return Location;
	}

	/* Simple set Location pure function. */
	FORCEINLINE void
	SetLocation(const FVector& InLocation)
	{
		Location = InLocation;
	}
};
