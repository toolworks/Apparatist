/*
 * ░▒▓ APPARATUS ▓▒░
 *
 * File: Chunk.inl
 * Created: 2022-04-16 22:52:51
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

inline EApparatusStatus
UChunk::Initialize(const int32       InId,
				   const FTraitmark& InTraitmark,
				   const int32       Capacity)
{
	check(Capacity >= 0);
	check(InId != InvalidId);
	checkf(Id == InvalidId, TEXT("Only uninitialized chunks are allowed to be initialized."));
	checkf(!IsLocked(), TEXT("A chunk must be unlocked in order to change its traitmark."));
	check(!Slots.Num());

	Id = InId;

	Traitmark.Set(InTraitmark);

	const int32 LinesCount = Traitmark.TraitsNum();
	Slots.Reserve(Capacity);
	TraitsTemp = (void**)Apparatus_MallocArray(LinesCount, sizeof(*TraitsTemp));

	// Pre-allocate the trait lines:
	for (int32 li = 0; li < LinesCount; li++)
	{
		UScriptStruct* const TraitType = Traitmark[li];
		check(TraitType);
		Lines.Add(FScriptStructArray(TraitType, Capacity));
		TraitsTemp[li] = FMemory::Malloc(TraitType->GetStructureSize());
		TraitType->InitializeStruct(TraitsTemp[li]);
	}

	return EApparatusStatus::Success;
}

FORCEINLINE UChunk*
UChunk::New(AMechanism* const Owner,
			const int32       Id,
			const FTraitmark& InTraitmark)
{
	const FString Name = FString::Format(TEXT("AutoChunk_{0}"), {Id});
	UChunk* const Chunk = NewObject<UChunk>(Owner, FName(Name));
	check(Chunk);
	Chunk->Initialize(Id, InTraitmark);
	APPARATUS_REPORT_SUCCESS(TEXT("Created the new '%s' chunk."),
								  *Name);
	return Chunk;
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm>
UChunk::SwapTraits(const int32   SubjectIndex,
				   UChunk* const OtherChunk,
				   const int32   OtherSubjectIndex,
				   const bool    bClearMissing/*=true*/)
{
	AssessCondition(Paradigm, OtherChunk != nullptr, EApparatusStatus::NullArgument);
	AssessCondition(Paradigm, SubjectIndex > FSubjectInfo::InvalidSlotIndex, EApparatusStatus::InvalidArgument);
	AssessCondition(Paradigm, OtherSubjectIndex > FSubjectInfo::InvalidSlotIndex, EApparatusStatus::InvalidArgument);
	AssessCondition(Paradigm, SubjectIndex < Slots.Num(), EApparatusStatus::OutOfRange);
	AssessCondition(Paradigm, OtherSubjectIndex < OtherChunk->Slots.Num(), EApparatusStatus::OutOfRange);

	if (UNLIKELY(this == OtherChunk))
	{
		// The source chunk is the same as the destination.
		if (UNLIKELY(SubjectIndex == OtherSubjectIndex))
		{
			// Even the indices are the same. Do nothing:
			return EApparatusStatus::Noop;
		}
		// Swap within the same line...
		for (int32 i = 0; i < TraitLinesNum(); ++i)
		{
			Lines[i].Swap(SubjectIndex, OtherSubjectIndex);
		}
	}
	else
	{
		// Use mapping for faster transition:
		TArray<int32, TInlineAllocator<16>> Mapping;
		Traitmark.FindMappingTo(OtherChunk->Traitmark, Mapping);

		for (int32 SrcTraitIndex = 0; SrcTraitIndex < TraitLinesNum(); ++SrcTraitIndex)
		{
			const auto DstTraitIndex = Mapping[SrcTraitIndex];
			if (UNLIKELY(DstTraitIndex == INDEX_NONE)) 
			{
				if (LIKELY(bClearMissing))
				{
					Lines[SrcTraitIndex].ClearElementAt(SubjectIndex);
				}
				continue;
			}
			Lines[SrcTraitIndex].SwapElementWith(SubjectIndex, OtherChunk->Lines[DstTraitIndex][OtherSubjectIndex]);
		}
	}

	return EApparatusStatus::Success;
}

