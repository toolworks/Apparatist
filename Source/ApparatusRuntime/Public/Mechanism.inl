/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Mechanism.inl
 * Created: 2022-04-16 18:52:39
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

FORCEINLINE AMechanism::SubjectIdType
AMechanism::SubjectsNum() const
{
	if (LIKELY((this->MechanismId != InvalidId) && UMachine::HasInstance()))
	{
		return UMachine::Instance->SubjectPools[this->MechanismId].SubjectsNum();
	}
	return 0;
}

template < EParadigm Paradigm/*=EParadigm::Default*/ >
inline TOutcome<Paradigm, FSubjectHandle>
AMechanism::SpawnSubject(const FSubjectRecord& SubjectRecord)
{
	const auto SubjectHandle = DoSpawnSubject<Paradigm>(SubjectRecord.GetTraitmark(), SubjectRecord.GetFlagmark());
	if (AvoidError(Paradigm, SubjectHandle))
	{
		return SubjectHandle;
	}
	if (AvoidError(Paradigm, SubjectHandle->template OverwriteTraits<Paradigm>(SubjectRecord)))
	{
		return MakeOutcome<Paradigm, FSubjectHandle>(FApparatusStatus::GetLastError(), FSubjectHandle::Invalid);
	}
	return SubjectHandle;
}

template < EParadigm Paradigm/*=EParadigm::Default*/ >
FORCEINLINE TOutcome<Paradigm>
AMechanism::DoSpawnSubjectDeferred(const FSubjectRecord& SubjectRecord)
{
	EnqueueDeferred(FDeferredSubjectSpawn(SubjectRecord));
	return EApparatusStatus::Deferred;
}

template < EParadigm Paradigm/*=EParadigm::Default*/ >
FORCEINLINE TOutcome<Paradigm>
AMechanism::DespawnAllSubjects()
{
	if (LIKELY(UMachine::HasInstance()))
	{
		return UMachine::Instance->template DoDespawnSubjects<Paradigm>(this);
	}
	return EApparatusStatus::Noop;
}

template < EParadigm Paradigm/*=EParadigm::Default*/ >
inline TOutcome<Paradigm>
AMechanism::UnregisterSubjective(ISubjective* const Subjective)
{
	AssessConditionFormat(Paradigm, Subjective, EApparatusStatus::NullArgument,
				 TEXT("A subjective to unregister must not be a 'nullptr'."));

	const auto Slot = Subjective->GetSlotPtr();
	if (LIKELY(Slot))
	{
		// A slot is assigned to a subjective...
		const auto Belt = Slot->GetOwner();
		if (LIKELY(Belt))
		{
			AssessOK(Paradigm, Belt->ReleaseSlotAt(Slot->GetIndex()));
		}
		else
		{
			// The slot of some not-valid belt.
			// Just mark the slot as invalid:
			Subjective->TakeBeltSlot(nullptr);
		}
		Subjectives.Remove(Subjective);
	}

	check(!Subjective->GetBelt());

	// Despawn the corresponding subject...
	if (LIKELY(Subjective->Handle))
	{
		AssessOK(Paradigm, Subjective->Handle.template Despawn<Paradigm>());
		const auto Object = Cast<UObject>(Subjective);
		if (LIKELY(Object))
		{
			const auto World = Object->GetWorld();
			if (World && (World->IsNetMode(NM_DedicatedServer) || World->IsNetMode(NM_ListenServer)) && Subjective->ShouldBeReplicated())
			{
				Subjective->AssignNetworkIdOnClient(FSubjectNetworkState::InvalidId);
			}
		}
	}

	return EApparatusStatus::Success;
}

template < EParadigm Paradigm/*=EParadigm::Default*/ >
inline TOutcome<Paradigm>
AMechanism::UnregisterAllSubjectives()
{
	EApparatusStatus Status = EApparatusStatus::Noop;

	auto Copy = Subjectives.Array();

	while (Copy.Num() > 0)
	{
		const auto s = Copy.Pop(/*Allow shrinking=*/false);
		if (UNLIKELY(!s)) continue;
		StatusAccumulate(Status, s->template DoUnregister<MakePolite(Paradigm)>());
	}

	Subjectives.Reset(); 

	return Status;
}

FORCEINLINE FSubjectInfo&
AMechanism::GetSubjectInfoByNetworkId(const uint32 NetworkId)
{
	check(NetworkId != FSubjectNetworkState::InvalidId);
	return UMachine::GetSubjectInfo(SubjectByNetworkId[NetworkId]);
}

