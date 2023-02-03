/*
 * ░▒▓ APPARATUS ▓▒░
 *
 * File: BeltSlot.inl
 * Created: 2023-01-18 11:05:10
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

#pragma region Belt Slot Inlines

FORCEINLINE FBeltSlot::~FBeltSlot()
{
	checkf(!HasLockedDetails(),
		   TEXT("The #%d slot must be unlocked before destruction."),
		   Index);
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm, UDetail*>
FBeltSlot::GetDetail(const TSubclassOf<UDetail> DetailClass) const
{
	check(DetailClass != nullptr);
	const auto& Detailmark = GetDetailmark();
	const auto DetailClassIndex = Detailmark.IndexOf(DetailClass);
	if (AvoidConditionFormat(Paradigm, DetailClassIndex == InvalidDetailLineIndex,
							 TEXT("An invalid index for a '%s' detail class to get: %d"),
							 *(DetailClass->GetName()), (int)DetailClassIndex))
	{
		return MakeOutcome<Paradigm, UDetail*>(EApparatusStatus::Missing, nullptr);
	}
	return DetailAtLine(DetailClassIndex);
}

template < EParadigm Paradigm   /*=EParadigm::DefaultInternal*/,
		   typename  AllocatorT /*=FDefaultAllocator*/ >
inline TOutcome<Paradigm>
FBeltSlot::GetDetails(const TSubclassOf<UDetail>    DetailClass,
					  TArray<UDetail*, AllocatorT>& OutDetails) const
{
	check(DetailClass != nullptr);
	OutDetails.Reset();
	const auto& Detailmark = GetDetailmark();
	TArray<DetailLineIndexType, TInlineAllocator<3>> Indices;
	Detailmark.IndicesOf(DetailClass, Indices);
	if (Indices.Num() == 0)
	{
		return MakeOutcome<Paradigm>(EApparatusStatus::NoItems);
	}
	return DetailsAtLines(Indices, OutDetails);
}

template < EParadigm Paradigm   /*=EParadigm::DefaultInternal*/,
		   class     D          /*=UDetail*/,
		   typename  AllocatorT /*=FDefaultAllocator*/,
		   TDetailClassSecurity<D> /*=true*/ >
FORCEINLINE TOutcome<Paradigm>
FBeltSlot::GetDetails(TArray<D*, AllocatorT>& OutDetails) const
{
	OutDetails.Reset();
	const auto& Detailmark = GetDetailmark();
	TArray<DetailLineIndexType, TInlineAllocator<3>> Indices;
	Detailmark.IndicesOf(D::StaticClass(), Indices);
	if (Indices.Num() == 0)
	{
		return MakeOutcome<Paradigm>(EApparatusStatus::NoItems);
	}
	return DetailsAtLines(Indices, OutDetails);
}

FORCEINLINE void
FBeltSlot::ResetDetails() const
{
	for (int32 i = 0; i < Details.Num(); i++)
	{
		Details[i].Reset();
	}
}

FORCEINLINE FSubjectHandle
FBeltSlot::GetSubject() const
{
	if (LIKELY(Subjective))
	{
		return Subjective->GetHandle();
	}
	checkf(!HasAnyDetails(), TEXT("A belt slot without a subjective must not have any details set."));
	return FSubjectHandle::Invalid;
}

inline void
FBeltSlot::SetSubjective(ISubjective* const InSubjective)
{
	if (UNLIKELY(Subjective == InSubjective))
	{
		// The subjective is the same as it was.
		// Maybe it was brought back actually?
		if (UNLIKELY(Subjective && bStale))
		{
			FetchDetails();
			bStale = false;
		}
		return;
	}

	if (Subjective && !InSubjective)
	{
		// Need to reset the subjective...
		if (!bStale)
		{
			// We are to reset the current subjective to a null.
			// Mark it as taking no slots explicitly...
			Subjective->TakeBeltSlot(nullptr, InvalidIndex);
		}
	}
	else
	{
		// Subjective is to be changed to a valid one...
		checkf(!HasLockedDetails(),
			   TEXT("The slot must be unlocked in order for "
					"subjective to be set or changed to a new valid one."));
		checkf(Subjective || bStale,
			   TEXT("Can set anew only a stale or an already occupied slot."));
	}

	Subjective = InSubjective;
	ResetDetails();
	bStale = !Subjective;
	if (!bStale)
	{
		// The new subjective is valid,
		// so fetch its details now:
		FetchDetails();
	}
}