inline EApparatusStatus
UChunk::DoReleaseSlot(const int32 SlotIndex)
{
	if (IsSlotLocked(SlotIndex))
	{
		// The subject is locked now, so the actual buffer deletions have to be
		// delayed until it's unlocked again...
		EnqueueForRemoval(SlotIndex);
	}
	else
	{
		// The chunk is not locked, so the necessary actions
		// may be performed right now...
		const int32 LastSlotIndex = Count - 1;
		if (SlotIndex < LastSlotIndex)
		{
			// Not the last element was removed,
			// so we swap the removed element with the last one:
			DoReplaceSlot(SlotIndex, LastSlotIndex);
		}

		// Always remove the last subject from the chunk:
		DoPop();
	}

	return EApparatusStatus::Success;
}

inline EApparatusStatus
UChunk::DoReplaceSlot(const int32 DstSlotIndex,
			 		  const int32 SrcSlotIndex)
{
	if (UNLIKELY(DstSlotIndex == SrcSlotIndex)) return EApparatusStatus::Noop;

	check(DstSlotIndex > FSubjectInfo::InvalidSlotIndex && DstSlotIndex < Slots.Num());
	check(SrcSlotIndex > FSubjectInfo::InvalidSlotIndex && SrcSlotIndex < Slots.Num());
	auto& DstSlot = Slots[DstSlotIndex];
	checkf(DstSlot.IsStale(),
		   TEXT("Only stale (disposed) slots may be replaced."));

	if ((DstSlot.SubjectId != FSubjectInfo::InvalidId)
	  && DstSlot.HasFlag(EFlagmarkBit::DeferredDespawn))
	{
		const auto DstInfo = DstSlot.FindInfo();
		if (LIKELY(DstInfo && (DstInfo->Chunk == this)))
		{
			check(DstInfo->SlotIndex == DstSlotIndex);
			// This can actually be a deferred-removed
			// subject, so we have to make sure its info
			// is no longer referencing this chunk.
			DstInfo->Chunk     = nullptr;
			DstInfo->SlotIndex = InvalidSlotIndex;
		}
	}

	auto& SrcSlot = Slots[SrcSlotIndex];
	// Note, that the source slot can actually
	// be stale, since it can already be marked
	// as such with a hard deferred despawning.
	auto& SrcInfo = SrcSlot.GetInfo();

	// Swap the slots...
	Slots.Swap(DstSlotIndex, SrcSlotIndex);
	SrcSlot.Reset();

	// Swap the subjects' traits...
	for (int32 i = 0; i < TraitLinesNum(); i++)
	{
		Lines[i].Swap(DstSlotIndex, SrcSlotIndex);
		// The former destination slot could actually
		// hold up some left-over initialized traits
		// so we have to clear them explicitly now:
		Lines[i].ClearElementAt(SrcSlotIndex);
	}

	// Map the last element to its new place...
	SrcInfo.SlotIndex = DstSlotIndex;

	check(Slots[DstSlotIndex].GetHandle().IsValid());

	return EApparatusStatus::Success;
}

