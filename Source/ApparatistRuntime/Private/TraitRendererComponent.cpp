// 2022 Vladislav Dmitrievich Turbanov

#include "TraitRendererComponent.h"

#include "Located.h"
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

	const auto Mechanism = UMachine::ObtainMechanism(GetWorld());

	// Register the new subjects...
	FFilter Filter(FLocated::StaticStruct());
	Filter += TraitType;
	Filter.Exclude<FRendering>();
	Mechanism->Enchain(Filter)->Operate(
	[=](FSubjectHandle Subject, FLocated Located)
	{
		FTransform SubjectTransform(
			FQuat::Identity,
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
	Filter = FFilter::Make<FLocated, FRendering>();
	Filter += TraitType;
	Mechanism->Enchain(Filter)->Operate(
	[=](FLocated Located, FRendering Rendering)
	{
		FTransform SubjectTransform(
			FQuat::Identity,
			Located.Location,
			Scale);
		ValidTransforms.SetAt(Rendering.InstanceId, true);
		Transforms[Rendering.InstanceId] = SubjectTransform;
	});

	FreeTransforms.Reset();
	for (int32 i = 0; i < Transforms.Num(); ++i)
	{
		if (!ValidTransforms[i])
		{
			FreeTransforms.Add(i);
			Transforms[i].SetScale3D(FVector::ZeroVector);
		}
	}

	if (Transforms.Num() > 0)
	{
		BatchUpdateInstancesTransforms(0, Transforms, true, true, /*bTeleport=*/false);
	}
}
