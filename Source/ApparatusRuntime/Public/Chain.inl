/*
 * ░▒▓ APPARATUS ▓▒░
 *
 * File: Chain.inl
 * Created: 2022-04-16 22:23:14
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

template < typename ChunkItType, typename BeltItType, EParadigm OuterParadigm >
template < EParadigm Paradigm, typename MechanicT, typename... Ts >
inline TOutcome<Paradigm>
TChain<ChunkItType, BeltItType, OuterParadigm>::DoOperateViaTasks(
	const MechanicT& InMechanic,
	const int32      ThreadsCountMax,
	const int32      SlotsPerThreadMin,
	const bool       bSync) const
{
	check(Owner);
	AssessConditionFormat(Paradigm, !bDisposed, EApparatusStatus::InvalidState,
						  TEXT("A chain should not be disposed in order to be processed via tasks."));

	typedef TMechanicTask<MechanicT, Ts...> TaskType;

	typedef FAsyncTask<TaskType> AsyncTaskType;
	typedef FAutoDeleteAsyncTask<TaskType> AutoDeleteAsyncTaskType;

	typedef typename TaskType::MechanicType MechanicType;
	static_assert(std::is_same<MechanicT, MechanicType>::value, "Mechanic types must be the same.");

	AssessCondition(Paradigm, SlotsPerThreadMin >= 1, EApparatusStatus::InvalidArgument);
	AssessCondition(Paradigm, ThreadsCountMax >= 1, EApparatusStatus::InvalidArgument);

	const int32 IterableCount = IterableNum();
	if (UNLIKELY(IterableCount == 0)) 
	{
		// Nothing to iterate upon:
		if (UsersNum() == 0)
		{
			const auto MutableChain = const_cast<TChain*>(this);
			AssessOK(Paradigm, MutableChain->Dispose());
		}
		return EApparatusStatus::Noop;
	}

	AsyncTaskType** Tasks = nullptr;

	const auto ThreadsCount = FMath::Clamp(IterableCount / SlotsPerThreadMin, 1, ThreadsCountMax);
	const auto SlotsPerThread = IterableCount / ThreadsCount;
	const auto Remainder = IterableCount % (ThreadsCount * SlotsPerThread);
	// Slots used in the current (game) thread
	// include the division left-over:
	const auto SlotsPerFirstThread = SlotsPerThread + Remainder;
	auto BackgroundTasksCount = ThreadsCount;
	// Cause of the sophisticated nature of threads,
	// make sure to retain manually:
	Retain();

	int32 Offset = 0;
	if (LIKELY(bSync))
	{
		BackgroundTasksCount -= 1;
		Offset = SlotsPerFirstThread;
	}

	if (LIKELY(BackgroundTasksCount > 0))
	{
		if (bSync)
		{
			Tasks = new AsyncTaskType*[BackgroundTasksCount];
		}
		for (int32 i = 0; i < BackgroundTasksCount; ++i)
		{
			if (bSync)
			{
				Tasks[i] = new AsyncTaskType(InMechanic, Iterate<Ts...>(Offset, SlotsPerThread),
											 /*bConcurrent=*/true);
				Tasks[i]->StartBackgroundTask();
				Offset += SlotsPerThread;
			}
			else
			{
				const auto SlotsCount = i == 0 ? SlotsPerFirstThread : SlotsPerThread;
				const auto Task = new AutoDeleteAsyncTaskType(InMechanic, Iterate<Ts...>(Offset, SlotsCount),
															  /*bConcurrent=*/true);
				Task->StartBackgroundTask();
				Offset += SlotsCount;
			}
		}
	}

	if (LIKELY(bSync))
	{
		const auto GameThreadTask = new AsyncTaskType(InMechanic, Iterate<Ts...>(0, SlotsPerFirstThread),
													  /*bConcurrent=*/true);
		GameThreadTask->StartSynchronousTask();
		delete GameThreadTask;

		if (LIKELY(BackgroundTasksCount > 0))
		{
			check(Tasks != nullptr);
			// Wait for background tasks to complete...
			for (int32 i = 0; i < BackgroundTasksCount; ++i)
			{
				Tasks[i]->EnsureCompletion();
				delete Tasks[i];
			}
			delete Tasks;
		}
	}
	// else: Asynchronous tasks are auto-destroyed.

	Release();

	return EApparatusStatus::Success;
}

