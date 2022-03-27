// 2022 Vladislav Dmitrievich Turbanov

#include "TraitRendererComponent.h"

#include "Located.h"
#include "Oriented.h"
#include "Rendering.h"


UTraitRendererComponent::UTraitRendererComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UTraitRendererComponent::TickComponent(
	float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bFirstTick)
	{
		while (GetInstanceCount())
		{
			RemoveInstance(0);
		}
		bFirstTick = false;
	}

	const auto Mechanism = UMachine::ObtainMechanism(GetWorld());

	// Register the new subjects...
	FFilter Filter = FFilter::Make<FLocated, FOriented>();
	Filter += TraitType;
	Filter.Exclude<FRendering>();
	Mechanism->Enchain(Filter)->Operate(
	[=](FSubjectHandle Subject, FLocated Located, FOriented Oriented)
	{
		FTransform SubjectTransform(
			Oriented.Orientation.Rotation().Quaternion(),
			Located.Location,
			Scale);
		
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

		Subject.SetTrait<FRendering>(FRendering(Id));
	});

	// Update the positions...
	ValidTransforms.Reset();
	Filter = FFilter::Make<FLocated, FOriented, FRendering>();
	Filter += TraitType;
	Mechanism->Enchain(Filter)->Operate(
	[=](FLocated Located, FOriented Oriented, FRendering Rendering)
	{
		FTransform SubjectTransform(
			Oriented.Orientation.Rotation().Quaternion(),
			Located.Location,
			Scale);
		ValidTransforms[Rendering.InstanceId] = true;
		Transforms[Rendering.InstanceId] = SubjectTransform;
	});

	// Zero-down the unoccupied transforms...
	FreeTransforms.Reset();
	for (int32 i = ValidTransforms.IndexOf(false); i < Transforms.Num();
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