inline EApparatusStatus
UChunk::DoUnlock(const bool bWasSolid) const
{
	check(IterableCount != -1);
	// The chunk becomes fully unlocked just now.
	// Remove all of the queued subject indices from it
	// and clear up the traits safely now...

	const auto MutableThis = const_cast<UChunk*>(this);
	int32 LastSlotIndex = Slots.Num() - 1;
	int32 RemovedCount = 0;
	// Defrag preserving the slots places...
	for (int32 SlotIndex; DequeueFromRemoved(SlotIndex);)
	{
		check(!bWasSolid);
		check(SlotIndex > InvalidSlotIndex);
		checkf(Slots[SlotIndex].IsStale(),
				TEXT("A slot queued for removal must always be stale."));
		// Find the last non-stale slot to replace with...
		for (; LastSlotIndex > SlotIndex; --LastSlotIndex)
		{
			if (LIKELY(!Slots[LastSlotIndex].IsStale()))
			{
				break;
			}
		}
		if (LIKELY(LastSlotIndex > SlotIndex))
		{
			// Not the last element was removed. Replace the removed
			// element with the last one:
			MutableThis->DoReplaceSlot(SlotIndex, LastSlotIndex);
		}
		RemovedCount += 1;
	}
	// Clean-up the left-over stale slots at the end:
	MutableThis->DoPop(RemovedCount);

	APPARATUS_REPORT_SUCCESS(TEXT("The #%s chunk is unlocked now."), *ToString());
	return EApparatusStatus::Success;
}

// TODO: Insecure method. Refactor.
inline EApparatusStatus
UChunk::FetchTraitsPtrs(const int32          SubjectIndex,
						const TArray<int32>& Mapping,
						void**               OutTraits)
{
	check(SubjectIndex > FSubjectInfo::InvalidSlotIndex);
	check(SubjectIndex < Slots.Num());
	check(Mapping.Num() <= TraitLinesNum());

	for (int32 i = 0; i < Mapping.Num(); ++i) 
	{
		const int32 TraitIndex = Mapping[i];
		OutTraits[i] = TraitPtrAt(SubjectIndex, TraitIndex);
	}

	return EApparatusStatus::Success;
}