inline bool
FBeltSlot::FetchDetails()
{
	check(Owner);

	if (UNLIKELY(Subjective == nullptr))
	{
		ResetDetails();
		return false;
	}

	const FDetailmark& BeltDetailmark = Owner->GetDetailmark();
	if (LIKELY(BeltDetailmark.DetailsNum() > 0))
	{
		// Self-expand just in case:
		Expand();

		check(Details.Num() == BeltDetailmark.DetailsNum());
		check(Details.Num() >= Subjective->GetFingerprint().DetailsNum());

		for (int32 i = 0; i < BeltDetailmark.DetailsNum(); ++i)
		{
			Details[i].Fetch(BeltDetailmark.DetailAt(i), Subjective);
		}
	}

	return true;
} //-FetchDetails

inline EApparatusStatus
FBeltSlot::SetRemoved(const bool bInRemoved) 
{
	check(Owner);
	if (UNLIKELY(bInRemoved == bStale))
	{
		return EApparatusStatus::Noop;
	}
	bStale = bInRemoved;
	if (bInRemoved)
	{
		if (LIKELY(Subjective && (Subjective->GetBelt() == Owner)))
		{
			// Clear the subjective slot if it's still
			// associated with this belt:
			Subjective->TakeBeltSlot(nullptr, InvalidIndex);
		}
		if (IsLocked())
		{
			// The belt is currently locked, and an iterable
			// slot is being removed. We have to defer
			// the actual removal for later, when the belt gets unlocked...
			Owner->EnqueueForRemoval(Index);
		}
		else
		{
			// If the slot is not locked now,
			// clear it instantly:
			Subjective = nullptr;
			ResetDetails();

			check(Owner->Count >= 1);
			Owner->Count -= 1;

			auto& Slots = Owner->Slots;

			if (UNLIKELY(Index == Owner->Count))
			{
				// The last slot is about to be removed.
				// Do nothing and return:
				return EApparatusStatus::Success;
			}

			// This is not the last slot to be removed.
			// Swap it with the last one...
			const auto IndexSave = Index; // Index needs to be saved.
			Slots.Swap(Index, Owner->Count);
			// Restore the indices...
			Slots[Owner->Count].Index = Owner->Count;
			Index = IndexSave;

			// We now should be in a new state of a swapped in slot:
			check(this == std::addressof(Slots[Index]));
			if (LIKELY(Subjective))
			{
				// Change the subjective's slot
				// to a new present place:
				Subjective->TakeBeltSlot(Owner, Index);
			}
		}
	}
	else
	{
		checkf(!Subjective,
			   TEXT("Only subjective-cleared slots may be set as not removed."));
	}
	return EApparatusStatus::Success;
}

FORCEINLINE bool
FBeltSlot::IsLocked() const
{
	check(Index > UBelt::InvalidDetailLineIndex);
	return Owner && (Index < Owner->IterableCount);
}

FORCEINLINE bool
FBeltSlot::PrepareForIteration(const FFilter& InFilter) const
{
	check(Owner);
	check((Index != InvalidIndex) && (Index < Owner->IterableCount));

	if (UNLIKELY(!Subjective->Matches(InFilter)))
	{
		return false;
	}

	// Matching by filter is not enough,
	// since the subjective may miss certain details
	// iterating-time.

	// Lock the cached details needed:
	for (int32 i = 0; i < InFilter.DetailsNum(); ++i)
	{
		const auto DetailClass = InFilter.DetailAt(i);
		check(DetailClass != nullptr);
		bool bHasMatch = false;
		for (int32 j = 0; j < Details.Num(); ++j)
		{
			if (Details[j].HasChildOf(DetailClass))
			{
				bHasMatch = true;
				Details[j].Lock();
			}
		}
		if (UNLIKELY(!bHasMatch))
			return false;
	}

	return true;
}

