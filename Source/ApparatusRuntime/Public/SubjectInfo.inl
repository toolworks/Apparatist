/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectInfo.inl
 * Created: 2022-04-16 18:48:00
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2022, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
FSubjectInfo::BringOnline()
{
	if (UNLIKELY(IsOnline()))
	{
		return EApparatusStatus::Noop;
	}
	const auto World = GetWorld();
	if (UNLIKELY(!ensureMsgf(World,
							 TEXT("Subject without world can't be made networked."))))
	{
		return EApparatusStatus::InvalidState;
	}

	if (UNLIKELY(!ensureMsgf(World->IsNetMode(NM_DedicatedServer) || World->IsNetMode(NM_ListenServer),
							 TEXT("Bringing networked subjects is only possible within the server."))))
	{
		return EApparatusStatus::NoPermission;
	}

	AssessOK(Paradigm, ObtainNetworkState());
	AssessOK(Paradigm, NetworkState->ServerObtainId());
	return SetFlag<MakeInternal(Paradigm)>(EFlagmarkBit::Online);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
FSubjectInfo::BringOnline(const TRange<SubjectNetworkIdType>& IdRange)
{
	if (UNLIKELY(IsOnline()))
	{
		return EApparatusStatus::Noop;
	}
	const auto World = GetWorld();
	if (UNLIKELY(!ensureMsgf(World,
							 TEXT("Subject without world can't be made networked."))))
	{
		return EApparatusStatus::InvalidState;
	}

	if (UNLIKELY(!ensureMsgf(World->IsNetMode(NM_DedicatedServer) || World->IsNetMode(NM_ListenServer),
							 TEXT("Bringing networked subjects is only possible within the server."))))
	{
		return EApparatusStatus::NoPermission;
	}

	AssessOK(Paradigm, ObtainNetworkState());
	AssessOK(Paradigm, NetworkState->ServerObtainId(IdRange));
	return SetFlag<MakeInternal(Paradigm)>(EFlagmarkBit::Online);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm, void*>
FSubjectInfo::GetTraitPtr(UScriptStruct* const TraitType)
{
	if (UNLIKELY(TraitType == nullptr))
	{
		return nullptr;
	}

	if (AvoidConditionFormat(
			Paradigm, Id == InvalidId,
			TEXT("Invalid subject identifier to get a trait of: #%d"), (int)Id))
	{
		return MakeOutcome<Paradigm, void*>(EApparatusStatus::InvalidState, nullptr);
	}
	if (AvoidConditionFormat(
			Paradigm, SlotIndex == FSubjectInfo::InvalidSlotIndex,
			TEXT("The subject to get a trait of is invalid - bad chunk index: %d"), (int)SlotIndex))
	{
		return MakeOutcome<Paradigm, void*>(EApparatusStatus::InvalidState, nullptr);
	}
	if (AvoidConditionFormat(
			Paradigm, Chunk == nullptr,
			TEXT("The subject to get a trait of is invalid: no chunk is set.")))
	{
		return MakeOutcome<Paradigm, void*>(EApparatusStatus::InvalidState, nullptr);
	}

	if (LIKELY(Chunk->GetTraitmark().Contains(TraitType)))
	{
		// The trait is there in the chunk. Get its data and return...
		return Chunk->TraitPtrAt(SlotIndex, TraitType);
	}

	return nullptr;
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm, const void*>
FSubjectInfo::GetTraitPtr(UScriptStruct* const TraitType) const
{
	if (UNLIKELY(TraitType == nullptr))
	{
		return nullptr;
	}

	if (AvoidConditionFormat(
			Paradigm, Id == InvalidId,
			TEXT("Invalid subject identifier to get a read-only trait of: #%d"), (int)Id))
	{
		return MakeOutcome<Paradigm, const void*>(EApparatusStatus::InvalidState, nullptr);
	}
	if (AvoidConditionFormat(
			Paradigm, SlotIndex == FSubjectInfo::InvalidSlotIndex,
			TEXT("The subject to get a read-only trait of is invalid - bad chunk index: %d"), (int)SlotIndex))
	{
		return MakeOutcome<Paradigm, const void*>(EApparatusStatus::InvalidState, nullptr);
	}
	if (AvoidConditionFormat(
			Paradigm, Chunk == nullptr,
			TEXT("The subject to get a read-only trait of is invalid: no chunk is set.")))
	{
		return MakeOutcome<Paradigm, const void*>(EApparatusStatus::InvalidState, nullptr);
	}

	if (LIKELY(Chunk->GetTraitmark().Contains(TraitType)))
	{
		// The trait is there in the chunk. Get its data and return...
		return Chunk->TraitPtrAt(SlotIndex, TraitType);
	}

	return nullptr;
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm>
FSubjectInfo::GetTrait(UScriptStruct* const TraitType,
					   void* const          OutTraitData,
					   const bool           bTraitDataInitialized) const
{
	if (UNLIKELY(TraitType == nullptr))
	{
		return EApparatusStatus::Noop;
	}
	if (AvoidConditionFormat(Paradigm, Id == InvalidId,
		  				 TEXT("Invalid subject identifier to get a trait of: #%d"), Id))
	{
		return EApparatusStatus::InvalidState;
	}
	if (AvoidConditionFormat(Paradigm, SlotIndex == FSubjectInfo::InvalidSlotIndex,
						 TEXT("The subject to get a trait of is invalid - bad chunk index: %d"), SlotIndex))
	{
		return EApparatusStatus::InvalidState;
	}
	if (AvoidConditionFormat(Paradigm, Chunk == nullptr,
						 TEXT("The subject to get a trait of is invalid: no chunk is set.")))
	{
		return EApparatusStatus::InvalidState;
	}

	if (LIKELY(Chunk->GetTraitmark().Contains(TraitType)))
	{
		// The trait is there in the chunk. Get its data and return...
		return Chunk->TraitAt(SlotIndex, TraitType, OutTraitData,
							  bTraitDataInitialized);
	}

	return EApparatusStatus::Missing;
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm>
FSubjectInfo::GetTrait(FTraitRecord& TraitRecord) const
{
	return GetTrait<Paradigm>(TraitRecord.GetType(), TraitRecord.GetData());
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
TOutcome<Paradigm>
FSubjectInfo::ObtainTrait(UScriptStruct* const TraitType)
{
	AssessCondition(Paradigm, TraitType != nullptr, EApparatusStatus::NullArgument);
	AssessConditionFormat(Paradigm, IsValid(), EApparatusStatus::InvalidState,
				 TEXT("The subject to obtain a trait from is invalid. "
					  "Was the subject already despawned?"));
	AssessConditionFormat(Paradigm, !IsSolid(), EApparatusStatus::InvalidState,
				 TEXT("Obtaining a trait from a subject is not possible during its solid state. "
					  "Do you have an unfinished solid iterating?"));
	AssessCondition(Paradigm, UMachine::HasInstance(), EApparatusStatus::InvalidState);

	const auto OldIndex = SlotIndex;
	const auto OldChunk = Chunk;
	auto& Fingerprint = GetFingerprintRef();
	// Assemble a new fingerprint.
	auto Status = Fingerprint.template Add<MakePolite(Paradigm)>(TraitType);
	AssessCondition(Paradigm, OK(Status), Status);
	if (Status == EApparatusStatus::Noop)
	{
		// The trait is already there in the chunk.
		return EApparatusStatus::Noop;
	}

#if WITH_EDITOR
	if (Subjective != nullptr)
	{
		// Make sure the trait record is added and
		// is visible in the Details panel during
		// the editing mode:
		Subjective->DoObtainTraitRecord(TraitType);
	}
#endif

	// No such trait in the current chunk...
	// Ensure the chunk with an updated target fingerprint.
	const auto Mechanism = OldChunk->GetOwner();
	AssessCondition(Paradigm, Mechanism != nullptr, EApparatusStatus::InvalidState);
	const auto NewChunk = Mechanism->ObtainChunk(Fingerprint);
	AssessConditionFormat(Paradigm, OK(NewChunk), NewChunk,
				 TEXT("Failed to ensure a new chunk to move the subject with an obtained trait to."));
	AssessConditionFormat(Paradigm, NewChunk != OldChunk, EApparatusStatus::SanityCheckFailed,
				 TEXT("The chunk must have been changed."));

	return UMachine::Instance->template MoveSubject<Paradigm>(Id, NewChunk);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm, void*>
FSubjectInfo::ObtainTraitPtr(UScriptStruct* const TraitType)
{
	if (UNLIKELY(TraitType == nullptr))
	{
		return MakeOutcome<Paradigm, void*>(EApparatusStatus::NullArgument, nullptr);
	}
	if (AvoidConditionFormat(Paradigm, !IsValid(),
							 TEXT("The subject to obtain a trait pointer from is invalid. "
								  "Was the subject already despawned?")))
	{
		return MakeOutcome<Paradigm, void*>(EApparatusStatus::InvalidState, nullptr);
	}
	if (AvoidConditionFormat(Paradigm, IsSolid(),
							 TEXT("Obtaining a trait pointer from a subject is not possible during its solid state. "
								  "Do you have an unfinished solid iterating?")))
	{
		return MakeOutcome<Paradigm, void*>(EApparatusStatus::InvalidState, nullptr);
	}
	if (AvoidCondition(Paradigm, !UMachine::HasInstance()))
	{
		return MakeOutcome<Paradigm, void*>(EApparatusStatus::InvalidState, nullptr);
	}

	const auto OldIndex = SlotIndex;
	const auto OldChunk = Chunk;
	auto& Fingerprint = GetFingerprintRef();
	// Assemble a new fingerprint.
	const auto Status = Fingerprint.template Add<MakePolite(Paradigm)>(TraitType);
	if (AvoidCondition(Paradigm, !OK(Status)))
	{
		return MakeOutcome<Paradigm, void*>(Status, nullptr);
	}
	if (IsNoop(Status))
	{
		// The trait is already there in the chunk. Get its data and return...
		return OldChunk->TraitPtrAt(OldIndex, TraitType);
	}

#if WITH_EDITOR
	if (Subjective != nullptr)
	{
		// Make sure the trait record is added and
		// is visible in the Details panel during
		// the editing mode:
		Subjective->DoObtainTraitRecord(TraitType);
	}
#endif

	// No such trait in the current chunk...
	// Ensure the chunk with an updated target fingerprint.
	const auto Mechanism = OldChunk->GetOwner();
	if (AvoidCondition(Paradigm, Mechanism == nullptr))
	{
		return MakeOutcome<Paradigm, void*>(EApparatusStatus::InvalidState, nullptr);
	}
	const auto NewChunk = Mechanism->ObtainChunk(Fingerprint);
	if (AvoidConditionFormat(Paradigm, !OK(NewChunk),
							 TEXT("Failed to ensure a new chunk to move the subject with an obtained trait to.")))
	{
		return MakeOutcome<Paradigm, void*>(EApparatusStatus::InvalidState, nullptr);
	}

	if (AvoidConditionFormat(Paradigm, NewChunk == OldChunk,
							 TEXT("The chunk must have been changed.")))
	{
		return MakeOutcome<Paradigm, void*>(EApparatusStatus::SanityCheckFailed, nullptr);
	}

	if (AvoidError(Paradigm, UMachine::Instance->template MoveSubject<Paradigm>(Id, NewChunk)))
	{
		return MakeOutcome<Paradigm, void*>(FApparatusStatus::GetLastError(), nullptr);
	}

	return NewChunk->TraitPtrAt(SlotIndex, TraitType);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
FSubjectInfo::ObtainTraitDeferred(UScriptStruct* const TraitType)
{
	if (AvoidCondition(Paradigm, TraitType == nullptr))
	{
		return EApparatusStatus::NullArgument;
	}
	AssessConditionFormat(Paradigm, IsValid(), EApparatusStatus::InvalidState,
				 		  TEXT("The subject to obtain a trait deferred from is invalid. "
							   "Was the subject already despawned?"));
	AssessCondition(Paradigm, UMachine::HasInstance(), EApparatusStatus::InvalidState);

	const auto& Fingerprint = GetFingerprintRef();
	if (UNLIKELY(Fingerprint.Contains(TraitType)))
	{
		// The trait is already there in the chunk, so just return:
		return EApparatusStatus::Noop;
	}

	const auto Mechanism = Chunk->GetOwner();
	AssessCondition(Paradigm, Mechanism != nullptr, EApparatusStatus::InvalidState);
	Mechanism->EnqueueDeferred(AMechanism::FDeferredTraitObtainment(GetHandle(), TraitType));
	return EApparatusStatus::Deferred;
}

template < EParadigm Paradigm, typename T >
inline TOutcomeIf<Paradigm, IsTraitType<T>(), T>
FSubjectInfo::ObtainTraitDeferred()
{
	if (AvoidConditionFormat(Paradigm, !IsValid(),
							 TEXT("The subject to obtain a trait deferred from is invalid. "
								  "Was the subject already despawned?")))
	{
		return MakeOutcome<Paradigm, T>(EApparatusStatus::InvalidState, T());
	}
	if (AvoidCondition(Paradigm, !UMachine::HasInstance()))
	{
		return MakeOutcome<Paradigm, T>(EApparatusStatus::InvalidState, T());
	}

	const auto& Fingerprint = GetFingerprintRef();
	if (UNLIKELY(Fingerprint.template ContainsTrait<T>()))
	{
		// The trait is already there in the chunk. Get its data and return:
		return Chunk->template TraitAt<T>(SlotIndex);
	}

	const auto Mechanism = Chunk->GetOwner();
	if (AvoidCondition(Paradigm, Mechanism == nullptr))
	{
		return MakeOutcome<Paradigm, T>(EApparatusStatus::InvalidState, T());
	}

	Mechanism->EnqueueDeferred(AMechanism::FDeferredTraitObtainment(GetHandle(), T::StaticStruct()));

	// Return the default instance for now.
	return T();
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
FSubjectInfo::ObtainTrait(UScriptStruct* const TraitType,
						  void* const          OutTraitData,
						  const bool           bTraitDataInitialized)
{
	AssessCondition(Paradigm, TraitType != nullptr, EApparatusStatus::NullArgument);
	AssessConditionFormat(Paradigm, IsValid(), EApparatusStatus::InvalidState,
						  TEXT("The subject to obtain a trait from is invalid. "
							   "Was the subject already despawned?"));
	AssessConditionFormat(Paradigm, !IsSolid(), EApparatusStatus::InvalidState,
						  TEXT("Obtaining traits from a subject is not possible during its solid state. "
							   "Do you have an unfinished solid iterating?"));
	AssessCondition(Paradigm, UMachine::HasInstance(), EApparatusStatus::InvalidState);

	const auto OldIndex = SlotIndex;
	const auto OldChunk = Chunk;
	auto& Fingerprint = GetFingerprintRef();	// Assemble a new fingerprint.
	auto Status = Fingerprint.template Add<MakePolite(Paradigm)>(TraitType);
	AssessCondition(Paradigm, OK(Status), Status);
	if (IsNoop(Status))
	{
		// The trait is already there in the chunk. Get its data and return...
		return OldChunk->TraitAt(OldIndex, TraitType, OutTraitData,
								 bTraitDataInitialized);
	}

#if WITH_EDITOR
	if (Subjective != nullptr)
	{
		// Make sure to update the Details panel
		// when during the editing mode:
		Subjective->DoObtainTraitRecord(TraitType);
	}
#endif

	// No such trait in the current chunk...
	// Ensure the chunk with an updated target fingerprint.
	const auto Mechanism = OldChunk->GetOwner();
	AssessCondition(Paradigm, Mechanism != nullptr, EApparatusStatus::InvalidState);
	const auto NewChunk = Mechanism->template ObtainChunk<Paradigm>(Fingerprint);
	AssessConditionFormat(Paradigm, OK(NewChunk), NewChunk,
						  TEXT("Failed to ensure a new chunk to move the subject with an obtained trait to."));
	AssessConditionFormat(Paradigm, NewChunk != OldChunk, EApparatusStatus::SanityCheckFailed,
						  TEXT("The chunk must have been changed."));

	AssessOK(Paradigm, UMachine::Instance->template MoveSubject<Paradigm>(Id, NewChunk));

	// Copy the newly initialized trait data:
	return NewChunk->TraitAt(SlotIndex, TraitType, OutTraitData, bTraitDataInitialized);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
FSubjectInfo::SetTrait(UScriptStruct* const TraitType,
					   const void* const    InTraitData)
{
	if (UNLIKELY((TraitType == nullptr)))
	{
		// Setting nothing is a valid noop:
		return EApparatusStatus::Noop;
	}
	if (TraitType == FTraitRecord::StaticStruct())
	{
		const auto TraitRecord = static_cast<const FTraitRecord*>(InTraitData);
		return SetTrait<Paradigm>(*TraitRecord);
	}

	AssessConditionFormat(Paradigm, InTraitData != nullptr, EApparatusStatus::NullArgument,
						  TEXT("The trait data must be provided, if the type is provided."));
	AssessConditionFormat(Paradigm, IsValid(), EApparatusStatus::InvalidState,
						  TEXT("The subject to set a trait for is invalid. "
							   "Was the subject already despawned?"));
	AssessConditionFormat(Paradigm, UMachine::HasInstance(), EApparatusStatus::InvalidState,
						  TEXT("There must be a machine instance available in order to set the trait."));

	const auto OldIndex = SlotIndex;
	const auto OldChunk = Chunk;

	// Assemble a new fingerprint:
	auto& Fingerprint = GetFingerprintRef();
	auto Status = Fingerprint.template Add<MakePolite(Paradigm)>(TraitType);
	AssessCondition(Paradigm, OK(Status), Status);

	if (IsNoop(Status))
	{
		// The trait is already there in the chunk. Set its data and return...
		return OldChunk->SetTraitAt(OldIndex, TraitType, InTraitData);
	}

	AssessConditionFormat(Paradigm, !IsSolid(), EApparatusStatus::InvalidState,
						  TEXT("Set-adding a '%s' trait to a subject is not possible during its solid state. "
							   "Do you have an unfinished solid iterating?"),
						  *(TraitType->GetName()));
	
	// This is a new trait for the subject...
	// Ensure the chunk with a target fingerprint.
	const auto Mechanism = OldChunk->GetOwner();
	AssessCondition(Paradigm, Mechanism != nullptr, EApparatusStatus::InvalidState);
	const auto NewChunk = Mechanism->template ObtainChunk<Paradigm>(Fingerprint);
	AssessConditionFormat(Paradigm, OK(NewChunk), NewChunk,
						  TEXT("Failed to ensure a new chunk to move the subject with a set trait to."));
	AssessConditionFormat(Paradigm, NewChunk != OldChunk, EApparatusStatus::SanityCheckFailed,
						  TEXT("The chunk must have been changed."));

	AssessOK(Paradigm, UMachine::Instance->template MoveSubject<Paradigm>(Id, NewChunk));

	return NewChunk->SetTraitAt(SlotIndex, TraitType, InTraitData);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm>
FSubjectInfo::SetTrait(const FTraitRecord& TraitRecord)
{
	return SetTrait<Paradigm>(TraitRecord.GetType(), TraitRecord.GetData());
}

template< EParadigm Paradigm, typename TraitsExtractorT >
inline TOutcome<Paradigm>
FSubjectInfo::SetTraits(const TraitsExtractorT& TraitsExtractor,
						const bool              bLeaveRedundant/*=true*/)
{
	if (UNLIKELY(TraitsExtractor.Num() == 0))
	{
		if (LIKELY(bLeaveRedundant))
		{
			return EApparatusStatus::Noop;
		}
		else
		{
			return RemoveAllTraits<Paradigm>();
		}
	}

	AssessConditionFormat(Paradigm, IsValid(), EApparatusStatus::InvalidState,
						  TEXT("The subject to set the traits for is invalid. "
							   "Was the subject already despawned?"));
	AssessCondition(Paradigm, UMachine::HasInstance(), EApparatusStatus::InvalidState);

	const auto OldIndex = SlotIndex;
	const auto OldChunk = Chunk;

	// Assemble a new fingerprint:
	auto& Fingerprint = GetFingerprintRef();
	const auto FingerprintStatus = Fingerprint.template Add<MakePolite(Paradigm)>(TraitsExtractor);
	AssessCondition(Paradigm, OK(FingerprintStatus), FingerprintStatus);

	// State change indicator:
	bool bHasTraitRemovals = false;

	// Clean-up redundant traits from the fingerprint (if needed)...
	if (UNLIKELY(!bLeaveRedundant))
	{
		// Remove all of the excessive traits from the fingerprint...
		for (int32 i = 0; i < Fingerprint.TraitsNum(); ++i)
		{
			const auto TraitType = Fingerprint.TraitAt(i);
			if (UNLIKELY(!TraitsExtractor.Contains(TraitType)))
			{
				bHasTraitRemovals = true;
				AssessOK(Paradigm, Fingerprint.template Remove<Paradigm>(TraitType));
				i -= 1; // Removal during iterating.
			}
		}
	}

	bool bChange = bHasTraitRemovals || !IsNoop(FingerprintStatus);
	if (bChange)
	{
		// The topology needs change.
		AssessConditionFormat(Paradigm, !IsSolid(), EApparatusStatus::InvalidState,
							TEXT("Set-adding/removing traits to a subject is not possible during its solid state. "
								"Do you have an unfinished solid iterating?"));
		
		// This is a new trait for the subject...
		// Ensure the chunk with a target fingerprint.
		const auto Mechanism = OldChunk->GetOwner();
		AssessCondition(Paradigm, Mechanism != nullptr, EApparatusStatus::InvalidState);
		const auto NewChunk = Mechanism->template ObtainChunk<Paradigm>(Fingerprint);
		AssessConditionFormat(Paradigm, OK(NewChunk), NewChunk,
							TEXT("Failed to ensure a new chunk to move the subject with set traits to."));
		AssessConditionFormat(Paradigm, NewChunk != OldChunk, EApparatusStatus::SanityCheckFailed,
							TEXT("The chunk must have been changed."));

		AssessOK(Paradigm, UMachine::Instance->template MoveSubject<Paradigm>(Id, NewChunk));
		check(SlotIndex != InvalidSlotIndex);
		check(Chunk == NewChunk);
	}

	// Set the new traits data to its places...
	for (int32 i = 0; i < TraitsExtractor.Num(); ++i)
	{
		if (LIKELY(TraitsExtractor.TypeAt(i) != nullptr))
		{
			AssessOK(Paradigm,
					 Chunk->SetTraitAt(SlotIndex,
									   TraitsExtractor.TypeAt(i),
									   TraitsExtractor.TraitAt(i)));
#if WITH_EDITOR
			if (Subjective)
			{
				// Make sure the Details panel
				// gets updated in the Editor:
				Subjective->DoSetTraitRecord(TraitsExtractor.TypeAt(i),
											 TraitsExtractor.TraitAt(i));
			}
#endif
			bChange = true;
		}
	}

	return bChange ? EApparatusStatus::Success : EApparatusStatus::Noop;
}

template< EParadigm Paradigm, typename A1, typename A2, typename T >
FORCEINLINE TOutcome<Paradigm>
FSubjectInfo::SetTraits(const TArray<UScriptStruct*, A1>& TraitsTypes,
						const TArray<const T*, A2>&       TraitsData,
						const bool                        bLeaveRedundant/*=true*/)
{
	return SetTraits<Paradigm>(
		TTraitsExtractor<TArray<UScriptStruct*, A1>, TArray<T*, A2>>(TraitsTypes, TraitsData),
		bLeaveRedundant);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm>
FSubjectInfo::SetTraits(const FSubjectRecord& SubjectRecord,
						const bool            bLeaveRedundant/*=true*/)
{
	return SetTraits<Paradigm>(
		TTraitsExtractor<FSubjectRecord>(SubjectRecord),
		bLeaveRedundant);
}

template < EParadigm Paradigm, typename TraitsExtractorT >
inline TOutcome<Paradigm>
FSubjectInfo::OverwriteTraits(const TraitsExtractorT& TraitsExtractor)
{
	if (UNLIKELY(TraitsExtractor.Num() == 0))
	{
		return EApparatusStatus::Noop;
	}

	AssessConditionFormat(Paradigm, IsValid(), EApparatusStatus::InvalidState,
						  TEXT("The subject to overwrite the traits for is invalid. "
							   "Was the subject already despawned?"));
	AssessCondition(Paradigm, UMachine::HasInstance(), EApparatusStatus::InvalidState);

	const auto& Fingerprint = GetFingerprint();

	// State change indicator:
	bool bChange = false;

	// The traits are already there in the chunk.
	// Just set them without adding nothing new...
	for (int32 i = 0; i < TraitsExtractor.Num(); ++i)
	{
		const auto Line = Chunk->FindTraitLine(TraitsExtractor.TypeAt(i));
		if (Line != nullptr)
		{
			AssessOK(Paradigm, Line->SetElementAt(SlotIndex, TraitsExtractor.TraitAt(i)));
#if WITH_EDITOR
			if (Subjective)
			{
				// Make sure to update the trait record
				// for the subjective during the editing mode,
				// to be displayed in the Details panel.
				Subjective->DoSetTraitRecord(TraitsExtractor.TypeAt(i),
											 TraitsExtractor.TraitAt(i));
			}
#endif // WITH_EDITOR
			bChange = true;
		}
	}
	return bChange ? EApparatusStatus::Success : EApparatusStatus::Noop;
}

template< EParadigm Paradigm, typename A1, typename A2, typename T >
FORCEINLINE TOutcome<Paradigm>
FSubjectInfo::OverwriteTraits(const TArray<UScriptStruct*, A1>& TraitsTypes,
							  const TArray<const T*, A2>&       TraitsData)
{
	return OverwriteTraits<Paradigm>(
		TTraitsExtractor<TArray<UScriptStruct*, A1>, TArray<T*, A2>>(TraitsTypes, TraitsData));
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm>
FSubjectInfo::OverwriteTraits(const FSubjectRecord& SubjectRecord)
{
	return OverwriteTraits<Paradigm>(TTraitsExtractor<FSubjectRecord>(SubjectRecord));
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
FSubjectInfo::CopyTraitsTo(FSubjectInfo* const DstInfo) const
{
	AssessCondition(Paradigm, Chunk != nullptr, EApparatusStatus::InvalidState);
	AssessCondition(Paradigm, DstInfo != nullptr, EApparatusStatus::NullArgument);
	AssessCondition(Paradigm, UMachine::HasInstance(), EApparatusStatus::InvalidState);

	if (UNLIKELY(this == DstInfo))
	{
		return EApparatusStatus::Noop;
	}
	if (LIKELY(DstInfo->Chunk->Traitmark != this->Chunk->Traitmark))
	{
		// The chunks are different in topology.
		// Construct a new fingerprint for the destination subject.
		auto& DstFingerprint = DstInfo->GetFingerprintRef();
		const auto DstFingerprintStatus = DstFingerprint.template Add<MakePolite(Paradigm)>(GetFingerprint().GetTraits());
		if (LIKELY(!IsNoop(DstFingerprintStatus)))
		{
			// The fingerprint was actually changed.
			// A new chunk is needed for a subject to move to...
			const auto DstMechanism = DstInfo->GetMechanism();
			AssessCondition(Paradigm, DstMechanism != nullptr, EApparatusStatus::InvalidState);
			const auto DstChunk = DstMechanism->template ObtainChunk<Paradigm>(DstFingerprint);
			AssessCondition(Paradigm, OK(DstChunk), DstChunk);
			AssessOK(Paradigm, (UMachine::Instance->template MoveSubject<Paradigm>(DstInfo->Id, DstChunk)));
		}
	}
	// Copy the traits now:
	return this->Chunk->OverwriteTraits(SlotIndex, DstInfo->Chunk, DstInfo->SlotIndex);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
FSubjectInfo::RemoveTrait(UScriptStruct* const TraitType)
{
	AssessCondition(Paradigm, TraitType != nullptr, EApparatusStatus::NullArgument);
	AssessConditionFormat(Paradigm, IsValid(), EApparatusStatus::InvalidState,
				 		  TEXT("The subject to remove the trait from is invalid. "
							   "Was the subject already despawned?"));
	AssessConditionFormat(Paradigm, !IsSolid(), EApparatusStatus::InvalidState,
						  TEXT("Removing traits from a subject is not possible during a solid state. "
							   "Do you have an unfinished solid iterating?"));
	AssessCondition(Paradigm, UMachine::HasInstance(), EApparatusStatus::InvalidState);

	const auto OldIndex = SlotIndex;
	const auto OldChunk = Chunk;

	// Modify the fingerprint:
	auto& Fingerprint = GetFingerprintRef();
	const auto FingerprintStatus = Fingerprint.template Remove<MakePolite(Paradigm)>(TraitType);
	AssessCondition(Paradigm, OK(FingerprintStatus), FingerprintStatus);

	if (UNLIKELY(IsNoop(FingerprintStatus)))
	{
		// The trait is not there in the chunk already. Just return:
		return EApparatusStatus::Noop;
	}

#if WITH_EDITOR
	if (Subjective != nullptr)
	{
		Subjective->DoRemoveTraitRecord(TraitType);
	}
#endif

	// Ensure the chunk within a target fingerprint chunk.
	const auto Mechanism = OldChunk->GetOwner();
	AssessCondition(Paradigm, Mechanism != nullptr, EApparatusStatus::InvalidState);
	const auto NewChunk = Mechanism->template ObtainChunk<Paradigm>(Fingerprint);
	AssessConditionFormat(Paradigm, OK(NewChunk), NewChunk,
						  TEXT("Failed to ensure a new chunk to move the subject with a removed trait to."));
	AssessConditionFormat(Paradigm, NewChunk != OldChunk, EApparatusStatus::SanityCheckFailed,
						  TEXT("The chunk must have been changed."));

	return UMachine::Instance->template MoveSubject<Paradigm>(Id, NewChunk);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
FSubjectInfo::RemoveAllTraits()
{
	AssessConditionFormat(Paradigm, IsValid(), EApparatusStatus::InvalidState,
						  TEXT("The subject to remove all the traits from is invalid. "
							   "Was the subject already despawned?"));
	AssessConditionFormat(Paradigm, !IsSolid(), EApparatusStatus::InvalidState,
						  TEXT("Removing all traits from a subject is not possible during its solid state. "
							   "Do you have an unfinished solid iterating?"));
	AssessCondition(Paradigm, UMachine::HasInstance(), EApparatusStatus::InvalidState);

	const auto OldIndex = SlotIndex;
	const auto OldChunk = Chunk;

	// Modify the fingerprint:
	auto& Fingerprint = GetFingerprintRef();
	const auto FingerprintStatus = Fingerprint.template RemoveAllTraits<MakePolite(Paradigm)>();
	AssessCondition(Paradigm, OK(FingerprintStatus), FingerprintStatus);

	if (UNLIKELY(IsNoop(FingerprintStatus)))
	{
		// The were no traits in the subject. Just return:
		return EApparatusStatus::Noop;
	}

#if WITH_EDITOR
	if (Subjective != nullptr)
	{
		Subjective->DoRemoveAllTraitRecords();
	}
#endif

	// Ensure the chunk within a target fingerprint chunk.
	const auto Mechanism = OldChunk->GetOwner();
	AssessCondition(Paradigm, Mechanism != nullptr, EApparatusStatus::InvalidState);
	const auto NewChunk = Mechanism->template ObtainChunk<Paradigm>(Fingerprint);
	AssessConditionFormat(Paradigm, OK(NewChunk), NewChunk,
						  TEXT("Failed to ensure a new chunk to move the subject with all traits removed to."));
	AssessConditionFormat(Paradigm, NewChunk != OldChunk, EApparatusStatus::SanityCheckFailed,
						  TEXT("The chunk after removing all traits must have been changed."));

	return UMachine::Instance->template MoveSubject<Paradigm>(Id, NewChunk);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm, bool>
FSubjectInfo::MarkBooted()
{
	static_assert(IsInternal(Paradigm),
				  "Marking a subject as booted can only be done under an internal paradigm.");
	if (AvoidConditionFormat(Paradigm, !IsValid(),
				 		 	 TEXT("The subject to mark as booted is invalid. "
								  "Was the subject already despawned?")))
	{
		return MakeOutcome<Paradigm, bool>(EApparatusStatus::InvalidState, false);
	}
	if (AvoidConditionFormat(Paradigm, IsSolid(),
							 TEXT("Marking the subject as booted is not possible during its solid state. "
								  "Do you have an unfinished solid iterating?")))
	{
		return MakeOutcome<Paradigm, bool>(EApparatusStatus::InvalidState, HasFlag(EFlagmarkBit::Booted));
	}

	// NOTE: The actual removal from the halted registry is done in AMechanism::BootAll()
	// since it's more effective to do it in the bulk there.
	return GetFingerprintRef().template SetBooted<Paradigm>();
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm, FSubjectNetworkState*>
FSubjectInfo::ObtainNetworkState(const uint32 InSubjectNetworkId)
{
	const auto State = ObtainNetworkState<Paradigm>(); // This just guarantees the struct exists.
	if (AvoidError(Paradigm, State))
	{
		return State;
	}
	if (AvoidFormat(Paradigm, State->Id != FSubjectNetworkState::InvalidId && State->Id != InSubjectNetworkId,
					TEXT("Changing the network identifier for an already set network state is prohibited.")))
	{
		return MakeOutcome<Paradigm, FSubjectNetworkState*>(EApparatusStatus::InvalidState, State);
	}
	if (LIKELY(State->Id != InSubjectNetworkId))
	{
		// The network identifier is to be assigned...
		if (AvoidCondition(Paradigm, InSubjectNetworkId == FSubjectNetworkState::InvalidId))
		{
			return MakeOutcome<Paradigm, FSubjectNetworkState*>(EApparatusStatus::InvalidArgument, State);
		}
		State->Id = InSubjectNetworkId;
		const auto Mechanism = GetMechanism();
		if (AvoidConditionFormat(Paradigm, Mechanism->SubjectByNetworkId.Contains(InSubjectNetworkId),
							 TEXT("The network identifier must not be already occupied by the subject.")))
		{
			return MakeOutcome<Paradigm, FSubjectNetworkState*>(EApparatusStatus::InvalidState, State);
		}
		Mechanism->SubjectByNetworkId.Add(InSubjectNetworkId, Id);
		if (AvoidError(Paradigm, SetFlag<MakeInternal(Paradigm)>(EFlagmarkBit::Online)))
		{
			return MakeOutcome<Paradigm, FSubjectNetworkState*>(FApparatusStatus::GetLastError(), State);
		}
	}
	return MakeOutcome<Paradigm, FSubjectNetworkState*>(EApparatusStatus::Success, State);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm>
FSubjectInfo::Despawn()
{
	if (LIKELY(UMachine::HasInstance()))
	{
		return UMachine::Instance->template DoDespawnSubject<Paradigm>(GetHandle());
	}
	return EApparatusStatus::Noop;
}

#pragma region Network State

inline FSubjectNetworkState::IdType
FSubjectNetworkState::ServerObtainId()
{
	check(SubjectId != FSubjectInfo::InvalidId);

	if (LIKELY(Id != InvalidId))
	{
		// The net state is already initialized,
		// so just return it.
		return Id;
	}

	const auto Machine = UMachine::RetainInstance();
	auto& Info = Machine->DoGetSubjectInfo(SubjectId);
	const auto Mechanism = Info.GetMechanism();

	if (ensure(Mechanism != nullptr))
	{
		Id = Machine->DoObtainNetworkId();
		if (ensure(Id != InvalidId))
		{
			// Register the subject by its network identity:
			Mechanism->SubjectByNetworkId.Add(Id, SubjectId);
			Info.SetFlag(EFlagmarkBit::Online);
		}
	}

	return Id;
}

inline FSubjectNetworkState::IdType
FSubjectNetworkState::ServerObtainId(const TRange<SubjectNetworkIdType>& IdRange)
{
	check(SubjectId != FSubjectInfo::InvalidId);

	if (LIKELY(Id != InvalidId))
	{
		// The net state is already initialized,
		// so just return it.
		return Id;
	}

	const auto Machine = UMachine::RetainInstance();
	auto& Info = Machine->DoGetSubjectInfo(SubjectId);
	const auto Mechanism = Info.GetMechanism();

	if (ensure(Mechanism != nullptr))
	{
		Id = Machine->DoObtainNetworkId(IdRange);
		if (ensure(Id != InvalidId))
		{
			// Register the subject by its network identity:
			Mechanism->SubjectByNetworkId.Add(Id, SubjectId);
			Info.SetFlag(EFlagmarkBit::Online);
		}
	}

	return Id;
}

FORCEINLINE UNetConnection*
FSubjectInfo::GetConnectionPermit() const
{
	checkf(IsServerSide(),
		   TEXT("Only online server-side subjects can have a connection permit. "
				"Are you examining it on a client?"));
	if (Subjective != nullptr)
	{
		if (const auto Actor = Subjective->GetActor())
		{
			return Actor->GetNetConnection();
		}
	}
	return NetworkState->GetConnectionPermit();
}

#pragma endregion Network State

FORCEINLINE const FChunkSlot&
FSubjectInfo::GetChunkSlot() const
{
	check(IsValid());
	return Chunk->Slots[SlotIndex];
}

FORCEINLINE FChunkSlot&
FSubjectInfo::GetChunkSlot()
{
	check(IsValid());
	return Chunk->Slots[SlotIndex];
}

FORCEINLINE FFingerprint&
FSubjectInfo::GetFingerprintRef()
{
	return GetChunkSlot().GetFingerprintRef();
}

FORCEINLINE const FFingerprint&
FSubjectInfo::GetFingerprint() const
{
	return GetChunkSlot().GetFingerprint();
}

FORCEINLINE EFlagmark
FSubjectInfo::GetFlagmark() const
{
	return GetFingerprint().GetFlagmark();
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm, EFlagmark>
FSubjectInfo::SetFlagmark(const EFlagmark Flagmark)
{
	if (IsInternal(Paradigm)) // Compile-time branch.
	{
		return GetFingerprintRef().template SetFlagmark<Paradigm>(Flagmark);
	}
	else
	{
		return GetFingerprintRef().template SetFlagmarkMasked<Paradigm>(Flagmark, FM_AllUserLevel);
	}
}

FORCEINLINE EApparatusStatus
FSubjectInfo::SetFlagmark_Status(const EFlagmark Flagmark)
{
	// This is an internal version and doesn't contain
	// any additional argument checks.
	return GetFingerprintRef().template SetFlagmark<EParadigm::PoliteSafe>(Flagmark);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm, EFlagmark>
FSubjectInfo::SetFlagmarkMasked(const EFlagmark Flagmark,
								const EFlagmark Mask)
{
	if (!IsInternal(Paradigm)) // Compile-time branch.
	{
		if (AvoidCondition(Paradigm, HasSystemLevelFlags(Mask)))
		{
			return MakeOutcome<Paradigm, EFlagmark>(EApparatusStatus::InvalidArgument, GetFingerprintRef().GetFlagmark() & Mask);
		}
	}
	return GetFingerprintRef().template SetFlagmarkMasked<Paradigm>(Flagmark, Mask);
}

FORCEINLINE EApparatusStatus
FSubjectInfo::SetFlagmarkMasked_Status(const EFlagmark Flagmark,
									   const EFlagmark Mask)
{
	return SetFlagmarkMasked<EParadigm::PoliteSafe>(Flagmark, Mask);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm, EFlagmark>
FSubjectInfo::AddToFlagmark(const EFlagmark Flagmark)
{
	if (!IsInternal(Paradigm)) // Compile-time branch.
	{
		if (AvoidCondition(Paradigm, HasSystemLevelFlags(Flagmark)))
		{
			return MakeOutcome<Paradigm, EFlagmark>(EApparatusStatus::InvalidArgument, GetFingerprintRef().GetFlagmark());
		}
	}
	return GetFingerprintRef().template AddToFlagmark<Paradigm>(Flagmark);
}

FORCEINLINE bool
FSubjectInfo::HasFlag(const EFlagmarkBit Flag) const
{
	return GetFingerprint().HasFlag(Flag);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm, bool>
FSubjectInfo::SetFlag(const EFlagmarkBit Flag,
					  const bool         bState/*=true*/)
{
	if (!IsInternal(Paradigm)) // Compile-time branch.
	{
		if (Avoid(Paradigm, IsSystemLevel(Flag)))
		{
			return MakeOutcome<Paradigm, bool>(EApparatusStatus::InvalidArgument, GetFingerprintRef().HasFlag(Flag));
		}
	}
	return GetFingerprintRef().template SetFlag<Paradigm>(Flag, bState);
}

FORCEINLINE EApparatusStatus
FSubjectInfo::SetFlag_Status(const EFlagmarkBit Flag,
							 const bool         bState/*=true*/)
{
	return GetFingerprintRef().SetFlag<EParadigm::PoliteSafe>(Flag, bState);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm, bool>
FSubjectInfo::ToggleFlag(const EFlagmarkBit Flag)
{
	if (!IsInternal(Paradigm)) // Compile-time branch.
	{
		if (AvoidCondition(Paradigm, IsSystemLevel(Flag)))
		{
			return MakeOutcome<Paradigm, bool>(EApparatusStatus::InvalidArgument, GetFingerprintRef().HasFlag(Flag));
		}
	}
	return GetFingerprintRef().template ToggleFlag<Paradigm>(Flag);
}

FORCEINLINE AMechanism*
FSubjectInfo::GetMechanism() const
{
	if (LIKELY(Chunk))
	{
		return Chunk->GetOwner();
	}
	return nullptr;
}

template < EParadigm Paradigm >
inline TOutcome<Paradigm>
FSubjectInfo::SetMechanism(AMechanism* const Mechanism)
{
	AssessCondition(Paradigm, Mechanism != nullptr, EApparatusStatus::NullArgument);
	AssessCondition(Paradigm, this->IsValid(), EApparatusStatus::InvalidState);
	const auto CurrentMechanism = this->GetMechanism();
	if (UNLIKELY(Mechanism == CurrentMechanism))
	{
		return EApparatusStatus::Noop;
	}

	if (Subjective)
	{
		const auto Actor = Subjective->GetActor();
		if (LIKELY(Actor))
		{
			const auto CurrentWorld = Actor->GetWorld();
			AssessConditionFormat(Paradigm, CurrentWorld == Mechanism->GetWorld(), EApparatusStatus::InvalidOperation,
								  TEXT("The '%s' subjective's subject may be transitioned to a mechanism within the same '%s' world only."),
								  *(Actor->GetName()), *(CurrentWorld->GetName()));
		}
		const auto Belt = Subjective->GetBelt();
		if (LIKELY(Belt != nullptr))
		{
			const auto NewBelt = Mechanism->template ObtainMostSpecificBelt<Paradigm>(Subjective);
			AssessOK(Paradigm, NewBelt->template Refresh<Paradigm>(Subjective));
		}
		CurrentMechanism->Subjectives.Remove(Subjective);
		Mechanism->Subjectives.Add(Subjective);
	}

	const auto Machine = UMachine::RetainInstance();
	const auto NewChunk = Mechanism->template ObtainChunk<Paradigm>(GetFingerprint());
	AssessOK(Paradigm, NewChunk);
	AssessOK(Paradigm, Machine->template MoveSubject<Paradigm>(this->Id, NewChunk));

	return EApparatusStatus::Success;
}

FORCEINLINE bool
FSubjectInfo::IsOnline() const
{
	return HasFlag(EFlagmarkBit::Online);
}

FORCEINLINE const FTraitmark&
FSubjectInfo::GetTraitmarkPermit() const
{
	checkf(IsServerSide(), TEXT("Only online server-side subjects can have a traitmark permit. "
								"Are you examining it on a client?"));
	return NetworkState->GetTraitmarkPermit();
}

FORCEINLINE UWorld*
FSubjectInfo::GetWorld() const
{
	const auto Mechanism = GetMechanism();
	if (LIKELY(Mechanism))
	{
		return Mechanism->GetWorld();
	}
	return nullptr;
}

FORCEINLINE bool
FSubjectInfo::IsSolid() const
{
	if (LIKELY(Chunk))
	{
		return Chunk->IsSolid();
	}
	return false;
}

FORCEINLINE bool
FSubjectInfo::IsServerSide() const
{
	const auto World = GetWorld();
	if (!World) return false;
	return IsOnline() && (World->IsNetMode(NM_DedicatedServer) || World->IsNetMode(NM_ListenServer));
}

FORCEINLINE bool
FSubjectInfo::IsClientSide() const
{
	const auto World = GetWorld();
	if (!World) return false;
	return IsOnline() && World->IsNetMode(NM_Client);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm>
FSubjectInfo::SetConnectionPermit(UNetConnection* const Connection)
{
	AssessConditionFormat(Paradigm, IsServerSide(), EApparatusStatus::InvalidState,
						  TEXT("Only online server-side subjects can have their connection permit changed. "
							   "Are you trying to change it on a client?"));

	if (Subjective != nullptr)
	{
		if (const auto Actor = Subjective->GetActor())
		{
			AssessConditionFormat(Paradigm, Actor->GetNetConnection() == Connection, EApparatusStatus::InvalidState,
								  TEXT("Changing the connection permit of the '%s' actor-based subject is prohibited."),
								  *(Actor->GetName()));
		}
	}
	return NetworkState->SetConnectionPermit(Connection);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm>
FSubjectInfo::SetTraitmarkPermit(const FTraitmark& InTraitmarkPermit)
{
	AssessConditionFormat(Paradigm, IsServerSide(), EApparatusStatus::InvalidState,
				 TEXT("Only online server-side subjects can have their traitmark permit changed. "
					  "Are you trying to change it on a client?"));
#if WITH_EDITOR
	// Also change the property within the subjective (if any)...
	if (Subjective)
	{
		Subjective->GetStandardNetworkPropertyBlock().TraitmarkPermit.Set<MakePolite(Paradigm)>(InTraitmarkPermit); 
	}
#endif // #if WITH_EDITOR
	return NetworkState->template SetTraitmarkPermit<Paradigm>(InTraitmarkPermit);
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
TOutcome<Paradigm>
FSubjectInfo::SetTraitDeferred(UScriptStruct* const TraitType,
							   const void* const    TraitData)
{
	if (UNLIKELY((TraitType == nullptr) && (TraitData == nullptr)))
	{
		return EApparatusStatus::Noop;
	}

	AssessCondition(Paradigm, TraitType != nullptr, EApparatusStatus::NullArgument);
	AssessCondition(Paradigm, TraitData != nullptr, EApparatusStatus::NullArgument);
	AssessCondition(Paradigm, IsValid(), EApparatusStatus::InvalidState);

	const auto Mechanism = GetMechanism();
	Mechanism->EnqueueDeferred(AMechanism::FDeferredTraitSetting(GetHandle(), TraitType, TraitData));

	return EApparatusStatus::Deferred;
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
FSubjectInfo::RemoveTraitDeferred(UScriptStruct* const TraitType,
								  const bool           bForceEnqueue /*=false*/)
{
	if (UNLIKELY(TraitType == nullptr))
	{
		return EApparatusStatus::Noop;
	}
	AssessCondition(Paradigm, IsValid(), EApparatusStatus::InvalidState);
	if (!bForceEnqueue && !GetFingerprint().Contains(TraitType))
	{
		return EApparatusStatus::Noop;
	}

	const auto Mechanism = GetMechanism();
	Mechanism->EnqueueDeferred(AMechanism::FDeferredTraitRemoval(GetHandle(), TraitType));

	return EApparatusStatus::Deferred;
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
FSubjectInfo::RemoveAllTraitsDeferred(const bool bForceEnqueue /*=false*/)
{
	AssessCondition(Paradigm, IsValid(), EApparatusStatus::InvalidState);

	if (!bForceEnqueue && (GetFingerprint().TraitsNum() == 0))
	{
		return EApparatusStatus::Noop;
	}

	const auto Mechanism = GetMechanism();
	Mechanism->EnqueueDeferred(AMechanism::FDeferredAllTraitsRemoval(GetHandle()));

	return EApparatusStatus::Deferred;
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
FSubjectInfo::DespawnDeferred(const bool bHard/*=true*/)
{
	if (UNLIKELY(!IsValid()))
	{
		return EApparatusStatus::Noop;
	}

	const auto Mechanism = GetMechanism();
	AssessCondition(Paradigm, Mechanism != nullptr, EApparatusStatus::InvalidState);

	if (LIKELY(bHard))
	{
		if (SetFlag(EFlagmarkBit::DeferredDespawn))
		{
			// The flag was already set before.
			return EApparatusStatus::Noop;
		}
		else
		{
			// This is the first time we're setting this flag.
			// Make sure the other handles get already invalidated:
			DoIncrementGeneration();
		}
	}

	// Enqueue the actual action till later.
	// The handle here can be of two kinds:
	// 1. In case of just set bDeferredDespawn flag it will have an already incremented generation.
	// 2. In case of not being incremented the handle will already be invalid and skipped silently
	// during the deferred removal process.
	Mechanism->EnqueueDeferred(AMechanism::FDeferredSubjectDespawn(GetHandle()));

	return EApparatusStatus::Deferred;
}

FORCEINLINE bool
FSubjectInfo::IsInNetworkMode(const ENetMode Mode) const
{
	const auto World = GetWorld();
	if (UNLIKELY(World == nullptr))
	{
		return Mode == ENetMode::NM_Standalone;
	}
	return World->IsNetMode(Mode);
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
FSubjectInfo::PushTrait(UScriptStruct* const TraitType,
						EPeerRole            PeerRole/*=EPeerRole::Auto*/,
						const bool           bReliable/*=true*/) const
{
	if (UNLIKELY(TraitType == nullptr))
	{
		return EApparatusStatus::Noop;
	}

	const void* const TraitPtr = GetTraitPtr<Paradigm>(TraitType);
	AssessConditionFormat(Paradigm, TraitPtr != nullptr, EApparatusStatus::Missing,
						  TEXT("The '%s' trait must be already within the subject in order to push it."),
						  *(TraitType->GetName()));
	const auto World = GetWorld();
	AssessCondition(Paradigm, World != nullptr, EApparatusStatus::InvalidState);

	if (AvoidConditionFormat(Paradigm, !IsOnline(), TEXT("The subject must be already online in order to push an existing trait for it.")))
	{
		return EApparatusStatus::InvalidState;
	}

	const bool IsListenServer = World->IsNetMode(NM_ListenServer);

	if (IsListenServer && (PeerRole == EPeerRole::Auto))
	{
		PeerRole = GetTraitmarkPermit().Contains(TraitType) ? EPeerRole::Client : EPeerRole::Server;
	}

	if (IsListenServer)
	{
		// The trait is already set for self.
		if (PeerRole == EPeerRole::Client)
		{
			// We don't have to do anything more,
			// if we're pushing a trait as a client.
			return EApparatusStatus::Success;
		}
	}

	// On a server, send the packed networked subject to all the clients in the same world.
	// On a client there will be only one player controller to use that will be used to transfer the trait...
	return UNetworkBearerComponent::BroadcastTrait<Paradigm>(World, GetNetworkId(), TraitType, TraitPtr, PeerRole, bReliable);
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
FSubjectInfo::PushTrait(UScriptStruct* const TraitType,
						const void* const    TraitData,
						EPeerRole            PeerRole/*=EPeerRole::Auto*/,
						const bool           bReliable/*=true*/)
{
	if (UNLIKELY(TraitType == nullptr))
	{
		return EApparatusStatus::Noop;
	}
	AssessCondition(Paradigm, TraitData != nullptr, EApparatusStatus::NullArgument);

	const auto World = GetWorld();
	AssessCondition(Paradigm, World != nullptr, EApparatusStatus::InvalidState);

	AssessOKFormat(Paradigm, BringOnline<Paradigm>(), TEXT("The subject must be online in order to push a trait for it."));

	const bool IsListenServer = World->IsNetMode(NM_ListenServer);

	if (IsListenServer && (PeerRole == EPeerRole::Auto))
	{
		PeerRole = GetTraitmarkPermit().Contains(TraitType) ? EPeerRole::Client : EPeerRole::Server;
	}

	if (IsListenServer)
	{
		// Also set up the trait directly when in listen server mode,
		// even without serializing it across the network:
		AssessOK(Paradigm, SetTrait<Paradigm>(TraitType, TraitData));
		if (PeerRole == EPeerRole::Client)
		{
			// We're a listen server acting as a client.
			// We don't have to do anything more, since
			// we already set the trait above.
			return EApparatusStatus::Success;
		}
	}

	// On a server, send the packed networked subject to all the clients in the same world.
	// On a client there will be only one player controller to use that will be used to transfer the trait...
	return UNetworkBearerComponent::BroadcastTrait<Paradigm>(World, GetNetworkId(), TraitType, TraitData, PeerRole, bReliable);
}
