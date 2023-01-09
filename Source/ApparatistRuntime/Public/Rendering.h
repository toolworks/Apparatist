// 2022 Vladislav Dmitrievich Turbanov

#pragma once

#include "CoreMinimal.h"

#include "Rendering.generated.h"


// Forward declarations:
class UTraitRendererComponent;

/**
 * Struct representing a render buffer state of the subject,
 * when the it is currently rendering.
 */
USTRUCT(Category = "TraitRenderer")
struct APPARATISTRUNTIME_API FRendering
{
	GENERATED_BODY()

  public:

	/**
	 * The owning trait renderer component.
	 */
	UTraitRendererComponent* Owner = nullptr;

	/**
	 * The unique identifier of the object. 
	 */
	int32 InstanceId = -1;

	/* Default constructor. */
	FRendering() {}

	/**
	 * Initialize a new rendering instance.
	 */
	FRendering(UTraitRendererComponent* const InOwner,
			   const int32                    InInstanceId)
	  : Owner(InOwner)
	  , InstanceId(InInstanceId)
	{}
};
