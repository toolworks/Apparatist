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

#include "More/Containers/Array.h"

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

	/**
	 * The type of the detail line identifier.
	 */
	using DetailLineIndexType = FBeltSlot::DetailLineIndexType;

	/**
	 * Invalid slot index.
	 */
	static constexpr auto InvalidSlotIndex = FBeltSlot::InvalidIndex;

	/**
	 * Invalid detail line index.
	 */
	static constexpr auto InvalidDetailLineIndex = FBeltSlot::InvalidDetailLineIndex;

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
	 * The current detailmark of the belt.
	 * 
	 * @note Belts are sparse and may still miss some of the detail
	 * places during the iterating process.
	 * 
	 * This detailmark is expanded with the new details when needed.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Belt,
			  Meta = (AllowPrivateAccess = "true"))
	FDetailmark Detailmark;

	/**
	 * The child lines cache entry type.
	 */
	using ChildLinesCacheEntryType = TArray<int32, TInlineAllocator<4>>;

	/**
	 * A static empty entry to return.
	 */
	static ChildLinesCacheEntryType EmptyChildLinesEntry;

	/**
	 * The child lines caching dictionary type.
	 */
	using ChildLinesCacheType = TMap<TSubclassOf<UDetail>, ChildLinesCacheEntryType,
									 TSetAllocator<typename FDefaultSetAllocator::SparseArrayAllocator, TInlineAllocator<8>>>;

	/**
	 * The cached mappings to detail line child classes.
	 */
	mutable ChildLinesCacheType ChildLinesCache;

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
	 * Respects the inheritance model.
	 * 
	 * @param DetailClass The detail class to query for.
	 * @return The index of the detail line.
	 */
	FORCEINLINE DetailLineIndexType
	DetailLineIndexOf(const TSubclassOf<UDetail> DetailClass) const
	{
		const auto& LinesCache = DetailLinesIndicesOf(DetailClass);
		if (UNLIKELY(LinesCache.Num() == 0))
		{
			return InvalidDetailLineIndex;
		}
		return LinesCache[0];
	}

	/**
	 * Get the line index for a detail class.
	 * 
	 * Respects the inheritance model.
	 * 
	 * @note The method actually supports non-detail classes,
	 * i.e. @c INDEX_NONE will be safely returned in that case.
	 * 
	 * @tparam D The detail class to query for.
	 * @return The index of the detail line.
	 */
	template < class D,
			   TDetailClassSecurity<D> = true >
	FORCEINLINE auto
	DetailLineIndexOf() const
	{
		return DetailLineIndexOf(D::StaticClass());
	}

#ifndef DOXYGEN_ONLY

	/**
	 * Get the line index for a detail class.
	 * 
	 * Respects the inheritance model.
	 * 
	 * @note The method actually supports non-detail classes,
	 * i.e. @c InvalidDetailLineIndex will be safely returned in that case.
	 * 
	 * @tparam D The detail class to query for.
	 * @return The index of the detail line.
	 */
	template < class D,
			   TNonDetailClassSecurity<D> = true >
	FORCEINLINE constexpr DetailLineIndexType
	DetailLineIndexOf() const
	{
		return InvalidDetailLineIndex;
	}

#endif // DOXYGEN_ONLY

	/**
	 * Get line indices of the details equal or derived from a detail class.
	 * 
	 * Respects the inheritance model.
	 * 
	 * @param DetailClass The class of the detail to get by.
	 * @return The detail lines of the specified class.
	 */
	FORCEINLINE const ChildLinesCacheEntryType& 
	DetailLinesIndicesOf(TSubclassOf<UDetail> DetailClass) const
	{
		const auto LinesPtr = ChildLinesCache.Find(DetailClass);
		if (UNLIKELY(LinesPtr == nullptr))
		{
			return EmptyChildLinesEntry;
		}
		return *LinesPtr;
	}

	/**
	 * Get line indices of the details equal or derived from a detail class.
	 * 
	 * Respects the inheritance model.
	 * 
	 * Safely returns an empty array for non-detail classes.
	 * 
	 * @tparam D The class of the detail to get by.
	 * @return The detail lines of the specified class.
	 */
	template < typename D,
			   TDetailClassSecurity<D> = true >
	FORCEINLINE const ChildLinesCacheEntryType& 
	DetailLinesIndicesOf() const
	{
		return DetailLinesIndicesOf(D::StaticClass());
	}

#ifndef DOXYGEN_ONLY

	/**
	 * Get line indices of the details equal or derived from a detail class.
	 * 
	 * Respects the inheritance model.
	 * 
	 * Safely returns an empty array for non-detail classes.
	 * 
	 * @tparam D The class of the detail to get by.
	 * @return The detail lines of the specified class.
	 */
	template < typename D,
			   TNonDetailClassSecurity<D> = true >
	FORCEINLINE const ChildLinesCacheEntryType& 
	DetailLinesIndicesOf() const
	{
		return EmptyChildLinesEntry;
	}

#endif // DOXYGEN_ONLY

	/**
	 * @brief Collect mainline indices for a list of details.
	 * 
	 * This respects the inheritance model.
	 * 
	 * @tparam DetailsAllocatorT The allocator of the details array.
	 * @tparam IndicesAllocatorT The allocator of the indices array.
	 * @param[in] InDetails The details to find mainline indices for.
	 * @param[out] OutIndices The result receiver.
	 */
	template < typename DetailsAllocatorT = FDefaultAllocator,
			   typename IndicesAllocatorT = FDefaultAllocator >
	FORCEINLINE void
	CollectMainlineIndices(const TArray<TSubclassOf<UDetail>, DetailsAllocatorT>& InDetails,
						   TArray<DetailLineIndexType, IndicesAllocatorT>&        OutIndices)
	{
		for (int32 i = 0; i < InDetails.Num(); ++i)
		{
			for (int32 j = 0; j < Detailmark.DetailsNum(); ++j)
			{
				if (Detailmark[j]->IsChildOf(InDetails[i]))
				{
					OutIndices.AddUnique(j);
				}
			}
		}
	}

	/**
	 * @brief Get mainline indices for a list of details.
	 * 
	 * This respects the inheritance model.
	 * 
	 * @tparam DetailsAllocatorT The allocator of the details array.
	 * @tparam IndicesAllocatorT The allocator of the indices array.
	 * @param[in] InDetails The details to find mainline indices for.
	 * @param[out] OutIndices The result receiver.
	 */
	template < typename DetailsAllocatorT = FDefaultAllocator,
			   typename IndicesAllocatorT = FDefaultAllocator >
	FORCEINLINE void
	GetMainlineIndices(const TArray<TSubclassOf<UDetail>, DetailsAllocatorT>& InDetails,
					   TArray<DetailLineIndexType, IndicesAllocatorT>&        OutIndices)
	{
		OutIndices.Reset();
		CollectMainlineIndices(InDetails, OutIndices);
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
