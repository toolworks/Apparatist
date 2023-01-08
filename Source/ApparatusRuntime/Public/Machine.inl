/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Machine.inl
 * Created: 2022-04-16 16:53:57
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#pragma region Detail Registration

FORCEINLINE FDetailInfo&
UMachine::DoObtainDetailInfo(const TSubclassOf<UDetail> DetailClass)
{
	checkf(DetailClass, TEXT("The detail class must be provided to get the info about."));

	FDetailInfo* InfoPtr = Details.Find(DetailClass);

	if (LIKELY(InfoPtr != nullptr))
	{
		return *InfoPtr;
	}

	// A static is used here, cause the id has to be consistently
	// incremented for correctly quering the base class infos in the constructor...
	static int32 DetailId = FDetailInfo::FirstId;

	return Details.Add(DetailClass, FDetailInfo(DetailClass, DetailId++));
}

FORCEINLINE int32
UMachine::DoObtainDetailId(const TSubclassOf<UDetail> DetailClass)
{
	checkf(DetailClass, TEXT("The detail class must be provided to get the id of."));

	return DoObtainDetailInfo(DetailClass).Id;
}

FORCEINLINE const FBitMask&
UMachine::DoObtainDetailMask(const TSubclassOf<UDetail> DetailClass)
{
	checkf(DetailClass, TEXT("The detail class must be provided to get the mask for."));

	return DoObtainDetailInfo(DetailClass).Mask;
}

FORCEINLINE const FBitMask&
UMachine::DoObtainExcludedDetailMask(const TSubclassOf<UDetail> DetailClass)
{
	checkf(DetailClass, TEXT("The detail class must be provided to get the excluded mask for."));

	return DoObtainDetailInfo(DetailClass).ExcludedMask;
}

#pragma endregion Detail Registration

