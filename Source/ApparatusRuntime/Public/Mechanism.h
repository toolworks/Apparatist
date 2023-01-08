/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Mechanism.h
 * Created: 2021-08-29 22:07:18
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
#include <type_traits>

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "Containers/Queue.h"
#include "Misc/TVariant.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define MECHANISM_H_SKIPPED_MACHINE_H
#endif

#include "Filter.h"
#include "UnsafeSubjectHandle.h"
#include "TraitInfo.h"
#include "Mechanical.h"
#include "AdjectiveInfo.h"
#include "MechanicInfo.h"
#include "Chain.h"
#include "ChunkProxy.h"
#include "SubjectRecordCollection.h"

#ifdef MECHANISM_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "Mechanism.generated.h"


// Forward declarations:
struct FSubjectHandle;
struct FCommonChain;
class UMachine;
class UNetworkBearerComponent;
class USubjectRecordCollection;
#if WITH_EDITOR
class FMechanismCustomization;
#endif

DECLARE_STATS_GROUP(TEXT("Mechanism"), STATGROUP_Mechanism, STATCAT_Advanced);

DECLARE_CYCLE_STAT_EXTERN(TEXT("Boot"),
						  STAT_MechanismBoot,
						  STATGROUP_Mechanism,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Buffer"),
						  STAT_MechanismBuffer,
						  STATGROUP_Mechanism,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Find Matching Iterables"),
						  STAT_MechanismFindMatchingIterables,
						  STATGROUP_Mechanism,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Evaluate"),
						  STAT_MechanismEvaluate,
						  STATGROUP_Mechanism,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Evaluate ~ Input"),
						  STAT_MechanismEvaluateInput,
						  STATGROUP_Mechanism,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Evaluate ~ Steady"),
						  STAT_MechanismEvaluateSteady,
						  STATGROUP_Mechanism,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Evaluate ~ Presentation"),
						  STAT_MechanismEvaluatePresentation,
						  STATGROUP_Mechanism,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Fetch"),
						  STAT_MechanismFetchDetails,
						  STATGROUP_Mechanism,
						  APPARATUSRUNTIME_API);

/**
 * The kind of deferred operation.
 * Matches the AMechanism::FDeferred union ordering.
 */
enum class EDeferred
{
	/**
	 * A special constant denoting
	 * all type of deferreds at the same time.
	 */
	All = -1,

	/**
	 * A special constant denoting
	 * no type of deferreds.
	 */
	None = 0,

	/**
	 * Spawning a new subject.
	 */
	SubjectSpawn,

	/**
	 * Despawning an existing subject.
	 */
	SubjectDespawn,

	/**
	 * Setting a trait for a subject.
	 */
	TraitSetting,

	/**
	 * Obtainment a trait for a subject.
	 */
	TraitObtainment,

	/**
	 * Removing a trait from a subject.
	 */
	TraitRemoval,

	/**
	 * Removing all of the traits from a subject.
	 */
	AllTraitsRemoval

}; // enum class EDeferred

/**
 * The scoped Apparatus state manager.
 *
 * This consists of subjects, subjectives
 * and the mechanicals in the current world.
 * 
 * An instance of mechanism is created
 * automatically per each UWorld, when it's needed.
 * You can also instantiate it manually
 * or even place it on the map in the Editor.
 */