template < typename ChunkItType, typename BeltItType, EParadigm OuterParadigm >
template < EParadigm Paradigm, typename MechanicT, typename... Ts >
inline TOutcome<Paradigm>
TChain<ChunkItType, BeltItType, OuterParadigm>::DoOperateViaThreads(
	const MechanicT& InMechanic,
	const int32      ThreadsCountMax,
	const int32      SlotsPerThreadMin,
	const bool       bSync) const
{
	check(Owner);
	AssessConditionFormat(Paradigm, !bDisposed, EApparatusStatus::InvalidState,
						  TEXT("A chain should not be disposed in order to be processed via threads."));

	typedef TRunnableMechanic<MechanicT, Ts...> RunnableType;
	typedef typename RunnableType::MechanicType MechanicType;
	static_assert(std::is_same<MechanicT, MechanicType>::value, "Mechanic types must be the same.");

	AssessCondition(Paradigm, SlotsPerThreadMin >= 1, EApparatusStatus::InvalidArgument);
	AssessCondition(Paradigm, ThreadsCountMax >= 1, EApparatusStatus::InvalidArgument);

	const int32 IterableCount = IterableNum();
	if (UNLIKELY(IterableCount == 0)) 
	{
		// Nothing to iterate upon:
		if (UsersNum() == 0)
		{
			const auto MutableChain = const_cast<TChain*>(this);
			verify(OK(MutableChain->Dispose()));
		}
		return EApparatusStatus::Noop;
	}

	TArray<RunnableType*> Runnables;
	const auto ThreadsCount = FMath::Clamp(IterableCount / SlotsPerThreadMin, 1, ThreadsCountMax);
	const auto SlotsPerThread = IterableCount / ThreadsCount;
	const auto Remainder = IterableCount % (ThreadsCount * SlotsPerThread);
	// Slots used in the current (game) thread
	// include the division left-over:
	const auto SlotsPerFirstThread = SlotsPerThread + Remainder;
	auto BackgroundThreadsCount = ThreadsCount;
	// Cause of the sophisticated nature of threads,
	// make sure to retain manually:
	Retain();

	int32 Offset = 0;
	if (LIKELY(bSync))
	{
		BackgroundThreadsCount -= 1;
		Offset = SlotsPerFirstThread;
	}

	if (LIKELY(BackgroundThreadsCount > 0))
	{
		if (bSync)
		{
			Runnables.Reserve(BackgroundThreadsCount);
			for (int32 i = 0; i < BackgroundThreadsCount; ++i)
			{
				const auto Runnable = new RunnableType(InMechanic, Iterate<Ts...>(Offset, SlotsPerThread),
													   /*bConcurrent=*/true);
				Runnables.Add(Runnable);
				Runnable->LaunchAsync(i);
				Offset += SlotsPerThread;
			}
		}
		else
		{
			FScopeLock Lock(&BackgroundRunnableMechanicsCS);
			// Clean-up existing finished threads...
			for (int32 i = 0; i < BackgroundRunnableMechanics.Num(); ++i)
			{
				const auto& BackgroundRunnable = BackgroundRunnableMechanics[i];
				if (BackgroundRunnable->HasFinished())
				{
					// Wait for a thread to complete to the end.
					// This won't inter-lock, since the retains
					// are guaranteed to not dispose the current chain.
					BackgroundRunnable->WaitForCompletion();
					BackgroundRunnableMechanics.RemoveAtSwap(i--);
				}
			}
			// Add new threads...
			BackgroundRunnableMechanics.Reserve(BackgroundRunnableMechanics.Num() + BackgroundThreadsCount);
			for (int32 i = 0; i < BackgroundThreadsCount; ++i)
			{
				const auto SlotsCount = (i == 0) ? SlotsPerFirstThread : SlotsPerThread;
				const auto Runnable = new RunnableType(InMechanic, Iterate<Ts...>(Offset, SlotsPerThread),
													   /*bConcurrent=*/true);
				BackgroundRunnableMechanics.Add(TUniquePtr<RunnableType>(Runnable));
				Runnable->LaunchAsync(BackgroundRunnableMechanics.Num() - 1);
				Offset += SlotsCount;
			}
		}
	}

	if (LIKELY(bSync))
	{
		// Occupy the main thread as well when in a syncing mode...
		RunnableType GameThreadRunnable(InMechanic, Iterate<Ts...>(0, SlotsPerFirstThread),
										/*bConcurrent=*/true);
		GameThreadRunnable.LaunchSync();

		// Wait till the background threads are also done...
		for (int32 i = 0; i < BackgroundThreadsCount; ++i)
		{
			Runnables[i]->WaitForCompletion();
			delete Runnables[i];
		}
	}

	Release();

	return EApparatusStatus::Success;
}

