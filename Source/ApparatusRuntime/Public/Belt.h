/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Belt.h
 * Created: Friday, 23rd October 2020 7:00:48 pm
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

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define BELT_H_SKIPPED_MACHINE_H
#endif

#include "Filter.h"
#include "Iterable.h"
#include "BeltSlot.h"

#ifdef BELT_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "Belt.generated.h"


DECLARE_STATS_GROUP(TEXT("Belt"), STATGROUP_Belt, STATCAT_Advanced);

// Forward declarations:
template < typename SubjectHandleT >
struct TBeltIt;
class AMechanism;
class UMachine;

/**
 * The conveyor belt consisting of subjects.
 */
UCLASS(BlueprintType)
class APPARATUSRUNTIME_API UBelt
  : public UObject
  , public IIterable
{
	GENERATED_BODY()

  public:

	enum
	{
		/**
		 * Invalid slot index.
		 */
		InvalidSlotIndex = FBeltSlot::InvalidIndex,

		/**
		 * Invalid detail line index.
		 */
		InvalidDetailLineIndex = FBeltSlot::InvalidDetailIndex
	};

	/**
	 * The belt's tag type.
	 * 
	 * This should be able to hold
	 * the UObject's Unique Id.
	 */
	typedef uint32 TagType;

	/**
	 * An invalid belt tag constant.
	 * 
	 * Matches the Object's default internal index value.
	 */
	static constexpr TagType InvalidTag = (TagType)INDEX_NONE;

  private:

	/**
	 * The detailmark of the belt.
	 * 
	 * The belt is sparse and may still miss some of the details
	 * during the iteration.
	 * 
	 * @note The detailmark is decomposed and contains
	 * separate detail entries for child and base classes
	 * for the sake of correct iterating.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Belt,
			  Meta = (AllowPrivateAccess = "true"))
	FDetailmark Detailmark;

	/**
	 * Is this a sparse belt allowing some empty detail spaces?
	 */
	static constexpr bool bSparse = true;

  protected:

	EApparatusStatus
	DoUnlock(const bool bWasSolid) const override;

  public:

	/**
	 * Is this a sparse belt allowing some empty detail spaces?
	 */
	constexpr FORCEINLINE bool
	IsSparse() const { return bSparse; }

	/**
	 * The current decomposed detailmark of the belt.
	 * 
	 * @note All of the base class details
	 * are decomposed into separate details here.
	 * 
	 * @note This is a detailmark for a sparse storage
	 * and some details may actually be missing
	 * within the physical belt slots.
	 * 
	 * @note This detailmark can change due to
	 * further belt expansions.
	 * 
	 * @return The current detailmark of the belt.
	 */
	FORCEINLINE const FDetailmark&
	GetDetailmark() const
	{
		return Detailmark;
	}

	/**
	 * Get the line index for a detail class.
	 * 
	 * @param DetailClass The detail class to query for.
	 * @return The hinting detail index.
	 */
	FORCEINLINE int32
	DetailLineIndexOf(const TSubclassOf<UDetail> DetailClass) const
	{
		return GetDetailmark().IndexOf(DetailClass);
	}

	/**
	 * Get the line index for a detail class.
	 * 
	 * @note The method actually supports non-detail classes,
	 * i.e. @c INDEX_NONE will be safely returned in that case.
	 * 
	 * @tparam D The detail class to query for.
	 * @return The hinting detail index.
	 */
	template < class D >
	FORCEINLINE constexpr int32
	DetailLineIndexOf() const
	{
		return GetDetailmark().template IndexOf<D>();
	}

  private:

	friend struct FBeltSlot;
	template < typename SubjectHandleT >
	friend struct TBeltIt;
	friend class USubjectiveActorComponent;
	friend class USubjectiveUserWidget;
	friend class UDetail;
	friend class AMechanism;
	friend class UMachine;
	friend class FApparatusRuntimeModule;
	friend class ISubjective;

	/**
	 * The unique tag for the belt.
	 */
	TagType Tag = InvalidTag;

	/**
	 * The slots of the belt holding the detail caches.
	 */
	TArray<FBeltSlot> Slots;

	/**
	 * Destroy the belt object.
	 */
	void
	BeginDestroy() override;

	/**
	 * Release a slot from the belt at a specific index.
	 */
	EApparatusStatus
	ReleaseSlotAt(const int32 SlotIndex);

  public:

	/**
	 * Get the owning mechanism of the belt.
	 */
	AMechanism*
	GetOwner() const;

	/**
	 * Get the preset tag of the belt.
	 * 
	 * This one is set automatically
	 * when creating a belt from a preferred one.
	 * 
	 * @return The preset tag of the belt
	 * or UBelt::InvalidTag, if none is assigned. 
	 */
	FORCEINLINE TagType
	GetTag() const
	{
		return Tag;
	}

	/**
	 * Expand a belt to accommodate the new detailmark.
	 *
	 * @note The target detailmark will be added
	 * while being decomposed to its base classes.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InDetailmark The new detailmark to include.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop If nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	Expand(const FDetailmark& InDetailmark);

	/**
	 * Get a belt slot by its index. Constant version.
	 * 
	 * @param SlotIndex The index of the belt slot.
	 * @return The belt slot.
	 */
	FORCEINLINE const FBeltSlot&
	SlotAt(const int32 SlotIndex) const
	{
		return Slots[SlotIndex];
	}

	/**
	 * Get a belt slot by its index.
	 * 
	 * @param SlotIndex The index of the belt slot.
	 * @return The belt slot.
	 */
	FORCEINLINE FBeltSlot&
	SlotAt(const int32 SlotIndex)
	{
		return Slots[SlotIndex];
	}

	/**
	 * Get a belt slot by its index. Constant version.
	 * 
	 * @param SlotIndex The index of the belt slot.
	 * @return The belt slot.
	 */
	FORCEINLINE const FBeltSlot&
	operator[](const int32 SlotIndex) const
	{
		return SlotAt(SlotIndex);
	}

	/**
	 * Get a belt slot by its index.
	 * 
	 * @param SlotIndex The index of the belt slot.
	 * @return The belt slot.
	 */
	FORCEINLINE FBeltSlot&
	operator[](const int32 SlotIndex)
	{
		return SlotAt(SlotIndex);
	}

	/**
	 * Begin iterating the belt under a certain filter.
	 * 
	 * @tparam BeltItT The type of the belt iterator to use.
	 * @param Filter The filter to iterate under.
	 * @param Offset The offset for the slot to begin with.
	 * @return The iterator to use for the iterating.
	 * Will point to the end, if the were no suiting slots
	 * currently available.
	 */
	template < typename BeltItT  >
	BeltItT
	Begin(const FFilter& Filter,
		  const int32    Offset = 0)
	{
		if (UNLIKELY((Count <= Offset) ||
					 (IsLocked() && IterableCount <= Offset)))
		{
			// The offset is out of iterable range.
			return End<BeltItT>();
		}

		return BeltItT(this, Filter, Offset);
	}

	/**
	 * Get the ending iterator.
	 *
	 * @tparam BeltItT The type of the belt iterator.
	 * @return The ending iterator.
	 */
	template < typename BeltItT >
	FORCEINLINE BeltItT
	End() const
	{
		return BeltItT(this); 
	}

	/**
	 * Refresh a subjective within the belt.
	 * 
	 * The subjective might actually be in a different
	 * belt before calling this method. In such
	 * case it will be moved to this belt.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param Subjective A subjective to refresh.
	 * Must not be a @c nullptr.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	Refresh(ISubjective* const Subjective)
	{
		AssessCondition(Paradigm, Subjective != nullptr, EApparatusStatus::NullArgument);

		FBeltSlot* Slot = Subjective->GetSlotPtr();
		if (Slot != nullptr)
		{
			// We were in some belt before...
			UBelt* Belt = Slot->GetOwner();
			if (Belt == this)
			{
				// The old belt is the same as new.
				// Just re-fetch the details and exit...
				Slot->FetchDetails();
				return EApparatusStatus::Success;
			}
			else if (Belt != nullptr)
			{
				// Remove from the previous belt first.
				Belt->ReleaseSlotAt(Subjective->GetSlotIndex());
			}
		}

		// The subjective now has no active slot (and no belt so to say).
		// Place the subjective within this belt...
		checkf(Detailmark.GetDetailsMask().Includes(Subjective->GetFingerprint().GetDetailsMask()),
			   TEXT("The belt must have been already expanded for the subjective's details."));

		check((!IsLocked()) || (Count >= IterableCount));

		if (Slots.Num() == Count)
		{
			// No more slots available.
			// Add a new one:
			Slots.Add(FBeltSlot(this));
			Slot = &Slots[Count];
		}
		else
		{
			// Allocated slots still available.
			// Use the first of the rest...
			Slot = &Slots[Count];
			check(Slot->Index == Count);
		}

		Slot->SetSubjective(Subjective);
		check(Slot->Subjective == Subjective);

		Subjective->TakeBeltSlot(this, Count);

		Count += 1;
		return EApparatusStatus::Success;
	}

	/**
	 * Check if the belt matches a filter.
	 */
	bool
	Matches(const FFilter& Filter) const override;

	/**
	 * Create a new belt instance.
	 */
	static UBelt*
	New(UObject* const     Owner,
		const FDetailmark& InDetailmark)
	{
		UBelt* const Belt = NewObject<UBelt>(Owner);
		Belt->Expand(InDetailmark);
		return Belt;
	}

	/**
	 * Create a new belt instance with a specific name.
	 */
	static UBelt*
	New(UObject* const     Owner,
	    const FName&       Name,
		const FDetailmark& InDetailmark)
	{
		UBelt* const Belt = NewObject<UBelt>(Owner, Name);
		Belt->Expand(InDetailmark);
		return Belt;
	}

}; //-class UBelt