FORCEINLINE FSubjectInfo*
AMechanism::FindSubjectInfoByNetworkId(const uint32 NetworkId)
{
	const auto SubjectIdPtr = SubjectByNetworkId.Find(NetworkId);
	if (UNLIKELY(SubjectIdPtr == nullptr))
	{
		return nullptr;
	}
	return UMachine::FindSubjectInfo(*SubjectIdPtr);
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
AMechanism::RegisterSubjective(ISubjective* const Subjective)
{
	AssessConditionFormat(Paradigm, Subjective, EApparatusStatus::NullArgument,
				 TEXT("A subjective to register must not be a 'nullptr'."));
	
	// Subjective may already have a valid handle assigned,
	// cause of the preceding cloning, for example...
	const auto FormerMechanism = Subjective->Handle.GetMechanism();
	if (UNLIKELY(this == FormerMechanism))
	{
		return EApparatusStatus::Noop;
	}

	const auto World = GetWorld();
	const auto CurrentWorld = CastChecked<UObject>(Subjective)->GetWorld();
	AssessConditionFormat(Paradigm, World == CurrentWorld, EApparatusStatus::InvalidState,
				 TEXT("Registering subjectives is only allowed within a corresponding world's mechanism."));

	const bool bCleanStart = FormerMechanism == nullptr;;
	if (LIKELY(bCleanStart))
	{
		// The subject handle was not assigned yet.
		// So spawn it anew.
		const auto SpawnOutcome = DoSpawnSubject<MakeSafe(Paradigm)>(Subjective->DoGetTraitRecordsTraitmark(),
																	 Subjective->GetFlagmarkPropRef());
		AssessConditionOK(Paradigm, SpawnOutcome);
		Subjective->Handle = SpawnOutcome;
	}
	else
	{
		// Unregister the subjective part from the former mechanism...
		FormerMechanism->Subjectives.Remove(Subjective);
	}

	// Add to the global set of subjectives:
	Subjectives.Add(Subjective);

	auto& SubjectInfo = Subjective->Handle.GetInfo();
	auto& Fingerprint = SubjectInfo.GetFingerprintRef();
	SubjectInfo.Subjective = Subjective;

	if (LIKELY(bCleanStart))
	{
		// Set all of the configured trait entries...
		auto& TraitRecords = Subjective->GetTraitRecordsRef();
		for (auto& TraitRecord : TraitRecords)
		{
			AssessFinalizeOK(Paradigm, Subjective->Handle.template SetTrait<Paradigm>(TraitRecord),
							 Subjective->Handle.Despawn());
		}
	}
	else
	{
		// Move the subject to the new mechanism.
		AssessOK(Paradigm, UMachine::ObtainInstance()->template MoveSubject<Paradigm>(Subjective->Handle.GetId(),
																					  this->template ObtainChunk<Paradigm>(Fingerprint)));
	}

	if (LIKELY(World))
	{
		if (Subjective->ShouldBeReplicated())
		{
			AssessCondition(Paradigm, Subjective->IsNetworkCapable(), EApparatusStatus::WrongType);
			if (LIKELY(!SubjectInfo.IsOnline()))
			{
				auto& NetProps = Subjective->GetStandardNetworkPropertyBlock();
				auto NetworkState = SubjectInfo.ObtainNetworkState(NetProps.SubjectNetworkId);
				if (UNLIKELY(NetProps.SubjectNetworkId == FSubjectNetworkState::InvalidId))
				{
					// The network identifier is not allocated yet.
					// Allocate it now.
					if (World->IsNetMode(NM_DedicatedServer) || World->IsNetMode(NM_ListenServer))
					{
						Subjective->AssignNetworkIdOnClient(NetworkState->ServerObtainId());
					}
					else if (World->IsNetMode(NM_Client))
					{
						Subjective->ObtainNetworkIdFromServer();
					}
				}
				// Copy the traitmark security filter:
				NetworkState->TraitmarkPermit = NetProps.TraitmarkPermit;
			}
		}
	}

	const TArray<UDetail*>& SubjectiveDetails = Subjective->GetDetailsRef();
	Fingerprint.Add(SubjectiveDetails);

	// Register the subjective within a belt:
	AssessFinalizeOK(Paradigm, ActualizeBelt<Paradigm>(Subjective),
					 Subjective->Handle.Despawn());
	check(Subjective->IsRegistered());

	// Call the events for active details now.
	// More traits/details can be added during the activation,
	// so we iterate via an index here...
	for (int32 i = 0; i < SubjectiveDetails.Num(); ++i)
	{
		if (UNLIKELY(!SubjectiveDetails[i]))
			continue;
		if (!SubjectiveDetails[i]->IsEnabled())
			continue;
		// This can trigger more details added:
		SubjectiveDetails[i]->Activated();
	}

	return EApparatusStatus::Success;
}

inline EApparatusStatus
AMechanism::CollectSubjects(USubjectRecordCollection* const OutSubjects,
							const FFilter&                  Filter/*=FFilter::Default*/,
							const EFlagmark                 FlagmarkMask/*=FM_AllUserLevel*/)
{
	check(OutSubjects);

	if (UNLIKELY(MechanismId == InvalidId))
	{
		// Not yet registered:
		return EApparatusStatus::Noop;
	}
	const auto Machine = UMachine::Instance;
	if (UNLIKELY(Machine == nullptr))
	{
		return EApparatusStatus::Noop;
	}

	if (Filter.Includes(EFlagmarkBit::Booted))
	{
		BootAll();
	}

	auto Status = EApparatusStatus::Noop;
	const auto& SubjectPool = Machine->DoGetSubjectPool(MechanismId);
	for (UMachine::SubjectIdType i = UMachine::FirstSubjectPlace;
								 i < SubjectPool.Subjects.Num();
							   ++i)
	{
		const auto& Info = SubjectPool.Subjects[i];
		if (Info.IsValid() && Info.GetFingerprint().Matches(Filter))
		{
			checkSlow(Info.GetMechanism() == this);
			OutSubjects->Add(Info.GetHandle(), FlagmarkMask);
			Status = EApparatusStatus::Success;
		}
	}

	return Status;
}

template < typename AllocatorT >
inline EApparatusStatus
AMechanism::CollectSubjects(TArray<FSubjectHandle, AllocatorT>& OutSubjects,
							const FFilter&                      Filter/*=FFilter::Default*/)
{
	if (UNLIKELY(MechanismId == InvalidId))
	{
		// Not yet registered:
		return EApparatusStatus::Noop;
	}

	const auto Machine = UMachine::Instance;
	if (UNLIKELY(Machine == nullptr))
	{
		return EApparatusStatus::Noop;
	}

	if (Filter.Includes(EFlagmarkBit::Booted))
	{
		BootAll();
	}

	auto Status = EApparatusStatus::Noop;

	const auto& SubjectPool = Machine->DoGetSubjectPool(MechanismId);
	for (UMachine::SubjectIdType i = UMachine::FirstSubjectPlace;
								 i < SubjectPool.Subjects.Num();
							   ++i)
	{
		const auto& Info = SubjectPool.Subjects[i];
		if (Info.IsValid() && Info.GetFingerprint().Matches(Filter))
		{
			check(Info.GetMechanism() == this);
			OutSubjects.Add(Info.GetHandle());
			Status = EApparatusStatus::Success;
		}
	}

	return Status;
}

inline void
AMechanism::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	this->Dispose();

	Super::EndPlay(EndPlayReason);
}