inline EApparatusStatus
FCommonChain::Dispose(bool bWaitForOperatings/*=true*/)
{
	// We need wait for operatings here as well,
	// since there may already be concurrent operatings
	// queued that require 'bDisposing' not to be set.
	// Waiting while self-destruction is activated doesn't
	// really make sense, since it (by design) should be
	// disposed by only a single last-using thread.
	if (bWaitForOperatings && !bSelfDestroyOnDisposal)
	{
		// Wait for all async operations to complete:
		WaitForOperatingsCompletion();
	}

	while (bDisposing.exchange(true, std::memory_order_acquire));

	if (UNLIKELY(bDisposed.load(std::memory_order_relaxed)))
	{
		bDisposing.store(false, std::memory_order_release);
		return EApparatusStatus::Noop;
	}

	// The wait for the concurrent operatings to be complete
	// is done in the Reset() also...
	verify(OK(Reset(/*bWaitForOperatings=*/false)));

	checkf(UsersNum() == 0, TEXT("Disposing a retained chain is not possible. Do you have any users still?"));
	checkf(ConcurrencyLevel == 0, TEXT("There must be no active concurrency within the chain in order to dispose it safely."));

	bDisposed.store(true, std::memory_order_release);
	// 'bStopIterating' is set in the base Reset().

	if (Owner->IsAutomaticDeferredsApplicationPossible())
	{
		Owner->ApplyDeferreds();
	}

	bDisposing.store(false, std::memory_order_release);

	// This check is out of locked scope,
	// since it can only happen on a last-using thread:
	if (bSelfDestroyOnDisposal && !bInDestructor)
	{
		delete this;
	}

	return EApparatusStatus::Success;
}

FORCEINLINE void
FCommonChain::IncrementOperatingsCount() const
{
	checkf(!IsDisposed(),
		   TEXT("Can't add new operating when the chain is already disposed. "
				"Have you already finished its iterating? "
				"Perhaps add a manual retainment?"));
	Owner->IncrementOperatingsCount();
	switch (OperatingsCount.fetch_add(1, std::memory_order_relaxed))
	{
		case 0:
			// We're starting the new operating(s)...
			OperatingsCompleted->Reset();
			UsersCount.fetch_add(1, std::memory_order_relaxed);
			break;
		case TNumericLimits<OperatingsCountValueType>::Max():
			checkf(false, TEXT("Operatings count overflow detected in chain: %s"),
				   *ToString());
			break;
	}
}

FORCEINLINE void
FCommonChain::DecrementOperatingsCount() const
{
	checkf(!bDisposed, TEXT("Can't decrement operating when the chain is already fully disposed."));
	switch (OperatingsCount.fetch_sub(1, std::memory_order_acq_rel))
	{
		case 0:
			checkf(false, TEXT("Operatings count underflow detected for chain: %s"), *ToString());
			break;
		case 1:
			// The users must be changed before the triggering,
			// so the counters are actually fine within Dispose.
			const auto PreviousUsersCount = UsersCount.fetch_sub(1, std::memory_order_acq_rel);
			// The last operating was processed.
			OperatingsCompleted->Trigger();
			if (PreviousUsersCount == 1)
			{
				const_cast<FCommonChain*>(this)->Dispose();
			}
			break;
	}
	Owner->DecrementOperatingsCount();
}

FORCEINLINE void
FCommonChain::IncrementConcurrencyLevel() const
{
	checkf(!IsDisposed(), TEXT("Can't add new concurrency when the chain is already disposed. "
							"Have you already finished its iterating? "
							"Perhaps add a manual retainment?"));
	
	switch (ConcurrencyLevel.fetch_add(1, std::memory_order_relaxed))
	{
		case 0:
			Owner->IncrementConcurrencyLevel();
			break;
		case TNumericLimits<ConcurrencyLevelValueType>::Max():
			checkf(false, TEXT("Concurrency level overflow detected."));
			break;
	}
}

FORCEINLINE void
FCommonChain::DecrementConcurrencyLevel() const
{
	checkf(!bDisposed, TEXT("Can't decrement concurrency level when the chain is already fully disposed."));
	switch (ConcurrencyLevel.fetch_sub(1))
	{
		case 0:
			checkf(false, TEXT("Concurrency level underflow detected."));
			break;
		case 1:
			Owner->DecrementConcurrencyLevel();
			break;
	}
}

template < typename ChainT >
FORCEINLINE void
FCommonChain::TCommonRunnableMechanic<ChainT>::DoBeginRun()
{
	bFinished.store(false, std::memory_order_relaxed);
	if (LIKELY(Owner != nullptr))
	{
		const auto Mechanism = Owner->Owner;
		check(Mechanism != nullptr);
		if (bConcurrent)
		{
			// The concurrency is incremented prior to thread's execution,
			// so not doing it here.
			Mechanism->LockShared();
		}
		else
		{
			// Lock for writing if we're bottom-most non-concurrent operating:
			Mechanism->LockExclusive();
		}
	}
}

template < typename ChainT >
FORCEINLINE void
FCommonChain::TCommonRunnableMechanic<ChainT>::DoEndRun()
{
	if (LIKELY(Owner != nullptr))
	{
		const auto Mechanism = Owner->Owner;
		check(Mechanism != nullptr);
		if (bConcurrent)
		{
			Owner->DecrementConcurrencyLevel();
			Mechanism->UnlockShared();
		}
		else
		{
			Mechanism->UnlockExclusive();
		}
		// This can trigger a chain's disposal
		// and destruction, also the deferreds application,
		// so should be the last one:
		Owner->DecrementOperatingsCount();
	}
	// Mark as finished explicitly:
	bFinished.store(true, std::memory_order_relaxed);
}

