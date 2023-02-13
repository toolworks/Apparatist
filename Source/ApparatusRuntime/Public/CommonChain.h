/*
 * ░▒▓ APPARATUS ▓▒░
 *
 * File: CommonChain.h
 * Created: 2022-07-01 13:51:45
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
#include "HAL/CriticalSection.h"
#include "HAL/Runnable.h"
#include "HAL/Event.h"
#include "Misc/ScopeLock.h"
#include "UObject/Object.h"
#include "UObject/WeakInterfacePtr.h"
#include "Templates/SharedPointer.h"
#include "Templates/UniquePtr.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define COMMON_CHAIN_H_SKIPPED_MACHINE_H
#endif

#include "Chunk.h"
#include "Belt.h"

#ifdef COMMON_CHAIN_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif


// Forward declarations:
class UApparatusFunctionLibrary;
class AMechanism;
class UMachine;

/**
 * Common chain functionality.
 * 
 * Contains the basic functionality universal for
 * all of the chains.
 */
struct FCommonChain
{
  public:

	/**
	 * Invalid chain identifier.
	 */
	static constexpr int32 InvalidId = -1;

	/**
	 * First valid chain identifier.
	 */
	static constexpr int32 FirstId = 0;

	enum
	{
		/**
		 * Invalid slot index.
		 */
		InvalidSlotIndex = -1,

		/**
		 * Invalid cursor identifier.
		 */
		InvalidCursorId = -1,

		/**
		 * Default cursor identifier.
		 */
		DefaultCursorId = 0,

		/**
		 * Invalid segment index.
		 */
		InvalidSegmentIndex = -1
	};

  protected:

	/**
	 * The common functionality for a single iterable entry within the chain.
	 * 
	 * @tparam bSolid Is the segment a solid-locking one?
	 */
	template < bool bSolid >
	struct TSegment
	{
		/**
		 * Is the segment a solid-locking one.
		 */
		static constexpr bool IsSolid = bSolid;

		/**
		 * A chunk or a belt of this segment.
		 */
		TWeakInterfacePtr<IIterable> Iterable = nullptr;

		/**
		 * Was the iterable locked by this segment?
		 */
		mutable bool bLocked = false;

		/**
		 * Get the chunk of the segment (if any).
		 */
		FORCEINLINE UChunk*
		GetChunk() const
		{
			return Cast<UChunk>(Iterable.GetObject());
		}

		/**
		 * Get the belt of the segment (if any).
		 */
		FORCEINLINE UBelt*
		GetBelt() const
		{
			return Cast<UBelt>(Iterable.GetObject());
		}

		/**
		 * Get the sequence of the segment.
		 */
		FORCEINLINE TScriptInterface<IIterable>
		GetIterable() const
		{
			return Iterable.ToScriptInterface();
		}

		/**
		 * Lock the segment by locking its iterable.
		 * 
		 * @return The status of the operation.
		 */
		inline EApparatusStatus
		Lock() const
		{
			if (UNLIKELY(bLocked))
			{
				check(Iterable.IsValid());
				return EApparatusStatus::Noop;
			}
			if (LIKELY(Iterable.IsValid()))
			{
				const auto Status = Iterable->template Lock<IsSolid>();
				if (LIKELY(ensure(OK(Status))))
				{
					bLocked = true;
				}
				return Status;
			}
			Report<EApparatusStatus::InvalidState>(TEXT("The belt for the chain segment to lock is not set."));
			return EApparatusStatus::InvalidState;
		}

		/**
		 * Unlock the segment by unlocking its target iterable.
		 * 
		 * @return The status of the operation.
		 */
		inline EApparatusStatus
		Unlock() const
		{
			if (UNLIKELY(!bLocked))
			{
				return EApparatusStatus::Noop;
			}
			if (LIKELY(Iterable.IsValid()))
			{
				const auto Status = Iterable->Unlock<IsSolid>();
				if (!OK(Status))
				{
					return Status;
				}
			}
			bLocked = false; // Unlock, even if the iterable is not available.
			return EApparatusStatus::Success;
		}