UCLASS()
class APPARATUSRUNTIME_API AMechanism
  : public AActor
{
	GENERATED_BODY()

  public:

	/**
	 * The type of the mechanism identifier.
	 */
	typedef FSubjectInfo::MechanismIdType IdType;

	/**
	 * Invalid mechanism identifier.
	 */
	static constexpr IdType InvalidId = 0x00;

	/**
	 * The first valid mechanism identifier.
	 */
	static constexpr IdType FirstId = 0x01;

	/**
	 * The last valid mechanism identifier.
	 */
	static constexpr IdType LastId = TNumericLimits<IdType>::Max() - 1;

	/**
	 * The maximum possible number of mechanisms within the machine.
	 */
	static constexpr int32 InstancesMax = LastId - FirstId + 1;

	/**
	 * The type for the unique subject identifiers.
	 */
	typedef FSubjectInfo::IdType SubjectIdType;

	/**
	 * Invalid subject identifier.
	 */
	static constexpr SubjectIdType InvalidSubjectId = FSubjectInfo::InvalidId;

	/**
	 * A first valid subject placement index within the pool.
	 */
	static constexpr SubjectIdType FirstSubjectPlace = FSubjectInfo::FirstPlace;

	/**
	 * The maximum valid subject placement index within the pool.
	 */
	static constexpr SubjectIdType LastSubjectPlace = FSubjectInfo::LastPlace;

	/**
	 * The maximum number of subjects within the machine.
	 */
	static constexpr SubjectIdType SubjectPlacesPerMechanism = FSubjectInfo::PlacesPerMechanismMax;

	/**
	 * The pre-allocated size of the iterables being searched.
	 */
	static constexpr int32 FoundIterablesInlineSize = 32;

	enum
	{
		/**
		 * Invalid chain identifier.
		 */
		InvalidChainId = FChain::InvalidId,

		/**
		 * First valid chain identifier.
		 */
		FirstChainId = FChain::FirstId
	};

	/**
	 * A safe applicator for the
	 * deferred changes (or deferreds).
	 */
	template < EParadigm DefaultApplicatorParadigm = EParadigm::Safe >
	struct TDeferredsApplicator
	{
	  private:

		friend class AMechanism;

		AMechanism* Owner = nullptr;

		EDeferred Filter = EDeferred::All;

		/**
		 * An internal constructor used
		 * to initialize an original instance
		 * of the applicator.
		 */
		FORCEINLINE
		TDeferredsApplicator(AMechanism* const InOwner,
							 const EDeferred   InFilter)
		  : Owner(InOwner)
		  , Filter(InFilter)
		{
			check(InOwner);
			check(!InOwner->IsInConcurrentEnvironment());
			Owner->DeferredsApplicatorsCount += 1;
		}

		/**
		 * Applicators are only allowed to be moved.
		 */
		TDeferredsApplicator(const TDeferredsApplicator& InApplicator) = delete;

		/**
		 * Applicators are only allowed to be moved.
		 */
		TDeferredsApplicator& operator=(const TDeferredsApplicator& InApplicator) = delete;

	  public:

		/**
		 * The move constructor is used to receive
		 * an applicator from a method.
		 */
		FORCEINLINE
		TDeferredsApplicator(TDeferredsApplicator&& InApplicator)
		{
			check(InApplicator.Owner);
			check(!InApplicator.Owner->IsInConcurrentEnvironment());
			Owner  = InApplicator.Owner;
			Filter = InApplicator.Filter;
			InApplicator.Owner = nullptr;
		}

		/**
		 * Apply the pending deferreds.
		 * 
		 * @tparam Paradigm The paradigm to work under.
		 * @return The outcome of the application. 
		 */
		template < EParadigm Paradigm = DefaultApplicatorParadigm >
		FORCEINLINE TOutcome<Paradigm>
		Apply()
		{
			check(Owner);
			const auto Outcome = Owner->template ApplyDeferreds<Paradigm>(Filter);
			Owner->DeferredsApplicatorsCount -= 1;
			Owner = nullptr;
			return Outcome;
		}

		/**
		 * Destroys the applicator,
		 * automatically applying the deferreds (if needed).
		 */
		~TDeferredsApplicator()
		{
			if (LIKELY(Owner))
			{
				Apply<DefaultApplicatorParadigm>();
			}
		}
	};

  private:

	friend struct FCommonChain;
	friend struct FSubjectNetworkState;
	friend class UNetworkBearerComponent;
	friend class UMachine;
	friend class IIterable;

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	template < typename M >
	struct TFunctionInfo;

	template < typename R, typename... Ts >
	struct TFunctionInfo<R(Ts...)>
	{
		/**
		 * The type of the automatic filter that
		 * should be used for iterating using the function.
		 */
		using FilterType = TFilter<typename more::flatten<Ts>::type...>;

		/**
		 * Get the filter matching the function arguments.r 
		 */
		static FORCEINLINE constexpr FilterType
		MakeFilter()
		{
			return FilterType();
		}

		/**
		 * Check if the solid chain needed for the function operating.
		 */
		static FORCEINLINE constexpr bool
		NeedsSolidChain()
		{
			return more::is_base_contained_flatly<TSubjectHandle<true, true, false>/*==FSolidSubjectHandle*/, Ts...>::value
				|| more::is_contained_flatly<FSolidChain, Ts...>::value
				|| THasDirectTraitAccess<Ts...>::Value;
		}
	};

	template < typename R, typename... Ts >
	struct TFunctionInfo<R(Ts...) const>
	  : TFunctionInfo<R(Ts...)>
	{};

	template < typename R, typename... Ts >
	struct TFunctionInfo<R(Ts...) &>
	  : TFunctionInfo<R(Ts...)>
	{};

	template < typename R, typename... Ts >
	struct TFunctionInfo<R(Ts...) const &>
	  : TFunctionInfo<R(Ts...)>
	{};

	template < typename R, typename... Ts >
	struct TFunctionInfo<R(Ts...) &&>
	  : TFunctionInfo<R(Ts...)>
	{};

	template < typename R, typename... Ts >
	struct TFunctionInfo<R(Ts...) const &&>
	  : TFunctionInfo<R(Ts...)>
	{};

#pragma region Function Pointer-Compatible

	template < typename R, typename... Ts >
	struct TFunctionInfo<R(&)(Ts...)>
	  : TFunctionInfo<R(Ts...)>
	{};

	template < typename R, typename... Ts >
	struct TFunctionInfo<R(*)(Ts...)>
	  : TFunctionInfo<R(Ts...)>
	{};

	template < typename R, typename... Ts >
	struct TFunctionInfo<R(* const)(Ts...)>
	  : TFunctionInfo<R(Ts...)>
	{};

#pragma endregion Function Pointer-Compatible

	template < typename Method >
	struct TMethodInfo;

	template < typename Method, class C >
	struct TMethodInfo<Method C::*>
	  : TFunctionInfo<Method>
	{};

#pragma region Registry

	/**
	 * The unique mechanism identifier.
	 * 
	 * Matches the index of the mechanism
	 * within the machine's subject pool registry.
	 */
	IdType MechanismId = InvalidId;

#pragma endregion Registry

#pragma region Lifetime

	/**
	 * Was the mechanism already finalized.
	 */
	bool bDisposed = false;

#pragma endregion Lifetime

#pragma region Threading Security

	/**
	 * The read-only and write-exclusive guard.
	 * 
	 * This is by design non-recursive, so writing access
	 * should be used within upper-most operatings only.
	 */
	mutable FRWLock AccessGuard;

	/**
	 * The id of the thread that currently has the writing
	 * access.
	 * 
	 * Needed for recursive locking support.
	 */
	mutable std::atomic<uint32> ExclusiveAccessThreadId{0x0};

	/**
	 * The number of times this mechanism was locked
	 * exclusively by the corresponding thread.
	 * 
	 * This doesn't need to be atomic, since
	 * is always changed in a synchronized context.
	 */
	mutable int32 ExclusiveLocks = 0;

	/**
	 * Lock the mechanism for the consisted reading mode.
	 */
	void
	LockShared() const
	{
		const auto CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
		if (CurrentThreadId == ExclusiveAccessThreadId.load(std::memory_order_relaxed))
		{
			// Already has exclusivity:
			return;
		}
		AccessGuard.ReadLock();
	}

	/**
	 * Unlock the mechanism from the consisted reading mode.
	 */
	void
	UnlockShared() const
	{
		const auto CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
		if (CurrentThreadId == ExclusiveAccessThreadId.load(std::memory_order_relaxed))
		{
			// Already has exclusivity:
			return;
		}
		AccessGuard.ReadUnlock();
	}

	/**
	 * Lock the mechanism for being able to do structural changes.
	 */
	void
	LockExclusive()
	{
		const auto CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
		if (CurrentThreadId == ExclusiveAccessThreadId.load(std::memory_order_relaxed))
		{
			// We already have the exclusivity on this thread:
			ExclusiveLocks += 1;
			return;
		}

		AccessGuard.WriteLock(); // Only a single thread would succeed in grabbing this one.
		ExclusiveAccessThreadId.store(CurrentThreadId, std::memory_order_relaxed);
		ExclusiveLocks = 1;
	}

	/**
	 * Unlock the mechanism for being able to do structural changes.
	 */
	void
	UnlockExclusive()
	{
		const auto CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
		check(CurrentThreadId == ExclusiveAccessThreadId.load(std::memory_order_relaxed));
		check(ExclusiveLocks >= 1);
		ExclusiveLocks -= 1;
		if (ExclusiveLocks == 0)
		{
			ExclusiveAccessThreadId.store(0x0, std::memory_order_relaxed);
			AccessGuard.WriteUnlock();
		}
	}

	/**
	 * Shared scope disallowing structural changes
	 * on the mechanism.
	 */
	struct FSharedScope final
	{
		const AMechanism* const Mechanism;

		FORCEINLINE
		FSharedScope(const AMechanism* const InMechanism)
		  : Mechanism(InMechanism)
		{
			Mechanism->LockShared();
		}

		FORCEINLINE
		~FSharedScope()
		{
			Mechanism->UnlockShared();
		}
	};

	/**
	 * Exclusive scope allowing structural changes
	 * on the mechanism.
	 */
	struct FExclusiveScope final
	{
		AMechanism* const Mechanism;

		FExclusiveScope(AMechanism* const InMechanism)
		  : Mechanism(InMechanism)
		{
			Mechanism->LockExclusive();
		}

		FORCEINLINE
		~FExclusiveScope()
		{
			Mechanism->UnlockExclusive();
		}
	};

#pragma endregion Threading Security

#pragma region Subjects

#if WITH_EDITORONLY_DATA

	friend class FMechanismCustomization;

	/**
	 * A subject filter used during the subject dumping process.
	 */
	UPROPERTY(EditAnywhere, Transient, Category = "Debugging", Meta = (AllowPrivateAccess="true"))
	FFilter DumpingFilter = FFilter::Zero;

#endif // WITH_EDITORONLY_DATA

	/**
	 * The type of the current operatings count value.
	 */
	using OperatingsCountValueType = uint16;

	/**
	 * The number of current operatings on the mechanism.
	 * 
	 * This is atomic cause the concurrent
	 * operatings are added up to this value.
	 */
	mutable std::atomic<OperatingsCountValueType> OperatingsCount{0};

	/**
	 * The event is fired once all of the operatings are
	 * completed.
	 */
	mutable FEventRef OperatingsCompleted{EEventMode::ManualReset};

	FORCEINLINE void
	IncrementOperatingsCount()
	{
		switch (OperatingsCount.fetch_add(1, std::memory_order_relaxed))
		{
			case 0:
				// We're starting the new operating(s)...
				OperatingsCompleted->Reset();
				break;
			case TNumericLimits<OperatingsCountValueType>::Max():
				checkf(false, TEXT("Mechanism operatings counting overflow detected."));
				break;
		}
	}

	FORCEINLINE void
	DecrementOperatingsCount()
	{
		switch (OperatingsCount.fetch_sub(1, std::memory_order_relaxed))
		{
			case 1:
				OperatingsCompleted->Trigger();
				break;
			case 0:
				checkf(false, TEXT("Mechanism operatings counting underflow detected."));
				break;
		}
	}

	/**
	 * All of the currently available chunks.
	 */
	UPROPERTY()
	TArray<UChunk*> Chunks;

	/**
	 * All of the currently available chunks, stored by their traitmarks.
	 */
	mutable TMap<FTraitmark, TWeakObjectPtr<UChunk>> ChunksByTraitmarksCache;

	/**
	 * All of the chunks stored by their matching filters.
	 */
	mutable TMap<FFilter, TArray<TWeakObjectPtr<UChunk>>> ChunksByFilterCache;

	/**
	 * The registered adjectives of the mechanism.
	 */
	TArray<FAdjectiveInfo> Adjectives;

	/**
	 * The registered mechanics of the mechanism.
	 */
	TArray<FMechanicInfo> Mechanics;

#pragma endregion Subjects

#pragma region Concurrency

	/**
	 * The type of the concurrency level value.
	 */
	using ConcurrencyLevelValueType = uint8;

	/**
	 * Is the mechanism currently in a concurrent environment?
	 */
	std::atomic<ConcurrencyLevelValueType> ConcurrencyLevel{0};

	/**
	 * Enter the concurrent environment for the mechanism.
	 * 
	 * Must be finalized by a call to AMechanism::DecrementConcurrencyLevel.
	 */
	FORCEINLINE void
	IncrementConcurrencyLevel()
	{
		switch (ConcurrencyLevel.fetch_add(1, std::memory_order_relaxed))
		{
			case TNumericLimits<ConcurrencyLevelValueType>::Max():
				checkf(false, TEXT("Concurrency level overflow detected on the '%s' mechanism."),
					   *GetName());
				break;
		}
	}

	/**
	 * Exit the concurrent environment of the mechanism.
	 */
	FORCEINLINE void
	DecrementConcurrencyLevel()
	{
		switch (ConcurrencyLevel.fetch_sub(1, std::memory_order_relaxed))
		{
			case 0:
				checkf(false, TEXT("Concurrency level underflow detected on the '%s' mechanism."),
					   *GetName());
				break;
		}
	}

#pragma endregion Concurrency

#pragma region Deferred Changes

	/**
	 * Deferred spawning a subject.
	 */
	struct FDeferredSubjectSpawn
	{
		FSubjectRecord SubjectRecord;

		FORCEINLINE
		FDeferredSubjectSpawn(FSubjectRecord&& InSubjectRecord)
		  : SubjectRecord(InSubjectRecord)
		{}

		FORCEINLINE
		FDeferredSubjectSpawn(const FSubjectRecord& InSubjectRecord)
		  : SubjectRecord(InSubjectRecord)
		{}

		/**
		 * Apply the actual change.
		 */
		template < EParadigm Paradigm >
		FORCEINLINE TOutcome<Paradigm>
		Apply(AMechanism* const InMechanism) const
		{
			check(InMechanism != nullptr);
			return InMechanism->template SpawnSubject<Paradigm>(SubjectRecord);
		}
	}; //-struct FDeferredSubjectSpawn

	/**
	 * Deferred despawn of a subject.
	 */
	struct FDeferredSubjectDespawn
	{
		FSubjectHandle Target;

		FORCEINLINE
		FDeferredSubjectDespawn(FSubjectHandle InTarget)
		  : Target(InTarget)
		{}

		/**
		 * Apply the actual change.
		 */
		template < EParadigm Paradigm >
		FORCEINLINE TOutcome<Paradigm>
		Apply() const
		{
			if (LIKELY(Target))
			{
				return Target.template Despawn<Paradigm>();
			}
			return EApparatusStatus::Noop;
		}
	}; //-struct FDeferredSubjectDespawn

	/**
	 * Deferred setting a trait.
	 */
	struct FDeferredTraitSetting
	{
		FSubjectHandle Target;
		FTraitRecord   Trait;

		FORCEINLINE
		FDeferredTraitSetting(FSubjectHandle       InTarget,
							  UScriptStruct* const TraitType,
							  const void* const    TraitData)
		  : Target(InTarget)
		  , Trait(TraitType, TraitData)
		{}

		/**
		 * Apply the actual change.
		 */
		template < EParadigm Paradigm >
		FORCEINLINE TOutcome<Paradigm>
		Apply() const
		{
			if (LIKELY(Target))
			{
				return Target.template SetTrait<Paradigm>(Trait);
			}
			return EApparatusStatus::Noop;
		}
	}; //-struct FDeferredTraitSetting

	/**
	 * Deferred obtaining of a trait.
	 */
	struct FDeferredTraitObtainment
	{
		FSubjectHandle Target;
		UScriptStruct* TraitType;

		FORCEINLINE
		FDeferredTraitObtainment(FSubjectHandle       InTarget,
								 UScriptStruct* const InTraitType)
		  : Target(InTarget)
		  , TraitType(InTraitType)
		{}

		/**
		 * Apply the actual change.
		 */
		template < EParadigm Paradigm >
		FORCEINLINE TOutcome<Paradigm>
		Apply() const
		{
			if (LIKELY(Target))
			{
				return Target.template ObtainTrait<Paradigm>(TraitType);
			}
			return EApparatusStatus::Noop;
		}
	}; //-struct FDeferredTraitRemoval

	/**
	 * Deferred removal of a trait.
	 */
	struct FDeferredTraitRemoval
	{
		FSubjectHandle Target;
		UScriptStruct* TraitType;

		FORCEINLINE
		FDeferredTraitRemoval(FSubjectHandle       InTarget,
							  UScriptStruct* const InTraitType)
		  : Target(InTarget)
		  , TraitType(InTraitType)
		{}

		/**
		 * Apply the actual change.
		 */
		template < EParadigm Paradigm >
		FORCEINLINE TOutcome<Paradigm>
		Apply() const
		{
			if (LIKELY(Target))
			{
				return Target.template RemoveTrait<Paradigm>(TraitType);
			}
			return EApparatusStatus::Noop;
		}
	}; //-struct FDeferredTraitRemoval

	/**
	 * Deferred removal of all the traits.
	 */
	struct FDeferredAllTraitsRemoval
	{
		FSubjectHandle Target;

		FORCEINLINE
		FDeferredAllTraitsRemoval(FSubjectHandle InTarget)
		  : Target(InTarget)
		{}

		/**
		 * Apply the actual change.
		 */
		template < EParadigm Paradigm >
		FORCEINLINE TOutcome<Paradigm>
		Apply() const
		{
			if (LIKELY(Target))
			{
				return Target.template RemoveAllTraits<Paradigm>();
			}
			return EApparatusStatus::Noop;
		}
	}; //-struct FDeferredAllTraitsRemoval

	/**
	 * The common deferred change.
	 */
	typedef TVariant</*None*/int32,
	  			 FDeferredSubjectSpawn,
				 FDeferredSubjectDespawn,
				 FDeferredTraitSetting,
				 FDeferredTraitObtainment,
				 FDeferredTraitRemoval,
				 FDeferredAllTraitsRemoval> FDeferred;

	/**
	 * Apply a single deferred change.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Deferred The deferred change to apply.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm >
	FORCEINLINE TOutcome<Paradigm>
	Apply(const FDeferred& Deferred)
	{
		static_assert(FDeferred::IndexOfType<FDeferredSubjectSpawn>()     == (SIZE_T)EDeferred::SubjectSpawn && 
					  FDeferred::IndexOfType<FDeferredSubjectDespawn>()   == (SIZE_T)EDeferred::SubjectDespawn &&
					  FDeferred::IndexOfType<FDeferredTraitSetting>()     == (SIZE_T)EDeferred::TraitSetting &&
					  FDeferred::IndexOfType<FDeferredTraitObtainment>()  == (SIZE_T)EDeferred::TraitObtainment &&
					  FDeferred::IndexOfType<FDeferredTraitRemoval>()     == (SIZE_T)EDeferred::TraitRemoval &&
					  FDeferred::IndexOfType<FDeferredAllTraitsRemoval>() == (SIZE_T)EDeferred::AllTraitsRemoval,
					  "The indices of the deferred types must match.");
		switch (Deferred.GetIndex())
		{
			case FDeferred::IndexOfType<FDeferredSubjectSpawn>():
			{
				return Deferred.template Get<FDeferredSubjectSpawn>().template Apply<Paradigm>(this);
			}
			case FDeferred::IndexOfType<FDeferredSubjectDespawn>():
			{
				return Deferred.template Get<FDeferredSubjectDespawn>().template Apply<Paradigm>();
			}
			case FDeferred::IndexOfType<FDeferredTraitSetting>():
			{
				return Deferred.template Get<FDeferredTraitSetting>().template Apply<Paradigm>();
			}
			case FDeferred::IndexOfType<FDeferredTraitObtainment>():
			{
				return Deferred.template Get<FDeferredTraitObtainment>().template Apply<Paradigm>();
			}
			case FDeferred::IndexOfType<FDeferredTraitRemoval>():
			{
				return Deferred.template Get<FDeferredTraitRemoval>().template Apply<Paradigm>();
			}
			case FDeferred::IndexOfType<FDeferredAllTraitsRemoval>():
			{
				return Deferred.template Get<FDeferredAllTraitsRemoval>().template Apply<Paradigm>();
			}
		}
		return EApparatusStatus::Noop;
	}

	/**
	 * The deferred changes queue.
	 */
	typedef TQueue<FDeferred, EQueueMode::Mpsc> FDeferredChanges;

	/**
	 * The buffer of deferred commands for later execution.
	 */
	FDeferredChanges Deferreds;

	/**
	 * The current number of reserved deferreds applicators.
	 * 
	 * The value here is always changed within the main thread,
	 * hence the non-atomicy.
	 * Those prevent the automatic deferreds application.
	 */
	int32 DeferredsApplicatorsCount = 0;

	/**
	 * Enqueue a new deferred.
	 */
	template < typename D >
	FORCEINLINE void
	EnqueueDeferred(D&& Deferred)
	{
		Deferreds.Enqueue(AMechanism::FDeferred(TInPlaceType<D>(), std::forward<D>(Deferred)));
	}

#pragma endregion Deferred Changes

#pragma region Subjectives

	/**
	 * All of the currently available belts.
	 */
	UPROPERTY()
	TArray<UBelt*> Belts;

	/**
	 * Belts sorted by their unique tags,
	 * possibly coming from the preferreds.
	 */
	TMap<UBelt::TagType, TWeakObjectPtr<UBelt>> BeltByTag;

	/**
	 * All of the belts stored by their matching filters.
	 */
	mutable TMap<FFilter, TArray<TWeakObjectPtr<UBelt>>> BeltsByFilterCache;

	/**
	 * All of the currently registered subjectives.
	 */
	TSet<ISubjective*> Subjectives;

#pragma endregion Subjectives

#pragma region Subjects

	/**
	 * All of the currently halted subjects.
	 */
	TArray<FSubjectHandle> HaltedSubjects;

#pragma endregion Subjects

#pragma region Networking

	/**
	 * Subject identifiers stored by their networking identifiers.
	 * 
	 * This LUT is mechanism-based for the correct multi-world support.
	 */
	TMap<uint32, int32> SubjectByNetworkId;

#pragma endregion Networking

	/**
	 * All of the currently available mechanicals.
	 */
	TArray<TWeakInterfacePtr<IMechanical>> Mechanicals;

	/**
	 * The current list of available mutable chains.
	 */
	mutable TArray<TSharedRef<FChain>> Chains;

	/**
	 * The current list of available uniform chains.
	 */
	mutable TArray<TSharedRef<FSolidChain>> SolidChains;

	/**
	 * The locks of the mechanism.
	 * 
	 * If it's positive - liquid-locked.
	 * If it's negative - solid-locked.
	 * If it's zero - non-locked.
	 */
	std::atomic<int16> LocksCount{0};

	/**
	 * Liquify the mechanism assessing it can be changed structurally.
	 */
	FORCEINLINE void
	Liquify()
	{
		verifyf(LocksCount.fetch_add(1, std::memory_order_relaxed) >= 0,
			    TEXT("The '%s' mechanism must not be already solid to become liquid."),
				*GetName());
	}

	/**
	 * Solidify the mechanism making it immune to structural changes.
	 */
	FORCEINLINE void
	Solidify()
	{
		verifyf(LocksCount.fetch_sub(1, std::memory_order_relaxed) <= 0,
			    TEXT("The '%s' mechanism must not already be liquid-locked to become solid."),
				*GetName());
	}

	/**
	 * Decrease the liquidity state of the mechanism.
	 */
	FORCEINLINE void
	Deliquify()
	{
		verifyf(LocksCount.fetch_sub(1, std::memory_order_relaxed) > 0,
				TEXT("The '%s' mechanism must already be liquid to drop the state."),
				*GetName());
	}

	/**
	 * Decrease the solidity state of the mechanism.
	 */
	FORCEINLINE void
	Desolidify()
	{
		verifyf(LocksCount.fetch_add(1, std::memory_order_relaxed) < 0,
				TEXT("The '%s' mechanism must already be solid to drop the state."),
				*GetName());
	}

	/**
	 * Clear the cache of the current mechanism (if any).
	 */
	FORCEINLINE EApparatusStatus
	ClearCache()
	{
		BeltsByFilterCache.Reset();
		ChunksByFilterCache.Reset();
		ChunksByTraitmarksCache.Reset();
		return EApparatusStatus::Success;
	}

	/**
	 * Obtain a belt most specific for a fingerprint.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, UBelt*>
	ObtainMostSpecificBelt(const FFingerprint& Fingerprint);

	/**
	 * Obtain a belt most specific for a subjective.
	 * 
	 * Subjective's preferred belt is taken into account.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, UBelt*>
	ObtainMostSpecificBelt(ISubjective* const Subjective);

	/**
	 * Fetch all of the details in all of the belts.
	 */
	static EApparatusStatus
	FetchDetailsInAllBelts();

	friend class IIterable;
	friend class UBelt;
	friend class UChunk;
	friend class UApparatusFunctionLibrary;
	friend struct FSubjectInfo;
	friend struct FCommonSubjectHandle;

	template < typename SubjectHandleT >
	friend struct TChunkIt;

	template < typename SubjectHandleT >
	friend struct TBeltIt;

	template < typename ChunkItT, typename BeltItT >
	struct TChain;

#pragma region Belts

	/**
	 * Obtain a belt with its unique tag.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Tag The tag to pick by.
	 * @return The belt for the tag.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE TOutcome<Paradigm, UBelt*>
	ObtainBeltByTag(const UBelt::TagType Tag)
	{
		if (AvoidCondition(Paradigm, Tag == UBelt::InvalidTag))
		{
			return MakeOutcome<Paradigm, UBelt*>(EApparatusStatus::InvalidArgument, nullptr);
		}
		const auto BeltPtr = BeltByTag.Find(Tag);
		if (LIKELY(BeltPtr && (BeltPtr->IsValid())))
		{
			return MakeOutcome<Paradigm, UBelt*>(EApparatusStatus::Noop, BeltPtr->Get());
		}
		const FString Name = FString::Format(TEXT("TaggedBelt_{0}"), {Tag});
		const auto Belt = NewObject<UBelt>(this, FName(Name));
		Belt->Tag = Tag;
		Belts.Add(Belt); // Register within itself.
		for (auto& Pair : BeltsByFilterCache)
		{
			if (Belt->Matches(Pair.Key))
			{
				Pair.Value.Add(Belt);
			}
		}
		return BeltByTag.Add(Tag, Belt).Get();
	}

#pragma endregion Belts

#pragma region Iterables

	template < typename IterableT >
	FORCEINLINE TArray<IterableT*>&
	GetIterablesRef();

	template <>
	FORCEINLINE TArray<UChunk*>&
	GetIterablesRef<UChunk>()
	{
		return Chunks;
	}

	template <>
	FORCEINLINE TArray<UBelt*>&
	GetIterablesRef<UBelt>()
	{
		return Belts;
	}

	template < typename IterableT >
	FORCEINLINE TMap<FFilter, TArray<TWeakObjectPtr<IterableT>>>&
	GetIterablesByFilterCacheRef();

	template <>
	FORCEINLINE TMap<FFilter, TArray<TWeakObjectPtr<UChunk>>>&
	GetIterablesByFilterCacheRef<UChunk>()
	{
		return ChunksByFilterCache;
	}

	template <>
	FORCEINLINE TMap<FFilter, TArray<TWeakObjectPtr<UBelt>>>&
	GetIterablesByFilterCacheRef<UBelt>()
	{
		return BeltsByFilterCache;
	}

	/**
	 * Find all of the iterables matching a specific filter.
	 *
	 * @note Empty iterables also get included in the results.
	 */
	template < typename IterableT,
			   typename FilterT = FFilter, typename TAllocator = FDefaultAllocator,
			   TFilterTypeSecurity<FilterT> = true >
	EApparatusStatus
	FindMatchingIterables(const FilterT&                  Filter,
						  TArray<IterableT*, TAllocator>& OutIterables)
	{
		SCOPE_CYCLE_COUNTER(STAT_MechanismFindMatchingIterables);
		auto& IterablesByFilter = GetIterablesByFilterCacheRef<IterableT>();
		auto* MatchingIterables = IterablesByFilter.Find(Filter);
		if (UNLIKELY(MatchingIterables == nullptr))
		{
			// No cache was found. Match and cache all of the iterables...
			MatchingIterables = &IterablesByFilter.Add(Filter);
			check(MatchingIterables);
			for (auto* Iterable : GetIterablesRef<IterableT>())
			{
				check(Iterable);
				if (Iterable->Matches(Filter))
				{
					MatchingIterables->Add(Iterable);
				}
			}
		}

		CopyFromWeakArray(OutIterables, *MatchingIterables);
		return OutIterables.Num() > 0 ? EApparatusStatus::Success : EApparatusStatus::NoItems;
	}

#pragma endregion Iterables

#pragma region Chains
	/// @name Chains
	/// @{

	/**
	 * Get the chain container reference.
	 * 
	 * @tparam ChainT The type of the chains to get.
	 * @return A reference for the chain container.
	 */
	template < typename ChainT >
	FORCEINLINE TArray<TSharedRef<ChainT>>&
	GetChainsRef() const
	{
		checkNoEntry(); // Sophisticated chains should be custom-allocated in ObtainChain()
		return *(new TArray<TSharedRef<ChainT>>());
	}

	/**
	 * Get the chain container reference.
	 * 
	 * @return A reference for the ordinary chain container.
	 */
	template <>
	FORCEINLINE TArray<TSharedRef<FChain>>&
	GetChainsRef<FChain>() const
	{
		return Chains;
	}

	/**
	 * Get the chain container reference.
	 * 
	 * @return A reference for the solid chain container.
	 */
	template <>
	FORCEINLINE TArray<TSharedRef<FSolidChain>>&
	GetChainsRef<FSolidChain>() const
	{
		return SolidChains;
	}

	/// @}
#pragma endregion Chains

#pragma region Networking
	/// @name Networking
	/// @{

	/**
	 * Get the subject information by the network identifier.
	 * 
	 * @note The returned information is not checked and
	 * may actually be invalid.
	 * 
	 * @param NetworkId The unique network identifier of the subject.
	 * @return Subject information structure.
	 */
	FSubjectInfo&
	GetSubjectInfoByNetworkId(const uint32 NetworkId);

	/**
	 * Find a subject information by its network identifier.
	 * 
	 * @param NetworkId The unique network identifier of a subject to find.
	 * @return Subject information structure or @c nullptr,
	 * if was not found.
	 */
	FSubjectInfo*
	FindSubjectInfoByNetworkId(const uint32 NetworkId);

	/// @}
#pragma endregion Networking

	/**
	 * Find a chunk by its unique identifier.
	 * 
	 * @param Id The identifier of the chunk to find.
	 * @return The found chunk or @c nullptr if was not found.
	 */
	FORCEINLINE UChunk*
	FindChunk(const int32 Id)
	{
		const int32 Index = Id - UChunk::FirstId;
		if (Index < 0) return nullptr;
		if (Index >= Chunks.Num()) return nullptr;
		return Chunks[Index];
	}

  public:

#pragma region Inspection
	/// @name Inspection
	/// @{

	/**
	 * Get the current number of subjects residing
	 * within the mechanism.
	 * 
	 * @return The current number of subjects in the mechanism.
	 */
	SubjectIdType
	SubjectsNum() const;

	/**
	 * Get the current number of effective subjectives
	 * in the mechanism.
	 * 
	 * @return The number of subjectives in the mechanism.
	 */
	FORCEINLINE int32
	SubjectivesNum() const
	{
		return Subjectives.Num();
	}

	/// @}
#pragma endregion Inspection

#pragma region Concurrency
	/// @name Concurrency
	/// @{

	/**
	 * Check if the mechanism is currently
	 * inside a multi-threaded context.
	 *
	 * The value returned by this method is guaranteed
	 * not to change for the caller's environment,
	 * as long as the caller is part of the normal
	 * built-in iterating/operating flow.
	 *
	 * Certain actions are performed differently
	 * when working among several threads.
	 *
	 * @return true The mechanism is currently
	 * in the concurrent-executed environment.
	 * @return false The single-threaded environment
	 * is taking place.
	 */
	FORCEINLINE bool
	IsInConcurrentEnvironment() const
	{
		return ConcurrencyLevel.load(std::memory_order_relaxed) > 0;
	}

	/// @}
#pragma endregion Concurrency

	/**
	 * All of the subjectives currently available and registered.
	 */
	FORCEINLINE const TSet<ISubjective*>&
	GetSubjectives() const
	{
		return Subjectives;
	}

	/**
	 * Get subject information by an identifier.
	 */
	FORCEINLINE FAdjectiveInfo&
	GetAdjective(const int32 Id)
	{
		return Adjectives[Id - FAdjectiveInfo::FirstId];
	}

	/**
	 * Get all of the belts matching a fingerprint.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Apparatus|Mechanism")
	void
	FindBeltsMatchingFingerprint(const FFingerprint& Fingerprint,
								 TArray<UBelt*>&     OutBelts)
	{
		EnsureOK(FindMatchingIterables<UBelt>(FFilter(Fingerprint), OutBelts));
	}

	/**
	 * Boot all of the halted subjectives currently present
	 * in the Mechanism.
	 */
	UFUNCTION(BlueprintCallable, Category = "Apparatus|Mechanism")
	void
	BootAll();

	/**
	 * Enchain multiple iterables with a filter.
	 * 
	 * @tparam IterableT The type of iterable to enchain.
	 * @tparam ChainT The type of chain to enchain with.
	 * @tparam FilterT The type of the filter to enchain under.
	 * @param Filter The filter to enchain with.
	 * @param OutChain The chain to enchain within.
	 * @return The resulting chain.
	 */
	template < typename IterableT,
			   typename ChainT = FChain, typename FilterT = FFilter,
			   TFilterTypeSecurity<std::decay_t<FilterT>> = true >
	auto
	EnchainIterables(FilterT&& Filter, ChainT& OutChain)
	{
		if (LIKELY(Filter.Includes(FM_Booted)))
		{
			// Trigger the booting when enchaining for booted entities:
			BootAll();
		}
		TArray<IterableT*, TInlineAllocator<FoundIterablesInlineSize>> FoundIterables;
		FindMatchingIterables(Filter, FoundIterables);
		return OutChain.Enchain(std::forward<FilterT>(Filter), FoundIterables);
	}

	/**
	 * Enchain multiple iterables with a filter.
	 * 
	 * @tparam IterableT The type of iterable to enchain.
	 * @tparam ChainT The type of chain to enchain with.
	 * @tparam FilterT The type of the filter to enchain under.
	 * @param Filter The filter to enchain with.
	 * @return The resulting chain.
	 */
	template < typename IterableT,
			   typename ChainT = FChain, typename FilterT = FFilter,
			   TFilterTypeSecurity<std::decay_t<FilterT>> = true >
	FORCEINLINE TSharedRef<ChainT>
	EnchainIterables(FilterT&& Filter)
	{
		auto Chain = ObtainChain<ChainT>();
		EnchainIterables<IterableT>(std::forward<FilterT>(Filter), *Chain);
		return Chain;
	}

	/**
	 * Enchain multiple iterable sequences based
	 * on a filter.
	 * 
	 * @tparam ChainT The type of the chain to enchain within.
	 * @tparam FilterT The type of the filter to enchain under.
	 * @param[in] Filter The filter to enchain under.
	 * @param[out] OutChain The chain to enchain within.
	 * @return The outcome of the operation.
	 */
	template < typename ChainT = FChain, typename FilterT = FFilter,
			   TFilterTypeSecurity<std::decay_t<FilterT>> = true >
	FORCEINLINE auto
	Enchain(FilterT&& Filter, ChainT& OutChain)
	{
		if (Filter.IsBeltBased())
		{
			// The filter has a details specification.
			// The belt enchaining must be used, since
			// the iterating will be lossy otherwise...
			return EnchainIterables<UBelt, ChainT>(std::template forward<FilterT>(Filter), OutChain);
		}
		return EnchainIterables<UChunk, ChainT>(std::template forward<FilterT>(Filter), OutChain);
	}

	/**
	 * Enchain multiple iterable sequences based
	 * on a filter.
	 * 
	 * @tparam ChainT The type of the chain to enchain within.
	 * @tparam FilterT The type of the filter to enchain under.
	 * @param Filter The filter to enchain under.
	 * @return The resulting chain.
	 */
	template < typename ChainT = FChain, typename FilterT = FFilter,
			   TFilterTypeSecurity<std::decay_t<FilterT>> = true >
	FORCEINLINE TSharedRef<ChainT>
	Enchain(FilterT&& Filter)
	{
		if (Filter.IsBeltBased())
		{
			// The filter has a details specification.
			// The belt enchaining must be used, since
			// the iterating will be lossy otherwise...
			return EnchainIterables<UBelt, ChainT>(std::template forward<FilterT>(Filter));
		}
		return EnchainIterables<UChunk, ChainT>(std::template forward<FilterT>(Filter));
	}

	/**
	 * Enchain the matching chunks into proxies.
	 *
	 * @tparam FilterT The type of filter to enchain with.
	 * @tparam Ts The types of traits to proxy.
	 * @param Filter The filter to enchain under.
	 * @param OutChunkProxies The chunk proxies array to output to.
	 * Those will be the locking ones.
	 * @param bIncludeEmpty Should empty chunks be included?
	 */
	template < typename FilterT = FFilter, typename... Ts,
			   TFilterTypeSecurity<std::decay_t<FilterT>> = true >
	FORCEINLINE void
	Enchain(FilterT&&                   Filter,
			TArray<TChunkProxy<Ts...>>& OutChunkProxies,
			const bool                  bIncludeEmpty = false)
	{
		check(!Filter.IsBeltBased());
		TArray<UChunk*, TInlineAllocator<FoundIterablesInlineSize>> FoundIterables;
		FindMatchingIterables(std::template forward<FilterT>(Filter), FoundIterables);
		OutChunkProxies.Reset();
		for (int32 i = 0; i < FoundIterables.Num(); ++i)
		{
			const auto Chunk = FoundIterables[i];
			check(Chunk);
			if (LIKELY(bIncludeEmpty || (Chunk->Num() > 0)))
			{
				OutChunkProxies.Add(TChunkProxy<Ts...>(FoundIterables[i], /*bShouldLock=*/true));
			}
		}
	}

	/**
	 * Enchain multiple iterable sequences based
	 * on a filter, returning a solid chain.
	 * 
	 * @param Filter The filter to enchain under.
	 * @return The resulting chain.
	 */
	template < typename FilterT = FFilter >
	FORCEINLINE TSharedRef<FSolidChain>
	EnchainSolid(FilterT&& Filter)
	{
		return Enchain<FSolidChain>(std::template forward<FilterT>(Filter));
	}

	/**
	 * Enchain subjects using a specific solidity.
	 * 
	 * @tparam bInSolidity The solidity to enchain in.
	 * @tparam FilterT The type of the filter to enchain by.
	 * @return The chain matching the solidity.
	 */
	template < bool bInSolidity, typename FilterT = FFilter >
	FORCEINLINE typename std::conditional<bInSolidity, TSharedRef<FSolidChain>, TSharedRef<FChain>>::type
	EnchainWithSolidity(FilterT&& Filter)
	{
		return Enchain<typename std::conditional<bInSolidity, FSolidChain, FChain>::type>(std::template forward<FilterT>(Filter));
	}

	/**
	 * Update a subjective's belt.
	 * 
	 * Should update the belt binding
	 * according to the current subjective's
	 * details. May actually assign a belt,
	 * if it's not yet set.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Subjective The subjective to
	 * actualize a belt for.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	ActualizeBelt(ISubjective* const Subjective)
	{
		AssessCondition(Paradigm, Subjective != nullptr, EApparatusStatus::NullArgument);
		auto Belt = Subjective->GetBelt();
		if (LIKELY(Belt))
		{
			// The belt was already assigned.
			// Expand the current belt as (if) necessary:
			AssessOK(Paradigm, Belt->template Expand<Paradigm>(Subjective->GetFingerprint()));
		}
		else
		{
			// There is no belt currently assigned to the subjective.
			// Obtain a new belt to assign...
			const auto NewBelt = ObtainMostSpecificBelt<Paradigm>(Subjective);
			AssessOK(Paradigm, NewBelt);
			Belt = NewBelt;
		}

		// Refresh the subjective within its (possibly new) belt:
		return Belt->template Refresh<Paradigm>(Subjective);
	}

	/**
	 * Register a subjective within the mechanism.
	 * 
	 * The method can be used to re-register a subjective
	 * within a different mechanism.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Subjective The subjective to register within the mechanism.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop If the subjective is already registered.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	RegisterSubjective(ISubjective* const Subjective);

	/**
	 * Remove a subjective from the mechanism completely.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param Subjective The subjective to unregister.
	 * @return The outcome of the operation.
 	 * @return EApparatusStatus::Noop If the subjective is already unregistered.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	UnregisterSubjective(ISubjective* const Subjective);

	/**
	 * Unregister all of the currently registered subjectives.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	UnregisterAllSubjectives();

	/**
	 * Obtain an embedded chain of a specific identifier.
	 * 
	 * This method is mainly used by the Blueprint frontend to
	 * provide the chain consistently during the iterating.
	 * 
	 * @param ChainId The identifier of the chain to obtain.
	 * @return The obtained chain reference.
	 */
	template < typename ChainT = FChain >
	TSharedRef<ChainT>
	ObtainChain(const int32 ChainId) const
	{
		check(ChainId != InvalidChainId);
		auto& ChainsRef = GetChainsRef<ChainT>();
		while (ChainId >= ChainsRef.Num() + FirstChainId)
		{
			auto& NewChain = ChainsRef.Add_GetRef(MakeShared<ChainT>(const_cast<AMechanism*>(this)));
			NewChain->Id = (ChainsRef.Num() - 1) + FirstChainId;
		}
		return ChainsRef[ChainId];
	}

	/**
	 * Obtain a chain of a specific type.
	 * 
	 * The chain can probably be reused among
	 * the embedded ones.
	 * 
	 * @tparam ChainT The type of the chain to obtain.
	 * @return A reference to the chain.
	 */
	template < typename ChainT = FChain >
	TSharedRef<ChainT>
	ObtainChain() const
	{
		if ((!std::is_same<ChainT, FChain>::value && !std::is_same<ChainT, FSolidChain>::value) ||
			IsInConcurrentEnvironment())
		{
			return MakeShared<ChainT>(const_cast<AMechanism*>(this));
		}
		else
		{
			auto& ChainsRef = GetChainsRef<ChainT>();
			for (auto& Chain : ChainsRef)
			{
				if (LIKELY(Chain.IsUnique() && Chain->CanBeReused()))
				{
					return Chain;
				}
			}
			return ObtainChain<ChainT>(ChainsRef.Num() + FirstChainId);
		}
	}

	/**
	 * @brief Register a mechanical within the mechanism.
	 * 
	 * @param Mechanical A mechanical to register.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	RegisterMechanical(TScriptInterface<IMechanical> Mechanical);

	/**
	 * @brief Unregister a mechanical from the mechanism.
	 * 
	 * @param Mechanical A mechanical to unregister.
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	UnregisterMechanical(TScriptInterface<IMechanical> Mechanical)
	{
		check(Mechanical);
		return Mechanicals.Remove(Mechanical.GetObject()) ? EApparatusStatus::Success : EApparatusStatus::Noop;
	}

	/**
	 * Unregister all of the currently registered mechanicals.
	 */
	EApparatusStatus
	UnregisterAllMechanicals()
	{
		EApparatusStatus Status = EApparatusStatus::Noop;
		while (Mechanicals.Num() > 0)
		{
			auto& m = Mechanicals.Last();
			if (!m.IsValid()) continue;
			StatusAccumulate(Status, UnregisterMechanical(m.ToScriptInterface()));
		}
		Mechanicals.Reset(); // Invalid weak pointers may left some elements here.
		return Status;
	}

	/**
	 * Find existing or create a new chunk for a traitmark.
	 * 
	 * @param Traitmark A traitmark to ensure for.
	 * @param OutStatus The status of the operation.
	 * @param Capacity The desired capacity of the chunk.
	 * @return The resulting chunk.
	 */
	[[deprecated("The method is deprecated. Use 'ObtainChunk' with a polite paradigm, please.")]]
	UChunk*
	ObtainChunk(const struct FTraitmark& Traitmark,
				EApparatusStatus&        OutStatus,
				const int32              Capacity = 32)
	{
		auto Outcome = ObtainChunk<EParadigm::Polite>(Traitmark, Capacity);
		OutStatus = Outcome;
		return Outcome;
	}

	/**
	 * Find existing or create a new chunk for a traitmark.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param Traitmark A traitmark to ensure for.
	 * @param Capacity The desired capacity of the chunk.
	 * @return The resulting chunk.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm, UChunk*>
	ObtainChunk(const struct FTraitmark& Traitmark,
				const int32              Capacity = 32)
	{
		// Find among existing chunks:
		auto* CachedChunk = ChunksByTraitmarksCache.Find(Traitmark);
		if (LIKELY(CachedChunk))
		{
			if (UNLIKELY(!CachedChunk->IsValid()))
			{
				// Stale cache:
				ChunksByTraitmarksCache.Remove(Traitmark);
			}
			else
			{
				(*CachedChunk)->Reserve(Capacity);
				return MakeOutcome<Paradigm, UChunk*>(EApparatusStatus::Noop, CachedChunk->Get());
			}
		}

		const int32 OldCount = Chunks.Num();
		if (AvoidFormat(Paradigm, OldCount == TNumericLimits<int32>::Max(),
						TEXT("The maximum number of chunks was already created: %i"), (int)OldCount))
		{
			return MakeOutcome<Paradigm, UChunk*>(EApparatusStatus::OutOfLimit, nullptr);
		}

		// Create a new one and register it...
		const int32 Id = Chunks.Num() + UChunk::FirstId;
		const auto Chunk = UChunk::New(this, Id, Traitmark);
		if (Avoid(Paradigm, Chunk == nullptr))
		{
			return MakeOutcome<Paradigm, UChunk*>(EApparatusStatus::NoMemory, nullptr);
		}
		Chunks.Add(Chunk);

		// Assign compatible adjectives from the mechanism...
		for (FAdjectiveInfo& ai : AMechanism::Adjectives)
		{
			if (Traitmark.Matches(ai.Filter))
			{
				check(ai.Id != FAdjectiveInfo::InvalidId);
				const auto AdjectiveStatus = Chunk->AddAdjective(&ai);
				if (AvoidErrorFormat(Paradigm, AdjectiveStatus, TEXT("Failed to add adjective #%d to a chunk."), (int)ai.Id))
				{
					Chunks.Remove(Chunk);
					return MakeOutcome<Paradigm, UChunk*>(FApparatusStatus::GetLastError(), nullptr);
				}
			}
		}

		// Assign to compatible mechanics from the mechanism...
		for (FMechanicInfo& mi : AMechanism::Mechanics)
		{
			if (Traitmark.Matches(mi.Filter))
			{
				check(mi.Id != FMechanicInfo::InvalidId);
				const auto MechanicStatus = mi.AddChunk(Chunk);
				if (AvoidErrorFormat(Paradigm, MechanicStatus, TEXT("Failed to add a chunk to a #%d mechanic."), (int)mi.Id))
				{
					Chunks.Remove(Chunk);
					return MakeOutcome<Paradigm, UChunk*>(FApparatusStatus::GetLastError(), nullptr);
				}
			}
		}
		
		// Add the chunk to the caches...
		ChunksByTraitmarksCache.Add(Traitmark) = Chunk;
		for (auto& Pair : ChunksByFilterCache)
		{
			if (Chunk->Matches(Pair.Key))
			{
				Pair.Value.Add(Chunk);
			}
		}

		return Chunk;
	}

  private:

	/**
	 * Spawn a new subject within the mechanism and a chunk of a specific traitmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Traitmark The traitmark for the chunk to spawn within.
	 * @param Flagmark The initial flagmark of the subject to spawn with.
	 * @return The initialized subject handle for the new subject.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, FSubjectHandle>
	DoSpawnSubject(const FTraitmark& Traitmark,
				   EFlagmark         Flagmark = FM_None);

	/**
	 * Spawn a new subject based on a packed subject data record.
	 * Internal deferred version.
	 * 
	 * The actual entity creation is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectRecord The subject record to initialize a new subject with.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	DoSpawnSubjectDeferred(const FSubjectRecord& SubjectRecord);

  public:

#pragma region Spawning
	/// @name Spawning
	/// @{

	/**
	 * Spawn a new subject within the mechanism and a chunk of a specific traitmark.
	 * 
	 * The chunk's slot will be initialized to a default trait state.
	 * 
	 * @param[out] OutSubjectHandle A subject handle to initialize with the new entity. 
	 * @param[in] Traitmark The traitmark for the chunk to spawn within.
	 * @param[in] Flagmark The initial flagmark of the subject to spawn with.
	 * May not contain any of the system-level flags.
	 * @return The status of the operation.
	 * 
	 * @deprecated since 1.23. Use SpawnSubject() within a polite paradigm, please.
	 */
	[[deprecated("This method is deprecated. Use SpawnSubject() within a polite paradigm, please.")]]
	EApparatusStatus
	SpawnSubject(FSubjectHandle&   OutSubjectHandle,
				 const FTraitmark& Traitmark,
				 const EFlagmark   Flagmark = FM_None)
	{
		const auto Outcome = DoSpawnSubject<EParadigm::Polite>(Traitmark, Flagmark);
		OutSubjectHandle = Outcome.Payload;
		return Outcome;
	}

	/**
	 * Spawn a new subject within the mechanism and a chunk of a specific traitmark.
	 * R-value version.
	 * 
	 * @param[out] OutSubjectHandle A subject handle to initialize with the new entity. 
	 * @param[in] Traitmark The traitmark for the chunk to spawn within.
	 * @param[in] Flagmark The initial flagmark of the subject to spawn with.
	 * @return The status of the operation.
	 *
	 * @deprecated since 1.23. Use SpawnSubject() within a polite paradigm, please.
	 */
	[[deprecated("This method is deprecated. Use SpawnSubject() within a polite paradigm, please.")]]
	FORCEINLINE EApparatusStatus
	SpawnSubject(FSubjectHandle& OutSubjectHandle,
				 FTraitmark&&    Traitmark,
				 const EFlagmark Flagmark = FM_None)
	{
		const auto Outcome = DoSpawnSubject<EParadigm::Polite>(Traitmark, Flagmark);
		OutSubjectHandle = Outcome.Payload;
		return Outcome;
	}

	/**
	 * Spawn a new subject within the mechanism.
	 * 
	 * @param[out] OutSubjectHandle A subject handle to initialize with the new entity. 
	 * @param[in] Flagmark The initial flagmark of the subject to spawn with.
	 * @return The status of the operation.
	 *
	 * @deprecated since 1.23. Use SpawnSubject() within a polite paradigm, please.
	 */
	[[deprecated("This method is deprecated. Use SpawnSubject() within a polite paradigm, please.")]]
	FORCEINLINE EApparatusStatus
	SpawnSubject(FSubjectHandle& OutSubjectHandle,
				 const EFlagmark Flagmark = FM_None)
	{
		const auto Outcome = DoSpawnSubject<EParadigm::Polite>(FTraitmark::Zero, Flagmark);
		OutSubjectHandle = Outcome.Payload;
		return Outcome;
	}

	/**
	 * Spawn a new subject based on a packed subject data record.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectRecord The subject record to initialize a new subject with.
	 * @return The handle for the newly created subject.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm, FSubjectHandle>
	SpawnSubject(const FSubjectRecord& SubjectRecord);

	/**
	 * Spawn a new subject based on a packed subject data record.
	 * RValue reference compatibility version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectRecord The subject record to initialize a new subject with.
	 * @return The handle for the newly created subject.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, FSubjectHandle>
	SpawnSubject(FSubjectRecord&& SubjectRecord)
	{
		return SpawnSubject((const FSubjectRecord&)SubjectRecord);
	}

	/**
	 * Spawn a new subject based on a packed subject data record.
	 * Non-constant refernce compatibility version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectRecord The subject record to initialize a new subject with.
	 * @return The handle for the newly created subject.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, FSubjectHandle>
	SpawnSubject(FSubjectRecord& SubjectRecord)
	{
		return SpawnSubject((const FSubjectRecord&)SubjectRecord);
	}

	/**
	 * Spawn a new subject within the mechanism based on a record.
	 * Status version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectRecord The subject record to initialize a new subject with.
	 * @param OutStatus The status of the operation.
	 * @return The newly spawned subject handle.
	 * 
	 * @deprecated since 1.23. Use SpawnSubject() within a polite paradigm, please.
	 */
	[[deprecated("This method is deprecated. Use SpawnSubject() within a polite paradigm, please.")]]
	FORCEINLINE FSubjectHandle
	SpawnSubject(const FSubjectRecord& SubjectRecord,
				 EApparatusStatus&     OutStatus)
	{
		const auto Outcome = SpawnSubject<EParadigm::SafePolite>(SubjectRecord);
		OutStatus = Outcome.Status;
		return Outcome.Payload;
	}
	
	/**
	 * Spawn a new subject based on a packed subject data record.
	 * RValue reference compatibility version with a status.
	 * 
	 * @param SubjectRecord The subject record to initialize a new subject with.
	 * @param OutStatus The status of the operation.
	 * @return The handle for the newly created subject.
	 * 
	 * @deprecated since 1.23. Use SpawnSubject() within a polite paradigm, please.
	 */
	[[deprecated("This method is deprecated. Use SpawnSubject() within a polite paradigm, please.")]]
	FORCEINLINE FSubjectHandle
	SpawnSubject(FSubjectRecord&&  SubjectRecord,
				 EApparatusStatus& OutStatus)
	{
		const auto Outcome = SpawnSubject<EParadigm::SafePolite>(SubjectRecord);
		OutStatus = Outcome.Status;
		return Outcome.Payload;
	}

	/**
	 * Spawn a new subject based on a packed subject data record.
	 * Non-constant compatibility version with a status.
	 * 
	 * @param SubjectRecord The subject record to initialize a new subject with.
	 * @param OutStatus The status of the operation.
	 * @return The handle for the newly created subject.
	 * 
	 * @deprecated since 1.23. Use SpawnSubject() within a polite paradigm, please.
	 */
	[[deprecated("This method is deprecated. Use SpawnSubject() within a polite paradigm, please.")]]
	FORCEINLINE FSubjectHandle
	SpawnSubject(FSubjectRecord&   SubjectRecord,
				 EApparatusStatus& OutStatus)
	{
		const auto Outcome = SpawnSubject<EParadigm::SafePolite>((const FSubjectRecord&)SubjectRecord);
		OutStatus = Outcome.Status;
		return Outcome.Payload;
	}

	/**
	 * Spawn a new subject based on a packed subject data record.
	 * Deferred version.
	 * 
	 * The actual entity creation is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectRecord The subject record to initialize a new subject with.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	SpawnSubjectDeferred(const FSubjectRecord& SubjectRecord)
	{
		return DoSpawnSubjectDeferred<Paradigm>(SubjectRecord);
	}

	/**
	 * Spawn a new subject based on a packed subject data record.
	 * Deferred version for RValue compatibility.
	 * 
	 * The actual entity creation is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectRecord The subject record to initialize a new subject with.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	SpawnSubjectDeferred(FSubjectRecord&& SubjectRecord)
	{
		return DoSpawnSubjectDeferred((const FSubjectRecord&)SubjectRecord);
	}

	/**
	 * Spawn a new subject based on a packed subject data record.
	 * Deferred version for non-constant compatibility.
	 * 
	 * The actual entity creation is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectRecord The subject record to initialize a new subject with.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	SpawnSubjectDeferred(FSubjectRecord& SubjectRecord)
	{
		return DoSpawnSubjectDeferred((const FSubjectRecord&)SubjectRecord);
	}

	/**
	 * Spawn a new subject with a flagmark within the mechanism,
	 * returning the handle to the caller.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flagmark The initial flagmark of the subject to spawn with.
	 * @return A subject handle to initialize with the new entity.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	SpawnSubject(const EFlagmark Flagmark)
	{
		return DoSpawnSubject<Paradigm>(FTraitmark::Zero, Flagmark);
	}

	/**
	 * Spawn a new subject with traits,
	 * returning the handle.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Ts The types of traits to spawn with.
	 * @param Traits The traits to spawn with.
	 * @return A handle for the newly spawned subject.
	 */
	template < EParadigm Paradigm, typename... Ts >
	FORCEINLINE TOutcome<Paradigm, FSubjectHandle>
	SpawnSubject(Ts&&... Traits)
	{
		const auto Outcome = DoSpawnSubject<Paradigm>(FTraitmark::template Make<Ts...>());
		AssessConditionOK(Paradigm, Outcome);
		const auto TraitsOutcome = OutcomeCombine(Outcome->template SetTrait<Paradigm>(std::forward<Ts>(Traits))...);
		if (!OK(TraitsOutcome))
		{
			Outcome->Despawn();
		}
		return OutcomeCombine(TraitsOutcome, Outcome);
	}

	/**
	 * Spawn a new subject with traits,
	 * returning the handle.
	 * 
	 * @tparam Ts The types of traits to spawn with.
	 * @param Traits The traits to spawn with.
	 * @param Flagmark The initial flagmark of the subject to spawn with.
	 * @return A handle for the newly spawned subject.
	 */
	template < EParadigm Paradigm, typename... Ts >
	FORCEINLINE TOutcome<Paradigm, FSubjectHandle>
	SpawnSubject(Ts&&... Traits, const EFlagmark Flagmark)
	{
		const auto Outcome = DoSpawnSubject<Paradigm>(FTraitmark::template Make<Ts...>(), Flagmark);
		AssessConditionOK(Paradigm, Outcome);
		const auto TraitsOutcome = OutcomeCombine(Outcome->template SetTrait<Paradigm>(std::forward<Ts>(Traits))...);
		if (!OK(TraitsOutcome))
		{
			Outcome->Despawn();
		}
		return OutcomeCombine(TraitsOutcome, Outcome);
	}

	/**
	 * Spawn a new subject with traits, returning the handle.
	 * Default paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Ts The types of traits to spawn with.
	 * @param Traits The traits to spawn with.
	 * @return A handle for the newly spawned subject.
	 */
	template < typename... Ts, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	SpawnSubject(Ts&&... Traits)
	{
		return SpawnSubject<Paradigm>(Traits...);
	}

	/**
	 * Spawn a new subject with traits, returning the handle.
	 * Default paradigm version.
	 * 
	 * @tparam Ts The types of traits to spawn with.
	 * @param Traits The traits to spawn with.
	 * @param Flagmark The initial flagmark of the subject to spawn with.
	 * @return A handle for the newly spawned subject.
	 */
	template < typename... Ts, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	SpawnSubject(Ts&&... Traits, const EFlagmark Flagmark)
	{
		return SpawnSubject<Paradigm>(Traits..., Flagmark);
	}

	/**
	 * Spawn a new subject with a typed trait data,
	 * returning the handle.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to spawn with.
	 * Must not be a @c nullptr.
	 * @param TraitData The data fo the trait to spawn with.
	 * Must not be a @c nullptr.
	 * @param Flagmark The initial flagmark of the subject to spawn with.
	 * @return A subject handle to initialize with the new entity.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, FSubjectHandle>
	SpawnSubject(UScriptStruct* const TraitType,
				 void* const          TraitData,
				 const EFlagmark      Flagmark = FM_None)
	{
		auto Outcome = DoSpawnSubject<Paradigm>(FTraitmark(TraitType), Flagmark);
		AssessConditionOK(Paradigm, Outcome);
		const auto Handle = GetPayload(Outcome);
		const auto TraitOutcome = Handle.template SetTrait<Paradigm>(TraitType, TraitData);
		if (!OK(TraitOutcome))
		{
			Handle.Despawn();
			return TraitOutcome;
		}
		return Outcome + TraitOutcome;
	}

	/// @}
#pragma endregion Spawning

#pragma region Deferred Spawning
	/// @name Deferred Spawning
	/// @{

	/**
	 * Spawn a new subject with traits.
	 * Deferred version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Ts The types of traits to spawn with.
	 * @param Traits The traits to spawn with.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename... Ts >
	FORCEINLINE TOutcome<Paradigm>
	SpawnSubjectDeferred(Ts&&... Traits)
	{
		FSubjectRecord SubjectRecord(FTraitmark::template Make<Ts...>());
		const auto TraitsStatus = StatusCombine(SubjectRecord.SetTrait(std::forward<Ts>(Traits))...);
		AssessCondition(Paradigm, OK(TraitsStatus), TraitsStatus);
		return DoSpawnSubjectDeferred(SubjectRecord);
	}

	/**
	 * Spawn a new subject with traits and a flagmark.
	 * Deferred version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Ts The types of traits to spawn with.
	 * @param Traits The traits to spawn with.
	 * @param Flagmark The initial flagmark of the subject to spawn with.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename... Ts >
	FORCEINLINE TOutcome<Paradigm>
	SpawnSubjectDeferred(Ts&&... Traits, const EFlagmark Flagmark)
	{
		FSubjectRecord SubjectRecord(FTraitmark::template Make<Ts...>(), Flagmark);
		const auto TraitsStatus = StatusCombine(SubjectRecord.SetTrait(std::forward<Ts>(Traits))...);
		AssessCondition(Paradigm, OK(TraitsStatus), TraitsStatus);
		return DoSpawnSubjectDeferred(SubjectRecord);
	}

	/**
	 * Spawn a new subject with traits.
	 * Deferred default paradigm version.
	 * 
	 * @tparam Ts The types of traits to spawn with.
	 * @tparam Paradigm The paradigm to work under.
	 * @param Traits The traits to spawn with.
	 * @return The outcome of the operation.
	 */
	template < typename... Ts, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	SpawnSubjectDeferred(Ts&&... Traits)
	{
		return SpawnSubjectDeferred<Paradigm>(std::forward<Ts>(Traits)...);
	}

	/**
	 * Spawn a new subject with traits and a flagmark.
	 * Deferred default paradigm version.
	 * 
	 * @tparam Ts The types of traits to spawn with.
	 * @tparam Paradigm The paradigm to work under.
	 * @param Traits The traits to spawn with.
	 * @param Flagmark The initial flagmark of the subject to spawn with.
	 * @return The outcome of the operation.
	 */
	template < typename... Ts, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	SpawnSubjectDeferred(Ts&&... Traits, const EFlagmark Flagmark)
	{
		return SpawnSubjectDeferred<Paradigm>(std::forward<Ts>(Traits)..., Flagmark);
	}

	/// @}
#pragma endregion Deferred Spawning

	/**
	 * Despawn all of the subjects currently available
	 * within the mechanism.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	DespawnAllSubjects();

	/**
	 * Reset the mechanism completely,
	 * unregistering all of the entities
	 * and despawning all of the subjects.
	 */
	void
	Reset() override;

	/**
	 * Check if the mechanism is currently in the uniform mode preventing
	 * any structural changes to its subjects and subjectives.
	 * 
	 * The mechanism is solid if it has any solid-locked
	 * iterables within itself.
	 * 
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	IsSolid() const
	{
		return LocksCount.load(std::memory_order_relaxed) < 0;
	}

	/**
	 * Check if the mechanism is currently in the mutable mode allowing
	 * any structural changes to its subjects and subjectives.
	 */
	FORCEINLINE bool
	IsLiquid() const
	{
		return LocksCount.load(std::memory_order_relaxed) >= 0;
	}

	/**
	 * Check if the mechanism is currently locked in any manner.
	 * 
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	IsLocked() const
	{
		return LocksCount.load(std::memory_order_relaxed) != 0;
	}

  private:

#pragma region Lifetime

	/**
	 * Finalize the mechanism, freeing
	 * all of its resources and unregistering
	 * it from the machine.
	 * 
	 * @param bWaitForOperatings Should the mechanism
	 * wait for the operatings to complete.
	 */
	void Dispose(const bool bWaitForOperatings = true);

#pragma endregion Lifetime

#pragma region UObject

	/**
	 * Destroy the mechanism object.
	 * 
	 * Makes sure to unregister itself
	 * from the machine.
	 */
	void
	BeginDestroy() override;

#pragma endregion UObject

#pragma region AActor

	/**
	 * Registers the mechanism within the machine.
	 */
	void BeginPlay() override;

	/**
	 * Destroy the mechanism object.
	 * 
	 * This should happen when the mechanism is no
	 * longer needed.
	 */
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma endregion AActor

#pragma region Deferreds
	/// @name Deferreds
	/// @{

  public:

	/**
	 * Check if there are any deferreds queued.
	 * 
	 * This operation is thread-safe and lock-free.
	 */
	FORCEINLINE bool
	HasDeferreds() const
	{
		return !Deferreds.IsEmpty();
	}

	/**
	 * Apply all of the currently pending changes.
	 * 
	 * The deferred application is only possible
	 * within the main thread.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	ApplyDeferreds()
	{
		AssessConditionFormat(
			Paradigm, IsLiquid(), EApparatusStatus::InvalidState,
			TEXT("The '%s' mechanism must be liquid in order to apply its deferreds. "
				 "Is there some concurrent operating still happening? "
				 "You should explicitly wait for their completion then."),
			*GetName());
		{
			FExclusiveScope ExclusiveScope(this); // The deferreds must be applied synchronously.
			bool bAppliedAny = false;
			FDeferred Deferred;
			while (Deferreds.Dequeue(Deferred))
			{
				AssessOK(Paradigm, Apply<Paradigm>(Deferred));
				bAppliedAny = true;
			}
			return bAppliedAny ? EApparatusStatus::Success : EApparatusStatus::Noop;
		}
	}

	/**
	 * Apply a certain kind of the currently pending changes.
	 * 
	 * The deferred application is only possible
	 * within the main thread.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFilter The kind of deferreds to apply.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	ApplyDeferreds(const EDeferred InFilter)
	{
		AssessConditionFormat(
			Paradigm, IsLiquid(), EApparatusStatus::InvalidState,
			TEXT("The '%s' mechanism must be liquid in order to apply its deferreds. "
				 "Is there some concurrent operating still happening? "
				 "You should explicitly wait for their completion then."),
			*GetName());
		{
			FExclusiveScope ExclusiveScope(this); // The deferreds must be applied synchronously.
			if (InFilter == EDeferred::All)
			{
				return ApplyDeferreds<Paradigm>();
			}
			else if (UNLIKELY(InFilter == EDeferred::None))
			{
				return EApparatusStatus::Noop;
			}

			bool bAppliedAny = false;
			auto Status = EApparatusStatus::Noop;
			FDeferred Deferred;
			while (Deferreds.Dequeue(Deferred))
			{
				const auto Kind = (EDeferred)Deferred.GetIndex();
				if (Kind != InFilter) continue;
				AssessOK(Paradigm, Apply<Paradigm>(Deferred));
				bAppliedAny = true;
			}
			return bAppliedAny ? EApparatusStatus::Success : EApparatusStatus::Noop;
		}
	}

	/**
	 * Apply a subset of the currently pending changes.
	 * 
	 * The deferred application is only possible
	 * within the main thread.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InKinds The list of deferred changes to apply.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop if nothing was actually changed.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   typename ContainerT = TArray<EDeferred> >
	TOutcome<Paradigm>
	ApplyDeferreds(const ContainerT& InKinds)
	{
		AssessConditionFormat(
			Paradigm, IsLiquid(), EApparatusStatus::InvalidState,
			TEXT("The '%s' mechanism must be liquid in order to apply its deferreds. "
				 "Is there some concurrent operating still happening? "
				 "You should explicitly wait for their completion then."),
			*GetName());
		{
			FExclusiveScope ExclusiveScope(this); // The deferreds must be applied synchronously.
			if (InKinds.Contains(EDeferred::All))
			{
				return ApplyDeferreds<Paradigm>();
			}

			bool bAppliedAny = false;
			auto Status = EApparatusStatus::Noop;
			FDeferred Deferred;
			while (Deferreds.Dequeue(Deferred))
			{
				const auto Kind = (EDeferred)Deferred.GetIndex();
				if (!InKinds.Contains(Kind)) continue;
				AssessOK(Paradigm, Apply<Paradigm>(Deferred));
				bAppliedAny = true;
			}
			return bAppliedAny ? EApparatusStatus::Success : EApparatusStatus::Noop;
		}
	}

	/**
	 * Check if the automatic deferreds application
	 * is possible within the current context.
	 * 
	 * @return Returns @c true if the deferreds can be automatically be applied now,
	 * @c false otherwise.
	 */
	FORCEINLINE bool
	IsAutomaticDeferredsApplicationPossible() const
	{
		return (DeferredsApplicatorsCount == 0) && IsLiquid();
	}

	/**
	 * Create a new deferreds applicator.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFilter A filter to request an applicator under.
	 * @return The resulting applicator.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TDeferredsApplicator<Paradigm>
	CreateDeferredsApplicator(const EDeferred InFilter = EDeferred::All)
	{
		return TDeferredsApplicator<Paradigm>(this, InFilter);
	}

	/// @}
#pragma endregion Deferreds

#pragma region Collecting
	/// @name Collecting
	/// @{

	/**
	 * Collect all of the subjects being part of the mechanism.
	 * 
	 * @note The @p OutSubjects array is not reset. The subjects
	 * are added up to it.
	 * 
	 * @warning The operation is relatively slow and shouldn't be used
	 * during normal frame time. Enchaining/operating should be used instead.
	 * 
	 * @param OutSubjects The array to store the subjects.
	 * @param Filter The filter used to match the collected subjects.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop If there were no items collected.
	 */
	template < typename AllocatorT >
	EApparatusStatus
	CollectSubjects(TArray<FSubjectHandle, AllocatorT>& OutSubjects,
					const FFilter&                      Filter = FFilter::Default);

	/**
	 * Collect the subjects being part of the mechanism.
	 *
	 * @note The @p OutSubjects collection is not reset. The subjects
	 * are added up to it.
	 * 
	 * @warning The operation is relatively slow and shouldn't be used
	 * during normal frame time. Enchaining/operating should be used instead.
	 * 
	 * @param OutSubjects The collection to store the subjects within.
	 * @param Filter The filter used to match the collected subjects.
	 * @param FlagmarkMask The mask for flags to capture. Only user-level flags are
	 * captured by default.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop If there were no items collected.
	 */
	EApparatusStatus
	CollectSubjects(USubjectRecordCollection* const OutSubjects,
					const FFilter&                  Filter = FFilter::Default,
					const EFlagmark                 FlagmarkMask = FM_AllUserLevel);

	/// @}
#pragma endregion Collecting

#pragma region Operating
	/// @name Operating
	/// @{

	/**
	 * Get the number of current operatings happening within the mechanism.
	 * 
	 * @note This value can actually misrepresent the actual
	 * number if queried after the WaitForOperatingsCompletion(),
	 * cause the newly queued operations may already be altering it.
	 * 
	 * Each chain also has its own local FCommonChain::OperatingsNum().
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
	 * to be completed within all chains.
	 * 
	 * You can use this method after
	 * you queued up the asynchronous
	 * operatings in order to sync them all.
	 * 
	 * Each chain also has its own local FCommonChain::WaitForOperatingsCompletion().
	 */
	FORCEINLINE void
	WaitForOperatingsCompletion() const
	{
		OperatingsCompleted->Wait();
	}

	/**
	 * Wait for all the current operatings
	 * to be completed within all chains.
	 * 
	 * You can use this method after
	 * you queued up the asynchronous
	 * operatings in order to sync them all.
	 * 
	 * Each chain also has its own local FCommonChain::WaitForOperatingsCompletion().
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
	 * to be completed within all chains.
	 * 
	 * You can use this method after
	 * you queued up the asynchronous
	 * operatings in order to sync them all.
	 * 
	 * Each chain also has its own local FCommonChain::WaitForOperatingsCompletion().
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

	/**
	 * Process the mechanism using a functor mechanic.
	 * Supports lambdas. Constant mechanic version.
	 * 
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor argument types by default.
	 * @tparam FilterT The type of filter that takes place.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Filter The filter to query with.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, typename FilterT = void, typename MechanicT = void,
			   TFilterTypeSecurity<std::decay_t<FilterT>> = true,
			   typename std::enable_if<!std::is_function<MechanicT>::value, bool>::type = true >
	FORCEINLINE auto
	Operate(FilterT&& Filter, const MechanicT& Mechanic)
	{
		using MethodInfoT = TMethodInfo<decltype(&MechanicT::operator())>;
		return Enchain<
			typename std::conditional<std::is_void<ChainT>::value,
									  typename std::conditional<MethodInfoT::NeedsSolidChain(),
																FSolidChain, FChain>::type,
									  ChainT>::type>(std::forward<FilterT>(Filter))
				->Operate(Mechanic);
	}

	/**
	 * Process the mechanism using a functor mechanic.
	 * Supports lambdas. Mutable mechanic version.
	 * 
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor argument types by default.
	 * @tparam FilterT The type of filter that takes place.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Filter The filter to query with.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, typename FilterT = void, typename MechanicT = void,
			   TFilterTypeSecurity<std::decay_t<FilterT>> = true,
			   typename std::enable_if<!std::is_function<MechanicT>::value, bool>::type = true >
	FORCEINLINE auto
	Operate(FilterT&& Filter, MechanicT& Mechanic)
	{
		using MethodInfoT = TMethodInfo<decltype(&MechanicT::operator())>;
		return Enchain<
			typename std::conditional<std::is_void<ChainT>::value,
									  typename std::conditional<MethodInfoT::NeedsSolidChain(),
																FSolidChain, FChain>::type,
									  ChainT>::type>(std::forward<FilterT>(Filter))
				->Operate(Mechanic);
	}

	/**
	 * Process the chain using a free function mechanic.
	 *
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on function's argument types by default.
	 * @tparam FilterT The type of filter that takes place.
	 * @tparam FunctionT The type of the mechanic function
	 * to operate on the slots.
	 * @param Filter The filter to query with.
	 * @param Mechanic The mechanical function to operate with.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, typename FilterT = void, typename FunctionT = void,
			   TFilterTypeSecurity<std::decay_t<FilterT>> = true,
			   typename std::enable_if<std::is_function<FunctionT>::value, bool>::type = true >
	FORCEINLINE auto
	Operate(FilterT&& Filter, FunctionT* const Mechanic)
	{
		using FunctionInfoT = TFunctionInfo<FunctionT>;
		return Enchain<
			typename std::conditional<std::is_void<ChainT>::value,
									  typename std::conditional<FunctionInfoT::NeedsSolidChain(),
																FSolidChain, FChain>::type,
									  ChainT>::type>(std::forward<FilterT>(Filter))
				->Operate(Mechanic);
	}

	/**
	 * Process the mechanism using a functor mechanic.
	 * Supports lambdas. Constant mechanic auto-filter version.
	 * 
	 * The filter is automatically derived from the mechanic's arguments.
	 * 
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor argument types by default.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, typename MechanicT = void,
			   typename std::enable_if<!std::is_function<MechanicT>::value, bool>::type = true >
	FORCEINLINE auto
	Operate(const MechanicT& Mechanic)
	{
		using MethodInfoT = TMethodInfo<decltype(&MechanicT::operator())>;
		return Operate<ChainT>(MethodInfoT::MakeFilter(), Mechanic);
	}

	/**
	 * Process the mechanism using a functor mechanic.
	 * Supports lambdas. Mutable mechanic auto-filter version.
	 * 
	 * The filter is automatically derived from the mechanic's arguments.
	 * 
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor argument types by default.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, typename MechanicT = void,
			   typename std::enable_if<!std::is_function<MechanicT>::value, bool>::type = true >
	FORCEINLINE auto
	Operate(MechanicT& Mechanic)
	{
		using MethodInfoT = TMethodInfo<decltype(&MechanicT::operator())>;
		return Operate<ChainT>(MethodInfoT::MakeFilter(), Mechanic);
	}

	/**
	 * Process the chain using a free function mechanic.
	 * Auto-filter version.
	 * 
	 * The filter is automatically derived from the mechanic's arguments.
	 *
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on function's argument types by default.
	 * @tparam FunctionT The type of the mechanic function
	 * to operate on the slots.
	 * @param Mechanic The mechanical function to operate with.
	 */
	template < typename ChainT = void, typename FunctionT = void,
			   typename std::enable_if<std::is_function<FunctionT>::value, bool>::type = true >
	FORCEINLINE auto
	Operate(FunctionT* const Mechanic)
	{
		using FunctionInfoT = TFunctionInfo<FunctionT>;
		return Operate<ChainT>(FunctionInfoT::MakeFilter(), Mechanic);
	}

	/**
	 * Process the mechanism using a functor mechanic in a threaded manner.
	 * Supports lambdas. Constant mechanic version.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 * 
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor's argument types by default.
	 * @tparam Paradigm The security paradigm to utilize.
	 * Taken from @p ChainT by default.
	 * @tparam FilterT The type of filter to query with.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Filter The filter to query with.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, EParadigm Paradigm = EParadigm::Auto,
			   typename FilterT = void, typename MechanicT = void,
			   TFilterTypeSecurity<std::decay_t<FilterT>> = true,
			   typename std::enable_if<!std::is_function<MechanicT>::value, bool>::type = true >
	FORCEINLINE auto
	OperateConcurrently(FilterT&&        Filter,
						const MechanicT& Mechanic,
						const int32      ThreadsCountMax,
						const int32      SlotsPerThreadMin = 1,
						const bool       bSync = true)
	{
		using RealChainT = typename std::conditional<std::is_void<ChainT>::value,
													 FSolidChain,
													 ChainT>::type;
		static constexpr EParadigm RealParadigm = (Paradigm == EParadigm::Auto) ? RealChainT::DefaultParadigm : Paradigm;
		const auto Chain = new RealChainT(this);
		Chain->bSelfDestroyOnDisposal = true;
		Enchain(std::forward<FilterT>(Filter), *Chain);
		return Chain->template OperateConcurrently<RealParadigm>(Mechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
	}

	/**
	 * Process the mechanism using a functor mechanic in a threaded manner.
	 * Supports lambdas. Mutable mechanic version.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 *
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor's argument types by default.
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam FilterT The type of filter to query with.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Filter The filter to query with.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, EParadigm Paradigm = EParadigm::Auto, typename FilterT = void, typename MechanicT = void,
			   TFilterTypeSecurity<std::decay_t<FilterT>> = true,
			   typename std::enable_if<!std::is_function<MechanicT>::value, bool>::type = true >
	FORCEINLINE auto
	OperateConcurrently(FilterT&&   Filter,
						MechanicT&  Mechanic,
						const int32 ThreadsCountMax,
						const int32 SlotsPerThreadMin = 1,
						const bool  bSync = true)
	{
		using RealChainT = typename std::conditional<std::is_void<ChainT>::value,
													 FSolidChain,
													 ChainT>::type;
		static constexpr EParadigm RealParadigm = (Paradigm == EParadigm::Auto) ? RealChainT::DefaultParadigm : Paradigm;
		const auto Chain = new RealChainT(this);
		Chain->bSelfDestroyOnDisposal = true;
		Enchain(std::forward<FilterT>(Filter), *Chain);
		return Chain->template OperateConcurrently<RealParadigm>(Mechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
	}

	/**
	 * Process the chain using a free function mechanic in a
	 * parallel manner.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 *
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on function's argument types by default.
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam FilterT The type of filter to query with.
	 * @tparam FunctionT The type of the mechanic function
	 * to operate on the slots.
	 * @param Filter The filter to query with.
	 * @param Mechanic The mechanical function to operate with.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The status of the operation. 
	 */
	template < typename ChainT = void, EParadigm Paradigm = EParadigm::Auto, typename FilterT = void, typename FunctionT = void,
			   TFilterTypeSecurity<std::decay_t<FilterT>> = true,
			   typename std::enable_if<std::is_function<FunctionT>::value, bool>::type = true >
	FORCEINLINE auto
	OperateConcurrently(FilterT&&        Filter,
						FunctionT* const Mechanic,
						const int32      ThreadsCountMax,
						const int32      SlotsPerThreadMin = 1,
						const bool       bSync = true)
	{
		using RealChainT = typename std::conditional<std::is_void<ChainT>::value,
													 FSolidChain,
													 ChainT>::type;
		static constexpr EParadigm RealParadigm = (Paradigm == EParadigm::Auto) ? RealChainT::DefaultParadigm : Paradigm;
		const auto Chain = new RealChainT(this);
		Chain->bSelfDestroyOnDisposal = true;
		Enchain(std::forward<FilterT>(Filter), *Chain);
		return Chain->template OperateConcurrently<RealParadigm>(Mechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
	}

	/**
	 * Process the mechanism using a functor mechanic in a threaded manner.
	 * Supports lambdas. Constant mechanic auto-filter version.
	 * 
	 * The filter is derived automatically from the mechanic's arguments.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 * 
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor's argument types by default.
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The outcome of the operation. 
	 */
	template < typename ChainT = void, EParadigm Paradigm = EParadigm::Auto, typename MechanicT = void,
			   typename std::enable_if<!std::is_function<MechanicT>::value, bool>::type = true >
	FORCEINLINE auto
	OperateConcurrently(const MechanicT& Mechanic,
						const int32      ThreadsCountMax,
						const int32      SlotsPerThreadMin = 1,
						const bool       bSync = true)
	{
		using MethodInfoT = TMethodInfo<decltype(&MechanicT::operator())>;
		return OperateConcurrently<ChainT, Paradigm>(
			MethodInfoT::MakeFilter(), Mechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
	}

	/**
	 * Process the mechanism using a functor mechanic in a threaded manner.
	 * Supports lambdas. Mutable mechanic auto-filter version.
	 * 
	 * The filter is derived automatically from the mechanic's arguments.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 *
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on functor's argument types by default.
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param Mechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The outcome of the operation. 
	 */
	template < typename ChainT = void, EParadigm Paradigm = EParadigm::Auto, typename MechanicT = void,
			   typename std::enable_if<!std::is_function<MechanicT>::value, bool>::type = true >
	FORCEINLINE auto
	OperateConcurrently(MechanicT&  Mechanic,
						const int32 ThreadsCountMax,
						const int32 SlotsPerThreadMin = 1,
						const bool  bSync = true)
	{
		using MethodInfoT = TMethodInfo<decltype(&MechanicT::operator())>;
		return OperateConcurrently<ChainT, Paradigm>(
			MethodInfoT::MakeFilter(), Mechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
	}

	/**
	 * Process the chain using a free function mechanic in a
	 * parallel manner. Auto-filter version.
	 * 
	 * The filter is derived automatically from the mechanic's arguments.
	 *
	 * @note Only solid chains can be safely operated concurrently.
	 *
	 * @tparam ChainT The type of chain to utilize.
	 * Detected automatically based on function's argument types by default.
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam FunctionT The type of the mechanic function
	 * to operate on the slots.
	 * @param Mechanic The mechanical function to operate with.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The outcome of the operation. 
	 */
	template < typename ChainT = void, EParadigm Paradigm = EParadigm::Auto, typename FunctionT = void,
			   typename std::enable_if<std::is_function<FunctionT>::value, bool>::type = true >
	FORCEINLINE auto
	OperateConcurrently(FunctionT* const Mechanic,
						const int32      ThreadsCountMax,
						const int32      SlotsPerThreadMin = 1,
						const bool       bSync = true)
	{
		using FunctionInfoT = TFunctionInfo<decltype(Mechanic)>;
		return OperateConcurrently<ChainT, Paradigm>(
			FunctionInfoT::MakeFilter(), Mechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
	}

	/// @}
#pragma endregion Operating

#pragma region Initialization

	/**
	 * Construct a new mechanism object.
	 * 
	 * @param ObjectInitializer The object initializer to use.
	 */
	AMechanism(const FObjectInitializer& ObjectInitializer)
	  : Super(ObjectInitializer)
	{
		OperatingsCompleted->Trigger();
	}

#pragma endregion Initialization

}; //-class AMechanism


inline EApparatusStatus
AMechanism::RegisterMechanical(TScriptInterface<IMechanical> Mechanical)
{
	check(Mechanical);

	EApparatusStatus Status = EApparatusStatus::Success;

	Mechanicals.Add(static_cast<IMechanical*>(Mechanical.GetInterface()));

	return Status;
}

#pragma region Subjective

FORCEINLINE AMechanism*
ISubjective::GetMechanism() const
{
	if (UNLIKELY(Belt != nullptr))
	{
		return Cast<AMechanism>(Belt->GetOwner());
	}
	return nullptr;
}

template < EParadigm Paradigm >
inline TOutcome<Paradigm>
ISubjective::SetMechanism(AMechanism* const Mechanism)
{
	AssessCondition(Paradigm, Mechanism != nullptr, EApparatusStatus::NullArgument);
	if (LIKELY(Handle.IsValid()))
	{
		return Handle.template SetMechanism<Paradigm>(Mechanism);
	}
	this->GetMechanismOverridePropRef() = Mechanism;
	return EApparatusStatus::Success;
}

template < EParadigm Paradigm/*=EParadigm::Default*/ >
inline TOutcome<Paradigm, UDetail*>
ISubjective::EnableDetail(const TSubclassOf<UDetail> DetailClass)
{
	if (AvoidConditionFormat(Paradigm, !GetMechanism() || GetMechanism()->IsSolid(),
							 TEXT("Enabling subjective details is not possible during a solid mode. "
								  "Do you have an unfinished solid iterating?")))
	{
		return MakeOutcome<Paradigm, UDetail*>(EApparatusStatus::InvalidState, nullptr);
	}

	if (GetFingerprint().Contains(DetailClass))
	{
		// The detail is already available and is active.
		const auto r = GetDetail(DetailClass);
		check(r);
		return MakeOutcome<Paradigm, UDetail*>(EApparatusStatus::Noop, r);
	}

	// Try to find among existing details first,
	// including the inactive ones.
	const bool bIncludeDisabled = true;
	UDetail* Detail = GetDetail(DetailClass, bIncludeDisabled);
	if (Detail)
	{
		if (!Detail->IsEnabled())
		{
			// Found an inactive detail.
			// Activate it and return.
			Detail->SetEnabled();
		}
		return MakeOutcome<Paradigm, UDetail*>(EApparatusStatus::Reused, Detail);
	}

	// Create a new detail of the needed type...
	Detail = NewObject<UDetail>(CastChecked<UObject>(this), DetailClass);
	check(Detail);

	GetDetailsRef().Add(Detail);
	GetFingerprintRef().Add(DetailClass);

	checkSlow(FindDetail(DetailClass));

	if (LIKELY(IsRegistered()))
	{
		GetMechanism()->ActualizeBelt(this);
		Detail->Activated();
	}

	return Detail;
}

template < EParadigm Paradigm/*=EParadigm::Default*/ >
inline TOutcome<Paradigm, UDetail*>
ISubjective::AddDetail(const TSubclassOf<UDetail> DetailClass,
					   const bool                 bReuseDisabled)
{
	check(IsValidSubjective(this));
	check(DetailClass);

	checkf(!GetMechanism() || !GetMechanism()->IsSolid(),
		   TEXT("Adding subjective details is not possible during uniform mode. "
				"Do you have an unfinished uniform iterating?"));

	if (UNLIKELY(bReuseDisabled))
	{
		// Try to find an inactive detail.

		// In the UDetail case we can skip the IsA checks for a slight
		// performance benefit.
		if (UNLIKELY(DetailClass == UDetail::StaticClass()))
		{
			for (auto Detail : GetDetailsRef())
			{
				if (UNLIKELY(!Detail))
					continue;
				if (!Detail->IsEnabled())
				{
					// Set the detail to be active and return it to the user.
					Detail->SetEnabled(true);
					return Detail;
				}
				return Detail;
			}
		}
		else
		{
			for (auto Detail : GetDetailsRef())
			{
				if (UNLIKELY(!Detail))
					continue;
				if (Detail->IsEnabled())
					continue;
				if (Detail->IsA(DetailClass))
				{
					// Set the detail to be active and return it to the user.
					Detail->SetEnabled(true);
					return Detail;
				}
			}
		}
	}

	// No existing inactive detail was found,
	// so create a new detail of a needed type.
	UDetail* Detail = NewObject<UDetail>(CastChecked<UObject>(this), DetailClass);
	check(Detail);

	GetDetailsRef().Add(Detail);
	if (LIKELY(Handle))
	{
		GetFingerprintRef().Add(DetailClass);
	}

	checkSlow(FindDetail(DetailClass) != nullptr);

	if (LIKELY(IsRegistered()))
	{
		GetMechanism()->ActualizeBelt(this);
		Detail->Activated();
	}

	return Detail;
}

template < EParadigm Paradigm/*=EParadigm::Default*/ >
TOutcome<Paradigm>
ISubjective::DisableDetail(const TSubclassOf<UDetail> DetailClass,
						   const bool                 bDisableMultiple/*=false*/)
{
	AssessConditionFormat(Paradigm, !GetMechanism() || !GetMechanism()->IsSolid(),
				 EApparatusStatus::InvalidState,
				 TEXT("Disabling subjective details is not possible during solid mode. "
					  "Do you have an unfinished solid iterating process?"));

	if (UNLIKELY(DetailClass == nullptr))
	{
		return EApparatusStatus::Noop;
	}

	// Check if the detail is in a current fingerprint first.
	if (UNLIKELY(!GetFingerprint().Contains(DetailClass)))
	{
		return EApparatusStatus::Noop;
	}

	checkSlow(FindDetail(DetailClass) != nullptr);

	EApparatusStatus Status = EApparatusStatus::Noop;

	// Details are never removed, just disabled,
	// so we can safely iterate that way here:
	for (auto Detail : GetDetailsRef())
	{
		if (UNLIKELY(!Detail)) continue;
		if (!Detail->IsEnabled()) continue;
		if (Detail->IsA(DetailClass))
		{
			// The following detail method will handle the
			// fingerprint and the (possible) slot change,
			// and the event calling:
			Detail->SetEnabled(false);

			if (LIKELY(!bDisableMultiple))
			{
				// Return immediately if only one is enough:
				return EApparatusStatus::Success;
			}

			Status = EApparatusStatus::Success;
		}
	}

	return Status;
}

#pragma endregion Subjective

#pragma region Iterable

FORCEINLINE AMechanism*
IIterable::GetOwner() const
{
	return Cast<AMechanism>(CastChecked<UObject>(this)->GetOuter());
}

FORCEINLINE bool
IIterable::IsSolid() const
{
	const auto Mechanism = GetOwner();
	check(Mechanism);
	return Mechanism->IsSolid();
}

#pragma endregion Iterable

#pragma region Belt

FORCEINLINE AMechanism*
UBelt::GetOwner() const
{
	// Unlike chunks, belts can be owner-less
	// since they can be assets...
	return Cast<AMechanism>(GetOuter());
}

#pragma endregion Belt

inline void
AMechanism::BootAll()
{
	SCOPE_CYCLE_COUNTER(STAT_MechanismBoot);

	if (IsInConcurrentEnvironment())
	{
		return;
	}

	if (HaltedSubjects.Num() == 0)
	{
		// Guaranteed to have no halted subjects.
		return;
	}

	// Boot with each available mechanical...
	for (auto Mechanical : Mechanicals)
	{
		if (UNLIKELY(!Mechanical.IsValid())) continue;
		Mechanical->Boot();
	}

	// Mark the subjects (and subjectives having a handle) as booted...
	for (const auto& HaltedSubject : HaltedSubjects)
	{
		// The list can also contain the already absent subjects...
		if (LIKELY(HaltedSubject.IsValid()))
		{
			HaltedSubject.MarkBooted();
		}
	}
	HaltedSubjects.Reset(); // TODO: Optimize this maybe. Don't want any lags on second allocations though.
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm, UBelt*>
AMechanism::ObtainMostSpecificBelt(const FFingerprint& Fingerprint)
{
	UBelt* Result = nullptr;
	int32 Diff = TNumericLimits<int32>::Max();
	// Find among the matching belts first...
	TArray<UBelt*> MatchingBelts;
	FindBeltsMatchingFingerprint(Fingerprint, MatchingBelts);
	for (auto Belt : MatchingBelts)
	{
		check(Belt);
		if (Belt->Detailmark.GetDetailsMask() ==
			Fingerprint.GetDetailsMask())
		{
			// A perfectly matching belt:
			return MakeOutcome<Paradigm, UBelt*>(EApparatusStatus::Noop, Belt);
		}
		const int32 NewDiff =
			Belt->Detailmark.GetDetailsMask().DifferencesCount(
				Fingerprint.GetDetailsMask());
		if (NewDiff < Diff)
		{
			// A better matching belt:
			Diff = NewDiff;
			Result = Belt;
		}
	}

	if (Result)
	{
		// The most matching belt was found:
		return MakeOutcome<Paradigm, UBelt*>(EApparatusStatus::Noop, Result);
	}

	// Find among all of the belts available...
	Diff = TNumericLimits<int32>::Max();
	for (int32 i = 0; i < Belts.Num(); ++i)
	{
		UBelt* const Belt = Belts[i];
		check(Belt);
		const int32 NewDiff =
			Belt->Detailmark.GetDetailsMask().DifferencesCount(
				Fingerprint.GetDetailsMask());
		if (NewDiff < Diff)
		{
			Diff = NewDiff;
			Result = Belt;
		}
	}

	if (Result)
	{
		// Some kind of belt was found. Expand it to the needed fingerprint and return...
		if (AvoidError(Paradigm, Result->template Expand<Paradigm>(Fingerprint)))
		{
			return MakeOutcome<Paradigm, UBelt*>(FApparatusStatus::GetLastError(), nullptr);	
		}
		return MakeOutcome<Paradigm, UBelt*>(EApparatusStatus::Success, Result);
	}

	// No matching belt was found. Create a new one...
	const FName Name = FName(FString::Format(TEXT("AutoBelt_{0}"), {Belts.Num() + 1}));
	Result = UBelt::New(this, Name, Fingerprint);
	check(Result);
	Belts.Add(Result);
	for (auto& Pair : BeltsByFilterCache)
	{
		if (Result->Matches(Pair.Key))
		{
			Pair.Value.Add(Result);
		}
	}

	return MakeOutcome<Paradigm, UBelt*>(EApparatusStatus::Success, Result);
}

template < EParadigm Paradigm/*=EParadigm::DefaultInternal*/ >
inline TOutcome<Paradigm, UBelt*>
AMechanism::ObtainMostSpecificBelt(ISubjective* const Subjective)
{
	const FFingerprint& SubjectiveFingerprint = Subjective->GetFingerprint();

	// Try to use a preset belt:
	const auto PreferredBelt = Subjective->GetPreferredBelt();
	if (PreferredBelt != nullptr)
	{
		if (IsHarsh(Paradigm)) // Compile-time branch.
		{
			const auto Result = ObtainBeltByTag<Paradigm>(PreferredBelt->GetUniqueID());
			Result->template Expand<Paradigm>(SubjectiveFingerprint);
			return Result;
		}
		else
		{
			// Need to preserve a Noop indication here, so explicit outcome...
			const auto ObtainmentOutcome = ObtainBeltByTag<Paradigm>(PreferredBelt->GetUniqueID());
			TOutcome<Paradigm> ExpansionOutcome;
			if (AvoidError(Paradigm, ExpansionOutcome = PreferredBelt->template Expand<Paradigm>(SubjectiveFingerprint)))
			{
				return MakeOutcome<Paradigm, UBelt*>(ExpansionOutcome, nullptr);
			}
			return OutcomeCombine(ObtainmentOutcome, ExpansionOutcome);
		}
	}

	return ObtainMostSpecificBelt<Paradigm>(SubjectiveFingerprint);
}

#if CPP && !defined(SKIP_MACHINE_H)
#include "Machine.h"
#endif
