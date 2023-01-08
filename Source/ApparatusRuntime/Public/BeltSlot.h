/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BeltSlot.h
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

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/WeakInterfacePtr.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define BELT_SLOT_H_SKIPPED_MACHINE_H
#endif

#include "ApparatusStatus.h"
#include "Fingerprint.h"
#include "Filter.h"
#include "Subjective.h"
#include "BeltSlotCache.h"

#ifdef BELT_SLOT_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif


// Forward declarations:
struct FDetailmark;
struct FSubjectHandle;
class UBelt;
class ISubjective;

/**
 * The belt slot, containing the details.
 * 
 * A faster cache is used for details.
 * 
 * @note The class should be considered an internal
 * functionality and not be used directly.
 */
struct APPARATUSRUNTIME_API FBeltSlot
{

  public:

	enum
	{
		/**
		 * Invalid belt slot index.
		 */
		InvalidIndex = ISubjective::InvalidSlotIndex,

		/**
		 * Invalid combination index.
		 */
		InvalidComboIndex = -1,

		/**
		 * Invalid detail index.
		 */
		InvalidDetailIndex = -1
	};

  private:

	friend class USubjectiveActorComponent;
	friend class UBelt;
	template < typename SubjectHandleT >
	friend struct TBeltIt;
	friend class TArray<FBeltSlot>;

	/**
	 * The subjective of the slot.
	 * 
	 * This is cleared automatically when the
	 * subjective is unregistered.
	 */
	ISubjective* Subjective = nullptr;

	/**
	 * A variadic list of details, stored in order
	 * by their respective types according to
	 * the belt's detailmark.
	 */
	mutable TArray<FBeltSlotCache> Details;

	/**
	 * The owning belt of the slot.
	 */
	UBelt* Owner = nullptr;

	/**
	 * The index of the slot within the belt.
	 */
	mutable int32 Index = InvalidIndex;

	/**
	 * Is the slot marked for removal and
	 * should be skipped during
	 * the iterating process?
	 */
	bool bStale = true;

	/**
	 * Calculate the total number of combinations possible
	 * within this slot according for the specified filter
	 * and details list.
	 * 
	 * @note Not all of those combos can be valid during the actual iteration,
	 * so they still need to be tested with FBeltSlot::IsComboValid().
	 * 
	 * @note Also matches the subjective's fingerprint against the filter.
	 * 
	 * @param InFilter The filter to calculate for.
	 * @param InDetailsIndices The indices of the details to consider.
	 * @return The number of iterable combinations within the slot.
	 */
	int32
	CalcIterableCombosCount(const FFilter&       InFilter,
							const TArray<int32>& InDetailsIndices) const;

	/**
	 * Lock the slot for an iteration with
	 * the specified details indices.
	 * 
	 * @note A full locked belt state is required
	 * for this method.
	 * 
	 * @return The number of iterations possible
	 * on this slot.
	 */
	int32
	BeginIteration(const FFilter&       InFilter,
				   const TArray<int32>& InDetailsIndices) const;

	/**
	 * Prepare the slot for an iteration locking it
	 * and fetching the combinations count for a
	 * specific 
	 * 
	 * @param InFilter The filter to prepare the slot for.
	 * @param InDetailsIndices The indices of the details to prepare
	 * for the iteration.
	 * @return The number of iterations
	 * possible on this slot.
	 */
	int32
	PrepareForIteration(const FFilter&       InFilter,
						const TArray<int32>& InDetailsIndices) const;

	/**
	 * Unlock the slot from iterating.
	 * 
	 * @note This method should be used only when the
	 * owning belt should no longer be locked as
	 * it unlocks all of the cache completely.
	 * 
	 * @note The full still locked belt state is required
	 * for this method.
	 * 
	 * @return The status of the operation.
	 */
	EApparatusStatus
	Unlock() const;

	/**
	 * The main slot constructor.
	 */
	FORCEINLINE FBeltSlot(UBelt* const InOwner,
						  const int32  InIndex)
	  : Owner(InOwner)
	  , Index(InIndex)
	{}

