/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Machine.h
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

#include <atomic>

#include "More/type_traits"

#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#include "Math/Range.h"
#include "Engine/World.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Class.h"
#include "UObject/CoreNet.h"
#include "UObject/Package.h"
#include "UObject/UObjectHash.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"
#include "Misc/ScopeLock.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define MACHINE_H_SKIPPED_MACHINE_H
#endif

#include "Mechanism.h"
#include "NetworkBearerComponent.h"

#ifdef MACHINE_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif


#include "Machine.generated.h"


// Forward declarations:
class AMechanism;

DECLARE_STATS_GROUP(TEXT("Machine"), STATGROUP_Machine, STATCAT_Advanced);

DECLARE_CYCLE_STAT_EXTERN(TEXT("Boot"),
						  STAT_MachineBoot,
						  STATGROUP_Machine,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Buffer"),
						  STAT_MachineBuffer,
						  STATGROUP_Machine,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Find Matching Iterables"),
						  STAT_MachineFindMatchingIterables,
						  STATGROUP_Machine,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Evaluate"),
						  STAT_MachineEvaluate,
						  STATGROUP_Machine,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Evaluate ~ Input"),
						  STAT_MachineEvaluateInput,
						  STATGROUP_Machine,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Evaluate ~ Steady"),
						  STAT_MachineEvaluateSteady,
						  STATGROUP_Machine,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Evaluate ~ Presentation"),
						  STAT_MachineEvaluatePresentation,
						  STATGROUP_Machine,
						  APPARATUSRUNTIME_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Fetch"),
						  STAT_MachineFetchDetails,
						  STATGROUP_Machine,
						  APPARATUSRUNTIME_API);

/**
 * The global state manager.
 *
 * This is the root Apparatus object. It is created
 * automatically and persists until it's no longer needed.
 */
