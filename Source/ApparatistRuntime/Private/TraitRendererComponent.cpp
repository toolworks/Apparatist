// 2022 Vladislav Dmitrievich Turbanov

#include "TraitRendererComponent.h"

#include "Located.h"
#include "Oriented.h"
#include "Rotated.h"
#include "Rendering.h"
#include "Scaled.h"


UTraitRendererComponent::UTraitRendererComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UTraitRendererComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FFilter Filter = FFilter::Make<FLocated, FRendering>();
	Filter += TraitType;

	if (EndPlayReason != EEndPlayReason::EndPlayInEditor)
	{
		const auto Mechanism = UMachine::ObtainMechanism(GetWorld());
		Mechanism->Enchain(Filter)->Operate(
		[](FSubjectHandle Subject)
		{
			Subject.RemoveTrait<FRendering>();
		});
	}
}

void UTraitRendererComponent::TickComponent(
	float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bFirstTick)
	{
		// Make sure there are no instances yet in the renderer...
		while (GetInstanceCount())
		{
			RemoveInstance(0);
		}
		bFirstTick = false;
	}

	const auto Mechanism = UMachine::ObtainMechanism(GetWorld());

	// Register the new subjects...
	FFilter Filter = FFilter::Make<FLocated>();
	Filter += TraitType;
	Filter.Exclude<FRendering>();
	Mechanism->Enchain<FUnsafeChain>(Filter)->Operate(
	[=](FUnsafeSubjectHandle Subject, const FLocated& Located, const FOriented* Oriented, const FRotated* Rotated, const FScaled* Scaled)
	{
		FQuat Rotation{FQuat::Identity};
		if (Oriented)
		{
			Rotation = Oriented->Orientation.Rotation().Quaternion();
		}
		if (Rotated)
		{
			Rotation *= Rotated->Rotation;
		}
		FVector FinalScale(Scale);
		if (Scaled)
		{
			FinalScale *= Scaled->Factors;
		}
		FTransform SubjectTransform(
			Rotation,
			Located.Location,
			FinalScale);
		
		int32 Id = INDEX_NONE;
		if (FreeTransforms.Num())
		{
			Id = FreeTransforms.Pop();
			Transforms[Id] = SubjectTransform;
		}
		else
		{
			Id = AddInstance(SubjectTransform);
			Transforms.AddDefaulted_GetRef() = SubjectTransform;
		}

		Subject.SetTrait(FRendering(this, Id));
	});

	// Update the positions...
	ValidTransforms.Reset();
	Filter = FFilter::Make<FLocated, FRendering>();
	Filter += TraitType;
	Mechanism->EnchainSolid(Filter)->Operate(
	[=](FSolidSubjectHandle Subject, FLocated Located, FRendering Rendering, FOriented* Oriented, FRotated* Rotated, FScaled* Scaled)
	{
		FQuat Rotation{ FQuat::Identity };
		if (Oriented)
		{
			Rotation = Oriented->Orientation.Rotation().Quaternion();
		}
		if (Rotated)
		{
			Rotation *= Rotated->Rotation;
		}
		FVector FinalScale(Scale);
		if (Scaled)
		{
			FinalScale *= Scaled->Factors;
		}
		FTransform SubjectTransform(
			Rotation,
			Located.Location,
			FinalScale);
		ValidTransforms[Rendering.InstanceId] = true;
		Transforms[Rendering.InstanceId] = SubjectTransform;
	});

	// Zero-down the unoccupied transforms...
	FreeTransforms.Reset();
	for (int32 i = ValidTransforms.IndexOf(false); 
			   i < Transforms.Num();
			   i = ValidTransforms.IndexOf(false, i + 1))
	{
		FreeTransforms.Add(i);
		Transforms[i].SetScale3D(FVector::ZeroVector);
	}

	if (Transforms.Num() > 0)
	{
		BatchUpdateInstancesTransforms(0, Transforms, true, true, /*bTeleport=*/false);
	}
}
