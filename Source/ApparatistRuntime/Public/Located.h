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

	/** Initialize a default instance. */
	FORCEINLINE
	FLocated()
	{}

	/** Initialize with a vector. */
	FORCEINLINE
	FLocated(const FVector& InLocation)
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

	/**
	 * Implicit conversion to an immutable vector.
	 */
	FORCEINLINE
	operator const FVector&() const
	{
		return Location;
	}

	/**
	 * Implicit conversion to a mutable vector.
	 */
	FORCEINLINE
	operator FVector&()
	{
		return Location;
	}
};