template < typename AllocatorT/*=FDefaultAllocator*/ >
FORCEINLINE bool
FBeltSlot::PrepareForIteration(const FFilter&                                            InFilter,
							   const TArray<FBeltSlot::DetailLineIndexType, AllocatorT>& InMainlineIndices) const
{
	check(Owner);
	check((Index != InvalidIndex) && (Index < Owner->IterableCount));

	if (UNLIKELY(!Subjective->Matches(InFilter)))
	{
		return false;
	}

	// Lock the cached details needed:
	for (int32 i = 0; i < InMainlineIndices.Num(); ++i)
	{
		const auto DetailLineIndex = InMainlineIndices[i];
		check(DetailLineIndex != InvalidDetailLineIndex);
		Details[DetailLineIndex].Lock();
		if (UNLIKELY(!Details[DetailLineIndex].HasAny()))
		{
			return false;
		}
	}

	return true;
}

inline EApparatusStatus
FBeltSlot::Unlock() const
{
	check(Owner);
	check(Owner->IterableCount >= 0);
	check((Index > InvalidIndex) && (Index < Owner->IterableCount));

	// Unlock all of the cache...
	for (int32 i = 0; i < Details.Num(); ++i)
	{
		Details[i].Unlock();
	}

	return EApparatusStatus::Success;
}

FORCEINLINE
FBeltSlot::FBeltSlot(class UBelt* Belt) : Owner(Belt) 
{ 
	Index = Belt->Slots.Num(); 
}