	/**
	 * Create a new belt slot with an owner.
	 */
	FBeltSlot(UBelt* InOwner);

	/**
	 * Set the subjective for the slot.
	 * 
	 * Will automatically reset the
	 * former subjective's slot state (if any).
	 */
	void
	SetSubjective(ISubjective* const InSubjective);

  public:

	/**
	 * The default invalid/uninitialized slot constructor.
	 */
	FORCEINLINE
	FBeltSlot() {}

	/**
	 * Check if the belt slot is currently locked.
	 * 
	 * The slot is locked if its belt is locked
	 * and its index is within the range of
	 * currently iterable slots.
	 */
	bool
	IsLocked() const;

	/**
	 * Get the owning belt of the slot.
	 */
	FORCEINLINE UBelt*
	GetOwner() const
	{
		return Owner;
	}

	/**
	 * Get the index of the slot in the belt.
	 *
	 * @return An index of the slot within its belt.
	 */
	FORCEINLINE int32
	GetIndex() const
	{
		return Index;
	}

	/**
	 * Check if the slot is marked to be removed.
	 * 
	 * Also returns @c true, if the subjective is disposed.
	 */
	FORCEINLINE bool
	IsStale() const
	{
		checkf(bStale || Subjective,
			   TEXT("Non-stale slots must always have a subjective set."));
		return bStale;
	}

	/**
	 * Check if a slot should be skipped during the iterating.
	 * 
	 * @note You should also check your calculated combinations
	 * count during the iterating process.
	 * 
	 * @param InFilter The filter to check compatibility with.
	 * @return The state of examination.
	 */
	bool
	IsSkipped(const FFilter& InFilter) const;

	/**
	 * Check if the slot is viable, i.e. meaningful
	 * for consideration.
	 */
	bool
	IsViable() const;

	/**
	 * Check if the slot is viable, i.e. meaningful
	 * for consideration within a certain filter.
	 */
	bool
	IsViable(const FFilter& InFilter) const;

	/**
	 * Set the slot as to be removed (or not).
	 *
	 * If the slot is locked at the moment, it is
	 * marked to be removed and is added to a list (for deferred removal) .
	 * Otherwise, it's instantly cleared.
	 */
	EApparatusStatus
	SetRemoved(const bool bInRemoved = true);

	/**
	 * Get the subject of the slot.
	 */
	FSubjectHandle
	GetSubject() const;

	/**
	 * Get the subjective of the slot.
	 */
	FORCEINLINE ISubjective*
	GetSubjective() const
	{
		check(Subjective || !HasAnyDetails());
		return Subjective;
	}

	/**
	 * Clear all of the cached details in the slot,
	 * while actually maintaining a link to subjective itself (if any).
	 * 
	 * Performs no memory deallocations.
	 */
	void
	ResetDetails() const;

	/**
	 * Deinitialize the belt slot.
	 */
	~FBeltSlot();

	/**
	 * Check if a detail exists at the specified index.
	 * 
	 * @param DetailIndex The index of the detail to check for availability.
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	IsDetailAvailableAtLine(const int32 DetailIndex)
	{
		if (UNLIKELY(DetailIndex < 0))
			return false;
		if (UNLIKELY(DetailIndex >= Details.Num()))
			return false;
		return Details[DetailIndex].HasAny();
	}

	/**
	 * Check if a combo is valid, i.e. has all
	 * the necessary details.
	 * 
	 * @param DetailsIndices The indices of the needed details to check for.
	 * @param ComboIndex The index of the combination to examine.
	 * @return The state of examination.
	 * @return false if this combo should be skipped.
	 */
	bool
	IsComboValid(const TArray<int32>& DetailsIndices,
				 const int32          ComboIndex) const;