		/**
		 * Check if the segment is currently locked.
		 */
		FORCEINLINE bool
		IsLocked() const
		{
			return bLocked;
		}

		/**
		 * The current number of iterable slots within the chain.
		 * 
		 * @note The flagmark matching is performed iterating-time,
		 * so its effect is not present within this count.
		 */
		FORCEINLINE int32
		IterableNum() const
		{
			if (!Iterable.IsValid()) return 0;
			return Iterable->IterableNum();
		}

		/**
		 * Move-assign a segment.
		 * 
		 * @param InSegment A segment to move. 
		 */
		FORCEINLINE TSegment&
		operator=(TSegment&& InSegment)
		{
			Unlock();
			Iterable = InSegment.Iterable;
			bLocked  = InSegment.bLocked;
			InSegment.Iterable = TWeakInterfacePtr<IIterable>();
			InSegment.bLocked  = false;
			return *this;
		}

		/**
		 * Copy-assign a segment.
		 */
		FORCEINLINE TSegment&
		operator=(const TSegment& InSegment)
		{
			if (std::addressof(InSegment) == this) return *this;
			Unlock();
			Iterable = InSegment.Iterable;
			if (InSegment.IsLocked())
			{
				Lock();
			}
			return *this;
		}

#pragma region Segment Initialization

		/**
		 * Default constructor.
		 */
		FORCEINLINE
		TSegment()
		{};

		/**
		 * Construct a new chain segment for a sequence.
		 */
		FORCEINLINE
		TSegment(UObject* const InSequence)
		  : Iterable(InSequence)
		{};

		/**
		 * Move-construct a new instance of segment.
		 * 
		 * @param InSegment A segment to move.
		 */
		FORCEINLINE
		TSegment(TSegment&& InSegment)
		  : Iterable(InSegment.Iterable)
		  , bLocked(InSegment.bLocked)
		{
			InSegment.bLocked  = false;
			InSegment.Iterable = TWeakInterfacePtr<IIterable>(nullptr);
		}

		/**
		 * Copy-construct a new chain segment.
		 * 
		 * @param InSegment A segment to copy.
		 */
		FORCEINLINE
		TSegment(const TSegment& InSegment)
		 : Iterable(InSegment.Iterable)
		{	
			if (InSegment.IsLocked() && Iterable.IsValid())
			{
				Lock();
			}
		}
		
		/**
		 * Destroy the segment.
		 */
		FORCEINLINE
		~TSegment()
		{
			Unlock();
		}

#pragma endregion Segment Initialization

	}; //-struct TSegment

	friend class UApparatusFunctionLibrary;
	friend class AMechanism;
	friend class UMachine;

	template < typename... Ps >
	friend struct TCursor;

	template < typename MechanicT, typename... Ps >
	friend struct TRunnableMechanic;

	/**
	 * The owner of the chain.
	 * 
	 * Must be set during the initialization.
	 */
	AMechanism* Owner = nullptr;

	/**
	 * A unique identifier of the chain.
	 * 
	 * Should be valid only for mechanism-embedded
	 * chains.
	 */
	int32 Id = InvalidId;

	/**
	 * The type of the number of chain users value.
	 */
	using UsersCountValueType = uint8;

	/**
	 * The number of users currently needing the chain.
	 * 
	 * If it becomes zero the chain is disposed.
	 */
	mutable std::atomic<UsersCountValueType> UsersCount{0};

	/**
	 * Was this chain disposed and ready to be reused?
	 */
	std::atomic<bool> bDisposed{true};

	/**
	 * Is the chain currently being disposed?
	 * 
	 * The flag is to prevent unnecessary recursion
	 * of the disposal procedure.
	 */
	std::atomic<bool> bDisposing{false};

	/**
	 * Was this chain stopped from further iterating?
	 */
	mutable std::atomic<bool> bStopIterating{false};

	/**
	 * A filter under which the iterables are locked
	 * within the chain.
	 */
	FFilter Filter;

	/**
	 * Should delete itself on disposal.
	 */
	bool bSelfDestroyOnDisposal = false;

	/**
	 * Are we currently in a destructor?
	 */
	bool bInDestructor = false;