UCLASS()
class APPARATUSRUNTIME_API UMachine
  : public UObject
  , public FNetworkNotify
{
	GENERATED_BODY()

  public:

	enum
	{
		/**
		 * Invalid chain identifier.
		 */
		InvalidChainId = FChain::InvalidId,

		/**
		 * First valid chain identifier.
		 */
		FirstChainId = FChain::FirstId,

		/**
		 * Invalid subject's slot index.
		 */
		InvalidSubjectIndex = FSubjectInfo::InvalidSlotIndex
	};

	typedef FSubjectInfo::IdType SubjectIdType;

	/**
	 * Invalid subject identifier.
	 */
	static constexpr SubjectIdType InvalidSubjectId = FSubjectInfo::InvalidId;

	/**
	 * A first valid subject place.
	 */
	static constexpr SubjectIdType FirstSubjectPlace = FSubjectInfo::FirstPlace;

	/**
	 * The last valid subject place.
	 */
	static constexpr SubjectIdType LastSubjectPlace = FSubjectInfo::LastPlace;

  private:

#pragma region Traits

	/**
	 * The registered traits types.
	 */
	UPROPERTY()
	TMap<const UScriptStruct*, FTraitInfo> Traits;

#pragma endregion Traits

#pragma region Details

	/**
	 * All of the currently registered details classes.
	 */
	UPROPERTY()
	TMap<TSubclassOf<UDetail>, FDetailInfo> Details;

#pragma endregion Details

#pragma region Subjects

	/**
	 * The mechanism-based subjects list.
	 * 
	 * This is a machine's internal subject
	 * storage place. We need this entity
	 * cause the subject handles are validated
	 * by their generation and we need to
	 * always keep track of it, even if its
	 * mechanism is updated.
	 */
	struct FSubjectPool
	{
		/**
		 * The mechanism these subjects relate to.
		 */
		AMechanism* Mechanism = nullptr;

		/**
		 * The subjects entries of the machine.
		 * 
		 * May actually have "holes" as marked by the UMachine::FreeSubjects
		 * array.
		 */
		TArray<FSubjectInfo> Subjects;

		/**
		 * An array of free subject identifiers to reuse
		 * within UMachine::FSubjectPool::Subjects.
		 */
		TArray<FSubjectInfo::IdType> FreePlaces;

		/**
		 * Get the current number of effective subjects
		 * within the mechanism.
		 */
		FORCEINLINE int32
		SubjectsNum() const
		{
			check(FreePlaces.Num() <= Subjects.Num() - FirstSubjectPlace);
			return (Subjects.Num() - FirstSubjectPlace) - FreePlaces.Num();
		}

		/**
		 * Get the subject information at a certain place.
		 * A harsh version that would trigger halts on errors.
		 * 
		 * @param SubjectPlace The place to query at.
		 * @return A reference to the subject information
		 * structure.
		 */
		FORCEINLINE FSubjectInfo&
		GetSubjectInfo(const FSubjectInfo::IdType SubjectPlace)
		{
			check((SubjectPlace & FSubjectInfo::ShiftedMechanismIdMask) == 0);
			check(SubjectPlace != FSubjectInfo::InvalidPlace);
			return Subjects[SubjectPlace];
		}

		/**
		 * Try to find a subject information
		 * by its place.
		 * 
		 * Will return @c nullptr if the subject information
		 * was not found.
		 * 
		 * @param SubjectPlace The place to search at.
		 * @return A pointer to the subject information
		 * structure.
		 */
		FORCEINLINE FSubjectInfo*
		FindSubjectInfo(const FSubjectInfo::IdType SubjectPlace)
		{
			check((SubjectPlace & FSubjectInfo::ShiftedMechanismIdMask) == 0);
			if (LIKELY(SubjectPlace != FSubjectInfo::InvalidPlace
					&& SubjectPlace < Subjects.Num()))
			{
				return &(Subjects[SubjectPlace]);
			}
			return nullptr;
		}

		/**
		 * Allocate or reuse a subject information structure.
		 * 
		 * @tparam Paradigm The paradigm to work under.
		 * @return The newly allocated subject structure.
		 */
		template < EParadigm Paradigm = EParadigm::DefaultInternal >
		TOutcome<Paradigm, FSubjectInfo*>
		AllocateSubjectInfo()
		{
			check(Mechanism != nullptr);
			AMechanism::FExclusiveScope Exclusive(Mechanism); // Only possible in a writing scope.
			FSubjectInfo* Info = nullptr;

			if (FreePlaces.Num() > 0)
			{
				// Reuse a removed subject:
				const auto SubjectPlace = FreePlaces.Pop();
				check(SubjectPlace != FSubjectInfo::InvalidId);
				Info = &(GetSubjectInfo(SubjectPlace));
				check(SubjectPlace == (Info->Id & FSubjectInfo::PlaceMask));

				// The generation should already be incremented
				// during a subject removal, so we don't change it here.
				check(Info->Id != FSubjectInfo::InvalidId);
				return MakeOutcome<Paradigm, FSubjectInfo*>(EApparatusStatus::Reused, Info);
			}
			else
			{
				// Allocate a new subject...
				const int32 OldSize = Subjects.Num();
				if (OldSize == FSubjectInfo::PlacesPerMechanismMax)
				{
					return MakeOutcome<Paradigm, FSubjectInfo*>(EApparatusStatus::OutOfLimit, nullptr);
				}
				const int32 SubjectPlace = OldSize;
				Info = &Subjects.AddDefaulted_GetRef();
				Info->Id = FSubjectInfo::MakeId(Mechanism->MechanismId, SubjectPlace);

				check(Info->Id != FSubjectInfo::InvalidId);
				return MakeOutcome<Paradigm, FSubjectInfo*>(EApparatusStatus::Success, Info);
			}
		}

		/**
		 * Release a subject information
		 * structure making it available for a later
		 * reuse.
		 * 
		 * @note This also cleans up the networking look-up dictionary.
		 */
		EApparatusStatus
		ReleaseSubjectInfo(const int32 SubjectPlace)
		{
			check((SubjectPlace & FSubjectInfo::ShiftedMechanismIdMask) == 0);
			if (UNLIKELY(Mechanism == nullptr))
			{
				return EApparatusStatus::Noop;
			}
			{
				AMechanism::FExclusiveScope Exclusive(Mechanism); // Only possible in a writing scope.
				const auto Info = this->FindSubjectInfo(SubjectPlace);
				if (UNLIKELY(Info == nullptr || !Info->IsValid()))
				{
					return EApparatusStatus::Noop;
				}
				if (LIKELY(Info->Chunk != nullptr))
				{
					// The subject is in a chunk, remove it from there also...
					verify(OK(Info->Chunk->ReleaseSlot(Info->SlotIndex, /*bHard=*/true)));
					Info->Chunk     = nullptr;
					Info->SlotIndex = FSubjectInfo::InvalidSlotIndex;
				}

				// Make sure the networking state
				// is finalized...
				if (Info->NetworkState != nullptr
				 && Info->NetworkState->IsValid())
				{
					Mechanism->SubjectByNetworkId.Remove(Info->NetworkState->Id);
					Info->NetworkState->Id = FSubjectNetworkState::InvalidId;
				}

				// Increment the generation marking the info as invalid
				// for existing subject handles pointing to it.
				// Adjust the generation with an overflow protection...
				Info->DoIncrementGeneration();

				// Register the info slot to be reused:
				FreePlaces.Add(SubjectPlace);

				// Reset the subjective level:
				if (Info->Subjective)
				{
					const auto SubjectiveSave = Info->Subjective;
					Info->Subjective = nullptr;
					SubjectiveSave->Handle.ResetHandle();
					SubjectiveSave->TakeBeltSlot(nullptr);
					SubjectiveSave->NotifyHandleDespawned();
				}
			}

			return EApparatusStatus::Success;
		}

		/**
		 * Despawn all of the subjects within the pool.
		 * 
		 * This is essentially the same as releasing
		 * all the valid subject infos.
		 * 
		 * @return The status of the operation. 
		 */
		EApparatusStatus
		ReleaseAllSubjectInfos()
		{
			// There was an idea to safely unregister
			// all the subjectives first, but that's
			// not very logically accurate. It's
			// better to left the subjective's as rogue (subjectless).

			auto Status = EApparatusStatus::Noop;

			if (LIKELY(Mechanism && (SubjectsNum() > 0)))
			{
				AMechanism::FExclusiveScope Exclusive(Mechanism); // Only possible in a writing scope.
				for (SubjectIdType SubjectPlace = FirstSubjectPlace; SubjectPlace < Subjects.Num(); ++SubjectPlace)
				{
					StatusAccumulate(Status, ReleaseSubjectInfo(SubjectPlace));
				}
			}
			
			// The subjects array itself is never reset,
			// so the existing subject handle ids would
			// actually be guaranteed to point to some entry.
			// Thereby the free places array is also not
			// reset here.

			return Status;
		}

		EApparatusStatus
		ReleaseSubjectInfos(const EFlagmark Flagmark          = FM_None,
							const EFlagmark ExcludingFlagmark = FM_None)
		{
			if ((Flagmark == FM_None) && (ExcludingFlagmark == FM_None))
			{
				return ReleaseAllSubjectInfos();
			}

			auto Status = EApparatusStatus::Noop;

			if (LIKELY(Mechanism && (SubjectsNum() > 0)))
			{
				AMechanism::FExclusiveScope Exclusive(Mechanism); // Only possible in a writing scope.
				for (SubjectIdType SubjectPlace = FirstSubjectPlace; SubjectPlace < Subjects.Num(); ++SubjectPlace)
				{
					auto& Info = Subjects[SubjectPlace];
					if (Info.IsValid() && Info.GetFingerprint().Matches(Flagmark, ExcludingFlagmark))
					{
						StatusAccumulate(Status, ReleaseSubjectInfo(Info.GetHandle()));
					}
				}
			}

			return Status;
		}

		FSubjectPool()
		{
			if (FSubjectInfo::FirstPlace > 0) // Compile-time branch.
			{
				// Padding subject at the first index:
				Subjects.AddDefaulted(FSubjectInfo::FirstPlace);
			}
		}

		FSubjectPool(const FSubjectPool&) = delete;

		FSubjectPool(FSubjectPool&&) = delete;

		FSubjectPool&
		operator=(const FSubjectPool&) = delete;

		FSubjectPool&
		operator=(FSubjectPool&&) = delete;
	}; //-struct FSubjectPool

	/**
	 * Subjects stored in mechanism-based bulks.
	 */
	TArray<FSubjectPool> SubjectPools;

	/**
	 * The indices of the available free subject pools.
	 */
	TArray<AMechanism::IdType> FreeMechanismIds;

	/**
	 * A critical section for (un)registration of
	 * the mechanisms.
	 */
	mutable FCriticalSection MechanismRegistrationSC;

#pragma endregion Subjects

#pragma region Networking

	/**
	 * The type of the network identifiers.
	 */
	using SubjectNetworkIdType = FSubjectNetworkState::IdType;

	/**
	 * The next unique network identifier to assign.
	 */
	SubjectNetworkIdType NextNetworkId = FSubjectNetworkState::FirstId; 

	/**
	 * A pool of network identifiers.
	 * 
	 * This is used for a deterministic network identifier allocation.
	 */
	struct FNetworkIdsPool
	  : public TRange<SubjectNetworkIdType>
	{
		/**
		 * The base range struct of the pool.
		 */
		using Super = TRange<SubjectNetworkIdType>;

		/**
		 * The next network identifier to return.
		 */
		SubjectNetworkIdType NextId;

		FNetworkIdsPool(const Super& Range)
		  : Super(Range)
		{
			checkf(Range.HasLowerBound(),
				   TEXT("The range of the subject network identifiers pool must have a lower bound."));
			NextId = Range.GetLowerBoundValue();
		}

		/**
		 * Obtain the next subject network identifier.
		 * 
		 * @return SubjectNetworkIdType 
		 */
		SubjectNetworkIdType
		ObtainId()
		{
			if (LIKELY(HasUpperBound()))
			{
				if (GetUpperBound().IsInclusive())
				{
					ensureAlwaysMsgf(NextId <= GetUpperBoundValue(),
									 TEXT("No more network identifiers are available within the pool."));
				}
				else
				{
					ensureAlwaysMsgf(NextId < GetUpperBoundValue(),
									 TEXT("No more network identifiers are available within the pool."));
				}
			}
			else
			{
				ensureAlwaysMsgf(NextId <= FSubjectNetworkState::LastId,
								 TEXT("No more network identifiers are available within the boundless pool."));
			}
			return NextId++;
		}

		/**
		 * Compare itself to a range.
		 */
		FORCEINLINE bool
		operator==(const Super& InRange) const
		{
			return Super::operator==(InRange);
		}
	};

	TArray<FNetworkIdsPool> NetworkIdsPools;

	/**
	 * A critical section for network identifier obtainment.
	 */
	FCriticalSection NetworkIdsCS;

	/**
	 * Obtain a network identifiers pool for a specific range.
	 */
	FNetworkIdsPool&
	DoObtainNetworkIdsPool(const TRange<SubjectNetworkIdType>& Range)
	{
		FScopeLock Lock(&NetworkIdsCS);
		// Find among the existing ones...
		for (auto& Pool : NetworkIdsPools)
		{
			if (Pool == Range)
			{
				return Pool;
			}
			checkf(!Range.Overlaps(Pool), TEXT("A new network identifiers pool must not overlap with an existing one."));
		}

		checkf(!Range.Contains(NextNetworkId), TEXT("The next globally assigned network identifier (%i) must not be already within a new pool."), (int)NextNetworkId);
		return NetworkIdsPools.Add_GetRef(Range);
	}

	/**
	 * Obtain a new unallocated network identifier.
	 */
	SubjectNetworkIdType
	DoObtainNetworkId()
	{
		FScopeLock Lock(&NetworkIdsCS);
		const auto Id = NextNetworkId;
		if (ensureAlwaysMsgf(Id <= FSubjectNetworkState::LastId,
							 TEXT("No more subject network identifiers are available. Please, reset the machine and start anew.")))
		{
			auto NextId = Id + 1;
			// Make sure not to overlap any pools...
			for (int32 i = 0; i < NetworkIdsPools.Num(); ++i)
			{
				const auto& Pool = NetworkIdsPools[i];
				if (Pool.Contains(NextId))
				{
					if (ensureMsgf(Pool.HasUpperBound(),
								   TEXT("No more global subject network identifiers are available, cause one of the pools has no upper bound.")))
					{
						if (Pool.GetUpperBound().IsExclusive())
						{
							NextId = Pool.GetUpperBoundValue();
						}
						else
						{
							NextId = Pool.GetUpperBoundValue() + 1;
						}
						if (!ensure(NextId <= FSubjectNetworkState::LastId))
						{
							return FSubjectNetworkState::InvalidId;
						}
						// Start from the beginning:
						i = -1;
					}
					else
					{
						return FSubjectNetworkState::InvalidId;
					}
				}
			}
			NextNetworkId = NextId;
		}
		else
		{
			return FSubjectNetworkState::InvalidId;
		}
		return Id;
	}

	/**
	 * Obtain a new unallocated network identifier
	 * from a pool.
	 */
	SubjectNetworkIdType
	DoObtainNetworkId(const TRange<SubjectNetworkIdType>& Range)
	{
		return DoObtainNetworkIdsPool(Range).ObtainId();
	}

#pragma endregion Networking

#pragma region Mechanisms

	/**
	 * All of the currently featured mechanisms,
	 * stored by their packages.
	 */
	TMap<UPackage*, AMechanism*> DefaultMechanismByPackageCache;

	/**
	 * All of the currently featured mechanisms,
	 * stored by their worlds.
	 */
	TMap<UWorld*, AMechanism*> DefaultMechanismByWorldCache;

#pragma endregion Mechanisms

	/**
	 * Filters by unique node keys used specifically for blueprint nodes.
	 */
	TMap<FString, FFilter> FiltersByKeysCache;

	/**
	 * Is the machine currently retained (added to root)?
	 */
	bool bRetained = false;

	/**
	 * The current instance of the machine as a non-grabbing (weak) pointer.
	 */
	static UMachine* Instance;

	friend struct FSubjectInfo;
	friend struct FSubjectNetworkState;
	friend struct FCommonSubjectHandle;
	friend struct FChunkSlot;
	friend class IIterable;
	friend class ISubjective;
	friend class IMechanical;
	friend class UBelt;
	friend class UChunk;
	friend class UNetworkBearerComponent;
	friend class AMechanism;
	friend class UApparatusFunctionLibrary;

	template < typename SubjectHandleT >
	friend struct TChunkIt;

	template < typename SubjectHandleT >
	friend struct TBeltIt;

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	FORCEINLINE EApparatusStatus
	DoClearCache()
	{
		if (LIKELY(FiltersByKeysCache.Num()))
		{
			FiltersByKeysCache.Reset();
			return EApparatusStatus::Success;
		}
		return EApparatusStatus::Noop;
	}

	/**
	 * Clear the cache of the current machine (if any).
	 */
	static FORCEINLINE EApparatusStatus
	ClearCache()
	{
		if (!HasInstance()) return EApparatusStatus::Noop;
		return Instance->DoClearCache();
	}

	/**
	 * Check if the machine is needed in its current state.
	 */
	FORCEINLINE bool
	ShouldBeRetained() const
	{
		return (DoGetMechanismsNum() > 0) ||
			   (Traits.Num()         > 0) ||
			   (Details.Num()        > 0);
	}

	/**
	 * Release the machine from the root set,
	 * if it's no longer needed, i.e. has no
	 * entities registered.
	 * 
	 * You can call this method explicitly
	 * after the UMachine::DoReset().
	 * 
	 * @param bReset Should the machine be reset before
	 * releasing? Guarantees to be released.
	 * 
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus 
	DoReleaseInstance(const bool bReset = true)
	{
		if (UNLIKELY(!bRetained))
		{
			return EApparatusStatus::Noop;
		}
		if (bReset)
		{
			DoReset();
		}
		if (LIKELY(ShouldBeRetained()))
		{
			return EApparatusStatus::Noop;
		}
		RemoveFromRoot();
		bRetained = false;
		return EApparatusStatus::Success;
	}

	/**
	 * Retain the machine instance, adding it to the root set as needed.
	 * 
	 * If the machine doesn't already exist, it is created
	 * anew and added to the root set.
	 * 
	 * This may only be called during the game playing
	 * as the machine shouldn't really exist in Editor.
	 * 
	 * @return The retained machine instance.
	 */
	FORCEINLINE static UMachine*
	RetainInstance()
	{
		UMachine* const i = ObtainInstance();
		if (UNLIKELY(!i->bRetained))
		{
			i->AddToRoot();
			i->bRetained = true;
		}
		return i;
	}

	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	DoReset();

#pragma region Trait Registry

	FORCEINLINE FTraitInfo&
	DoObtainTraitInfo(const UScriptStruct* const TraitType)
	{
		checkf(TraitType != nullptr, TEXT("The trait type must be provided to get the info about."));
		auto* InfoPtr = Traits.Find(TraitType);

		if (LIKELY(InfoPtr != nullptr))
		{
			return *InfoPtr;
		}

		// A static is used here, cause the id has to be consistently
		// incremented for correctly querying the base type infos in the constructor...
		static auto TraitId = FTraitInfo::FirstId;

		return Traits.Add(TraitType, FTraitInfo(TraitType, TraitId++));
	}

	FORCEINLINE FTraitInfo::IdType
	DoObtainTraitId(const UScriptStruct* const TraitType)
	{
		checkf(TraitType != nullptr, TEXT("The trait type must be provided to get the id of."));

		return DoObtainTraitInfo(TraitType).Id;
	}

	FORCEINLINE const FBitMask&
	DoObtainTraitMask(const UScriptStruct* const TraitType)
	{
		checkf(TraitType != nullptr, TEXT("The trait type must be provided to get the mask for."));

		return DoObtainTraitInfo(TraitType).Mask;
	}

	FORCEINLINE const FBitMask&
	DoObtainExcludingTraitMask(const UScriptStruct* const TraitType)
	{
		checkf(TraitType != nullptr, TEXT("The trait type must be provided to get the excluding mask for."));

		return DoObtainTraitInfo(TraitType).ExcludingMask;
	}

#pragma endregion Trait Registry

#pragma region Detail Registry

	FORCEINLINE FDetailInfo&
	DoObtainDetailInfo(const TSubclassOf<UDetail> DetailClass)
	{
		checkf(DetailClass != nullptr, TEXT("The detail class must be provided to get the info about."));
		auto* InfoPtr = Details.Find(DetailClass);

		if (LIKELY(InfoPtr != nullptr))
		{
			return *InfoPtr;
		}

		// A static is used here, cause the id has to be consistently
		// incremented for correctly querying the base class infos in the constructor...
		static auto DetailId = FDetailInfo::FirstId;

		return Details.Add(DetailClass, FDetailInfo(DetailClass, DetailId++));
	}

	FORCEINLINE FDetailInfo::IdType
	DoObtainDetailId(const TSubclassOf<UDetail> DetailClass)
	{
		checkf(DetailClass != nullptr, TEXT("The detail class must be provided to get the id of."));

		return DoObtainDetailInfo(DetailClass).Id;
	}

	FORCEINLINE const FBitMask&
	DoObtainDetailMask(const TSubclassOf<UDetail> DetailClass)
	{
		checkf(DetailClass != nullptr, TEXT("The detail class must be provided to get the mask for."));

		return DoObtainDetailInfo(DetailClass).Mask;
	}

	FORCEINLINE const FBitMask&
	DoObtainExcludingDetailMask(const TSubclassOf<UDetail> DetailClass)
	{
		checkf(DetailClass != nullptr, TEXT("The detail class must be provided to get the excluding mask for."));

		return DoObtainDetailInfo(DetailClass).ExcludingMask;
	}

#pragma endregion Detail Registry

#pragma region Mechanism Registry

	/**
	 * Get the number of currently
	 * registered (active) mechanisms.
	 * 
	 * The method is thread-safe.
	 */
	int32
	DoGetMechanismsNum() const
	{
		FScopeLock Lock(&MechanismRegistrationSC);
		return SubjectPools.Num() - FreeMechanismIds.Num() - AMechanism::FirstId;
	}

	/**
	 * Register a mechanism within the machine,
	 * reserving a unique identifier for it.
	 * 
	 * The method is thread-safe.
	 * 
	 * @param Mechanism The mechanism to register
	 * within the machine.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	DoRegisterMechanism(AMechanism* const Mechanism)
	{
		check(Mechanism != nullptr);
		if (Mechanism->MechanismId != AMechanism::InvalidId)
		{
			// Already allocated.
			return EApparatusStatus::Noop;
		}

		{
			FScopeLock Lock(&MechanismRegistrationSC);
			if (FreeMechanismIds.Num() > 0)
			{
				const auto Id = FreeMechanismIds.Pop();
				check(Id != AMechanism::InvalidId);
				Mechanism->MechanismId = Id;
				check(SubjectPools[Id].SubjectsNum() == 0);
				check(SubjectPools[Id].Mechanism == nullptr);
				SubjectPools[Id].Mechanism = Mechanism;
				return EApparatusStatus::Success;
			}
			else
			{
				// Try allocating a new mechanism entry.
				// This is not actually used right now,
				// as all pools are pre-allocated.
				for (AMechanism::IdType Id = AMechanism::FirstId; Id < SubjectPools.Num(); ++Id)
				{
					auto& Subjects = SubjectPools[Id];
					if (Subjects.Mechanism == nullptr)
					{
						check(Subjects.SubjectsNum() == 0);
						Subjects.Mechanism = Mechanism;
						Mechanism->MechanismId = (AMechanism::IdType)Id;
						return EApparatusStatus::Success;
					}
				}
				if (ensureMsgf(SubjectPools.Num() < (int32)TNumericLimits<AMechanism::IdType>::Max(),
							   TEXT("The maximum number of active subjects is reached.")))
				{
					Mechanism->MechanismId = SubjectPools.AddDefaulted();
					SubjectPools[Mechanism->MechanismId].Mechanism = Mechanism;
					return EApparatusStatus::Success;
				}
			}
		}
		return EApparatusStatus::OutOfLimit;
	}

	/**
	 * Unregister the mechanism from the machine,
	 * releasing its identifier.
	 * 
	 * @param InMechanism The mechanism to unregister
	 * from the machine.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	DoUnregisterMechanism(AMechanism* const InMechanism)
	{
		check(InMechanism != nullptr);
		if (InMechanism->MechanismId == AMechanism::InvalidId)
		{
			// Already deallocated.
			return EApparatusStatus::Noop;
		}
		{
			FScopeLock Lock(&MechanismRegistrationSC);
			const auto Id = InMechanism->MechanismId;
			auto& Subjects = SubjectPools[Id];
			check(Subjects.Mechanism == InMechanism);
			checkf(Subjects.SubjectsNum() == 0,
				   TEXT("The subjects of the '%s' mechanism must have been already despawned."),
				   *InMechanism->GetName());

			for (auto& Pair : DefaultMechanismByWorldCache)
			{
				if (Pair.Value == InMechanism)
				{
					DefaultMechanismByWorldCache.Remove(Pair.Key);
					break;
				}
			}
			for (auto& Pair : DefaultMechanismByPackageCache)
			{
				if (Pair.Value == InMechanism)
				{
					DefaultMechanismByPackageCache.Remove(Pair.Key);
					break;
				}
			}

			Subjects.Mechanism = nullptr;
			InMechanism->MechanismId = AMechanism::InvalidId;
			FreeMechanismIds.Add(Id);
			return EApparatusStatus::Success;
		}
	}

	/**
	 * Unregister all of the mechanisms from the
	 * machine, releasing their identifiers.
	 * 
	 * @return The status of the operation.
	 */
	EApparatusStatus
	DoUnregisterAllMechanisms()
	{
		auto MechanismsCount = DoGetMechanismsNum();
		if (UNLIKELY(MechanismsCount == 0))
		{
			return EApparatusStatus::Noop;
		}
		auto Status = EApparatusStatus::Noop;
		{
			FScopeLock Lock(&MechanismRegistrationSC);
			for (auto MechanismId = AMechanism::FirstId;
					  MechanismId <= AMechanism::LastId && (MechanismsCount > 0);
					++MechanismId)
			{
				auto& Subjects = SubjectPools[MechanismId];
				checkf(Subjects.SubjectsNum() == 0,
					   TEXT("The subjects of the #%i mechanism must have been already despawned."),
					   (int)MechanismId);
				if (Subjects.Mechanism == nullptr)
				{
					continue;
				}

				Subjects.Mechanism->MechanismId = AMechanism::InvalidId;
				Subjects.Mechanism = nullptr;
				FreeMechanismIds.Push(MechanismId);
				Status = EApparatusStatus::Success;
				// A slight optimization to cancel the loop earlier:
				MechanismsCount -= 1;
			}

			DefaultMechanismByPackageCache.Reset();
			DefaultMechanismByWorldCache.Reset();
		}

		return Status;
	}

	/**
	 * Get a subject pool for a mechanism identifier.
	 * Constant version.
	 * 
	 * @param[in] InMechanismId The mechanism identifier to get
	 * the pool for.
	 * @return The subject pool for the mechanism.
	 */
	FORCEINLINE const FSubjectPool&
	DoGetSubjectPool(const AMechanism::IdType InMechanismId) const
	{
		check(InMechanismId != AMechanism::InvalidId);
		return SubjectPools[InMechanismId];
	}

	/**
	 * Get a subject pool for a mechanism identifier.
	 * 
	 * @param[in] InMechanismId The mechanism identifier to get
	 * the pool for.
	 * @return The subject pool for the mechanism.
	 */
	FORCEINLINE FSubjectPool&
	DoGetSubjectPool(const AMechanism::IdType InMechanismId)
	{
		check(InMechanismId != AMechanism::InvalidId);
		return SubjectPools[InMechanismId];
	}

	/**
	 * Obtain a subject pool for a mechanism.
	 * 
	 * @param[in] InMechanism The mechanism to obtain a subject pool for.
	 * @return The subject pool for the mechanism.
	 */
	FORCEINLINE FSubjectPool&
	DoObtainSubjectPool(AMechanism* const InMechanism)
	{
		check(InMechanism != nullptr);
		verify(OK(DoRegisterMechanism(InMechanism)));
		return SubjectPools[InMechanism->MechanismId];
	}

#pragma endregion Mechanism Registry

#pragma region Subjects

	FORCEINLINE FSubjectInfo&
	DoGetSubjectInfo(const FSubjectInfo::IdType SubjectId)
	{
		check(SubjectId != FSubjectInfo::InvalidId);
		const auto MechanismId = FSubjectInfo::ExtractMechanismId(SubjectId);
		check(MechanismId != AMechanism::InvalidId && MechanismId <= AMechanism::LastId);
		const auto Place = SubjectId & FSubjectInfo::PlaceMask;
		check(Place >= FSubjectInfo::FirstPlace);
		return SubjectPools[MechanismId].Subjects[Place];
	}

	/**
	 * Get the subject information by the identifier.
	 * 
	 * @note The returned information is not checked and
	 * may actually be invalid.
	 * 
	 * @param Id The global unique identifier of the subject.
	 * @return Subject information structure. May be invalid (empty).
	 */
	static FORCEINLINE FSubjectInfo&
	GetSubjectInfo(const FSubjectInfo::IdType Id)
	{
		checkf(HasInstance(), TEXT("There must be a machine instance in order to get the subject information from it."));
		return Instance->DoGetSubjectInfo(Id);
	}

	/**
	 * Try to find a subject information
	 * by its unique identifier.
	 */
	FORCEINLINE FSubjectInfo*
	DoFindSubjectInfo(const FSubjectInfo::IdType SubjectId)
	{
		if (UNLIKELY(SubjectId == InvalidSubjectId)) return nullptr;
		const auto MechanismId = FSubjectInfo::ExtractMechanismId(SubjectId);
		check((int32)MechanismId < SubjectPools.Num());
		if (UNLIKELY(MechanismId < AMechanism::FirstId)) return nullptr;
		return SubjectPools[MechanismId].FindSubjectInfo(SubjectId & FSubjectInfo::PlaceMask);
	}

	/**
	 * Find a subject information by its identifier.
	 * 
	 * @param SubjectId The unique identifier of the subject to find.
	 * @return Subject information structure or @c nullptr,
	 * if was not found.
	 */
	static FORCEINLINE FSubjectInfo*
	FindSubjectInfo(const FSubjectInfo::IdType SubjectId)
	{
		if (LIKELY(HasInstance()))
		{
			return Instance->DoFindSubjectInfo(SubjectId);
		}
		return nullptr;
	}

	/**
	 * Allocate a new subject information entry.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] InMechanism The mechanism to allocate the subject for.
	 * @return The resulting, possibly reused entry.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE TOutcome<Paradigm, FSubjectInfo*>
	DoAllocateSubjectInfo(AMechanism* const InMechanism)
	{
		return DoObtainSubjectPool(InMechanism).template AllocateSubjectInfo<Paradigm>();
	}

	/**
	 * Reserve a new subject information structure.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return The newly reserved subject information structure.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	static FORCEINLINE auto
	AllocateSubjectInfo(AMechanism* const InMechanism)
	{
		return RetainInstance()->template DoAllocateSubjectInfo<Paradigm>(InMechanism);
	}

	/**
	 * Release the information record of the subject
	 * by its identifier.
	 * 
	 * This actually does not alter the subjects array
	 * but marks its slots as free to reuse,
	 * so you can execute this method during iterating.
	 * 
	 * @param SubjectId The unique identifier of the subject
	 * to release.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	DoReleaseSubjectInfo(const FSubjectInfo::IdType SubjectId)
	{
		const auto MechanismId = FSubjectInfo::ExtractMechanismId(SubjectId);
		if (UNLIKELY(MechanismId == AMechanism::InvalidId || MechanismId >= SubjectPools.Num()))
		{
			return EApparatusStatus::Noop;
		}
		auto& SubjectPool = SubjectPools[MechanismId];
		if (UNLIKELY(SubjectPool.Mechanism == nullptr))
		{
			return EApparatusStatus::Noop;
		}
		return SubjectPool.ReleaseSubjectInfo(SubjectId & FSubjectInfo::PlaceMask);
	}

	/**
	 * Move a subject from its current chunk to a designated one.
	 * 
	 * Supports moving subjects between mechanisms.
	 * Doesn't move the subjective part though.
	 * 
	 * @tparam Paradigm The safety paradigm to use.
	 * @param SubjectId The identifier of the subject to move.
	 * @param NewChunk A chunk to move to. Must not be in a solid state.
	 * @return The status of the operation.-
	 */
	template < EParadigm Paradigm = EParadigm::Internal >
	TOutcome<Paradigm>
	MoveSubject(const FSubjectInfo::IdType SubjectId,
				UChunk* const              NewChunk);

	/**
	 * Count the current number of effective subjects
	 * within all registered (active) mechanisms.
	 * 
	 * @note This method is generally not thread-safe
	 * and shouldn't be called during spawning/despawning
	 * of the subjects on different threads.
	 * 
	 * The method is generally not optimized and
	 * should be used occasionally.
	 */
	int32
	DoCountSubjectsNum() const
	{
		int32 Count = 0;
		for (AMechanism::IdType MechanismId = AMechanism::FirstId;
								MechanismId < SubjectPools.Num(); 
							  ++MechanismId)
		{
			// This would never overflow cause the highest byte
			// is occupied by a mechanism identifier:
			Count += SubjectPools[MechanismId].SubjectsNum();
		}
		return Count;
	}

	/**
	 * Despawn a subject. If the handle is not viable, nothing is performed and
	 * EApparatusStatus::Noop is returned.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectHandle The handle for a subject to despawn.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	DoDespawnSubject(const FSubjectHandle& SubjectHandle);

	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	DoDespawnAllSubjects()
	{
		auto Status = EApparatusStatus::Noop;
		auto MechanismsToGo = DoGetMechanismsNum();

		for (AMechanism::IdType MechanismId =  AMechanism::FirstId;
								MechanismId <= AMechanism::LastId
							 && MechanismsToGo > 0;
								MechanismId += 1)
		{
			auto& MechanismPool = SubjectPools[MechanismId];
			if (MechanismPool.Mechanism != nullptr)
			{
				StatusAccumulate(Status, MechanismPool.ReleaseAllSubjectInfos());
				MechanismsToGo -= 1;
			}
		}
		AssessCondition(Paradigm, MechanismsToGo == 0, EApparatusStatus::SanityCheckFailed);

		return Status;
	}

	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	DoDespawnSubjects(const EFlagmark Flagmark          = FM_None,
					  const EFlagmark ExcludingFlagmark = FM_None)
	{
		if (UNLIKELY((Flagmark == FM_None) && (ExcludingFlagmark == FM_None)))
		{
			return DoDespawnAllSubjects<Paradigm>();
		}

		auto Status = EApparatusStatus::Noop;
		auto MechanismsToGo = DoGetMechanismsNum();

		for (AMechanism::IdType MechanismId = AMechanism::FirstId;
								MechanismId <= AMechanism::LastId
							 && MechanismsToGo > 0;
								MechanismId += 1)
		{
			auto& MechanismPool = SubjectPools[MechanismId];
			if (MechanismPool.Mechanism != nullptr)
			{
				StatusAccumulate(Status, MechanismPool.ReleaseSubjectInfos(Flagmark, ExcludingFlagmark));
				MechanismsToGo -= 1;
			}
		}

		return Status;
	}

	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	DoDespawnSubjects(AMechanism* const Mechanism)
	{
		if (UNLIKELY((Mechanism == nullptr) || (Mechanism->MechanismId == AMechanism::InvalidId)))
		{
			return EApparatusStatus::Noop;
		}

		auto& Pool = SubjectPools[Mechanism->MechanismId];
		AssessCondition(Paradigm, Pool.Mechanism == Mechanism, EApparatusStatus::SanityCheckFailed);
		return Pool.ReleaseAllSubjectInfos();
	}

#pragma endregion Subjects

#pragma region Mechanisms

	void OnWorldCleanup(UWorld* const InWorld,
						bool          bSessionEnded,
						bool          bCleanupResources);

	/**
	 * Obtain a mechanism for a package instance.
	 * 
	 * @param InPackage A package to get a mechanism for.
	 * @return The package's mechanism.
	 */
	AMechanism*
	DoObtainMechanism(UPackage* const InPackage);

	/**
	 * Obtain a mechanism for a world instance.
	 * 
	 * @param InWorld A world to get a mechanism for.
	 * @return The world's mechanism.
	 */
	AMechanism*
	DoObtainMechanism(UWorld* const InWorld);

	/**
	 * Set the default mechanism
	 * 
	 * @param InWorld 
	 * @param InMechanism 
	 * @return EApparatusStatus 
	 */
	EApparatusStatus
	DoSetMechanism(UWorld* const InWorld, AMechanism* InMechanism);

#pragma endregion Mechanisms

  public:

#pragma region Trait Registration

	/**
	 * Obtain a trait's unique identifier.
	 */
	static FORCEINLINE auto
	ObtainTraitId(const UScriptStruct* TraitType)
	{
		check(TraitType != nullptr);
		return RetainInstance()->DoObtainTraitId(TraitType);
	}

	/**
	 * Get the total number of registered traits so far.
	 */
	static FORCEINLINE int32
	RegisteredTraitsNum()
	{
		if (UNLIKELY(!HasInstance())) return 0;
		return Instance->Traits.Num();
	}

	/**
	 * Obtain the cached mask for a trait type.
	 */
	static FORCEINLINE const FBitMask&
	ObtainTraitMask(const UScriptStruct* TraitType)
	{
		check(TraitType);
		return RetainInstance()->DoObtainTraitMask(TraitType);
	}

	/**
	 * Obtain the mask of a trait. Templated version.
	 */
	template < typename T > 
	static FORCEINLINE const FBitMask&
	ObtainTraitMask()
	{
		return ObtainTraitMask(T::StaticStruct());
	}

	/**
	 * Obtain the excluded mask of a trait type.
	 */
	static FORCEINLINE const FBitMask&
	ObtainExcludingTraitMask(const UScriptStruct* const TraitType)
	{
		checkf(TraitType != nullptr,
			   TEXT("The trait type must be provided in order to get its excluding mask."));
		return RetainInstance()->DoObtainExcludingTraitMask(TraitType);
	}

	/**
	 * Obtain the excluding mask of a trait type.
	 */
	template < typename T >
	static FORCEINLINE const FBitMask&
	ObtainExcludingTraitMask()
	{
		return ObtainExcludingTraitMask(T::StaticStruct());
	}

#pragma endregion Trait Registration

#pragma region Detail Registration

	/**
	 * Obtain a detail's unique identifier.
	 */
	static FORCEINLINE auto
	ObtainDetailId(const TSubclassOf<UDetail> DetailClass)
	{
		check(DetailClass != nullptr);
		return RetainInstance()->DoObtainDetailId(DetailClass);
	}

	/**
	 * Get the total number of registered details so far.
	 */
	static FORCEINLINE int32
	RegisteredDetailsNum()
	{
		if (UNLIKELY(!HasInstance())) return 0;
		return Instance->Details.Num();
	}

	/**
	 * Obtain the cached mask of a detail type.
	 */
	static FORCEINLINE const FBitMask&
	ObtainDetailMask(const TSubclassOf<UDetail> DetailClass)
	{
		check(DetailClass != nullptr);
		return RetainInstance()->DoObtainDetailMask(DetailClass);
	}

	/**
	 * Obtain the excluded mask of a detail class.
	 */
	static FORCEINLINE const FBitMask&
	ObtainExcludingDetailMask(const TSubclassOf<UDetail> DetailClass)
	{
		checkf(DetailClass,
			   TEXT("The detail class must be provided in order to get its mask."));
		return RetainInstance()->DoObtainExcludingDetailMask(DetailClass);
	}

	/**
	 * Obtain the cached mask of a detail's class.
	 */
	static FORCEINLINE const FBitMask&
	ObtainDetailMask(const UDetail* Detail)
	{
		return ObtainDetailMask(Detail->GetClass());
	}

	/**
	 * Obtain the excluded mask of a details's class.
	 */
	static FORCEINLINE const FBitMask&
	ObtainExcludingDetailMask(const UDetail* Detail)
	{
		return ObtainExcludingDetailMask(Detail->GetClass());
	}

	/**
	 * Obtain the mask of a detail class. Templated version.
	 */
	template < class T >
	static FORCEINLINE const FBitMask&
	ObtainDetailMask()
	{
		return ObtainDetailMask(T::StaticClass());
	}

	/**
	 * Obtain the excluding mask of a detail class. Templated version.
	 */
	template < class T >
	static FORCEINLINE const FBitMask&
	ObtainExcludingDetailMask()
	{
		return ObtainExcludingDetailMask(T::StaticClass());
	}

#pragma endregion Detail Registration

#pragma region Singleton

	/**
	 * Check if there is a global machine instance currently.
	 */
	static FORCEINLINE bool
	HasInstance()
	{
		return Instance != nullptr;
	}

	/**
	 * Obtain the global machine instance.
	 * 
	 * The returned instance doesn't actually
	 * gets retained and will be a subject to
	 * garbage collection, unless explicitly retained.
	 * 
	 * This may only be called during the game running
	 * as the machine should not be part of the
	 * normal editing context.
	 */
	static FORCEINLINE UMachine*
	ObtainInstance()
	{
		if (LIKELY(Instance))
		{
			return Instance;
		}
		Instance = NewObject<UMachine>(GetTransientPackage());
		check(Instance);
		UE_LOG(LogApparatus, Display,
			   TEXT("Created a new machine instance: %s"), *(Instance->GetName()));
		// A weak reference to UObject is kept here, so we don't need to manage
		// this explicitly:
		FWorldDelegates::OnWorldCleanup.AddUObject(Instance, &UMachine::OnWorldCleanup);
		return Instance;
	}

#pragma endregion Singleton

	/**
	 * Obtain a default mechanism instance for a package.
	 * 
	 * @note If you want to prevent the resulting mechanism
	 * from being destroyed by the garbage collector,
	 * you have to add an explicit UPROPERTY pointer to it.
	 * 
	 * @param InPackage The package to get a mechanism for.
	 * @return The resulting mechanism.
	 */
	static FORCEINLINE AMechanism*
	ObtainMechanism(UPackage* const InPackage)
	{
		return RetainInstance()->DoObtainMechanism(InPackage);
	}

	/**
	 * Obtain a mechanism instance for a world.
	 * 
	 * @param InWorld A world to obtain a mechanism for.
	 * @return The resulting mechanism.
	 */
	static FORCEINLINE AMechanism*
	ObtainMechanism(UWorld* const InWorld)
	{
		return RetainInstance()->DoObtainMechanism(InWorld);
	}

	/**
	 * Get the number of currently
	 * registered (active) mechanisms.
	 */
	static FORCEINLINE int32
	MechanismsNum()
	{
		if (LIKELY(HasInstance()))
		{
			return Instance->DoGetMechanismsNum();
		}
		return 0;
	}

	/**
	 * Count the current number of effective subjects
	 * within all registered (active) mechanisms.
	 * 
	 * @note This method is generally not thread-safe
	 * and shouldn't be called during spawning/despawning
	 * of the subjects on different threads.
	 * 
	 * The method is generally not optimized and
	 * should be used occasionally.
	 */
	static FORCEINLINE int32
	CountSubjectsNum()
	{
		if (LIKELY(HasInstance()))
		{
			return Instance->DoCountSubjectsNum();
		}
		return 0;
	}

	/**
	 * Copy an existing subject traits to another one.
	 * 
	 * The existing traits of the destination subject are not removed
	 * but the new ones are added and override the existing matching.
	 * Supports copying traits among subjects of different mechanisms.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param SrcSubjectHandle The source subject handle to copy from.
	 * @param DstSubjectHandle The destination subject handle to copy to.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	CopyTraitsFromTo(const FSubjectHandle& SrcSubjectHandle,
					 const FSubjectHandle& DstSubjectHandle);

	/**
	 * Despawn all the subjects matching a filter
	 * within all of the mechanisms.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flagmark The including flagmark to match.
	 * @param ExcludingFlagmark The excluding flagmark to match.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	static FORCEINLINE TOutcome<Paradigm>
	DespawnSubjects(const EFlagmark Flagmark,
					const EFlagmark ExcludingFlagmark = FM_None)
	{
		if (LIKELY(HasInstance()))
		{
			return Instance->template DoDespawnSubjects<Paradigm>(Flagmark, ExcludingFlagmark);
		}
		return EApparatusStatus::Noop;
	}

	/**
	 * Despawn all the available subjects
	 * within all of the mechanisms.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	static FORCEINLINE TOutcome<Paradigm>
	DespawnAllSubjects()
	{
		if (LIKELY(HasInstance()))
		{
			return Instance->template DoDespawnAllSubjects<Paradigm>();
		}
		return EApparatusStatus::Noop;
	}

	/**
	 * Reset the machine completely,
	 * removing all of the subjects, unregistering
	 * all of the parts.
	 * 
	 * This will also clear up the network id assignment state
	 * for the subjects completely.
	 * 
	 * @note This won't automatically trigger the
	 * release of the machine, which must be
	 * executed explicitly.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	static TOutcome<Paradigm>
	Reset();

	/**
	 * Release the machine from the root set,
	 * but only if it's no longer needed, i.e. has no
	 * entities registered.
	 * 
	 * You can call this method explicitly
	 * after the UMachine::Reset().
	 * 
	 * @param bReset Should the machine be reset before
	 * releasing? Guarantees the machine to be released.
	 * 
	 * @return The status of the operation.
	 */
	static FORCEINLINE EApparatusStatus 
	ReleaseInstance(const bool bReset = true)
	{
		if (UNLIKELY(!HasInstance()))
		{
			return EApparatusStatus::Noop;
		}
		return Instance->DoReleaseInstance(bReset);
	}

  protected:

	/**
	 * Destroy the machine object.
	 * 
	 * This should happen when the machine is no
	 * longer needed.
	 */
	void BeginDestroy() override;

	/**
	 * The default constructor for the machine.
	 * 
	 * The constructor is made non-public,
	 * cause it shouldn't be called directly
	 * but only through new object creation procedure,
	 * which is called as a result of a machine's
	 * obtainment/retainment flow.
	 */
	UMachine()
	{
		// Pre-allocate all the possible pools for automatic thread-safety:
		SubjectPools.AddDefaulted(AMechanism::InstancesMax + AMechanism::FirstId);
		for (auto Id = AMechanism::FirstId; Id <= AMechanism::LastId; Id += 1)
		{
			FreeMechanismIds.Add(Id);
		}
	}

}; //-class UMachine