FORCEINLINE bool
UBelt::Matches(const FFilter& InFilter) const
{
	if (!bSparse)
	{
		return Detailmark.Matches(InFilter);
	}
	// Sparse belts may actually have holes in the subjective slots,
	// so the details <- filter exclusions are tested during the iteration, not here.
	return Detailmark.GetDetailsMask().Includes(InFilter.GetDetailsMask());
}

inline EApparatusStatus
UBelt::DoUnlock(const bool bWasSolid) const
{
	check(IterableCount != -1);
	// Unlock the slots also now...
	for (int32 i = 0; i < IterableCount; ++i)
	{
		Slots[i].Unlock();
	}

	const auto MutableThis = const_cast<UBelt*>(this);
	int32 LastSlotIndex = Count - 1;
	int32 RemovedCount = 0;

	// Defrag while preserving the slot indices...
	for (int32 SlotIndex; DequeueFromRemoved(SlotIndex);)
	{
		check(!bWasSolid);
		check(SlotIndex != InvalidSlotIndex);
		auto& Slot = MutableThis->Slots[SlotIndex];
		checkf(Slot.IsStale(),
			   TEXT("A slot queued to be removed should be stale."));
		Slot.SetSubjective(nullptr);
		// Find the last viable slot to swap with...
		for (; LastSlotIndex > SlotIndex; --LastSlotIndex)
		{
			if (LIKELY(Slots[LastSlotIndex].IsViable()))
			{
				break;
			}
		}
		if (LIKELY(SlotIndex < LastSlotIndex))
		{
			// Replace the removed slot with the last one:
			MutableThis->Slots.Swap(SlotIndex, LastSlotIndex);
			// Restore the indices...
			MutableThis->Slots[SlotIndex].Index     = SlotIndex;
			MutableThis->Slots[LastSlotIndex].Index = LastSlotIndex;
			Slots[LastSlotIndex].Subjective->TakeBeltSlot(SlotIndex);
		}
		RemovedCount += 1;
	}
	MutableThis->Count -= RemovedCount;

	APPARATUS_REPORT_SUCCESS(TEXT("The '%s' belt is unlocked now."), *GetName());
	return EApparatusStatus::Success;
}

