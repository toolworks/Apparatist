// 2022 Vladislav Dmitrievich Turbanov

#pragma once

#include "CoreMinimal.h"

#include "Rendering.generated.h"


/**
 * Struct representing a render buffer state of the object,
 * when the object currently rendering.
 */
USTRUCT(Category = "TraitRenderer")
struct APPARATISTRUNTIME_API FRendering
{
	GENERATED_BODY()

  public:

	/* The unique identifier of the object. */
	int32 InstanceId = -1;

	/* Default constructor. */
	FRendering() {}

	/* Constructor with all variables specified. */
	FRendering(const int32 InInstanceId)
	  : InstanceId(InInstanceId)
	{}
};