	/**
	 * Get the detail at a specified index
	 * according to the combination supplied.
	 * 
	 * @param DetailsIndices The indices of the details to consider.
	 * @param ComboIndex The index of the combination to examine.
	 * @param DetailLineIndex The index of the detail to get.
	 * Must also exist within the details indices array.
	 * @return The detail at specified offset within the indices.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, UDetail*>
	DetailAtLine(const TArray<int32>& DetailsIndices,
				 const int32          ComboIndex,
				 const int32          DetailLineIndex) const;

	/**
	 * Get the detail at a specified index hinted
	 * according to the combination supplied.
	 * 
	 * @param DetailsIndices The indices of the details to consider.
	 * @param ComboIndex The index of the combination.
	 * @param DetailClass The class of the detail to get.
	 * @param DetailIndexHint The hinting index of the detail to get.
	 * Relative to the belt's detailmark.
	 * Should also exist within the details indices array.
	 * May be a FBeltSlot::InvalidDetailIndex to suppress the hinting.
	 * @return The detail of the specified type.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, UDetail*>
	GetDetailHinted(const TArray<int32>&       DetailsIndices,
					const int32                ComboIndex,
					const TSubclassOf<UDetail> DetailClass,
					const int32                DetailIndexHint) const;

	/**
	 * Get the detail at a specified index hinted
	 * according to the combination supplied.
	 * Statically typed version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The class of the detail to get.
	 * @param DetailsIndices The indices of the details to consider.
	 * @param ComboIndex The index of the combination to examine.
	 * @param DetailIndexHint The hinting index of the detail
	 * relative to the belt's detailmark.
	 * Should also exist within the details indices array.
	 * May be a FBeltSlot::InvalidDetailIndex to suppress the hinting.
	 * @return The detail of the specified type.
	 */
	template < EParadigm Paradigm, typename D >
	FORCEINLINE TOutcome<Paradigm, D*>
	GetDetailHinted(const TArray<int32>& DetailsIndices,
					const int32          ComboIndex,
					const int32          DetailIndexHint) const
	{
		return OutcomeStaticCast<D*>(
					GetDetailHinted<Paradigm>(DetailsIndices, ComboIndex,
											  D::StaticClass(), DetailIndexHint));
	}

	/**
	 * Get the detail at a specified index hinted
	 * according to the combination supplied.
	 * Statically typed default paradigm version.
	 * 
	 * @tparam D The class of the detail to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailsIndices The indices of the details to consider.
	 * @param ComboIndex The index of the combination to examine.
	 * @param DetailIndexHint The hinting index of the detail
	 * relative to the belt's detailmark.
	 * Should also exist within the details indices array.
	 * May be a FBeltSlot::InvalidDetailIndex to suppress the hinting.
	 * @return The detail of the specified type.
	 */
	template < typename D, EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE auto
	GetDetailHinted(const TArray<int32>& DetailsIndices,
					const int32          ComboIndex,
					const int32          DetailIndexHint) const
	{
		return GetDetailHinted<Paradigm, D>(DetailsIndices, ComboIndex, DetailIndexHint);
	}

	/**
	 * Get the detail of a specific class
	 * according to the combination supplied.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE TOutcome<Paradigm, UDetail*>
	GetDetail(const TArray<int32>&       DetailsIndices,
			  const int32                ComboIndex,
			  const TSubclassOf<UDetail> DetailClass) const;

	/**
	 * Get the detail of a specific class
	 * according to the combination supplied.
	 * Statically typed version.
	 */
	template < EParadigm Paradigm, class D >
	FORCEINLINE TOutcome<Paradigm, D*>
	GetDetail(const TArray<int32>& DetailsIndices,
			  const int32          ComboIndex) const
	{
		return OutcomeStaticCast<D*>(GetDetail<Paradigm>(DetailsIndices, ComboIndex, D::StaticClass()));
	}

	/**
	 * Get the detail of a specific class
	 * according to the combination supplied.
	 * Statically typed version.
	 */
	template < class D,  EParadigm Paradigm =  EParadigm::DefaultInternal >
	FORCEINLINE auto
	GetDetail(const TArray<int32>& DetailsIndices,
			  const int32          ComboIndex) const
	{
		return GetDetail<Paradigm, D>(DetailsIndices, ComboIndex);
	}

	/**
	 * Fetch the details from the current subjective.
	 * 
	 * This never expands the belt itself, but
	 * can expand the slot according to the belt's
	 * detailmark.
	 */
	bool FetchDetails();