#pragma region Belt Slot Cache

FORCEINLINE void
FBeltSlotCache::Fetch(const TSubclassOf<UDetail> DetailClass,
					  const ISubjective* const   Subjective)
{
	check(DetailClass != nullptr);
	check(Subjective != nullptr);

	Fetch(DetailClass, Subjective->GetDetailsRef());
}

#pragma endregion Belt Slot Cache

#pragma region Belt Slot

FORCEINLINE bool
FBeltSlot::IsLocked() const
{
	check(Index > UBelt::InvalidDetailLineIndex);
	return Owner && (Index < Owner->IterableCount);
}

inline int32
FBeltSlot::CalcIterableCombosCount(const FFilter&       InFilter,
								   const TArray<int32>& InDetailsIndices) const
{
	check(Owner);
	check(Owner->IsLocked())

	// Check if the current subjective is valid at all...
	if (UNLIKELY(IsStale()))
	{
		return 0;
	}

	// Check the subjective matches the filter...
	if (!Subjective->Matches(InFilter))
	{
		return 0;
	}

	int32 IterableCombosCount = 1; // 1 by default, since we'll multiply.
	for (int32 i = 0; i < InDetailsIndices.Num(); ++i)
	{
		const int32           DetailIndex = InDetailsIndices[i];
		const FBeltSlotCache& Cache       = Details[DetailIndex];
		const int32           c           = Cache.IterableNum();
		if (c == 0)
		{
			return 0;
		}
		IterableCombosCount *= c;
	}

	return IterableCombosCount;
} //-CalcIterableCombosCount

