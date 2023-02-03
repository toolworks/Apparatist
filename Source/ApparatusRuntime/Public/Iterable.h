/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Iterable.h
 * Created: 2021-06-11 23:06:18
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

#include <atomic>

#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#include "UObject/Interface.h"
#include "Containers/Queue.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define ITERABLE_H_SKIPPED_MACHINE_H
#endif

#include "Filter.h"

#ifdef ITERABLE_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "Iterable.generated.h"


// Forward declarations:
class UMachine;
class AMechanism;

/**
 * @internal Exists only for reflection. Do not use.
 */
UINTERFACE(MinimalAPI, BlueprintType)
class UIterable : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for all sorts of sequences.
 */
class APPARATUSRUNTIME_API IIterable
{
	GENERATED_BODY()

  public:

	enum
	{
		/**
		 * Invalid iterable slot index.
		 */
		InvalidSlotIndex = -1,
	};

  private:

	/**
	 * The type of the locks counter.
	 */
	using LocksCountValueType = int8;

	/**
	 * The current number of locks held on the chunk.
	 *
	 * If it's positive, the iterable is normal-locked.
	 * If it's negative, it's solid-locked.
	 * If it's 0, the iterable is not locked at all.
	 * 
	 * When you iterate on some chunk, it always has to be locked.
	 * This is defined as an atomic field, cause it can be changed
	 * from several iterators in parallel threads.
	 */
	mutable std::atomic<LocksCountValueType> LocksCount{0};

	/**
	 * The mutex is for locking/unlocking of the iterable.
	 */
	mutable FCriticalSection LockageMutex;

	/**
	 * The indices of the slots queued
	 * to be removed once the iterable becomes unlocked.
	 *
	 * The deferred-removed are not within
	 * this queue at all.
	 */
	mutable TQueue<int32, EQueueMode::Mpsc> Removed;

  protected:

	/**
	 * The logical (un-buffered) number of slots in the belt
	 * that are actually candidates for being iterated
	 * once the iterable is locked.
	 */
	int32 Count = 0;

	/**
	 * A number of currently iterable slots,
	 * excluding those, that were added
	 * before the unlocking.
	 * 
	 * This value is available and is valid only
	 * during the locked iterating. Otherwise, it should be -1.
	 */
	mutable std::atomic<int32> IterableCount{-1};

	/**
	 * Enqueue a slot for removal.
	 *
	 * @param SlotIndex The index of the slot
	 * to queue for the removal.
	 */
	FORCEINLINE void
	EnqueueForRemoval(const int32 SlotIndex) const
	{
		Removed.Enqueue(SlotIndex);
	}

	/**
	 * Dequeue a slot from being removed.
	 * Reference version.
	 *
	 * @param OutSlotIndex The index of the slot
	 * to queue for the removal.
	 * @return Was the dequeueing operation
	 * successful or the queue is empty.
	 */
	FORCEINLINE bool
	DequeueFromRemoved(int32& OutSlotIndex) const
	{
		return Removed.Dequeue(OutSlotIndex);
	}

	/**
	 * Dequeue a slot from being removed.
	 */
	FORCEINLINE int32
	DequeueFromRemoved() const
	{
		int32 SlotIndex;
		if (DequeueFromRemoved(SlotIndex))
		{
			return SlotIndex;
		}
		return InvalidSlotIndex;
	}

#pragma region Concurrency


#pragma endregion Concurrency

  public:

	/**
	 * Get the owning mechanism of the iterable.
	 */
	AMechanism*
	GetOwner() const;

	/**
	 * Check if the sequence is currently locked.
	 * 
	 * The sequence is locked if its locks count
	 * is non-zero.
	 *
	 * @return The current locking state.
	 */
	FORCEINLINE bool
	IsLocked() const
	{
		return LocksCount.load(std::memory_order_relaxed) != 0;
	}

	/**
	 * Check if the iterable is currently
	 * locked in a liquid manner.
	 * 
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	IsLiquidLocked() const
	{
		return LocksCount.load(std::memory_order_relaxed) > 0;
	}

	/**
	 * Check if the iterable is currently
	 * locked in a solid manner.
	 * 
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	IsSolidLocked() const
	{
		return LocksCount.load(std::memory_order_relaxed) < 0;
	}

	/**
	 * Check if the iterable is currently solid.
	 *
	 * Being solid means being protected from
	 * any structural changes.
	 * 
	 * The iterable is solid if it's residing
	 * within a currently solid mechanism.
	 * 
	 * @return The state of examination.
	 */
	bool
	IsSolid() const;

	/**
	 * Check if there are any slots marked for removal
	 * within the iterable.
	 * 
	 * The actual removal is delayed until the iterable
	 * becomes fully unlocked (stops being iterated).
	 */
	FORCEINLINE bool
	HasQueuedForRemoval() const
	{
		return !Removed.IsEmpty();
	}

	/**
	 * Get the total number of slots currently in the iterable.
	 *
	 * @note Returns the active iterable slots number
	 * when the iterable is locked, which also includes 
	 * the stale (to be skipped) slots.
	 * 
	 * @return The current number of slots.
	 */
	FORCEINLINE int32
	Num() const
	{
		if (IsLocked())
		{
			return IterableNum();
		}
		return Count;
	}

	/**
	 * Get the current number of elements
	 * valid for iterating.
	 * 
	 * @note This number includes the stale (to be skipped)
	 * slots also.
	 */
	FORCEINLINE int32
	IterableNum() const
	{
		return IterableCount;
	}

	/**
	 * Check whether the sequence matches a filter.
	 * 
	 * @param InFilter A filter to match.
	 * @return The state of matching.
	 */
	virtual FORCEINLINE bool
	Matches(const FFilter& InFilter) const
	{
		unimplemented();
		return false;
	}

#pragma region Locking & Unlocking
	/// @name Locking & Unlocking
	/// @{

	/**
	 * Lock the iterable, preparing it for the iterating process.
	 * 
	 * @tparam bInSolid Should the iterable be locked in a solid manner.
	 * @return The status of the operation.
	 */
	template < bool bInSolid >
	EApparatusStatus
	Lock() const;

	/**
	 * Lock the iterable in a liquid manner.
	 */
	FORCEINLINE EApparatusStatus
	LockLiquid() const
	{
		return Lock</*bInSolid=*/false>();
	}

	/**
	 * Lock the iterable in a solid manner.
	 */
	FORCEINLINE EApparatusStatus
	LockSolid() const
	{
		return Lock</*bInSolid=*/true>();
	}

	/**
	 * Unlock the iterable, applying the pending changes.
	 * 
	 * @tparam bInSolid Should the iterable be unlocked from a solid state.
	 * @return The status of the operation.
	 */
	template < bool bInSolid >
	EApparatusStatus
	Unlock() const;

	/// @}
#pragma endregion Locking & Unlocking

  protected:

	template < typename SubjectHandleT >
	friend struct TChunkIt;

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	/**
	 * Unlock the iterable, applying the pending changes.
	 * Must be implemented in the descendants.
	 * 
	 * The procedure is guaranteed to be executed under
	 * a locked critical section for this chunk.
	 * 
	 * @param bWasSolid Should the iterable be unlocked from a solid state.
	 * @return The status of the operation.
	 */
	virtual EApparatusStatus
	DoUnlock(const bool bWasSolid) const
	{
		unimplemented();
		return EApparatusStatus::NoImplementation;
	}

}; //-class IIterable