	/**
	 * Expand the slot according to a belt's
	 * detailmark.
	 */
	void Expand();

	/**
	 * The active detailmark of the slot,
	 * 
	 * It is the same as its owning belt's unless
	 * the belt is locked in which case it's of
	 * the locking filter.
	 */
	const FDetailmark&
	GetDetailmark() const;

	/**
	 * Is this slot actually valid?
	 */
	FORCEINLINE operator bool() const
	{ 
		return IsViable();
	}

	/**
	 * Compare two belt slots for equality.
	 */
	FORCEINLINE bool
	operator==(const FBeltSlot& Other) const
	{
		return (std::addressof(Other) == this) ||
			   (Owner == Other.Owner && Index == Other.Index);
	}

	/**
	 * Compare two belt slots for inequality.
	 */
	FORCEINLINE bool
	operator!=(const FBeltSlot& Other) const
	{
		return (std::addressof(Other) != this) &&
			   (Owner != Other.Owner || Index != Other.Index);
	}

	/**
	 * Set the slot as another one from the same belt.
	 * 
	 * @param InSlot The slot to set as.
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	Set(const FBeltSlot& InSlot)
	{
		if (std::addressof(InSlot) == this)
		{
			return EApparatusStatus::Noop;
		}

		checkf(Owner == InSlot.Owner,
			TEXT("Copying slots from different belts is not supported."));
		checkf(bStale == InSlot.bStale,
			TEXT("Copying slots with different stale states is not supported."));
		checkf(Details.Num() == InSlot.Details.Num(),
			TEXT("Copying slots of different details counts is not supported."));
		check(Index > InvalidIndex);
		check(InSlot.Index > InvalidIndex);
		checkf(Index != InSlot.Index, TEXT("Slot indices must be different."));

		for (int32 i = 0; i < Details.Num(); i++)
		{
			Details[i] = InSlot.Details[i];
		}

		Subjective = InSlot.Subjective;

		return EApparatusStatus::Success;
	}

	/**
	 * Set the slot as another one from the same belt.
	 * 
	 * @param InSlot The slot to set as.
	 */
	FORCEINLINE FBeltSlot&
	operator=(const FBeltSlot& InSlot)
	{
		verify(OK(Set(InSlot)));
		return *this;
	}

	/**
	 * Check if there are any details available
	 * within the caches.
	 */
	FORCEINLINE bool
	HasAnyDetails() const
	{
		for (int32 i = 0; i < Details.Num(); ++i)
		{
			if (LIKELY(Details[i].HasAny()))
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * Check if the slot has any
	 * locked details within itself.
	 */
	FORCEINLINE bool
	HasLockedDetails() const
	{
		for (int32 i = 0; i < Details.Num(); ++i)
		{
			if (LIKELY(Details[i].IsLocked()))
			{
				return true;
			}
		}
		return false;
	}

}; //-struct FBeltSlot

#pragma region Belt Slot Inlines

FORCEINLINE FBeltSlot::~FBeltSlot()
{
	checkf(!HasLockedDetails(),
		   TEXT("The #%d slot must be unlocked before destruction."),
		   Index);
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm, UDetail*>
FBeltSlot::GetDetail(const TArray<int32>&       DetailsIndices,
					 const int32                ComboIndex,
					 const TSubclassOf<UDetail> DetailClass) const
{
	check(ComboIndex >= 0);
	check(DetailClass);
	const auto& Detailmark = GetDetailmark();
	const auto DetailClassIndex = Detailmark.IndexOf(DetailClass);
	if (AvoidConditionFormat(Paradigm, DetailClassIndex == InvalidDetailIndex,
							 TEXT("An invalid index for a '%s' detail class to get: %d"),
							 *DetailClass->GetName(), (int)DetailClassIndex))
	{
		return MakeOutcome<Paradigm, UDetail*>(EApparatusStatus::Missing, nullptr);
	}
	return DetailAtLine(DetailsIndices, ComboIndex, DetailClassIndex);
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

#pragma endregion Belt Slot Inlines