FORCEINLINE int32
FBeltSlot::BeginIteration(const FFilter&       InFilter,
						  const TArray<int32>& InDetailsIndices) const
{
	check(Owner);
	check((Index > InvalidIndex) && (Index < Owner->IterableCount));

	// Lock the cached details needed:
	for (int32 i = 0; i < InDetailsIndices.Num(); ++i)
	{
		// Isn't using MapIndex() here since it requires
		// an already locked state we don't have yet:
		const int32 DetailIndex = InDetailsIndices[i];
		Details[DetailIndex].Lock();
	}

	return CalcIterableCombosCount(InFilter, InDetailsIndices);
}

FORCEINLINE int32
FBeltSlot::PrepareForIteration(const FFilter&       InFilter,
							   const TArray<int32>& InDetailsIndices) const
{
	return BeginIteration(InFilter, InDetailsIndices);
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
FBeltSlot::DetailAtLine(const TArray<int32>& DetailsIndices,
						const int32          ComboIndex,
						const int32          DetailIndex) const
{
	check(Owner);
	check(ComboIndex > InvalidComboIndex);
	check(DetailIndex >= 0 && DetailIndex < Details.Num());
	checkSlow(DetailsIndices.Contains(DetailIndex));

	UDetail* RetDetail = nullptr;

	if (LIKELY(Details[DetailIndex].IterableNum() == 1))
	{
		// A single detail is present, so return it.
		RetDetail = Details[DetailIndex][0].Get();
	}
	else
	{
		// Multiple details are in the list.
		// Get a detail according to a current combination...
		int32 SubIndex = 0;
		int32 AccumCount = 1; // Accumulated detail count.
		int32 i = 0; // Detail counter.
		int32 SubDetailIndex; // Sub-detail index.
		do
		{
			SubDetailIndex = DetailsIndices[i++];
			const int32 SubCount = Details[SubDetailIndex].IterableNum();
			check(SubCount >= 1);

			SubIndex = (ComboIndex / AccumCount) % SubCount;
			AccumCount *= SubCount;
		} while (SubDetailIndex != DetailIndex);

		RetDetail = Details[DetailIndex][SubIndex].Get();
	}

	if (AvoidConditionFormat(Paradigm, RetDetail == nullptr, TEXT("The detail must be present (fetched).")))
	{
		return MakeOutcome<Paradigm, UDetail*>(EApparatusStatus::SanityCheckFailed, nullptr);
	}

	return RetDetail;
} //-DetailAt

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm, UDetail*>
FBeltSlot::GetDetailHinted(const TArray<int32>&       DetailsIndices,
						   const int32                ComboIndex,
						   const TSubclassOf<UDetail> DetailClass,
						   const int32                DetailIndexHint) const
{
	check(Owner);
	check(ComboIndex > InvalidComboIndex);

	const FDetailmark& Detailmark = GetDetailmark();
	if (LIKELY((DetailIndexHint > InvalidDetailIndex) && (DetailIndexHint < Detailmark.DetailsNum())))
	{
		const TSubclassOf<UDetail> RealDetailClass = Detailmark[DetailIndexHint];
		if (LIKELY(RealDetailClass->IsChildOf(DetailClass)))
		{
			return DetailAtLine(DetailsIndices, ComboIndex, DetailIndexHint);
		}
	}

	// The detail class doesn't match.
	// Try to find the detail index within the active
	// detailmark...
	const int32 RealIndex = Detailmark.IndexOf(DetailClass);
	if (LIKELY(RealIndex != UBelt::InvalidDetailLineIndex)) 
	{
		return DetailAtLine(DetailsIndices, ComboIndex, RealIndex);
	}
	// Detail still was not found.
	// This may happen when the subjective is removed from the belt...
	if (LIKELY(Subjective))
	{
		return Subjective->GetDetail(DetailClass);
	}
	return nullptr;
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

#pragma endregion Belt Slot

#pragma region Subjective

FORCEINLINE struct FBeltSlot*
ISubjective::GetSlotPtr()
{
	if (UNLIKELY(!Belt))
	{
		return nullptr;
	}
	if (UNLIKELY(SlotIndex <= FBeltSlot::InvalidIndex))
	{
		return nullptr;
	}
	return &((*Belt)[SlotIndex]);
}

FORCEINLINE const struct FBeltSlot*
ISubjective::GetSlot() const
{
	if (UNLIKELY(!Belt))
	{
		return nullptr;
	}
	if (UNLIKELY(SlotIndex <= FBeltSlot::InvalidIndex))
	{
		return nullptr;
	}
	return &((*Belt)[SlotIndex]);
}

#pragma endregion Subjective

#pragma region Belt Iterator

#pragma endregion Belt Iterator

#if CPP && !defined(SKIP_MACHINE_H)
#include "Machine.h"
#endif
