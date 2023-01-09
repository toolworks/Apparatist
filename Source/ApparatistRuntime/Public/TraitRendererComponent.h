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

	/**
	 * The type of the trait that should trigger its instance to be rendered.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, NoClear,
			  Category = "TraitRenderer")
	UScriptStruct* TraitType = nullptr;

	/**
	 * The global scale for the rendered instances.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, 
			  Category = "TraitRenderer")
	FVector Scale = {1.0f, 1.0f, 1.0f};

	void
	EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void
	TickComponent(float DeltaTime, enum ELevelTick TickType,
				  FActorComponentTickFunction* ThisTickFunction) override;

  public:

	UTraitRendererComponent();
};