	/**
	 * Check if a chain can be reused as a new chain.
	 * 
	 * This mainly means that the embedded chain is disposed.
	 * 
	 * @return The result of examination.
	 */
	FORCEINLINE bool
	CanBeReused() const
	{
		check(Id != InvalidId);
		return bDisposed;
	}

  public:

	/**
	 * Common chain cursor functionality.
	 * 
	 * All cursors inherit from this class.
	 */
	struct FCommonCursor
	{
	  protected:

		/**
		 * The owner of the cursor.
		 * 
		 * The chain being iterated.
		 * If it's a non-null, the chain is being retained.
		 */
		const FCommonChain* Owner = nullptr;

		/**
		 * The chain slot index offset to begin with.
		 */
		int32 Offset = 0;

		/**
		 * The maximum number of slots to iterate by.
		 * 
		 * If the slot index is equal to or greater than this,
		 * the iterating should stop.
		 */
		int32 Limit = TNumericLimits<int32>::Max();

		/**
		 * The current local slot index.
		 */
		int32 SlotIndex = InvalidSlotIndex;

		/**
		 * The index of the currently
		 * iterated chain segment.
		 */
		int32 SegmentIndex = InvalidSegmentIndex;

#pragma region Common Cursor Initialization

		/**
		 * Initialize a new cursor instance for a chain.
		 * 
		 * @param InOwner The owner of the cursor to iterate.
		 * @param InOffset The slot offset of the cursor to begin with.
		 * @param InLimit The maximum number of slots to iterate by.
		 */
		FORCEINLINE
		FCommonCursor(const FCommonChain* const InOwner,
					  const int32               InOffset = 0,
					  const int32               InLimit = TNumericLimits<int32>::Max())
		  : Owner(InOwner)
		  , Offset(InOffset)
		  , Limit(InLimit)
		{
			check(Owner != nullptr);
			check(Offset >= 0);
			check(Limit > 0);
			Owner->Retain();
		}

		/**
		 * Move-initialize a cursor.
		 */
		FORCEINLINE
		FCommonCursor(FCommonCursor&& InCursor)
		  : Owner(InCursor.Owner)
		  , Offset(InCursor.Offset)
		  , Limit(InCursor.Limit)
		  , SlotIndex(InCursor.SlotIndex)
		  , SegmentIndex(InCursor.SegmentIndex)
		{
			if (LIKELY(InCursor.Owner))
			{
				InCursor.Owner = nullptr;
			}
		}

		/**
		 * Copy-initialize a cursor.
		 * 
		 * @param InCursor The original cursor to copy.
		 */
		FORCEINLINE
		FCommonCursor(const FCommonCursor& InCursor)
		  : Owner(InCursor.Owner)
		  , Offset(InCursor.Offset)
		  , Limit(InCursor.Limit)
		  , SlotIndex(InCursor.SlotIndex)
		  , SegmentIndex(InCursor.SegmentIndex)
		{
			if (LIKELY(Owner))
			{
				Owner->Retain();
			}
		}

		/**
		 * Initialize a new invalid cursor.
		 */
		FORCEINLINE
		FCommonCursor()
		{}

		/**
		 * Finalize the cursor.
		 */
		virtual FORCEINLINE
		~FCommonCursor()
		{
			// This is done in the descendants,
			// but leave it hear for future-proofing and symmetry.
			// Resetting the owner must be at last,
			// cause of possible self-destruction due to
			// release.
			if (LIKELY(Owner))
			{
				// Using a temporary because of possible destruction
				// due to owner's release.
				const auto OwnerSave = Owner;
				Owner = nullptr;
				OwnerSave->Release(); // This may actually trigger a disposal of the chain.
			}
		}

#pragma endregion Common Cursor Initialization

	  public:

#pragma region Common Cursor Examination

		/**
		 * Get the owning chain of the cursor.
		 */
		FORCEINLINE const FCommonChain*
		GetOwner() const
		{
			return Owner;
		}

		/**
		 * Get the global (chain-relative) index of the
		 * current slot.
		 */
		FORCEINLINE int32
		GetChainSlotIndex() const
		{
			return Offset + SlotIndex;
		}