#include "SubjectHandle.h"

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm>
UMachine::DoDespawnSubject(const FSubjectHandle& SubjectHandle)
{
	const auto Info = SubjectHandle.FindInfo();

	if (LIKELY(Info))
	{
		// The subject is fine to be released now:
		return DoReleaseSubjectInfo(Info->Id);
	}

	// The subject is already despawned or does not exist.
	return EApparatusStatus::Noop;
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
UMachine::DoReset()
{
	EApparatusStatus Status = EApparatusStatus::Noop;

	// Despawn the subjects. This should also
	// invalidate the subjectives...
	StatusAccumulate(Status, DoDespawnAllSubjects<MakePolite(Paradigm)>());
	StatusAccumulate(Status, DoUnregisterAllMechanisms());
	StatusAccumulate(Status, DoClearCache());

	// The types and classes registry...
	if (Traits.Num())
	{
		Traits.Reset();
		StatusAccumulate(Status, EApparatusStatus::Success);
	}
	if (Details.Num())
	{
		Details.Reset();
		StatusAccumulate(Status, EApparatusStatus::Success);
	}

	// Clear the network identifier assignment state....
	{
		FScopeLock Lock(&NetworkIdsCS);
		NetworkIdsPools.Reset();
		NextNetworkId = FSubjectNetworkState::FirstId;
	}

	// The self is no longer needed for sure...
	check(!ShouldBeRetained());

	return Status;
}

template < EParadigm Paradigm/*=EParadigm::Default*/ >
inline TOutcome<Paradigm>
UMachine::CopyTraitsFromTo(const FSubjectHandle& SrcSubjectHandle,
						   const FSubjectHandle& DstSubjectHandle)
{
	const FSubjectInfo* const SrcInfo = SrcSubjectHandle.FindInfo();
	AssessConditionFormat(Paradigm, SrcInfo, EApparatusStatus::InvalidArgument,
				 TEXT("A subject to copy the traits from is not a valid one. Was it despawned already?"));
	FSubjectInfo* const DstInfo = DstSubjectHandle.FindInfo();
	AssessConditionFormat(Paradigm, DstInfo, EApparatusStatus::InvalidArgument,
				 TEXT("A subject to copy the traits to is not a valid one. Was it despawned already?"));
	return SrcInfo->template CopyTraitsTo<Paradigm>(DstInfo);
}

template < EParadigm Paradigm/*=EParadigm::Default*/ >
FORCEINLINE TOutcome<Paradigm>
UMachine::Reset()
{
	if (!HasInstance()) return EApparatusStatus::Noop;
	return Instance->template DoReset<Paradigm>();
}

#include "Mechanism.h"

inline AMechanism*
UMachine::DoObtainMechanism(UPackage* const InPackage)
{
	check(InPackage);
	const auto MechanismPtr = DefaultMechanismByPackageCache.Find(InPackage);
	if (LIKELY(MechanismPtr && (*MechanismPtr)))
	{
		return *MechanismPtr;
	}

	auto Base = FindObjectWithOuter(InPackage, AMechanism::StaticClass());
	auto Mechanism = static_cast<AMechanism*>(Base);
	if (UNLIKELY(Mechanism == GetDefault<AMechanism>() || !Mechanism))
	{
		// Create a new mechanism for the package:
		Mechanism = NewObject<AMechanism>(InPackage);
	}
	// Do not add the mechanism to the registry
	// to not hold it back from the normal GC.
	// If the user wants to save the instance,
	// this has to be done by an explicit GC pointer.
	return DefaultMechanismByPackageCache.Add(InPackage, Mechanism);
}

inline AMechanism*
UMachine::DoObtainMechanism(UWorld* const InWorld)
{
	check(InWorld);
	auto* MechanismPtr = DefaultMechanismByWorldCache.Find(InWorld);
	if (LIKELY(MechanismPtr && (*MechanismPtr)))
	{
		return *MechanismPtr;
	}
	auto* Mechanism = Cast<AMechanism>(
		UGameplayStatics::GetActorOfClass(InWorld, AMechanism::StaticClass()));
	if (LIKELY(!Mechanism))
	{
		// Create a new mechanism for the world:
		FActorSpawnParameters Params;
		Params.Name = TEXT("AutoMechanism");
		Mechanism = InWorld->SpawnActor<AMechanism>(Params);
	}
	// Add to the cache:
	return DefaultMechanismByWorldCache.Add(InWorld, Mechanism);
}

inline void
UMachine::OnWorldCleanup(UWorld* const InWorld,
						 bool          bSessionEnded,
						 bool          bCleanupResources)
{
	if (UNLIKELY(!InWorld)) return;
	AMechanism* Mechanism = nullptr;
	auto* MechanismPtr = DefaultMechanismByWorldCache.Find(InWorld);
	if (LIKELY(MechanismPtr && (*MechanismPtr)))
	{
		Mechanism = *MechanismPtr;
	}
	else
	{
		// Try to free up for the auto-generated mechanism.
		Mechanism = FindObject<AMechanism>(InWorld, TEXT("AutoMechanism"), /*ExactClass=*/true);
	}
	if (LIKELY(Mechanism))
	{
		// Though not really needed, since this should be done
		// during the disposal of the mechanism...
		verify(OK(DoDespawnSubjects(Mechanism)));
	}
	DefaultMechanismByWorldCache.Remove(InWorld);
}

template < EParadigm Paradigm/*=EParadigm::Internal*/ >
TOutcome<Paradigm>
UMachine::MoveSubject(const FSubjectInfo::IdType SubjectId,
					  UChunk* const              NewChunk)
{
	AssessConditionFormat(Paradigm,
				 SubjectId != InvalidSubjectId,
				 EApparatusStatus::InvalidArgument,
				 TEXT("Moving an invalid subject is not supported."));
	AssessConditionFormat(Paradigm,
				 NewChunk != nullptr,
				 EApparatusStatus::NullArgument,
				 TEXT("You must submit a valid chunk to move to."));

	auto& Info = GetSubjectInfo(SubjectId);
	const auto OldChunk = Info.Chunk;
	if (UNLIKELY(NewChunk == OldChunk))
	{
		// No actual change. The chunks are the same.
		return EApparatusStatus::Noop;
	}
	AssessConditionFormat(Paradigm,
				 !NewChunk->IsSolid(),
				 EApparatusStatus::InvalidArgument,
				 TEXT("You must provide a non-solid chunk to move to."));
	const auto OldIndex = Info.SlotIndex;
	const auto NewIndex = NewChunk->template ReserveSubjectSlot<MakeInternal(Paradigm)>();

	AssessConditionOKf(Paradigm, NewIndex,
				   TEXT("Failed to spawn the subject within its destination #%d chunk."),
				   (int)NewChunk->Id);

	auto& NewSlot = NewChunk->Slots[NewIndex];
	AssessCondition(Paradigm, NewSlot.IsStale(),
				EApparatusStatus::InvalidState);

	if (LIKELY(OldChunk))
	{
		check(OldIndex > FSubjectInfo::InvalidSlotIndex);
		auto& OldSlot = OldChunk->Slots[OldIndex];
		AssessConditionFormat(Paradigm,
					 !OldSlot.IsStale(),
					 EApparatusStatus::InvalidState,
					 TEXT("Moving of the stale subjects is not supported."));

		// Swap the traits with the new chunk's ones.
		// The former chunk's exclusive traits get cleared automatically.
		OldChunk->SwapTraits(OldIndex, NewChunk, NewIndex);

		// Swap the fingerprints instead of assignment for performance reasons.
		// Do not swap the handles though, as the former
		// slot have to reference the correct subject still
		// when during the iteration.
		// Note that the stale flag gets swapped to the old
		// slot from the new (still uninitialized) one.
		FMoreMemory::Memswap(NewSlot.Fingerprint, OldSlot.Fingerprint);

		// Release the subject from the former chunk.
		// We're using the low-level version since the swap
		// should've already de-initialized the traits.
		verifyf(OK(OldChunk->DoReleaseSlot(OldIndex)),
				TEXT("Failed to release the #%d ")
				TEXT("subject from the previous chunk #%d"),
				OldIndex, OldChunk->Id);
	}
	else
	{
		NewSlot.Fingerprint.SetFlag(EFlagmarkBit::Stale, false);
	}

	// Update the info for the new chunk:
	Info.Chunk     = NewChunk;
	Info.SlotIndex = NewIndex;
	check(Info.IsValid());

	// Make sure to update the traitmark of the new slot,
	// since it may actually be erroneous after the swap:
	AssessOK(Paradigm, NewSlot.Fingerprint.template SetTraitmark<Paradigm>(NewChunk->GetTraitmark()));

	// Set the subject id within a new chunk accordingly:
	NewSlot.SubjectId = Info.GetId(); // Don't use assignment here, cause of the previous intrusive fingerprint swap.
	check(!NewSlot.IsStale());

	// Apply the matching adjectives within a new chunk:
	NewChunk->ApplyAdjectives(&Info);

	return EApparatusStatus::Success;
}