template < typename ChainT >
FORCEINLINE void
FCommonChain::TCommonMechanicTask<ChainT>::DoBeginWork()
{
	if (LIKELY(Owner != nullptr))
	{
		const auto Mechanism = Owner->Owner;
		check(Mechanism != nullptr);
		if (bConcurrent)
		{
			// The concurrency is incremented prior to thread's execution,
			// so not doing it here.
			Mechanism->LockShared();
		}
		else
		{
			Mechanism->LockExclusive();
		}
	}
}

template < typename ChainT >
FORCEINLINE void
FCommonChain::TCommonMechanicTask<ChainT>::DoEndWork()
{
	if (LIKELY(Owner != nullptr))
	{
		const auto Mechanism = Owner->Owner;
		check(Mechanism != nullptr);
		if (bConcurrent)
		{
			Owner->DecrementConcurrencyLevel();
			Mechanism->UnlockShared();
		}
		else
		{
			Mechanism->UnlockExclusive();
		}
		// This can trigger a chain's disposal,
		// self-destruction and deferreds application,
		// so should be the last one:
		Owner->DecrementOperatingsCount();
	}
}

template < typename ChunkItType, typename BeltItType, EParadigm Paradigm >
template < typename T, typename AllocatorT >
EApparatusStatus
TChain<ChunkItType, BeltItType, Paradigm>::GetTraits(TArray<T, AllocatorT>& OutArray,
													 const EGatheringType   GatheringType/*=Clear*/) const
{
	checkf(!bDisposed,
			TEXT("Copying the traits from the disposed chain is not supported."));
	const auto TraitIndex = Filter.GetTraitmark().template IndexOf<T>();
	if (UNLIKELY(TraitIndex < 0))
	{
		// The trait must be in the filter.
		return EApparatusStatus::InvalidState;
	}
	if (UNLIKELY(Filter.DetailsNum() > 0))
	{
		// The iterating must be chunk-based.
		// TODO: Implement grabbing traits from belts also.
		return EApparatusStatus::UnsupportedOperation;
	}
	if (UNLIKELY(EnumHasAnyFlags(Filter.GetFlagmark(), FM_AllUserLevel)))
	{
		// The filter must not have any flags specified.
		return EApparatusStatus::UnsupportedOperation;
	}
	auto Status = EApparatusStatus::Noop;
	const auto Count = IterableNum();
	if (LIKELY(GatheringType == Clear))
	{
		if (UNLIKELY(OutArray.Num() > 0))
		{
			Status = EApparatusStatus::Success;
			OutArray.Reset(Count);
		}
	}
	else
	{
		OutArray.Reserve(OutArray.Num() + Count);
	}
	if (UNLIKELY(Count == 0))
	{
		return Status;
	}
	// Get segment-wise.
	for (const auto& Segment : Segments)
	{
		const auto Chunk = Segment.GetChunk();
		if (Chunk)
		{
			// TODO: Also check for booting here.
			// TODO: Some of the slots may still be stale
			// here if they are deferred-despawned in a hard way.
			checkf(!Chunk->HasQueuedForRemoval(), TEXT(
				"The '%s' chunk must not have any stale slots prior to the '%s' traits copying."
			), *Chunk->GetName(), *T::StaticStruct()->GetName());
			checkf(!Owner->HasDeferreds(), TEXT(
				"Copying during a concurrency is not supported. "
				"The '%s' chunk may have the deferred stale slots while '%s' traits copying."
			), *Chunk->GetName(), *T::StaticStruct()->GetName());
			const auto& TraitLine = Chunk->template GetTraitLine<T>();
			StatusAccumulate(Status, TraitLine.CopyTo(OutArray, FScriptStructArray::Collect));
		}
	}
	return Status;
}

template < typename ChunkItType, typename BeltItType, EParadigm Paradigm >
template < typename MechanicT, typename... Ps >
FORCEINLINE void
TChain<ChunkItType, BeltItType, Paradigm>::TRunnableMechanic<MechanicT, Ps...>::LaunchSync()
{
	checkf(Super::Thread == nullptr,
		   TEXT("May not launch in a synchronous mode since already running concurrently."));
	if (Owner != nullptr)
	{
		Super::bStop.store(false, std::memory_order_relaxed);
		Owner->IncrementOperatingsCount();
		if (Super::bConcurrent)
		{
			Owner->IncrementConcurrencyLevel();
		}
		Super::DoBeginRun();
		DoRun();
		Super::DoEndRun();
	}
}