inline EApparatusStatus
UChunk::ApplyAdjectives(FSubjectInfo* Subject)
{
	// Create a temporary subject handle for safety:
	FSubjectHandle s;
	Subject->GetHandle(s);
	check(s.IsValid());

	for (int32 i = 0; i < Adjectives.Num(); ++i)
	{
		const auto& Info = Adjectives[i];
		check(Info.Handler != nullptr);
		check(Info.Lines.Num() <= TraitLinesNum());

		// Fetch the traits:
		for (int32 j = 0; j < Info.Lines.Num(); ++j)
		{
			const int32 TraitIndex = Info.Lines[j];
			verify(OK(TraitAt(Subject->SlotIndex, TraitIndex, TraitsTemp[TraitIndex])));
		}

		// Call the handler now:
		Info.Handler(s, Info.Traits);
	}

	return EApparatusStatus::Success;
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm, FSubjectInfo::SlotIndexType>
UChunk::ReserveSubjectSlot()
{
	if (AvoidConditionFormat(Paradigm, IsSolidLocked(),
							 TEXT("Can not reserve a slot in a solid-locked chunk. Is there a solid iterating happening?")))
	{
		return MakeOutcome<Paradigm, FSubjectInfo::SlotIndexType>(EApparatusStatus::InvalidState, FSubjectInfo::InvalidSlotIndex);
	}
	if (AvoidConditionFormat(Paradigm, GetOwner()->IsInConcurrentEnvironment(),
							 TEXT("Can not reserve a chunk slot in a concurrent environment. Is there a concurrent operating currently happening?")))
	{
		return MakeOutcome<Paradigm, FSubjectInfo::SlotIndexType>(EApparatusStatus::InvalidState, FSubjectInfo::InvalidSlotIndex);
	}

	auto SlotIndex = FSubjectInfo::InvalidSlotIndex;

	const auto OldCount = Slots.Num();
	if (AvoidConditionFormat(Paradigm, OldCount == FSubjectInfo::SlotsPerChunkMax,
						 TEXT("The maximum number of subject per chunk was already reached: %i"), (int)OldCount))
	{
		return MakeOutcome<Paradigm, FSubjectInfo::SlotIndexType>(EApparatusStatus::OutOfLimit, FSubjectInfo::InvalidSlotIndex);
	}

	const auto NewCount = OldCount + 1;
	const auto LinesCount = TraitLinesNum();

	SlotIndex = OldCount;

	// Add an actual slot:
	Slots.AddDefaulted();

	// The fingerprint still has to be empty
	// for the empty slots, for the purpose of
	// using the move instead of copying later,
	// so we don't initialize it here at all.

	// Initialize the new traits for the new subject:
	for (int32 li = 0; li < LinesCount; li++)
	{
		Lines[li].AppendDefaulted();
	}

	Count = NewCount;

	return MakeOutcome<Paradigm, FSubjectInfo::SlotIndexType>(EApparatusStatus::Success, SlotIndex);
}

FORCEINLINE EApparatusStatus
UChunk::OverwriteTraits(const int32   SrcSubjectIndex,
						UChunk* const DstChunk,
						const int32   DstSubjectIndex) const
{
	check(DstChunk);
	check(SrcSubjectIndex != FSubjectInfo::InvalidSlotIndex);
	check(DstSubjectIndex != FSubjectInfo::InvalidSlotIndex);
	check(SrcSubjectIndex < Slots.Num());
	check(DstSubjectIndex < DstChunk->Slots.Num());
	checkf(this != DstChunk, TEXT("Can't copy traits to itself when using a constant version."));

	// Use mapping for faster transition:
	TArray<int32, TInlineAllocator<16>> Mapping;
	Traitmark.FindMappingTo(DstChunk->Traitmark, Mapping);

	for (int32 SrcTraitIndex = 0; SrcTraitIndex < TraitLinesNum(); ++SrcTraitIndex)
	{
		const int32 DstTraitIndex = Mapping[SrcTraitIndex];
		if (DstTraitIndex == -1) continue;
		Lines[SrcTraitIndex].ElementAt(SrcSubjectIndex, DstChunk->Lines[DstTraitIndex][DstSubjectIndex]);
	}

	return EApparatusStatus::Success;
}

inline EApparatusStatus
UChunk::OverwriteTraits(const int32   SrcSubjectIndex,
						UChunk* const DstChunk,
						const int32   DstSubjectIndex)
{
	check(DstChunk);
	check(SrcSubjectIndex != FSubjectInfo::InvalidSlotIndex);
	check(DstSubjectIndex != FSubjectInfo::InvalidSlotIndex);
	check(SrcSubjectIndex < Slots.Num());
	check(DstSubjectIndex < DstChunk->Slots.Num());

	if (UNLIKELY(TraitLinesNum() == 0))
	{
		// There are no trait lines to copy:
		return EApparatusStatus::Noop;
	}

	if (UNLIKELY(this == DstChunk))
	{
		// The source chunk is the same as the destination.
		if (UNLIKELY(SrcSubjectIndex == DstSubjectIndex))
		{
			// Even the indices are the same. Do nothing:
			return EApparatusStatus::Noop;
		}
		for (int32 i = 0; i < TraitLinesNum(); ++i)
		{
			Lines[i].ElementAt(SrcSubjectIndex, Lines[i][DstSubjectIndex]);
		}
	}
	else
	{
		// Use mapping for faster transition:
		TArray<int32, TInlineAllocator<16>> Mapping;
		Traitmark.FindMappingTo(DstChunk->Traitmark, Mapping);

		for (int32 SrcTraitIndex = 0; SrcTraitIndex < TraitLinesNum(); ++SrcTraitIndex)
		{
			const int32 DstTraitIndex = Mapping[SrcTraitIndex];
			if (UNLIKELY(DstTraitIndex == -1)) continue;
			Lines[SrcTraitIndex].ElementAt(SrcSubjectIndex, DstChunk->Lines[DstTraitIndex][DstSubjectIndex]);
		}
	}

	return EApparatusStatus::Success;
}

FORCEINLINE AMechanism*
UChunk::GetOwner() const
{
	// Cast checked has asserted once, so...
	return Cast<AMechanism>(GetOuter());
}

inline bool
UChunk::Check() const
{
	if (!IsValidChunk(this)) return false;
	if (!IsLocked())
	{
		for (int32 i = 0; i < Slots.Num(); ++i)
		{
			if (!Slots[i].GetHandle())
			{
				return false;
			}
		}
	}
	return true;
}