#if CPP
#include "CommonSubjectHandle.inl"
#include "Machine.inl"
#endif

#pragma region Traitmark Inlines

FORCEINLINE FTraitInfo::IdType
FTraitmark::GetTraitId(const UScriptStruct* const TraitType)
{
	return UMachine::ObtainTraitId(TraitType);
}

FORCEINLINE const FBitMask&
FTraitmark::GetTraitMask(const UScriptStruct* const TraitType)
{
	return UMachine::ObtainTraitMask(TraitType);
}

FORCEINLINE const FBitMask&
FTraitmark::GetExcludingTraitMask(const UScriptStruct* const TraitType)
{
	return UMachine::ObtainExcludingTraitMask(TraitType);
}

FORCEINLINE
FTraitmark::FTraitmark(UScriptStruct* const TraitType)
  : TraitsMask(UMachine::RegisteredTraitsNum())
{
	check(TraitType);
	Traits.Add(TraitType);
	TraitsMask = GetTraitMask(TraitType); // A slight optimization here.
}

FORCEINLINE
FTraitmark::FTraitmark(std::initializer_list<UScriptStruct*> InTraits)
  : TraitsMask(UMachine::RegisteredTraitsNum())
{
	Add(InTraits);
}

template < typename AllocatorT >
inline
FTraitmark::FTraitmark(const TArray<UScriptStruct*, AllocatorT>& InTraits)
  : TraitsMask(UMachine::RegisteredTraitsNum())
{
	Add(InTraits);
}

