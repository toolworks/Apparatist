// 2022 Vladislav Dmitrievich Turbanov

#include "TraitRendererComponent.h"

#include "Located.h"
#include "Directed.h"
#include "Rotated.h"
#include "Rendering.h"
#include "Scaled.h"


TMap<UScriptStruct*, UTraitRendererComponent*> UTraitRendererComponent::InstancesByTraitTypes;

UTraitRendererComponent::UTraitRendererComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void
UTraitRendererComponent::BeginPlay()
{
	Super::BeginPlay();

	if (TraitType != nullptr)
	{
		InstancesByTraitTypes.Add(TraitType, this);
	}
}

void
UTraitRendererComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FFilter Filter = FFilter::Make<FLocated, FRendering>();
	Filter += TraitType;

	const auto World = GetWorld();
	if (World != nullptr && EndPlayReason != EEndPlayReason::EndPlayInEditor)
	{
		const auto Mechanism = UMachine::ObtainMechanism(World);
		Mechanism->Enchain(Filter)->Operate(
		[](FSubjectHandle Subject)
		{
			Subject.RemoveTrait<FRendering>();
		});
	}

	if (TraitType != nullptr)
	{
		InstancesByTraitTypes.Remove(TraitType);
	}

	if (AsyncRenderStateUpdateTask != nullptr)
	{
		AsyncRenderStateUpdateTask->EnsureCompletion();
		delete AsyncRenderStateUpdateTask;
		AsyncRenderStateUpdateTask = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void UTraitRendererComponent::TickComponent(
	float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bManualUpdate)
	{
		Update();
	}
}

void UTraitRendererComponent::Update()
{
	if (bFirstUpdate)
	{
		// Make sure there are no instances yet in the renderer...
		while (GetInstanceCount())
		{
			RemoveInstance(0);
		}
		bFirstUpdate = false;
	}

	const auto Mechanism = UMachine::ObtainMechanism(GetWorld());

	// Register the new subjects...
	FFilter Filter = FFilter::Make<FLocated>();
	Filter += TraitType;
	Filter.Exclude<FRendering>();
	Mechanism->Enchain<FUnsafeChain>(Filter)->Operate(
	[=, this](FUnsafeSubjectHandle Subject, const FLocated& Located, const FDirected* Directed, const FRotated* Rotated, const FScaled* Scaled)
	{
		FQuat Rotation{FQuat::Identity};
		if (Directed)
		{
			Rotation = Directed->Direction.Rotation().Quaternion();
		}
		if (Rotated)
		{
			Rotation *= Rotated->Rotation;
		}
		FVector FinalScale(this->Scale);
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
	[=, this](FSolidSubjectHandle Subject, FLocated Located, FRendering Rendering, FDirected* Directed, FRotated* Rotated, FScaled* Scaled)
	{
		FQuat Rotation{ FQuat::Identity };
		if (Directed)
		{
			Rotation = Directed->Direction.Rotation().Quaternion();
		}
		if (Rotated)
		{
			Rotation *= Rotated->Rotation;
		}
		FVector FinalScale(this->Scale);
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

	if (!bManualRenderStateUpdate)
	{
		UpdateRenderState();
	}
}

void UTraitRendererComponent::UpdateRenderState()
{
	if (Transforms.Num() > 0)
	{
		BatchUpdateInstancesTransforms(0, Transforms, true, /*bMarkRenderStateDirty=*/true, /*bTeleport=*/bUpdateViaTeleport);
	}
}