		/**
		 * Check if the iterating limit was reached.
		 */
		FORCEINLINE bool
		IsLimitReached() const
		{
			return SlotIndex >= Limit;
		}

		/**
		 * Check if the cursor should stop iterating.
		 */
		FORCEINLINE bool
		ShouldStop() const
		{
			check(Owner);
			return Owner->ShouldStopIterating() || IsLimitReached();
		}

#pragma endregion Common Cursor Examination

	}; //-struct FCommonCursor

	/**
	 * Get the owning mechanism of the chain.
	 *
	 * @return The mechanism of the chain.
	 */
	FORCEINLINE AMechanism*
	GetOwner() const
	{
		return Owner;
	}

	/**
	 * Get the current filter used to enchain
	 * the iterables.
	 */
	FORCEINLINE const FFilter&
	GetFilter() const
	{
		checkf(!bDisposed,
			   TEXT("Getting a filter of a disposed chain is not supported."))
		return Filter;
	}

	/**
	 * Check if the chain is disposed and
	 * is ready to be reused.
	 * 
	 * @return The status of examination.
	 */
	FORCEINLINE bool
	IsDisposed() const
	{
		return bDisposed.load(std::memory_order_relaxed) ||
			   bDisposing.load(std::memory_order_relaxed);
	}

	/**
	 * Get the current number of users of the chain.
	 * 
	 * @note Even an unused chain can be indisposed.
	 * This is by design and is needed to correctly
	 * initialize the chain.
	 * 
	 * The operation is thread-safe.
	 * 
	 * @return The current number of users of the chain.
	 */
	FORCEINLINE int32
	UsersNum() const
	{
		return UsersCount.load(std::memory_order_relaxed);
	}

	/**
	 * Add a user of the chain preventing it from being disposed.
	 */
	FORCEINLINE void
	Retain() const
	{
		switch (UsersCount.fetch_add(1, std::memory_order_relaxed))
		{
			case TNumericLimits<UsersCountValueType>::Max(): 
				checkf(false, TEXT("Chain users count overflow detected."))
				break;
		}
	}

	/**
	 * Remove a user of the chain.
	 * 
	 * This can possibly trigger the chain disposal.
	 */
	FORCEINLINE void
	Release() const
	{
		switch (UsersCount.fetch_sub(1, std::memory_order_acq_rel))
		{
			case 1:
				const_cast<FCommonChain*>(this)->Dispose();
				break;
			case 0:
				checkf(false, TEXT("Chain users count underflow detected."));
				break;
		}
	}

	/**
	 * Reset the chain.
	 * 
	 * All of the chain segments (iterables) get
	 * released as part of this operation.
	 * 
	 * The embedded cursors will get reset as well,
	 * but you have to release all of the manual
	 * users beforehand or otherwise the chain
	 * won't get disposed automatically.
	 * 
	 * @param bWaitForOperatings Should the operation wait
	 * till all the operatings are complete.
	 * @return The status of the operation.
	 */
	virtual FORCEINLINE EApparatusStatus
	Reset(bool bWaitForOperatings = true)
	{
		auto Status = Filter.Reset();

		if (bWaitForOperatings)
		{
			WaitForOperatingsCompletion();
		}

		checkf((UsersCount.load(std::memory_order_relaxed) == 0)
			&& (OperatingsCount.load(std::memory_order_relaxed) == 0),
			   TEXT("Resetting a currently used/operated chain is strictly prohibited. "
					"Is there a left-over external cursor, operating(s) or some manual retainment active?"));
		
		// We do not clear the disposal state here,
		// since it is done in a separate Dispose() method,
		// that would actually call this Reset method.
		// So, the reset won't actually trigger the disposal.
		bStopIterating = false;

		return EApparatusStatus::Success;
	}

	/**
	 * Check if the chain should stop iterating.
	 */
	FORCEINLINE bool
	ShouldStopIterating() const
	{
		return bDisposed.load(std::memory_order_relaxed)
		    || bStopIterating.load(std::memory_order_relaxed);
	}