FORCEINLINE int32
FTraitmark::RegisteredTraitsNum()
{
	return UMachine::RegisteredTraitsNum();
}

#pragma endregion Traitmark Inlines

#pragma region Detailmark Inlines

FORCEINLINE FDetailInfo::IdType
FDetailmark::GetDetailId(const TSubclassOf<UDetail> DetailClass)
{
	return UMachine::ObtainDetailId(DetailClass);
}

FORCEINLINE const FBitMask&
FDetailmark::GetDetailMask(const TSubclassOf<UDetail> DetailClass)
{
	return UMachine::ObtainDetailMask(DetailClass);
}

FORCEINLINE const FBitMask&
FDetailmark::GetExcludingDetailMask(const TSubclassOf<UDetail> DetailClass)
{
	return UMachine::ObtainExcludingDetailMask(DetailClass);
}

FORCEINLINE const FBitMask&
FDetailmark::GetDetailMask(const UDetail* Detail)
{
	return UMachine::ObtainDetailMask(Detail->GetClass());
}

FORCEINLINE
FDetailmark::FDetailmark(const TSubclassOf<UDetail> DetailClass)
  : DetailsMask(UMachine::RegisteredDetailsNum())
{
	check(DetailClass);
	Add(DetailClass);
}

FORCEINLINE
FDetailmark::FDetailmark(std::initializer_list<TSubclassOf<UDetail>> InDetailsClasses)
  : DetailsMask(UMachine::RegisteredDetailsNum())
{
	Add(InDetailsClasses);
}