FORCEINLINE const FDetailmark&
FBeltSlot::GetDetailmark() const
{
	check(Owner);
	return Owner->GetDetailmark();
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm, UDetail*>
FBeltSlot::DetailAtLine(const int32 DetailLineIndex) const
{
	check(Owner);
	check((DetailLineIndex != InvalidDetailLineIndex) && (DetailLineIndex < Details.Num()));

	check(Details[DetailLineIndex].IterableNum() >= 1);
	return MakeOutcome<Paradigm, UDetail*, EApparatusStatus::Success>(Details[DetailLineIndex][0]);
} //-DetailAtLine

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/,
		   typename AllocatorT/*=FDefaultAllocator*/ >
inline TOutcome<Paradigm>
FBeltSlot::DetailsAtLine(const int32                   DetailIndex,
						 TArray<UDetail*, AllocatorT>& OutDetails) const
{
	check(Owner);
	check((DetailIndex > INDEX_NONE) && (DetailIndex < Details.Num()));

	OutDetails = Details[DetailIndex];
} //-DetailsAtLine

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/,
		   class     D/*=UDetail*/,
		   typename  LinesAllocatorT/*=FDefaultAllocator*/,
		   typename  AllocatorT/*=FDefaultAllocator*/,
		   TDetailClassSecurity<D>/*=true*/ >
inline TOutcome<Paradigm>
FBeltSlot::DetailsAtLines(const TArray<int32, LinesAllocatorT>& DetailLineIndices,
						  TArray<D*, AllocatorT>&               OutDetails) const
{
	check(Owner);
	
	OutDetails.Reset();
	for (const auto DetailIndex : DetailLineIndices)
	{
		check((DetailIndex != INDEX_NONE) && (DetailIndex < Details.Num()));
		for (int32 i = 0; Details[DetailIndex].IterableNum(); ++i)
		{
			const auto Detail = CastChecked<D>(Details[DetailIndex][i]);
			if (LIKELY(Detail->IsEnabled()))
			{
				OutDetails.Add(Detail);
			}
		} 
	}
	return (OutDetails.Num() > 0) ? EApparatusStatus::Success : EApparatusStatus::NoItems;
} //-DetailsAtLines

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/,
		   typename  LinesAllocatorT/*=FDefaultAllocator*/,
		   typename  AllocatorT/*=FDefaultAllocator*/ >
inline TOutcome<Paradigm>
FBeltSlot::DetailsAtLines(const TArray<int32, LinesAllocatorT>& DetailLineIndices,
						  TArray<UDetail*, AllocatorT>&         OutDetails) const
{
	check(Owner);
	
	OutDetails.Reset();
	for (const auto DetailIndex : DetailLineIndices)
	{
		check((DetailIndex != INDEX_NONE) && (DetailIndex < Details.Num()));
		for (int32 i = 0; Details[DetailIndex].IterableNum(); ++i)
		{
			const auto Detail = Details[DetailIndex][i];
			check(Detail != nullptr);
			if (Detail->IsEnabled())
			{
				OutDetails.Add(Detail);
			}
		} 
	}
	return (OutDetails.Num() > 0) ? EApparatusStatus::Success : EApparatusStatus::NoItems;
} //-DetailsAtLines

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm, UDetail*>
FBeltSlot::GetDetailHinted(const TSubclassOf<UDetail> DetailClass,
						   const int32                DetailIndexHint) const
{
	check(Owner);

	const FDetailmark& Detailmark = GetDetailmark();
	if (LIKELY((DetailIndexHint != InvalidDetailLineIndex) && (DetailIndexHint < Detailmark.DetailsNum())))
	{
		const TSubclassOf<UDetail> RealDetailClass = Detailmark[DetailIndexHint];
		if (LIKELY(RealDetailClass->IsChildOf(DetailClass)))
		{
			return DetailAtLine(DetailIndexHint);
		}
	}

	// The detail class doesn't match.
	// Try to find the detail index within the active
	// detailmark...
	const int32 RealIndex = Detailmark.IndexOf(DetailClass);
	if (LIKELY(RealIndex != UBelt::InvalidDetailLineIndex)) 
	{
		return DetailAtLine(RealIndex);
	}
	// Detail still was not found.
	// This may happen when the subjective is removed from the belt...
	if (LIKELY(Subjective))
	{
		return Subjective->GetDetail(DetailClass);
	}
	return nullptr;
}

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/,
		   typename  AllocatorT /*=FDefaultAllocator*/ >
inline TOutcome<Paradigm>
FBeltSlot::GetDetailsHinted(const TSubclassOf<UDetail>    DetailClass,
							const int32                   DetailIndexHint,
                            TArray<UDetail*, AllocatorT>& OutDetails) const
{
	check(Owner);

    OutDetails.Reset();
	const FDetailmark& Detailmark = GetDetailmark();
	if (LIKELY((DetailIndexHint != InvalidDetailLineIndex) && (DetailIndexHint < Detailmark.DetailsNum())))
	{
		const TSubclassOf<UDetail> RealDetailClass = Detailmark[DetailIndexHint];
		if (LIKELY(RealDetailClass->IsChildOf(DetailClass)))
		{
			return DetailsAtLine<Paradigm>(DetailIndexHint, OutDetails);
		}
	}

	// The detail class doesn't match.
	// Try to find the detail index within the active
	// detailmark...
	const auto RealIndex = Detailmark.IndexOf(DetailClass);
	if (LIKELY(RealIndex != InvalidDetailLineIndex))
	{
		return DetailsAtLine<Paradigm>(RealIndex);
	}
	// Detail still was not found.
	// This may happen when the subjective is already removed from the belt...
	if (LIKELY(Subjective != nullptr))
	{
		return Subjective->template GetDetails<Paradigm>(DetailClass, OutDetails);
	}
	return (OutDetails.Num() > 0) ? EApparatusStatus::Success : EApparatusStatus::NoItems;
}

inline bool
FBeltSlot::IsSkipped(const FFilter& InFilter) const
{
	if (UNLIKELY(IsStale())) return true;
	if (UNLIKELY(!Owner)) return true;
	check(Owner->IsLocked());
	
	if (UNLIKELY(!Subjective->Matches(InFilter)))
	{
		return true;
	}

	return false;
}

FORCEINLINE bool
FBeltSlot::IsViable() const
{
	if (IsStale()) return false;
	return (Index > InvalidIndex) && (Index < Owner->Count);
}

FORCEINLINE bool
FBeltSlot::IsViable(const FFilter& InFilter) const
{
	if (UNLIKELY(IsStale())) return false;
	if (UNLIKELY(!Owner)) return false;
	
	return Subjective->Matches(InFilter);
}

FORCEINLINE void
FBeltSlot::Expand()
{
	check(Owner);
	// We need a full detailmark here, not the possibly locked one:
	const FDetailmark& TargetDetailmark = Owner->GetDetailmark();
	Details.Reserve(TargetDetailmark.DetailsNum());
	while (Details.Num() < TargetDetailmark.DetailsNum())
	{
		Details.Add(FBeltSlotCache(this));
	}
}

#pragma endregion Belt Slot Inlines
