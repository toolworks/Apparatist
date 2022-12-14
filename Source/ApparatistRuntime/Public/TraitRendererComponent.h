// 2022 Vladislav Dmitrievich Turbanov

#pragma once

#include "Components/InstancedStaticMeshComponent.h"
#include "CoreMinimal.h"

#include "Machine.h"

#include "TraitRendererComponent.generated.h"


/**
 * Basic class for render subjects. To set up which subjects you want to render,
 * please, initialize the TraitType variable.
 */
UCLASS(ClassGroup = Apparatist, meta = (BlueprintSpawnableComponent),
	   Blueprintable, Category = "TraitRenderer")
class APPARATISTRUNTIME_API UTraitRendererComponent
  : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()

	/**
	 * All of the current transforms.
	 */
	TArray<FTransform> Transforms;

	/**
	 * Transforms that are actually used.
	 */
	FBitMask ValidTransforms;

	/**
	 * Transforms available for reuse.
	 */
	TArray<int32> FreeTransforms;


	bool bFirstTick = true;

  protected:

	/* This trait tells to the Renderer which objects it should render.
	 *   Please, for efficiency never give to different Renderers same
	 * TraitTypes.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, NoClear,
			  Category = "TraitRenderer")
	UScriptStruct* TraitType = nullptr;

	/* Scale with which to render objects. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, 
			  Category = "TraitRenderer")
	FVector Scale = {1.0f, 1.0f, 1.0f};

	void
	EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  public:

	void
	TickComponent(float DeltaTime, enum ELevelTick TickType,
				  FActorComponentTickFunction* ThisTickFunction) override;

	UTraitRendererComponent();
};