	/**
	 * Stop the current iterating of the chain (if any).
	 */
	EApparatusStatus
	StopIterating() const
	{
		if (UNLIKELY(bDisposed.load(std::memory_order_relaxed)))
		{
			return EApparatusStatus::Noop;
		}
		bool bExpected = false;
		if (LIKELY(!bStopIterating.exchange(true)))
		{
			return EApparatusStatus::Success;
		}
		return EApparatusStatus::Noop;
	}

	/**
	 * Get the number of current operatings happening within the chain.
	 * 
	 * @note This value can actually misrepresent the actual
	 * number if queried after the WaitForOperatingsCompletion(),
	 * cause the newly queued operations may already be altering it.
	 * 
	 * Each mechanism also has its own local AMechanism::OperatingsNum().
	 * 
	 * @return The current number of operatings within the chain. 
	 */
	FORCEINLINE auto
	OperatingsNum() const
	{
		return OperatingsCount.load(std::memory_order_relaxed);
	}

	/**
	 * Wait for all the current operatings
	 * to be completed.
	 * 
	 * You can use this method after
	 * you queued up the asynchronous
	 * operatings in order to sync them all.
	 * 
	 * Each mechanism also has its own global AMechanism::WaitForOperatingsCompletion().
	 */
	FORCEINLINE void
	WaitForOperatingsCompletion() const
	{
		OperatingsCompleted->Wait();
	}

	/**
	 * Wait for all the current operatings
	 * to be completed.
	 * 
	 * You can use this method after
	 * you queued up the asynchronous
	 * operatings in order to sync them all.
	 * 
	 * Each mechanism also has its own global AMechanism::WaitForOperatingsCompletion().
	 * 
	 * @param WaitTimeMsec The time to wait. Measured in milliseconds.
	 * @param bIgnoreThreadIdleStats If @c true, ignores @c ThreadIdleStats.
	 * @return @c true if the event was triggered, @c false if the wait timed out.
	 */
	FORCEINLINE bool
	WaitForOperatingsCompletion(const uint32 WaitTimeMsec,
								const bool   bIgnoreThreadIdleStats = false) const
	{
		return OperatingsCompleted->Wait(WaitTimeMsec, bIgnoreThreadIdleStats);
	}

	/**
	 * Wait for all the current operatings
	 * to be completed.
	 * 
	 * You can use this method after
	 * you queued up the asynchronous
	 * operatings in order to sync them all.
	 * 
	 * Each mechanism also has its own global AMechanism::WaitForOperatingsCompletion().
	 *
	 * @param WaitTime The time to wait.
	 * @param bIgnoreThreadIdleStats If @c true, ignores @c ThreadIdleStats
	 * @return @c true if the event was triggered, @c false if the wait timed out.
	 */
	FORCEINLINE bool
	WaitForOperatingsCompletion(const FTimespan& WaitTime,
								const bool       bIgnoreThreadIdleStats = false) const
	{
		return OperatingsCompleted->Wait(WaitTime, bIgnoreThreadIdleStats);
	}

#pragma region Conversion
	/// @name Conversion
	/// @{

	/**
	 * Convert the chain to a textual representation.
	 * 
	 * @return The resulting textual representation.
	 */
	FString
	ToString() const
	{
		return TEXT("Chain #") + FString::FromInt(Id) + TEXT(" with Filter: ") + Filter.ToString();
	}

	/// @}
#pragma endregion Conversion

	/**
	 * Finalize the chain. 
	 * 
	 * Always call the Dispose() method in the
	 * descendants in order to wait properly.
	 */
	FORCEINLINE virtual
	~FCommonChain()
	{}

  protected:

	/**
	 * The common runnable mechanic base.
	 */
	struct FCommonRunnableMechanic
	  : public FRunnable
	{
	  protected:

		/**
		 * Is the runnable asynchronous (concurrent).
		 */
		bool bConcurrent = false;

		/**
		 * The executing thread.
		 */
		FRunnableThread* Thread = nullptr;

		/**
		 * Manual stop signal.
		 */
		std::atomic<bool> bStop{false};

		/**
		 * Has the runnable finished executing.
		 */
		std::atomic<bool> bFinished{false};

	  public:

		FORCEINLINE bool
		ShouldStop() const
		{
			return bStop.load(std::memory_order_relaxed);
		}

		FORCEINLINE void
		Stop() override
		{
			bStop.store(true, std::memory_order_relaxed);
		}

		/**
		 * Check if the runnable has finished its execution.
		 */
		FORCEINLINE auto
		HasFinished() const
		{
			return bFinished.load(std::memory_order_relaxed);
		}

		/**
		 * Halt the execution of the runnable on its thread.
		 * 
		 * @return The status of the operation.
		 */
		FORCEINLINE EApparatusStatus
		Halt(bool bShouldWait = true)
		{
			if (LIKELY(Thread != nullptr))
			{
				Thread->Kill(bShouldWait);
				delete Thread;
				Thread = nullptr;
				return EApparatusStatus::Success;
			}
			return EApparatusStatus::Noop;
		}

		/**
		 * Wait for the mechanic to complete.
		 */
		FORCEINLINE EApparatusStatus
		WaitForCompletion()
		{
			if (LIKELY(Thread != nullptr))
			{
				Thread->WaitForCompletion();
				return EApparatusStatus::Success;
			}
			return EApparatusStatus::Noop;
		}

		/**
		 * Initialize the base mechanic.
		 * 
		 * @param bInConcurrent Should the mechanic be executed in a concurrent environment.
		 */
		FORCEINLINE
		FCommonRunnableMechanic(const bool bInConcurrent)
		  : bConcurrent(bInConcurrent)
		{}

		FORCEINLINE
		~FCommonRunnableMechanic() override
		{
			// We don't wait here, since
			// the wait should be performed elsewhere.
			Halt(/*bShouldWait=*/false);
		}
	}; //-struct FCommonRunnableMechanic

	/**
	 * The runnable mechanical wrapping.
	 * 
	 * Can be run both in the main and parallel threads.
	 * 
	 * The wrapping is also used in the context of
	 * usual single-threaded operating as a wrapper.
	 */
	template < typename ChainT >
	struct TCommonRunnableMechanic
	  : public FCommonRunnableMechanic
	{
	  protected:

		/**
		 * The owner of the runnable.
		 */
		const ChainT* Owner = nullptr;

		/**
		 * This must be executed within a new thread's context,
		 * which is always within a solid mechanism.
		 */
		void
		DoBeginRun();

		/**
		 * This must be executed within a new thread context.
		 */
		void
		DoEndRun();

		FORCEINLINE uint32
		Run() override
		{
			DoBeginRun();
			DoEndRun();
			return 0;
		}

		/**
		 * Launch the asynchronous execution.
		 * 
		 * @note This method should be launched in
		 * the context of an origin thread.
		 *
		 * The thread is created automatically.
		 * 
		 * @param InId The unique identifier of the thread.
		 */
		FORCEINLINE EApparatusStatus
		DoLaunchAsync(int32 InId)
		{
			if (Owner != nullptr)
			{
				check(bConcurrent);
				if (Thread == nullptr)
				{
					bStop.store(false, std::memory_order_relaxed);
					Owner->IncrementOperatingsCount();
					Owner->IncrementConcurrencyLevel();
					const auto ThreadName = FString::Format(TEXT("MechanicThread_{0}@{1}"),
															{FString::FromInt(InId), FString::FromInt(Owner->Id)});
					Thread = FRunnableThread::Create(this, *ThreadName);
					return EApparatusStatus::Success;
				}
			}
			return EApparatusStatus::Noop;
		}

	  public:

		/**
		 * Get the owning chain of the runnable.
		 */
		FORCEINLINE auto
		GetOwner() const
		{
			return Owner;
		}

		/**
		 * Initialize a new runnable mechanic instance.
		 * 
		 * @param InOwner The owning mechanic to operate.
		 * May actually be a @c nullptr.
		 * @param bInConcurrent Should the mechanic
		 * run as part of a concurrent workflow.
		 */
		FORCEINLINE
		TCommonRunnableMechanic(const ChainT* const InOwner,
								const bool          bInConcurrent = false)
		  : FCommonRunnableMechanic(bInConcurrent)
		  , Owner(InOwner)
		{}
		
		/**
		 * Finalize the mechanic runnable.
		 */
		FORCEINLINE
		~TCommonRunnableMechanic() override
		{}

	}; //-struct TCommonRunnableMechanic