template < typename AllocatorT >
FORCEINLINE
FDetailmark::FDetailmark(const TArray<TSubclassOf<UDetail>, AllocatorT>& InDetailClasses)
  : DetailsMask(UMachine::RegisteredDetailsNum())
{
	Add(InDetailClasses);
}

template < typename AllocatorT >
FORCEINLINE
FDetailmark::FDetailmark(const TArray<UDetail*, AllocatorT>& InDetails)
  : DetailsMask(UMachine::RegisteredDetailsNum())
{
	Add(InDetails);
}

FORCEINLINE int32
FDetailmark::RegisteredDetailsNum()
{
	return UMachine::RegisteredDetailsNum();
}

#pragma endregion Detailmark Inlines

#pragma region Trait Info Inlines

FORCEINLINE
FTraitInfo::FTraitInfo(const UScriptStruct* const InType,
					   const IdType               InId)
  : Type(InType)
  , Id(InId)
  , Mask(InId + 1)
  , ExcludingMask(InId + 1)
{
	check(InType != nullptr);
	check(InId != InvalidId);

	Mask.SetAt(Id, true); // Own bit

	// Base classes bits...
	UScriptStruct* BaseType = Cast<UScriptStruct>(InType->GetSuperStruct());
	if (BaseType != nullptr)
	{
		Mask.Include(UMachine::ObtainTraitMask(BaseType));
	}

	ExcludingMask.SetAt(Id, true);
}

