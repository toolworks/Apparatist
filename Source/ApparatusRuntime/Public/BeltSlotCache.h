/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BeltSlotCache.h
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

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define BELT_SLOT_CACHE_H_SKIPPED_MACHINE_H
#endif

#include "Detail.h"

#ifdef BELT_SLOT_CACHE_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif


// Forward declarations:
struct FBeltSlot;

/**
 * A detail caching for subjects used in the belts' slots.
 */
struct APPARATUSRUNTIME_API FBeltSlotCache
{

  private:

	friend class UBelt;
	template < typename SubjectHandleT >
	friend struct TBeltIt;
	friend struct FBeltSlot;
	friend class UObject;
	friend class UClass;
	friend class TArray<FBeltSlotCache>;

	/**
	 * The owner of this belt cache.
	 */
	FBeltSlot* Owner = nullptr;

	/**
	 * The fetched/cached details of the slot type
	 * stored as weak references.
	 */
	TArray<UDetail*> Details;

	/**
	 * The current number of iterable details.
	 * 
	 * This is also a marker of the slot cache being
	 * locked. If this count is >= 0 the cache is locked.
	 */
	int32 IterableCount = -1;

	/**
	 * Check if the cache is currently locked.
	 * 
	 * A locked cache fixates the iterable count
	 * number and doesn't remove the elements
	 * from the internal array.
	 */
	FORCEINLINE bool
	IsLocked() const
	{
		check(Owner != nullptr);
		return IterableCount >= 0;
	}

	/**
	 * Lock the slot cache.
	 */
	FORCEINLINE auto
	Lock()
	{
		if (UNLIKELY(IsLocked())) return IterableCount;
		return IterableCount = Details.Num();
	}

	/**
	 * Unlock the slot cache.
	 */
	inline void
	Unlock()
	{
		if (UNLIKELY(!IsLocked())) return;

		IterableCount = -1;

		// Clean-up the leftover details now...
		for (int32 i = 0; i < Details.Num(); ++i)
		{
			check(Details[i] != nullptr);
			if (!Details[i]->IsEnabled())
			{
				Details.RemoveAtSwap(i--, 1, /*bAllowShrinking=*/false);
			}
		}
	}

	/**
	 * Create a new belt slot cache instance.
	 */
	FORCEINLINE
	FBeltSlotCache(FBeltSlot* const InOwner,
				   const int32      InCapacity = 1)
	  : Owner(InOwner)
	{
		check(InOwner);
		Details.Reserve(InCapacity);
	}

	/**
	 * Clear the cache.
	 * 
	 * @note The function performs no deallocations.
	 */
	FORCEINLINE void
	Reset()
	{
		if (IsLocked())
		{
			// Preserve the array when locked,
			// just mark the elements as invalid...
			for (int32 i = 0; i < Details.Num(); ++i)
			{
				Details[i] = nullptr;
			}
		}
		else
		{
			Details.Reset();
		}
	}

  public:

	FORCEINLINE FBeltSlotCache(const FBeltSlotCache& InCache)
	  : Owner(InCache.Owner)
	  , Details(InCache.Details)
	  , IterableCount(InCache.IterableCount)
	{}

	FORCEINLINE FBeltSlotCache(FBeltSlotCache&& InCache)
	  : Owner(InCache.Owner)
	  , Details(MoveTemp(InCache.Details))
	  , IterableCount(InCache.IterableCount)
	{
		InCache.Owner         = nullptr;
		InCache.IterableCount = -1;
	}

	/**
	 * Construct an invalid/uninitialized cache.
	 */
	FORCEINLINE
	FBeltSlotCache()
	{}

	/**
	 * Get the owner of the cache.
	 */
	FORCEINLINE FBeltSlot*
	GetOwner() const
	{
		return Owner;
	}

	/**
	 * Get the class of details stored within the cache.
	 */
	FORCEINLINE TSubclassOf<UDetail>
	GetDetailClass() const
	{
		for (int32 i = 0; i < Details.Num(); ++i)
		{
			if (Details[i] != nullptr)
			{
				return Details[i]->GetClass();
			}
		}
		return nullptr;
	}

	/**
	 * Get the list of all fetched details.
	 */
	FORCEINLINE const TArray<class UDetail*>&
	GetDetails() const
	{
		return Details;
	}

	/**
	 * Get the current number of iterable details.
	 */
	FORCEINLINE int32
	IterableNum() const
	{
		if (IsLocked())
		{
			return IterableCount;
		}
		return Details.Num();
	}

	/**
	 * Check if there are any viable detail(s) within the cache.
	 * 
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	HasAny() const
	{
		const auto Count = IterableNum();
		for (int32 i = 0; i < Count; ++i)
		{
			const auto Detail = Details[i];
			check(Detail != nullptr);
			if (LIKELY(Detail->IsEnabled()))
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * Check if the cache has a valid subclass.
	 * 
	 * @param BaseDetailClass The base class to find a child of.
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	HasChildOf(TSubclassOf<UDetail> BaseDetailClass) const
	{
		const auto Count = IterableNum();
		for (int32 i = 0; i < Count; ++i)
		{
			const auto Detail = Details[i];
			check(Detail != nullptr);
			if (LIKELY(Detail->IsEnabled() && Detail->IsA(BaseDetailClass)))
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * Check if there are any viable detail(s) within the cache.
	 */
	FORCEINLINE operator bool() const
	{
		return HasAny();
	}

	/**
	 * Get a detail at specified index.
	 * 
	 * @param Index The index of the detail to get.
	 */
	FORCEINLINE class UDetail*
	operator[] (const int32 Index) const
	{
		return Details[Index];
	}

	/**
	 * Fetch the detail instances from the subject.
	 */
	void
	Fetch(const TSubclassOf<UDetail> DetailClass,
		  const ISubjective* const   Subjective);

	/**
	 * Fetch the detail instances from the user-provided list of details.
	 */
	void
	Fetch(const TSubclassOf<UDetail> DetailClass,
		  const TArray<UDetail*>&    InDetails)
	{
		check(DetailClass != nullptr);

		if (IsLocked())
		{
			for (auto InDetail : InDetails)
			{
				if (UNLIKELY(InDetail == nullptr)) continue;
				if (!InDetail->IsEnabled()) continue;
				if (!InDetail->IsA(DetailClass)) continue;
				// As we don't reset the array we have to check
				// for duplicates here:
				Details.AddUnique(InDetail);
			}
		}
		else
		{
			// We can reset only if unlocked:
			Details.Reset();
			for (auto InDetail : InDetails)
			{
				if (UNLIKELY(InDetail == nullptr)) continue;
				if (!InDetail->IsEnabled()) continue;
				if (!InDetail->IsA(DetailClass)) continue;
				check(Details.Find(InDetail) == INDEX_NONE);
				Details.Add(InDetail);
			}
		}
	}

	/**
	 * Copy the cache from another cache instance.
	 */
	FBeltSlotCache&
	operator=(const FBeltSlotCache& Cache);

	/**
	 * Append the cache from another cache instance.
	 */
	FBeltSlotCache&
	operator+=(const FBeltSlotCache& Cache);

}; //- class FBeltSlotCache