inline void
AMechanism::BeginDestroy()
{
	this->Dispose();

	Super::BeginDestroy();
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm, FSubjectHandle>
AMechanism::DoSpawnSubject(const FTraitmark& Traitmark,
						   EFlagmark         Flagmark/*=FM_None*/)
{
	if (!IsInternal(Paradigm)) // Compile-time branch.
	{
		Flagmark &= FM_AllUserLevel;
	}

	// All of the subjects are spawned within an empty-trait chunk by default:
	const auto Chunk = ObtainChunk<Paradigm>(Traitmark);
	if (!OK(Chunk))
	{
		return MakeOutcome<Paradigm, FSubjectHandle>(ToStatus(Chunk), FSubjectHandle::Invalid);
	}

	// Reserve a slot within an empty-trait chunk...
	const auto SlotIndex = Chunk->template ReserveSubjectSlot<Paradigm>();
	if (!OK(SlotIndex))
	{
		return MakeOutcome<Paradigm, FSubjectHandle>(ToStatus(SlotIndex), FSubjectHandle::Invalid);
	}

	auto& Slot = Chunk->Slots[SlotIndex];
	check(Slot.IsStale());

	const auto Info = UMachine::template AllocateSubjectInfo<Paradigm>(this);
	if (!OK(Info))
	{
		return MakeOutcome<Paradigm, FSubjectHandle>(ToStatus(Info), FSubjectHandle::Invalid);
	}

	// Fully initialize the subject information structure,
	// for some checks to pass later...
	Info->Chunk     = Chunk;
	Info->SlotIndex = SlotIndex;
	check(Info->IsValid());

	const auto SubjectHandle = Info->GetHandle();
	check(SubjectHandle.IsValid());

	// Register as a halted (not booted) subject at start:
	HaltedSubjects.Add(SubjectHandle);

	// Set the subject within the chunk:
	Slot = Info;
	check(!Slot.IsStale());
	Slot.Fingerprint.AddToFlagmark(Flagmark);
#if WITH_EDITOR
	const auto World = GetWorld();
	if (World && !World->IsGameWorld())
	{
		// Safely mark an Editor-based subject:
		Slot.Fingerprint.AddToFlagmark(FM_Editor);
	}
#endif
	Slot.Fingerprint.SetTraitmark(Traitmark);

	// TODO: Adjectives should be applied only after the traits are set.
	Chunk->ApplyAdjectives(Info);

	APPARATUS_REPORT_SUCCESS(TEXT("Spawned a new subject: #%d"), Info->Id);
	return MakeOutcome<Paradigm, FSubjectHandle>(EApparatusStatus::Success, SubjectHandle);
}