#pragma endregion Trait Info Inlines

#pragma region Detail Info Inlines

FORCEINLINE
FDetailInfo::FDetailInfo(const TSubclassOf<UDetail> InClass,
						 const int32                InId)
  : Class(InClass)
  , Id(InId)
  , Mask(InId + 1)
  , ExcludingMask(InId + 1)
{
	check(InClass != nullptr);
	check(InId != InvalidId);

	Mask.SetAt(Id, true); // Own bit

	// Base classes bits...
	TSubclassOf<UDetail> BaseClass = InClass->GetSuperClass();
	if (BaseClass != nullptr)
	{
		Mask.Include(UMachine::ObtainDetailMask(BaseClass));
	}

	ExcludingMask.SetAt(Id, true);
}

#pragma endregion Detail Info Inlines

#pragma region Chunk Inlines

#if CPP
#include "Chunk.inl"
#endif

#pragma endregion Chunk Inlines

#pragma region Mechanism

#if CPP
#include "Mechanism.inl"
#endif

#pragma endregion Mechanism

#pragma region Subject Info

#if CPP
#include "SubjectInfo.inl"
#endif

#pragma endregion Subject Info

#pragma region Common Subject Handle

#if CPP
#include "CommonSubjectHandle.h"
#endif

#pragma endregion Common Subject Handle

#pragma region Iterable

#if CPP
#include "Iterable.inl"
#endif

#pragma endregion Iterable

#pragma region Mechanical

#if CPP
#include "Mechanical.inl"
#endif

#pragma endregion Mechanical

#pragma region Subjective

#if CPP
#include "Subjective.inl"
#endif

#pragma endregion Subjective

#pragma region Network Bearer

#if CPP
#include "NetworkBearerComponent.inl"
#endif

#pragma endregion Network Bearer

#pragma region Subject Record

#if CPP
#include "SubjectRecord.inl"
#endif

#pragma endregion Subject Record

#pragma region Belt Slot

#if CPP
#include "BeltSlot.inl"
#endif

#pragma endregion Belt Slot

#pragma region Belt

#if CPP
#include "Belt.inl"
#endif

#pragma endregion Belt

#pragma region Chain

#if CPP
#include "Chain.inl"
#endif

#pragma endregion Chain

#pragma region Chunk Slot

#if CPP
#include "ChunkSlot.inl"
#endif

#pragma endregion Chunk Slot