	/**
	 * A common asynchronous task.
	 */
	template < typename ChainT >
	struct TCommonMechanicTask : public FNonAbandonableTask
	{
	  protected:

		/**
		 * The owning chain of the task.
		 */
		const ChainT* Owner = nullptr;

		/**
		 * Is this task running in a concurrent mode.
		 */
		bool bConcurrent = false;

		/**
		 * This must be executed within a new thread's context,
		 * which is always within a solid mechanism.
		 */
		void
		DoBeginWork();

		/**
		 * This must be executed within a new thread context.
		 */
		void
		DoEndWork();

		FORCEINLINE void
		DoWork()
		{
			DoBeginWork();
			DoEndWork();
		}

	  public:

		FORCEINLINE
		TCommonMechanicTask()
		{}

		FORCEINLINE
		TCommonMechanicTask(const ChainT* const InOwner,
							const bool          bInConcurrent = false)
		  : Owner(InOwner)
		  , bConcurrent(bInConcurrent)
		{
			if (LIKELY(Owner != nullptr))
			{
				if (LIKELY(bConcurrent))
				{
					Owner->IncrementConcurrencyLevel();
				}
				Owner->IncrementOperatingsCount();
			}
		}

	}; //-struct TCommonMechanicTask

	/**
	 * The type of the value for the number of
	 * operatings within the chain.
	 */
	using OperatingsCountValueType = uint8;

	/**
	 * The number of current operatings on the chain.
	 * 
	 * This is atomic cause the concurrent
	 * operatings are added up to this value.
	 */
	mutable std::atomic<OperatingsCountValueType> OperatingsCount{0};

	/**
	 * The type of the concurrency value.
	 */
	using ConcurrencyLevelValueType = uint8;

	/**
	 * The current concurrency level of the chain.
	 */
	mutable std::atomic<ConcurrencyLevelValueType> ConcurrencyLevel{0};

	/**
	 * The event is fired once all of the operatings are
	 * completed.
	 */
	mutable FEventRef OperatingsCompleted{EEventMode::ManualReset};

	/**
	 * The list of enqueued runnable mechanics.
	 * 
	 * This ensures that all of the runnables
	 * get executed to the end.
	 */
	mutable TArray<TUniquePtr<FCommonRunnableMechanic>> BackgroundRunnableMechanics;

	/**
	 * A critical section for the TChain::BackgroundRunnableMechanics access.
	 */
	mutable FCriticalSection BackgroundRunnableMechanicsCS;

	void
	IncrementConcurrencyLevel() const;

	void
	DecrementConcurrencyLevel() const;

	void
	IncrementOperatingsCount() const;

	void
	DecrementOperatingsCount() const;

	/**
	 * Reset the state of the chain and mark as disposed (ready to be reused).
	 * 
	 * @param bWaitForOperatings Should the operation wait
	 * till all the operatings are complete.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	Dispose(bool bWaitForOperatings = true);

	/**
	 * Initialize a chain using its owning mechanism.
	 * 
	 * @param InOwner The owning mechanism.
	 */
	FORCEINLINE
	FCommonChain(AMechanism* const InOwner)
	  : Owner(InOwner)
	{
		check(InOwner != nullptr);
		// The trigger is set by default,
		// since no operatings should exit the wait process
		// immediately.
		OperatingsCompleted->Trigger();
	}

}; //-struct FCommonChain

using FCommonChainCursor = FCommonChain::FCommonCursor;

/**
 * Check if the supplied type is actually a chain cursor.
 * 
 * @tparam T The type to examine.
 * @return The state of examination.
 */
template < typename T >
constexpr bool IsChainCursorType()
{
	return std::is_base_of<FCommonChainCursor, T>::value;
}
