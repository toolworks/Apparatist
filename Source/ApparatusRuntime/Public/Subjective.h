/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Subjective.h
 * Created: Friday, 23rd October 2020 7:00:48 pm
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

#pragma once

#include "More/type_traits"

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "UObject/WeakInterfacePtr.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define SUBJECTIVE_H_SKIPPED_MACHINE_H
#endif

#include "Detail.h"
#include "ConstSubjectHandle.h"

#ifdef SUBJECTIVE_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "Subjective.generated.h"


// Forward declarations:
struct FBeltSlot;
struct FBeltSlotCache;
class UBelt;
class AMechanism;
class ISolidSubjective;

/**
 * The standard subjective property block.
 * 
 * All of the descendants must implement
 * this with fields as UPROPERTY's.
 */
struct APPARATUSRUNTIME_API FStandardSubjectivePropertyBlock
{
	/**
	 * The list of traits.
	 */
	TArray<FTraitRecord> Traits;

	/**
	 * The list of details.
	 */
	TArray<UDetail*> Details;

	/**
	 * An optional preferred belt for the subject to be placed in.
	 */
	UBelt* PreferredBelt = nullptr;

	/**
	 * The flagmark of the subjective.
	 */
	int32 Flagmark = FM_None;

	/**
	 * The mechanism to use as a default one,
	 * when registering the subjective.
	 * 
	 * If not set, the default mechanism
	 * of the world will be used.
	 */
	AMechanism* MechanismOverride = nullptr;
};

/**
 * A block of standard properties used for networking.
 */
struct APPARATUSRUNTIME_API FStandardSubjectiveNetworkPropertyBlock
{
	/**
	 * The list of traits allowed to be received on the server.
	 * 
	 * Only for networkable subjectives.
	 */
	FTraitmark TraitmarkPermit;

	/**
	 * The replicated network identifier
	 * of the subject.
	 * 
	 * Only for networkable subjectives.
	 */
	uint32 SubjectNetworkId = FSubjectNetworkState::InvalidId;
};

#define ASSERT_SUBJECTIVE_PROPERTY_BLOCK(ClassT)                                   \
	static constexpr size_t StandardBlockOffset = offsetof(ClassT, Traits);        \
	static_assert(offsetof(FStandardSubjectivePropertyBlock, Traits) ==            \
				  offsetof(ClassT, Traits) - StandardBlockOffset,                  \
				  "The traits offsets must match.");                               \
	static_assert(offsetof(FStandardSubjectivePropertyBlock, Details) ==           \
				  offsetof(ClassT, Details) - StandardBlockOffset,                 \
				  "The details offsets must match.");                              \
	static_assert(offsetof(FStandardSubjectivePropertyBlock, PreferredBelt) ==     \
				  offsetof(ClassT, PreferredBelt) - StandardBlockOffset,           \
				  "The preferred belt offsets must match.");                       \
	static_assert(offsetof(FStandardSubjectivePropertyBlock, Flagmark) ==          \
				  offsetof(ClassT, Flagmark) - StandardBlockOffset,                \
				  "The flagmark offsets must match.");                             \
	static_assert(offsetof(FStandardSubjectivePropertyBlock, MechanismOverride) == \
				  offsetof(ClassT, MechanismOverride) - StandardBlockOffset,       \
				  "The mechanism override offsets must match.")                    \

#define ASSERT_NETWORKED_SUBJECTIVE_PROPERTY_BLOCK(ClassT)                                   \
	ASSERT_SUBJECTIVE_PROPERTY_BLOCK(ClassT);                                                \
	static constexpr size_t StandardNetworkBlockOffset = offsetof(ClassT, TraitmarkPermit);  \
	static_assert(offsetof(FStandardSubjectiveNetworkPropertyBlock, TraitmarkPermit) ==      \
				  offsetof(ClassT, TraitmarkPermit) - StandardNetworkBlockOffset,            \
				  "The traitmark permit offsets must match.");                               \
	static_assert(offsetof(FStandardSubjectiveNetworkPropertyBlock, SubjectNetworkId) ==     \
				  offsetof(ClassT, SubjectNetworkId) - StandardNetworkBlockOffset,           \
				  "The subject network identifier offsets must match.")

/**
 * Finish the standard subjective properties specification.
 * 
 * Checks the necessary data and its layout.
 * Provides the standard properties block accessor.
 * 
 * @param ClassT A class that actually implements the subjective.
 */
#define END_STANDARD_SUBJECTIVE_PROPERTY_BLOCK(ClassT)                         \
FORCEINLINE FStandardSubjectivePropertyBlock*                                  \
DoGetStandardPropertyBlock() override                                          \
{                                                                              \
	ASSERT_SUBJECTIVE_PROPERTY_BLOCK(ClassT);                                  \
	return (FStandardSubjectivePropertyBlock*)std::addressof(Traits);          \
}                                                                              \
FORCEINLINE FStandardSubjectivePropertyBlock*                                  \
DoGetStandardPropertyBlock() const override                                    \
{                                                                              \
	return (FStandardSubjectivePropertyBlock*)std::addressof(Traits);          \
}

/**
 * Finish the standard networked subjective properties specification.
 * 
 * Checks the necessary data and its layout.
 * Provides the standard properties block accessor.
 * 
 * @param ClassT A class that actually implements the networked subjective.
 */
#define END_STANDARD_NETWORKED_SUBJECTIVE_PROPERTY_BLOCK(ClassT)                      \
FORCEINLINE bool IsNetworkCapable() const override                                    \
{                                                                                     \
	return true;                                                                      \
}                                                                                     \
FORCEINLINE FStandardSubjectivePropertyBlock*                                         \
DoGetStandardPropertyBlock() override                                                 \
{                                                                                     \
	ASSERT_NETWORKED_SUBJECTIVE_PROPERTY_BLOCK(ClassT);                               \
	return (FStandardSubjectivePropertyBlock*)std::addressof(Traits);                 \
}                                                                                     \
FORCEINLINE FStandardSubjectivePropertyBlock*                                         \
DoGetStandardPropertyBlock() const override                                           \
{                                                                                     \
	return (FStandardSubjectivePropertyBlock*)std::addressof(Traits);                 \
}                                                                                     \
FORCEINLINE FStandardSubjectiveNetworkPropertyBlock*                                  \
DoGetStandardNetworkPropertyBlock() override                                          \
{                                                                                     \
	ASSERT_NETWORKED_SUBJECTIVE_PROPERTY_BLOCK(ClassT);                               \
	return (FStandardSubjectiveNetworkPropertyBlock*)std::addressof(TraitmarkPermit); \
}                                                                                     \
FORCEINLINE FStandardSubjectiveNetworkPropertyBlock*                                  \
DoGetStandardNetworkPropertyBlock() const override                                    \
{                                                                                     \
	return (FStandardSubjectiveNetworkPropertyBlock*)std::addressof(TraitmarkPermit); \
}

/**
 * @internal Exists only for Unreal Engine's reflection system. Do not use directly.
 */
UINTERFACE(MinimalAPI, BlueprintType)
class USubjective : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for all sorts of subjectives.
 * 
 * All of the subjectives are essentially
 * subjects design-wise and inherit all of
 * their functionality internally.
 * 
 * @note The current semantic for the constant
 * subjectives is basically being solid in terms
 * of iterating on them.
 */
class APPARATUSRUNTIME_API ISubjective
{
	GENERATED_BODY()

  public:

	/**
	 * The type of the slot index.
	 */
	typedef int32 SlotIndexType;

	/**
	 * Invalid belt slot index.
	 */
	static constexpr SlotIndexType InvalidSlotIndex = -1;

  private:

	friend struct FFingerprint;
	friend struct FBeltSlot;
	friend struct FBeltSlotCache;
	friend struct FSubjectInfo;
	friend class UDetail;
	friend class UBelt;
	friend class AMechanism;
	friend class UMachine;
	friend class UApparatusFunctionLibrary;
	friend class ISolidSubjective;

	static TArray<UDetail*> EmptyDetails;

	static TArray<FTraitRecord> EmptyTraits;

  protected:

	/**
	 * A reference to a current subjective's belt (if any).
	 * 
	 * If is equal to @c nullptr, the subjective
	 * is not a part of any belt.
	 */
	UBelt* Belt = nullptr;

	/**
	 * The index of the belt slot, this subject currently resides in.
	 */
	SlotIndexType SlotIndex = InvalidSlotIndex;

	/**
	 * The subject handle which points to a global subjects registry.
	 */
	FSubjectHandle Handle;

	/**
	 * Was the subjective already unregistered via
	 * the normal DoUnregister() means.
	 */
	bool bUnregistered = false;

	/**
	 * Set the subjective as booted.
	 * 
	 * @return The previous state of the booted flag.
	 */
	bool
	MarkBooted();

	/**
	 * Set or reset the current belt slot of the subjective.
	 * 
	 * This is a simple internal fields setter with a minimal logic.
	 * You may use this method to reset the slot.
	 * 
	 * @param InBelt The belt to set. May be a @c nullptr in order
	 * to reset the slot.
	 * @param InSlotIndex The slot index to set.
	 * May be a InvalidSlotIndex if the first argument is @c nullptr.
	 */
	FORCEINLINE void
	TakeBeltSlot(UBelt* const InBelt,
				 const int32  InSlotIndex)
	{
		if (InBelt == nullptr)
		{
			// Reset the slot assignment.
			Belt      = nullptr;
			check(InSlotIndex == InvalidSlotIndex);
			SlotIndex = InvalidSlotIndex;
		}
		else
		{
			check(InSlotIndex > InvalidSlotIndex);
			Belt      = InBelt;
			SlotIndex = InSlotIndex;
		}
	}

#pragma region Property Block

	virtual FStandardSubjectivePropertyBlock*
	DoGetStandardPropertyBlock()
	{
		unimplemented();
		return nullptr;
	}

	virtual FStandardSubjectivePropertyBlock*
	DoGetStandardPropertyBlock() const
	{
		unimplemented();
		return nullptr;
	}

	mutable FStandardSubjectivePropertyBlock* StandardPropertyBlockCache = nullptr;

	FORCEINLINE FStandardSubjectivePropertyBlock&
	GetStandardPropertyBlock()
	{
		if (UNLIKELY(!StandardPropertyBlockCache))
		{
			StandardPropertyBlockCache = DoGetStandardPropertyBlock();
		}
		return *StandardPropertyBlockCache;
	}

	FORCEINLINE const FStandardSubjectivePropertyBlock&
	GetStandardPropertyBlock() const
	{
		if (UNLIKELY(!StandardPropertyBlockCache))
		{
			StandardPropertyBlockCache = DoGetStandardPropertyBlock();
		}
		return *StandardPropertyBlockCache;
	}

#pragma endregion Property Block

#pragma region Network Property Block

	virtual FStandardSubjectiveNetworkPropertyBlock*
	DoGetStandardNetworkPropertyBlock()
	{
		unimplemented();
		return nullptr;
	}

	virtual FStandardSubjectiveNetworkPropertyBlock*
	DoGetStandardNetworkPropertyBlock() const
	{
		unimplemented();
		return nullptr;
	}

	mutable FStandardSubjectiveNetworkPropertyBlock* StandardNetworkPropertyBlockCache = nullptr;

	FORCEINLINE FStandardSubjectiveNetworkPropertyBlock&
	GetStandardNetworkPropertyBlock()
	{
		check(IsNetworkCapable());
		if (UNLIKELY(!StandardNetworkPropertyBlockCache))
		{
			StandardNetworkPropertyBlockCache = DoGetStandardNetworkPropertyBlock();;
		}
		return *StandardNetworkPropertyBlockCache;
	}

	FORCEINLINE const FStandardSubjectiveNetworkPropertyBlock&
	GetStandardNetworkPropertyBlock() const
	{
		check(IsNetworkCapable());
		if (UNLIKELY(!StandardNetworkPropertyBlockCache))
		{
			StandardNetworkPropertyBlockCache = DoGetStandardNetworkPropertyBlock();;
		}
		return *StandardNetworkPropertyBlockCache;
	}

#pragma endregion Network Property Block

#pragma region Direct Property Access

	/**
	 * Get the internal flagmark property reference.
	 * Constant version.
	 */
	FORCEINLINE const EFlagmark&
	GetFlagmarkPropRef() const
	{
		static_assert(sizeof(EFlagmark) == sizeof(int32), "Flagmark size must be that of a 32-bit integer.");
		return reinterpret_cast<const EFlagmark&>(GetStandardPropertyBlock().Flagmark);
	}

	/**
	 * Get the internal flagmark property reference.
	 */
	FORCEINLINE EFlagmark&
	GetFlagmarkPropRef()
	{
		static_assert(sizeof(EFlagmark) == sizeof(int32), "Flagmark size must be that of a 32-bit integer.");
		return reinterpret_cast<EFlagmark&>(GetStandardPropertyBlock().Flagmark);
	}

	/**
	 * Get the mechanism override property of the subjective.
	 * Constant version.
	 */
	FORCEINLINE auto&
	GetMechanismOverridePropRef() const
	{
		return GetStandardPropertyBlock().MechanismOverride;
	}

	/**
	 * Get the mechanism override property of the subjective.
	 */
	FORCEINLINE auto&
	GetMechanismOverridePropRef()
	{
		return GetStandardPropertyBlock().MechanismOverride;
	}

	/**
	 * Direct access for the internal traits array.
	 * Mutable version.
	 */
	FORCEINLINE TArray<FTraitRecord>&
	GetTraitRecordsRef()
	{
		return GetStandardPropertyBlock().Traits;
	}

	/**
	 * Direct access for the internal details array.
	 */
	FORCEINLINE TArray<UDetail*>&
	GetDetailsRef()
	{
		return GetStandardPropertyBlock().Details;
	}

#pragma endregion Direct Property Access

	/**
	 * Reset the belt slot.
	 */
	FORCEINLINE void
	TakeBeltSlot(TYPE_OF_NULLPTR)
	{
		TakeBeltSlot(nullptr, InvalidSlotIndex);
	}

	/**
	 * Assign a unique network identifier to
	 * the subject of the subjective. The standard implementation for the RPC.
	 */
	void
	ClientReceiveNetworkId_Implementation(const uint32 NetworkId);

	/**
	 * Request a unique network identifier for
	 * the subject of the subjective. The standard implementation for the RPC.
	 */
	void
	ServerRequestNetworkId_Implementation();

	/**
	 * Receive a trait on the server or the client.
	 * The standard implementation for the RPC.
	 */
	void
	PeerReceiveTrait_Implementation(EPeerRole            PeerRole,
									UScriptStruct* const TraitType,
									const TArray<uint8>& TraitData);

	/**
	 * Validate receiving a trait on the server or the client.
	 * The standard implementation for the RPC validation.
	 */
	bool
	PeerReceiveTrait_Validate(EPeerRole            PeerRole,
							  UScriptStruct* const TraitType,
							  const TArray<uint8>& TraitData);

	/**
	 * @internal Send an existing trait to a remote peer.
	 * Immutable version.
	 * 
	 * On a client the trait can only be sent from a current connection's
	 * owned actor, i.e. a pawn or a player controller.
	 * 
	 * Executes in a safe polite paradigm.
	 * 
	 * @param TraitType The type of the trait to send.
	 * Must not be a @c nullptr.
	 * @param bSetForLocal Should the trait be also
	 * set for the local version of the subject.
	 * @param PeerRole The explicit peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should reliable channel be used
	 * for the transaction.
	 * @return The outcome of the operation.
	 */
	virtual TPortableOutcome<>
	DoPushTrait(const EParadigm      Paradigm,
				UScriptStruct* const TraitType,
				const EPeerRole      PeerRole = EPeerRole::Auto,
				const bool           bReliable = true) const
	{
		AssessCondition(Paradigm, Handle.IsOnline(), EApparatusStatus::InvalidState);
		if (IsHarsh(Paradigm))
		{
			Handle.PushTrait<EParadigm::HarshSafe>(TraitType, PeerRole, bReliable);
			return EApparatusStatus::Success;
		}
		else
		{
			return Handle.PushTrait<EParadigm::Polite>(TraitType, PeerRole, bReliable);
		}
	}

	/**
	 * @internal Send an additional trait to a remote peer.
	 * 
	 * On a client the trait can only be sent from a current connection's
	 * owned actor, i.e. a pawn or a player controller.
	 * 
	 * Executes in a safe polite paradigm.
	 * 
	 * @param TraitType The type of the trait to send.
	 * Must not be a @c nullptr.
	 * @param TraitData The data of the trait to send.
	 * Must not be a @c nullptr.
	 * @param bSetForLocal Should the trait be also
	 * set for the local version of the subject.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should reliable channel be used
	 * for the transaction.
	 * @return The outcome of the operation.
	 */
	virtual TPortableOutcome<>
	DoPushTrait(const EParadigm      Paradigm,
				UScriptStruct* const TraitType,
				const void* const    TraitData,
				const bool           bSetForLocal = false,
				const EPeerRole      PeerRole = EPeerRole::Auto,
				const bool           bReliable = true)
	{
		AssessConditionFormat(Paradigm, bSetForLocal, EApparatusStatus::InvalidArgument,
							  TEXT("The 'bSetForLocal' argument is deprecated. Set the trait explicitly and clear this flag, please."));
		AssessCondition(Paradigm, Handle.IsOnline(), EApparatusStatus::InvalidState);
		if (IsHarsh(Paradigm))
		{
			Handle.PushTrait<EParadigm::HarshSafe>(TraitType, TraitData, PeerRole, bReliable);
			return EApparatusStatus::Success;
		}
		else
		{
			return Handle.PushTrait<EParadigm::Polite>(TraitType, TraitData, PeerRole, bReliable);
		}
	}

	/**
	 * Assign a network identifier on a client.
	 * 
	 * Should be implemented via a client RPC.
	 */
	FORCEINLINE virtual void
	AssignNetworkIdOnClient(const uint32 NetworkId)
	{
		unimplemented();
	}

	/**
	 * Obtain a networking subject identifier from the server.
	 * 
	 * Should be implemented via a client RPC.
	 */
	FORCEINLINE virtual void
	ObtainNetworkIdFromServer()
	{
		unimplemented();
	}

	/**
	 * Change the current slot of the subjective,
	 * while preserving the belt.
	 * 
	 * This is a simple internal fields setter with minimal logic.
	 * 
	 * @param InSlotIndex The slot index to set.
	 */
	FORCEINLINE void
	TakeBeltSlot(const int32 InSlotIndex)
	{
		check(!Belt || (InSlotIndex > InvalidSlotIndex));
		SlotIndex = InSlotIndex;
	}

	/**
	 * Get the active belt of the subjective (if any).
	 */
	FORCEINLINE UBelt*
	GetBelt() const
	{
		check(!Belt || (SlotIndex > InvalidSlotIndex));
		return Belt;
	}

	/**
	 * Get the internal fingerprint of the subjective.
	 */
	FORCEINLINE FFingerprint& 
	GetFingerprintRef()
	{
		if (LIKELY(Handle))
		{
			return Handle.GetInfo().GetFingerprintRef();
		}
		checkNoEntry();
		return *(new FFingerprint());
	}

	/**
	 * Get the internal fingerprint of the subjective. Constant version.
	 */
	FORCEINLINE const FFingerprint& 
	GetFingerprintRef() const
	{
		if (LIKELY(Handle))
		{
			return Handle.GetInfo().GetFingerprintRef();
		}
		checkNoEntry();
		return *(new FFingerprint());
	}

	/**
	 * Get the current belt slot of the subjective (if any).
	 *
	 * @return The active belt slot of the subject or <c>nullptr</c>,
	 * if the subject is not (yet) part of any belt.
	 */
	struct FBeltSlot*
	GetSlotPtr();

	/**
	 * Perform an actual registration of the subjective.
	 * 
	 * Calls the UMechanism::RegisterSubjective() method in turn.
	 */
	void DoRegister();

	/**
	 * Perform an actual de-registration procedure on the subjective.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	DoUnregister();

	/**
	 * Begin the serialization routine.
	 */
	void DoStartSerialization(FArchive& Archive);

	/**
	 * End the serialization routine.
	 */
	void DoFinishSerialization(FArchive& Archive);

	FORCEINLINE bool
	DoesHaveTraitRecord(UScriptStruct* const TraitType) const
	{
		const auto& Traits = GetTraitRecordsRef();
		for (const auto& TraitRecord : Traits)
		{
			if (TraitRecord.GetType() == TraitType)
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * Get the traitmark based on the existing trait records.
	 */
	FORCEINLINE FTraitmark
	DoGetTraitRecordsTraitmark() const
	{
		FTraitmark Traitmark;
		const auto& Traits = GetTraitRecordsRef();
		for (const auto& TraitRecord : Traits)
		{
			const auto TraitType = TraitRecord.GetType();
			if (LIKELY(TraitType))
			{
				Traitmark.Add(TraitType);
			}
		}
		return MoveTemp(Traitmark);
	}

	FORCEINLINE const FTraitRecord*
	DoFindTraitRecord(UScriptStruct* const TraitType) const
	{
		if (UNLIKELY(TraitType == nullptr))
		{
			return nullptr;
		}
		auto& Traits = GetTraitRecordsRef();
		for (const auto& TraitRecord : Traits)
		{
			if (TraitRecord.GetType() == TraitType)
			{
				return &TraitRecord;
			}
		}
		return nullptr;
	}

	FORCEINLINE EApparatusStatus
	DoGetTraitRecord(UScriptStruct* const TraitType,
					 void* const          OutTraitData,
					 const bool           bTraitDataInitialized = true) const
	{
		auto& Traits = GetTraitRecordsRef();
		for (const auto& TraitRecord : Traits)
		{
			if (TraitRecord.GetType() == TraitType)
			{
				TraitRecord.GetData(OutTraitData, bTraitDataInitialized);
				return EApparatusStatus::Success;
			}
		}
		return EApparatusStatus::Missing;
	}

	template < typename T >
	FORCEINLINE EApparatusStatus
	DoGetTraitRecord(T* const   OutTraitData,
					 const bool bTraitDataInitialized = true) const
	{
		auto& Traits = GetTraitRecordsRef();
		for (const auto& TraitRecord : Traits)
		{
			if (TraitRecord.GetType() == T::StaticStruct())
			{
				TraitRecord.GetData(OutTraitData, bTraitDataInitialized);
				return EApparatusStatus::Success;
			}
		}
		return EApparatusStatus::Missing;
	}

	template < typename T >
	FORCEINLINE EApparatusStatus
	DoGetTraitRecord(T&         OutTraitData,
					 const bool bTraitDataInitialized = true) const
	{
		auto& Traits = GetTraitRecordsRef();
		for (const auto& TraitRecord : Traits)
		{
			if (TraitRecord.GetType() == T::StaticStruct())
			{
				TraitRecord.GetData(OutTraitData, bTraitDataInitialized);
				return EApparatusStatus::Success;
			}
		}
		return EApparatusStatus::Missing;
	}

	FORCEINLINE EApparatusStatus
	DoSetTraitRecord(UScriptStruct* TraitType,
			 		 const void*    TraitData)
	{
		if (!TraitType && !TraitData)
		{
			return EApparatusStatus::Noop;
		}
		check(TraitType && TraitData);
		if (TraitType == FTraitRecord::StaticStruct())
		{
			const auto TraitRecord = static_cast<const FTraitRecord*>(TraitData);
			TraitType = TraitRecord->GetType();
			TraitData = TraitRecord->GetData();
		}
		auto& Traits = GetTraitRecordsRef();
		// Find among existing records...
		for (auto& TraitRecord : Traits)
		{
			if (TraitRecord.GetType() == TraitType)
			{
				TraitRecord.SetData(TraitData);
				return EApparatusStatus::Success;
			}
		}
		Traits.Add(FTraitRecord(TraitType, TraitData));
		return EApparatusStatus::Success;
	}

	template < typename T >
	FORCEINLINE EApparatusStatus
	DoSetTraitRecord(const T& InTrait)
	{
		return DoSetTraitRecord(T::StaticStruct(), static_cast<const void*>(&InTrait));
	}

	template < typename T >
	FORCEINLINE EApparatusStatus
	DoSetTraitRecord(const T* const InTrait)
	{
		return DoSetTraitRecord(T::StaticStruct(), static_cast<const void*>(InTrait));
	}

	FORCEINLINE const FTraitRecord&
	DoObtainTraitRecord(UScriptStruct* const TraitType)
	{
		check(TraitType);
		auto& Traits = GetTraitRecordsRef();
		for (const auto& TraitRecord : Traits)
		{
			if (TraitRecord.GetType() == TraitType)
			{
				return TraitRecord;
			}
		}
		return Traits.Add_GetRef(FTraitRecord(TraitType));
	}

	FORCEINLINE EApparatusStatus
	DoObtainTraitRecord(UScriptStruct* const TraitType,
						void* const          OutTraitData,
						const bool           bTraitDataInitialized = true)
	{
		check(TraitType);
		check(OutTraitData);
		DoObtainTraitRecord(TraitType).GetData(OutTraitData, bTraitDataInitialized);
		return EApparatusStatus::Success;
	}

	FORCEINLINE EApparatusStatus
	DoRemoveTraitRecord(UScriptStruct* const TraitType)
	{
		if (TraitType == nullptr)
		{
			return EApparatusStatus::Noop;
		}
		auto Status = EApparatusStatus::Noop;
		auto& Traits = GetTraitRecordsRef();
		for (int32 i = 0; i < Traits.Num(); ++i)
		{
			if (Traits[i].GetType() == TraitType)
			{
				Traits.RemoveAt(i--);
				Status = EApparatusStatus::Success;
			}
		}
		return Status;
	}

	FORCEINLINE EApparatusStatus
	DoRemoveAllTraitRecords()
	{
		auto& Traits = GetTraitRecordsRef();
		if (UNLIKELY(Traits.Num() == 0))
		{
			return EApparatusStatus::Noop;
		}
		Traits.Reset();
		return EApparatusStatus::Success;
	}

	/**
	 * Check if the entity be replicated.
	 * 
	 * Should be overriden in the descendants
	 * to explicitly state the the network
	 * functionality is needed on the subjective.
	 */
	virtual FORCEINLINE bool
	ShouldBeReplicated() const
	{
		return false;
	}

	/**
	 * Push a trait to the remote version of the subject.
	 * Standard implementation.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam S The type of the subjective to issue from.
	 * @param Subjective The subjective to use.
	 * @param TraitType The type of the trait to push.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should reliable channel be used for transaction.
	 * @param ServerReceiveTraitRPC The RPC to receive the trait on a server.
	 * @param ClientReceiveTraitRPC The RPC to receive the trait on a client.
	 * @param ServerReceiveTraitUnreliableRPC The RPC to receive the trait on a server unreliably.
	 * @param ClientReceiveTraitUnreliableRPC The RPC to receive the trait on a client unreliably.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal, class S = void >
	static TOutcome<Paradigm>
	DoPushTrait(const S* const       Subjective,
				UScriptStruct* const TraitType,
				EPeerRole            PeerRole,
				const bool           bReliable,
				void (S::*ServerReceiveTraitRPC)(UScriptStruct*       TraitType,
												 const TArray<uint8>& TraitData),
				void (S::*ClientReceiveTraitRPC)(UScriptStruct*       TraitType,
												 const TArray<uint8>& TraitData),
				void (S::*ServerReceiveTraitUnreliableRPC)(UScriptStruct*       TraitType,
														   const TArray<uint8>& TraitData),
				void (S::*ClientReceiveTraitUnreliableRPC)(UScriptStruct*       TraitType,
														   const TArray<uint8>& TraitData));

	/**
	 * Push a trait to the remote version of the subject.
	 * Standard implementation.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam S The type of the subjective to issue from.
	 * @param Subjective The subjective to use.
	 * @param TraitType The type of the trait to push.
	 * @param TraitData The trait data to push.
	 * @param bSetForLocal Should the trait also be set for the local
	 * version of the subject.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should reliable channel be used for transaction.
	 * @param ServerReceiveTraitRPC The RPC to receive the trait on a server.
	 * @param ClientReceiveTraitRPC The RPC to receive the trait on a client.
	 * @param ServerReceiveTraitUnreliableRPC The RPC to receive the trait on a server unreliably.
	 * @param ClientReceiveTraitUnreliableRPC The RPC to receive the trait on a client unreliably.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal, class S = void >
	static TOutcome<Paradigm>
	DoPushTrait(S* const             Subjective,
				UScriptStruct* const TraitType,
				const void* const    TraitData,
				bool                 bSetForLocal,
				EPeerRole            PeerRole,
				const bool           bReliable,
				void (S::*ServerReceiveTraitRPC)(UScriptStruct*       TraitType,
												 const TArray<uint8>& TraitData),
				void (S::*ClientReceiveTraitRPC)(UScriptStruct*       TraitType,
												 const TArray<uint8>& TraitData),
				void (S::*ServerReceiveTraitUnreliableRPC)(UScriptStruct*       TraitType,
														   const TArray<uint8>& TraitData),
				void (S::*ClientReceiveTraitUnreliableRPC)(UScriptStruct*       TraitType,
														   const TArray<uint8>& TraitData));

  public:

	/**
	 * Get the mechanism this subjective
	 * is currently part of (if registered).
	 */
	AMechanism*
	GetMechanism() const;

	/**
	 * Set the mechanism of the subjective.
	 * 
	 * For the actor-based subjectives the new
	 * mechanism must reside within the same world.
	 * 
	 * This procedure will essentially move
	 * the traits and the details to the
	 * new mechanism's own iterables.
	 * 
	 * @tparam Paradigm The safety paradigm to use.
	 * @param Mechanism The mechanism to set to.
	 * Must not be a @c nullptr.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Safe >
	TOutcome<Paradigm>
	SetMechanism(AMechanism* const Mechanism);

	/**
	 * Direct access for the internal traits array. Constant version.
	 */
	FORCEINLINE const TArray<FTraitRecord>&
	GetTraitRecordsRef() const
	{
		return GetStandardPropertyBlock().Traits;
	}

	/**
	 * Direct access for the internal details array. Constant version.
	 */
	virtual FORCEINLINE const TArray<UDetail*>&
	GetDetailsRef() const
	{
		return GetStandardPropertyBlock().Details;
	}

	/**
	 * Get the active fingerprint of the subjective. Constant version.
	 */
	const FFingerprint&
	GetFingerprint() const
	{
		if (LIKELY(Handle))
		{
			return Handle.GetFingerprint();
		}
		// The subjective is not registered just yet.
		// Assemble a temporary fingerprint as a workaround...
		static thread_local FFingerprint FingerprintTmp;
		FingerprintTmp.Reset(GetFlagmark());
		const auto& Traits = GetTraitRecordsRef();
		for (const auto& TraitRecord : Traits)
		{
			const auto TraitType = TraitRecord.GetType();
			if (LIKELY(TraitType))
			{
				FingerprintTmp.Add(TraitType);
			}
		}
		const auto& Details = GetDetailsRef();
		FingerprintTmp.Add(Details);
		return FingerprintTmp;
	}

	/**
	 * Check if the subjective matches a supplied filter.
	 */
	FORCEINLINE bool
	Matches(const FFilter& InFilter) const
	{
		const FFingerprint& Fingerprint = GetFingerprint();
		return Fingerprint.Matches(InFilter);
	}

	/**
	 * Check if the subjective matches a supplied traitmark as a filter.
	 */
	FORCEINLINE bool
	Matches(const FTraitmark& InTraitmark) const
	{
		return GetFingerprint().Matches(InTraitmark);
	}

	/**
	 * Check if the subjective matches a supplied detailmark as a filter.
	 */
	FORCEINLINE bool
	Matches(const FDetailmark& InDetailmark) const
	{
		return GetFingerprint().Matches(InDetailmark);
	}

#pragma region Flagmark
	/// @name Flagmark
	/// @{

	/**
	 * Get the current flagmark of the subjective.
	 * 
	 * @return The current flagmark of the subjective.
	 */
	FORCEINLINE EFlagmark
	GetFlagmark() const
	{
		if (LIKELY(Handle))
		{
			return Handle.GetFlagmark();
		}
		// The subjective is not registered just yet.
		// Return the sanitized property's value.
		return GetFlagmarkPropRef() & FM_AllUserLevel;
	}

	/**
	 * Set the current flagmark for the subjective.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFlagmark The flagmark to set to.
	 * Must not contain any system-level flags.
	 * @return The previous flagmark state.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm, EFlagmark>
	SetFlagmark(const EFlagmark InFlagmark)
	{
		if (LIKELY(Handle))
		{
			return Handle.template SetFlagmark<Paradigm>(InFlagmark);			
		}

		// The subjective is not registered just yet.
		// Set the property for now...
		auto& Prop = GetFlagmarkPropRef();

		AssessCondition(Paradigm, IsInternal(Paradigm) || !HasSystemLevelFlags(InFlagmark),
						(MakeOutcome<Paradigm, EFlagmark>(EApparatusStatus::InvalidArgument, Prop)));

		if (UNLIKELY(Prop == InFlagmark))
		{
			return MakeOutcome<Paradigm, EFlagmark>(EApparatusStatus::Noop, Prop);
		}

		const auto Prev = Prop;
		Prop = InFlagmark;
		return MakeOutcome<Paradigm, EFlagmark>(EApparatusStatus::Success, Prev);
	}

	/**
	 * Set the current flagmark for the subjective
	 * in a masked fashion.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFlagmark The flagmark to set to.
	 * Must not contain any system-level flags.
	 * @param InMask The mask to apply while setting.
	 * @return The previous flagmark state with the
	 * passed-in mask applied.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm, EFlagmark>
	SetFlagmarkMasked(const EFlagmark InFlagmark,
					  const EFlagmark InMask = FM_AllUserLevel)
	{
		if (LIKELY(Handle))
		{
			return Handle.template SetFlagmarkMasked<Paradigm>(InFlagmark, InMask);
		}

		// The subjective is not registered just yet.
		// Set the property for now...
		auto& Prop = GetFlagmarkPropRef();

		AssessCondition(Paradigm, !HasSystemLevelFlags(InFlagmark & InMask),
					(MakeOutcome<Paradigm, EFlagmark>(EApparatusStatus::InvalidArgument, Prop & InMask)));

		if (IsPolite(Paradigm) && ((Prop & InMask) == (InFlagmark & InMask))) // Semi-compile-time branch.
		{
			return MakeOutcome<Paradigm, EFlagmark>(EApparatusStatus::Noop, Prop & InMask);
		}

		const auto Prev = Prop;
		Prop = (Prop & ~InMask) | (InFlagmark & InMask);
		return MakeOutcome<Paradigm, EFlagmark>(EApparatusStatus::Success, Prev & InMask);
	}

	/**
	 * Get the current state of a flag for the subjective.
	 * 
	 * @param Flag The flag to get the state of.
	 * @return The state of the flag.
	 */
	FORCEINLINE bool
	HasFlag(const EFlagmarkBit Flag) const
	{
		if (LIKELY(Handle))
		{
			return Handle.HasFlag(Flag);
		}
		// The subjective is not registered just yet.
		// Return the property's state.
		return EnumHasFlag(GetFlagmarkPropRef(), Flag);
	}

	/**
	 * Set a flag for the subjective.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flag The flag to set. Must not be a system-level one.
	 * @param bState The state of the flag to set.
	 * @return The previous state of the flag.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm, bool>
	SetFlag(const EFlagmarkBit Flag, const bool bState = true)
	{
		if (LIKELY(Handle))
		{
			// The flag is checked here:
			return Handle.template SetFlag<Paradigm>(Flag, bState);
		}

		// The subjective is not registered just yet.
		// Change the property field for now...
		auto& Prop = GetFlagmarkPropRef();

		AssessCondition(Paradigm, IsInternal(Paradigm) || !IsSystemLevel(Flag),
						(MakeOutcome<Paradigm, bool>(EApparatusStatus::InvalidArgument, EnumHasFlag(Prop, Flag))));
		
		const auto NewFlagmark = EnumWithFlagSet(Prop, Flag, bState);
		if (UNLIKELY(Prop == NewFlagmark))
		{
			return MakeOutcome<Paradigm, bool>(EApparatusStatus::Noop, EnumHasFlag(Prop, Flag));
		}
		Prop = NewFlagmark;
		return MakeOutcome<Paradigm, bool>(EApparatusStatus::Success, !EnumHasFlag(Prop, Flag));
	}

	/**
	 * Toggle a flag for the subjective.
	 * 
	 * @tparam The paradigm to work under.
	 * @param Flag The flag to toggle. Must not be a system-level one.
	 * @return The new state of the flag.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm, bool>
	ToggleFlag(const EFlagmarkBit Flag)
	{
		if (LIKELY(Handle))
		{
			// The flag is checked here:
			return Handle.template ToggleFlag<Paradigm>(Flag);
		}

		// The subjective is not registered just yet.
		// Change the property field for now.
		if (AvoidConditionFormat(
				Paradigm, !IsInternal(Paradigm) && IsSystemLevel(Flag),
				TEXT("Toggling system-level flags is not allowed on the '%s' subjective."),
				*(CastChecked<UObject>(this)->GetName())))
		{
			return MakeOutcome<Paradigm, bool>(EApparatusStatus::InvalidArgument, HasFlag(Flag));
		}
		auto& Prop = GetFlagmarkPropRef();
		const auto NewFlagmark = EnumWithFlagToggled(Prop, Flag);
		Prop = NewFlagmark;
		return MakeOutcome<Paradigm, bool>(EApparatusStatus::Success, EnumHasFlag(Prop, Flag));
	}

	/// @}
#pragma endregion Flagmark

#pragma region Traits
	/// @name Traits
	/// @{

	/**
	 * Check if the subjective has a certain trait.
	 */
	FORCEINLINE bool
	HasTrait(UScriptStruct* const TraitType) const
	{
		if (LIKELY(Handle))
		{
			return Handle.HasTrait(TraitType);
		}
		
		// Find among existing records...
		return DoesHaveTraitRecord(TraitType);
	}

	/**
	 * Check if there is a trait of a specific type in the subjective. Templated version.
	 */
	template < typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE bool
	HasTrait() const
	{
		return HasTrait(T::StaticStruct());
	}

	/**
	 * Set a trait of the subjective by its type.
	 * Dynamically typed version.
	 * 
	 * If the trait is not currently within the subjective,
	 * it gets created anew and set accordingly
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to add.
	 * @param TraitData The trait data to initialize with.
	 * May not be a @c nullptr, if the @p TraitType is not a @c nullptr.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	SetTrait(UScriptStruct* const TraitType,
			 const void* const    TraitData)
	{
		AssessCondition(Paradigm, IsValidSubjective(this), EApparatusStatus::InvalidState);

		if (LIKELY(Handle))
		{
			return Handle.template SetTrait<Paradigm>(TraitType, TraitData);
		}

		return DoSetTraitRecord(TraitType, TraitData);
	}
	
	/**
	 * Set a trait of the subjective.
	 * Statically typed version.
	 * 
	 * If the trait is not currently within the subjective,
	 * it gets created anew and set accordingly.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to set.
	 * @param Trait The trait to initialize with.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	SetTrait(const T& Trait)
	{
		AssessCondition(Paradigm, IsValidSubjective(this), EApparatusStatus::InvalidState);

		if (LIKELY(Handle))
		{
			return Handle.template SetTrait<Paradigm, T>(Trait);
		}

		return DoSetTraitRecord(Trait);
	}

	/**
	 * Get a trait from the subjective by its type.
	 * 
	 * If the trait is missing within the subject,
	 * the behavior is undefined and an assertion may be raised.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] TraitType The type of the trait to get.
	 * @param[out] OutTraitData The trait data receiver.
	 * @param[in] bTraitDataInitialized Is the @p OutTraitData buffer actually initialized?
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	GetTrait(UScriptStruct* const TraitType,
			 void* const          OutTraitData,
			 const bool           bTraitDataInitialized = true) const
	{
		if (LIKELY(Handle))
		{
			return Handle.template GetTrait<Paradigm>(TraitType, OutTraitData, bTraitDataInitialized);
		}
		// The subjective is not registered just yet.
		// Ge the trait from the records.
		return DoGetTraitRecord(TraitType, OutTraitData, bTraitDataInitialized);
	}

	/**
	 * Get a trait from the subjective by its type.
	 * Statically typed pointer version.
	 *
	 * If the trait is missing within the subject,
	 * the output data is unchanged and EApparatusStatus::Missing is returned.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @param[out] OutTrait The trait data receiver.
	 * @param[in] bTraitDataInitialized Is the @p OutTrait actually initialized?
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Missing If there is no such trait in the subject.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm>
	GetTrait(T* const OutTrait, const bool bTraitDataInitialized = true) const
	{
		if (LIKELY(Handle))
		{
			return Handle.template GetTrait<Paradigm>(OutTrait, bTraitDataInitialized);
		}
		// The subjective is not registered just yet.
		// Ge the trait from the records:
		return DoGetTraitRecord(OutTrait, bTraitDataInitialized);
	}

	/**
	 * Get a trait from the subjective by its type.
	 * Statically typed reference version.
	 *
	 * If the trait is missing within the subject,
	 * the output data is unchanged and EApparatusStatus::Missing is returned.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @param[out] OutTrait The trait receiver.
	 * @param[in] bTraitDataInitialized Is the @p OutTrait actually initialized?
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Missing If there is no such trait in the subject.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm>
	GetTrait(T& OutTrait, const bool bTraitDataInitialized = true) const
	{
		if (LIKELY(Handle))
		{
			return Handle.template GetTrait<Paradigm>(OutTrait, bTraitDataInitialized);
		}
		// The subjective is not registered just yet.
		// Ge the trait from the records:
		return DoGetTraitRecord(OutTrait, bTraitDataInitialized);
	}

	/**
	 * Get a copy of a trait from the subjective by its type.
	 * Statically typed version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return The copy of the trait.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, T>
	GetTrait() const
	{
		T TraitTemp;
		if (AvoidError(Paradigm, GetTrait(TraitTemp)))
		{
			return MakeOutcome<Paradigm, T>(FApparatusStatus::GetLastError(), MoveTempIfPossible(TraitTemp));
		}
		return GetTrait<Paradigm, T>(EApparatusStatus::Success, MoveTempIfPossible(TraitTemp));
	}

	/**
	 * Get a copy of a trait from the subjective by its type.
	 * Statically typed version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return The copy of the trait.
	 */
	template < typename T,
			   EParadigm Paradigm = EParadigm::Default,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, T>
	GetTrait() const
	{
		return GetTrait<Paradigm, T>();
	}

	/**
	 * Get the trait data from the subjective by its type.
	 * Constant version.
	 * 
	 * If the trait is missing within the subject,
	 * a @c nullptr is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to get.
	 * Must not be a @c nullptr.
	 * @return A pointer to the trait's data.
	 * @return nullptr If there is no such trait in the subjective.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcomeIfUnsafe<Paradigm, const void*>
	GetTraitPtr(UScriptStruct* const TraitType) const
	{
		if (LIKELY(Handle))
		{
			return Handle.template GetTraitPtr<Paradigm>(TraitType);
		}
		if (UNLIKELY(TraitType == nullptr))
		{
			return nullptr;
		}
		// The subjective is not registered just yet.
		// Get the trait from the records.
		auto& Traits = GetTraitRecordsRef();
		for (auto& TraitRecord : Traits)
		{
			if (TraitRecord.GetType() == TraitType)
			{
				return TraitRecord.GetData();
			}
		}
		return nullptr;
	}

	/**
	 * Get the trait data from a subjective by its type.
	 * 
	 * If the trait is missing within the subject,
	 * a @c nullptr is returned.
	 *
	 * @param TraitType The type of the trait to get.
	 * Must not be a @c nullptr.
	 * @return A pointer to the trait's data.
	 * @return nullptr If there is no such trait in the subjective.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcomeIfUnsafe<Paradigm, void*>
	GetTraitPtr(UScriptStruct* const TraitType)
	{
		if (LIKELY(Handle))
		{
			return Handle.template GetTraitPtr<Paradigm>(TraitType);
		}
		if (UNLIKELY(TraitType == nullptr))
		{
			return nullptr;
		}
		// The subjective is not registered just yet.
		// Get the trait from the records.
		auto& Traits = GetTraitRecordsRef();
		for (auto& TraitRecord : Traits)
		{
			if (TraitRecord.GetType() == TraitType)
			{
				return TraitRecord.GetData();
			}
		}
		return nullptr;
	}

	/**
	 * Get the trait data from the subjective by its type.
	 * Statically typed constant version.
	 * 
	 * If the trait is missing within the subject,
	 * a @c nullptr is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A pointer to the constant trait's data.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>() && IsUnsafe(Paradigm), const T*>
	GetTraitPtr() const
	{
		return static_cast<const T*>(GetTraitPtr<Paradigm>(T::StaticStruct()));
	}

	/**
	 * Get the trait data from the subjective by its type.
	 * Statically typed mutable version.
	 * 
	 * If the trait is missing within the subject,
	 * a @c nullptr is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A pointer to the trait's data.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>() && IsUnsafe(Paradigm), T*>
	GetTraitPtr()
	{
		return static_cast<T*>(GetTraitPtr<Paradigm>(T::StaticStruct()));
	}

	/**
	 * Get the trait data from the subjective by its type.
	 * Statically typed constant version.
	 * 
	 * If the trait is missing within the subject,
	 * a @c nullptr is returned.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @return A pointer to the constant trait's data.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	GetTraitPtr() const
	{
		return GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Get the trait data from the subjective by its type.
	 * Statically typed mutable version.
	 * 
	 * If the trait is missing within the subject,
	 * a @c nullptr is returned.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @return A pointer to the trait's data.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	GetTraitPtr()
	{
		return GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Obtain a trait from the subjective by its type.
	 * Dynamically typed outputting version.
	 * 
	 * If the trait is missing within the subject,
	 * the output data is unchanged and EApparatusStatus::Missing is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] TraitType The type of the trait to obtain.
	 * @param[out] OutTraitData The trait data receiver.
	 * @param[in] bTraitDataInitialized Is the @p OutTraitData buffer actually initialized?
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	ObtainTrait(UScriptStruct* TraitType,
				void* const    OutTraitData,
				const bool     bTraitDataInitialized = true)
	{
		if (LIKELY(Handle))
		{
			return Handle.template ObtainTrait<Paradigm>(TraitType, OutTraitData, bTraitDataInitialized);
		}

		AssessCondition(Paradigm, IsValidSubjective(this), EApparatusStatus::InvalidState);
		AssessCondition(Paradigm, TraitType != nullptr, EApparatusStatus::NullArgument);
		AssessCondition(Paradigm, OutTraitData != nullptr, EApparatusStatus::NullArgument);

		// The subjective is not registered just yet.
		// Ge the trait from the records.
		return DoObtainTraitRecord(TraitType, OutTraitData, bTraitDataInitialized);
	}

	/**
	 * Obtain a trait from the subjective.
	 * Statically typed pointer version.
	 * 
	 * If the trait is not currently within the subjective,
	 * it gets created anew and the default value gets copied
	 * to the receiver.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to obtain.
	 * @param[out] OutTrait The pointer to the buffer to receive
	 * the trait.
	 * @param[in] bTraitDataInitialized Is the @p OutTrait actually initialized?
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	ObtainTrait(T* const   OutTrait,
				const bool bTraitDataInitialized = true)
	{
		return ObtainTrait<Paradigm>(T::StaticStruct(),
									 static_cast<void*>(OutTrait),
									 bTraitDataInitialized);
	}

	/**
	 * Obtain a trait from the subjective.
	 * Templated version.
	 * 
	 * If the trait is not currently within the subjective,
	 * it gets created anew and the default value gets copied
	 * to the receiver.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to obtain.
	 * @param[out] OutTrait The trait receiver.
	 * @param[in] bTraitDataInitialized Is the @p OutTrait actually initialized?
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm>
	ObtainTrait(T&         OutTrait,
				const bool bTraitDataInitialized = true)
	{
		return ObtainTrait<Paradigm>(T::StaticStruct(),
									 static_cast<void*>(&OutTrait),
									 bTraitDataInitialized);
	}

	/**
	 * Get a copy of an obtained trait from the subjective.
	 * Statically typed paradigm version.
	 * 
	 * If the trait is not currently within the subjective,
	 * it gets created anew and the default value gets copied
	 * to the receiver.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to obtain.
	 * @return The copy of the obtained trait.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, T>
	ObtainTrait()
	{
		T TraitTemp;
		if (AvoidError(Paradigm, ObtainTrait(TraitTemp)))
		{
			return MakeOutcome<Paradigm, T>(FApparatusStatus::GetLastError(), MoveTempIfPossible(TraitTemp));
		}
		return MoveTempIfPossible(TraitTemp);
	}

	/**
	 * Get a copy of an obtained trait from the subjective.
	 * Statically typed version.
	 * 
	 * If the trait is not currently within the subjective,
	 * it gets created anew and the default value gets copied
	 * to the receiver.
	 *
	 * @tparam T The type of the trait to obtain.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The copy of the obtained trait.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, T>
	ObtainTrait()
	{
		return ObtainTrait<Paradigm, T>();
	}

	/**
	 * Remove a trait from the subjective.
	 * Dynamically typed version.
	 * 
	 * If there is no such trait in the subjective,
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to remove.
	 * May be a @c nullptr and EApparatusStatus::Noop will be returned in that case.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	RemoveTrait(UScriptStruct* const TraitType)
	{
		if (LIKELY(Handle))
		{
			return Handle.template RemoveTrait<Paradigm>(TraitType);
		}
		return DoRemoveTraitRecord(TraitType);
	}

	/**
	 * Remove a trait from the subjective.
	 * Statically typed paradigm version.
	 * 
	 * If there is no such trait in the subjective,
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to remove.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm>
	RemoveTrait()
	{
		return RemoveTrait(T::StaticStruct());
	}

	/**
	 * Remove a trait from the subjective.
	 * Statically typed version.
	 * 
	 * If there is no such trait in the subjective,
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 *
	 * @tparam T The type of the trait to remove.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The status of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm>
	RemoveTrait()
	{
		return RemoveTrait<Paradigm, T>();
	}

	/**
	 * Remove all of the traits from the subjective.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	RemoveAllTraits()
	{
		if (LIKELY(Handle))
		{
			return Handle.template RemoveAllTraits<Paradigm>();
		}
		return DoRemoveAllTraitRecords();
	}

	/// @}
#pragma endregion Traits

#pragma region Details
	/// @name Details
	/// @{

	/**
	 * Get all of the details of the subjective.
	 * 
	 * @param bIncludeDisabled Should disabled details be included?
	 * @param OutDetails An array to store the details in.
	 */
	void
	GetDetails(const bool        bIncludeDisabled,
			   TArray<UDetail*>& OutDetails) const;

	/**
	 * Get all the enabled details of the subjective.
	 * 
	 * @param OutDetails An array to store the details in.
	 * @param bIncludeDisabled Should disabled details be included?
	 */
	FORCEINLINE void
	GetDetails(TArray<UDetail*>& OutDetails, const bool bIncludeDisabled = false) const
	{
		GetDetails(bIncludeDisabled, OutDetails);
	}

	// TODO: Remove the deprecated method.
	/**
	 * @deprecated Find a detail by its class.
	 * 
	 * Supports searching by a base class.
	 * 
	 * @param DetailClass The class of the detail to search for.
	 * @param bIncludeDisabled Should disabled details be included?
	 * @return A detail of the specified class if found, @c nullptr otherwise.
	 */
	[[deprecated("The 'FindDetail' method is deprecated for consistency reasons. Use the 'GetDetail' method instead, please.")]]
	FORCEINLINE UDetail*
	FindDetail(TSubclassOf<UDetail> DetailClass,
			   const bool           bIncludeDisabled = false) const
	{
		return GetDetail(DetailClass, bIncludeDisabled);
	}

	// TODO: Remove the deprecated method.
	/**
	 * @deprecated Find a detail by its class. Templated version.
	 * 
	 * Supports searching by a base class.
	 * 
	 * @tparam D The class of the detail to search for.
	 * @param bIncludeDisabled Should disabled details be included?
	 * @return A detail of the specified class if found, @c nullptr otherwise.
	 */
	template < class D >
	[[deprecated("The 'FindDetail' method is deprecated for consistency reasons. Use the 'GetDetail' method instead, please.")]]
	FORCEINLINE typename std::enable_if<IsDetailClass<D>(), D*>::type
	FindDetail(const bool bIncludeDisabled = false) const
	{
		return GetDetail<D>(bIncludeDisabled);
	}

	/**
	 * Get a detail by its class.
	 * 
	 * Supports searching by a base class.
	 * 
	 * @param DetailClass The class of the detail to search for.
	 * @param bIncludeDisabled Should disabled details be considered?
	 * @return A detail of the specified class.
	 * @return nullptr If there is no such detail within the subjective.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm, UDetail*>
	GetDetail(TSubclassOf<UDetail> DetailClass,
			  const bool           bIncludeDisabled = false) const
	{
		if (UNLIKELY(DetailClass == nullptr))
		{
			return MakeOutcome<Paradigm, UDetail*>(EApparatusStatus::Noop, nullptr);
		}
		if (AvoidCondition(Paradigm, !IsValidSubjective(this)))
		{
			return MakeOutcome<Paradigm, UDetail*>(EApparatusStatus::InvalidState, nullptr);
		}

		// A fast check for the detail in the fingerprint...
		if (UNLIKELY(!bIncludeDisabled && !GetFingerprint().Contains(DetailClass)))
		{
			return nullptr;
		}

		if (UNLIKELY(DetailClass == UDetail::StaticClass()))
		{
			// In the UDetail case we can skip the IsA checks for a slight performance
			// benefit...
			for (UDetail* Detail : GetDetailsRef())
			{
				if (UNLIKELY(!Detail))
					continue;
				if (!bIncludeDisabled && !Detail->IsEnabled())
					continue;
				return Detail;
			}
		}
		else
		{
			for (UDetail* Detail : GetDetailsRef())
			{
				if (UNLIKELY(!Detail))
					continue;
				if (!bIncludeDisabled && !Detail->IsEnabled())
					continue;
				if (Detail->IsA(DetailClass))
				{
					return Detail;
				}
			}
		}
		return nullptr;
	}

	/**
	 * Get a detail by its class.
	 * Statically typed version.
	 * 
	 * Supports searching by a base class.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The type of the detail to get.
	 * @param bIncludeDisabled Should disabled details be included?
	 * @return A detail of the specified type.
	 * @return nullptr If there is no such detail in the subjective.
	 */
	template < EParadigm Paradigm, class D >
	FORCEINLINE TOutcomeIf<Paradigm, IsDetailClass<D>(), D*>
	GetDetail(const bool bIncludeDisabled = false) const
	{
		return OutcomeStaticCast<D*>(GetDetail(D::StaticClass(), bIncludeDisabled));
	}

	/**
	 * Get a detail by its class.
	 * Statically typed version.
	 * 
	 * Supports searching by a base class.
	 * 
	 * @tparam D The type of the detail to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @param bIncludeDisabled Should disabled details be included?
	 * @return A detail of the specified type.
	 * @return nullptr If there is no such detail in the subjective.
	 */
	template < class D, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIf<Paradigm, IsDetailClass<D>(), D*>
	GetDetail(const bool bIncludeDisabled = false) const
	{
		return GetDetail<Paradigm, D>(bIncludeDisabled);
	}

	/**
	 * Collect all of the details of a certain class.
	 * Dynamically-typed version.
	 * 
	 * Supports searching by a base class.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator to feature in the array.
	 * @param[in] DetailClass The class of the details to collect.
	 * @param[out] OutDetails The output array to store the details in.
	 * @param[in] bIncludeDisabled Should disabled details be collected.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT = FDefaultAllocator >
	TOutcome<Paradigm>
	CollectDetails(TSubclassOf<UDetail>          DetailClass,
				   TArray<UDetail*, AllocatorT>& OutDetails,
				   const bool                    bIncludeDisabled = false) const
	{
		if (UNLIKELY(!IsValidSubjective(this) || !DetailClass))
		{
			return EApparatusStatus::Noop;
		}
		// A fast check for the detail in the fingerprint...
		if (!bIncludeDisabled && !GetFingerprint().Contains(DetailClass))
		{
			return EApparatusStatus::Noop;
		}

		auto bCollected = false;
		auto& Details = GetDetailsRef();
		if (UNLIKELY(DetailClass == UDetail::StaticClass()))
		{
			// In the UDetail case we can skip the IsA checks for a slight
			// performance benefit...
			if (UNLIKELY(bIncludeDisabled))
			{
				OutDetails.Reserve(Details.Num());
				for (UDetail* Detail : Details)
				{
					if (UNLIKELY(!Detail))
						continue;
					OutDetails.Add(Detail);
					bCollected = true;
				}
			}
			else
			{
				for (UDetail* const Detail : Details)
				{
					if (UNLIKELY(!Detail))
						continue;
					if (!Detail->IsEnabled())
						continue;
					OutDetails.Add(Detail);
					bCollected = true;
				}
			}
		}
		else
		{
			for (UDetail* const Detail : Details)
			{
				if (UNLIKELY(!Detail))
					continue;
				if (!bIncludeDisabled && !Detail->IsEnabled())
					continue;
				if (Detail->IsA(DetailClass))
				{
					OutDetails.Add(Detail);
					bCollected = true;
				}
			}
		}

		return bCollected ? EApparatusStatus::Success : EApparatusStatus::Noop;
	}

	/**
	 * Get all of the details of a certain class.
	 * Dynamically-typed version.
	 * 
	 * Supports searching by a base class.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator to feature in the array.
	 * @param[in] DetailClass The class of the details to collect.
	 * @param[out] OutDetails The output array to store the details in.
	 * @param[in] bIncludeDisabled Should disabled details be collected.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename AllocatorT = FDefaultAllocator >
	TOutcome<Paradigm>
	GetDetails(TSubclassOf<UDetail>          DetailClass,
			   TArray<UDetail*, AllocatorT>& OutDetails,
			   const bool                    bIncludeDisabled = false) const
	{
		OutDetails.Reset();
		return CollectDetails<Paradigm>(DetailClass, OutDetails, bIncludeDisabled).MapStatus(EApparatusStatus::Noop, EApparatusStatus::NoItems);
	}

	/**
	 * Get all of the details of a certain class.
	 * Statically typed version.
	 * 
	 * Supports searching by a base class.
	 * 
	 * @tparam D The class of the details to collect.
	 * @tparam AllocatorT The type of the allocator to utilize
	 * within the details.
	 * @param[out] OutDetails The output array to store the details in.
	 * @param[in] bIncludeDisabled Should disabled details be collected.
	 */
	template < class D, typename AllocatorT >
	typename std::enable_if<IsDetailClass<D>(), void>::type
	GetDetails(TArray<D*, AllocatorT>& OutDetails,
			   const bool              bIncludeDisabled = false) const
	{
		OutDetails.Reset();

		if (UNLIKELY(!IsValidSubjective(this)))
		{
			return;
		}
		// A fast check for the detail in the fingerprint...
		if (!bIncludeDisabled && !GetFingerprint().Contains(D::StaticClass()))
		{
			return;
		}

		// Search within the details array now...
		const TArray<UDetail*>& Details = GetDetailsRef();
		if (UNLIKELY(D::StaticClass() == UDetail::StaticClass()))
		{
			// In the UDetail case we can skip the IsA checks for a slight
			// performance benefit...
			if (UNLIKELY(bIncludeDisabled))
			{
				OutDetails.Reserve(Details.Num());
				for (UDetail* Detail : Details)
				{
					if (UNLIKELY(!Detail))
						continue;
					OutDetails.Add(Detail);
				}
			}
			else
			{
				for (UDetail* const Detail : Details)
				{
					if (UNLIKELY(!Detail))
						continue;
					if (!Detail->IsEnabled())
						continue;
					OutDetails.Add(Detail);
				}
			}
		}
		else
		{
			for (UDetail* const Detail : Details)
			{
				if (UNLIKELY(!Detail))
					continue;
				if (!bIncludeDisabled && !Detail->IsEnabled())
					continue;
				const auto CastDetail = ::template Cast<D>(Detail);
				if (CastDetail)
				{
					OutDetails.Add(CastDetail);
				}
			}
		}
	}

	// TODO: Remove the deprecated method.
	/**
	 * @deprecated Find the details by their class.
	 * 
	 * Supports searching by a base class.
	 */
	[[deprecated("The 'FindDetails' method is deprecated for consistency reasons. Use the 'GetDetails' method instead, please.")]]
	FORCEINLINE void
	FindDetails(TSubclassOf<UDetail> DetailClass,
				TArray<UDetail*>&    OutDetails,
				const bool           bIncludeDisabled = false) const
	{
		GetDetails(DetailClass, OutDetails, bIncludeDisabled);
	}

	/**
	 * Check if there is a detail of a specific class in the subjective.
	 * 
	 * Supports examining by a base class.
	 * 
	 * @param DetailClass The class of the detail to test for.
	 * @param bIncludeDisabled Should disabled details be considered?
	 * @return The state of examination.
	 */
	bool
	HasDetail(TSubclassOf<UDetail> DetailClass,
			  const bool           bIncludeDisabled = false) const;

	/**
	 * Check if there is a detail of a specific class in the subjective.
	 * Templated version.
	 * 
	 * Supports examining by a base class.
	 * 
	 * @tparam D The class of the detail to test for.
	 * @param bIncludeDisabled Should disabled details be considered?
	 * @return The state of examination.
	 */
	template < class D >
	FORCEINLINE typename std::enable_if<IsDetailClass<D>(), bool>::type
	HasDetail(const bool bIncludeDisabled = false) const
	{
		return HasDetail(D::StaticClass(), bIncludeDisabled);
	}

	/**
	 * Enable a detail of a certain type.
	 * 
	 * Adds the detail if there is none of the class already,
	 * or reuses an inactive one, enabling it.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass A class of detail to enable.
	 * @return The enabled detail.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm, UDetail*>
	EnableDetail(const TSubclassOf<UDetail> DetailClass);

	/**
	 * Enable a detail of a certain type.
	 * Statically typed paradigm version.
	 * 
	 * Adds the detail if there is none of the class already,
	 * or reuses an inactive one, enabling it.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The class of the detail to enable.
	 * @return The enabled detail.
	 */
	template < EParadigm Paradigm, class D >
	FORCEINLINE TOutcomeIf<Paradigm, IsDetailClass<D>(), D*>
	EnableDetail()
	{
		return OutcomeStaticCast<D*>(EnableDetail(D::StaticClass()));
	}

	/**
	 * Enable a detail of a certain type.
	 * Statically typed version.
	 * 
	 * Adds the detail if there is none of the class already,
	 * or reuses an inactive one, enabling it.
	 * 
	 * @tparam D The class of the detail to enable.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The enabled detail.
	 */
	template < class D, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIf<Paradigm, IsDetailClass<D>(), D*>
	EnableDetail()
	{
		return EnableDetail<Paradigm, D>();
	}

	/**
	 * Add a new active detail or reuse an inactive one.
	 * Dynamically typed version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass The class of the detail to add.
	 * @param bReuseDisabled Should disabled details be reused instead of adding a new one?
	 * @return The newly added detail.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm, UDetail*>
	AddDetail(const TSubclassOf<UDetail> DetailClass,
			  const bool           		 bReuseDisabled = false);

	/**
	 * Add a new active detail or reuse an inactive one.
	 * Statically typed paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The class of the detail to add.
	 * Must be ::UDetail based.
	 * @param bReuseDisabled Should disabled details be reused instead of adding a new one?
	 * @return The newly added detail.
	 */
	template < EParadigm Paradigm, class D >
	FORCEINLINE TOutcomeIf<Paradigm, IsDetailClass<D>(), D*>
	AddDetail(const bool bReuseDisabled = false)
	{
		return OutcomeStaticCast<D*>(AddDetail<Paradigm>(D::StaticClass(), bReuseDisabled));
	}

	/**
	 * Add a new active detail or reuse an inactive one.
	 * Statically typed version.
	 * 
	 * @tparam D The class of the detail to add.
	 * Must be ::UDetail based.
	 * @tparam Paradigm The paradigm to work under.
	 * @param bReuseDisabled Should disabled details be reused instead of adding a new one?
	 * @return The newly added detail.
	 */
	template < class D, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIf<Paradigm, IsDetailClass<D>(), D*>
	AddDetail(const bool bReuseDisabled = false)
	{
		return AddDetail<Paradigm, D>(bReuseDisabled);
	}

	/**
	 * Disable detail(s) by class.
	 * Dynamically typed version.
	 * 
	 * Details can't be removed from subjectives.
	 * They can only be disabled.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass The class of the detail(s) to disable.
	 * @param bDisableMultiple Should all matching details be disabled instead of first one?
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop If there are no such details
	 * in the subjective currently enabled.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	DisableDetail(const TSubclassOf<UDetail> DetailClass,
				  const bool                 bDisableMultiple = false);

	/**
	 * Disable detail(s) by class.
	 * Statically typed paradigm version.
	 * 
	 * Details can't be removed from subjectives.
	 * They can only be disabled.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The class of the detail to disable.
	 * Must be UDetail subclass.
	 * @param bDisableMultiple Should all matching details be disabled instead of first one?
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop If there are no such details
	 * in the subjective currently enabled.
	 */
	template < EParadigm Paradigm, class D >
	FORCEINLINE TOutcomeIf<Paradigm, IsDetailClass<D>()>
	DisableDetail(const bool bDisableMultiple = false)
	{
		return DisableDetail(D::StaticClass(), bDisableMultiple);
	}

	/**
	 * Disable detail(s) by class.
	 * Statically typed version.
	 * 
	 * Details can't be removed from subjectives.
	 * They can only be disabled.
	 * 
	 * @tparam D The class of the detail to disable.
	 * Must be UDetail subclass.
	 * @param bDisableMultiple Should all matching details be disabled instead of first one?
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop If there are no such details
	 * in the subjective currently enabled.
	 */
	template < class D >
	FORCEINLINE auto
	DisableDetail(const bool bDisableMultiple = false)
	{
		return DisableDetail<EParadigm::Default, D>(bDisableMultiple);
	}

	/// @}
#pragma endregion Details

#pragma region Networking
	/// @name Networking
	/// @{

	/**
	 * Check if the subjective class is capable of networking.
	 * 
	 * This method should be overriden by the
	 * interface implementors to explicitly indicate
	 * that they have some mandatory properties
	 * for the functionality.
	 */
	virtual FORCEINLINE bool
	IsNetworkCapable() const
	{
		return false;
	}

	/**
	 * Get the unique cross-peer
	 * network identifier of the subjective (if any).
	 */
	FORCEINLINE uint32
	GetNetworkId() const
	{
		if (LIKELY(Handle))
		{
			return Handle.GetNetworkId();
		}
		return FSubjectNetworkState::InvalidId;
	}

	/**
	 * Check if the subjective is a server-side one.
	 */
	FORCEINLINE bool
	IsServerSide() const
	{
		if (LIKELY(Handle))
		{
			return Handle.IsServerSide();
		}
		else if (ShouldBeReplicated())
		{
			check(IsNetworkCapable());
			const auto Object = CastChecked<UObject>(this);
			const auto World = Object->GetWorld();
			if (UNLIKELY(!World)) return false;
			return World->IsNetMode(NM_DedicatedServer) || World->IsNetMode(NM_ListenServer);
		}
		return false;
	}

	/**
	 * Check if the subjective is a client-side one.
	 */
	FORCEINLINE bool
	IsClientSide() const
	{
		if (LIKELY(Handle))
		{
			return Handle.IsClientSide();
		}
		else if (ShouldBeReplicated())
		{
			check(IsNetworkCapable());
			const auto Object = CastChecked<UObject>(this);
			const auto World = Object->GetWorld();
			if (UNLIKELY(!World)) return false;
			return World->IsNetMode(NM_Client) || World->IsNetMode(NM_ListenServer);
		}
		return false;
	}

	/**
	 * Get the server-side connection to a client from which it is allowed
	 * to push traits to this subjective.
	 * 
	 * @return The active connection permit.
	 */
	FORCEINLINE UNetConnection*
	GetConnectionPermit() const
	{
		if (const auto Actor = GetActor())
		{
			return Actor->GetNetConnection();
		}
		if (LIKELY(ensureMsgf(Handle && Handle.IsServerSide(),
							  TEXT("The subjective must be a registered server-side "
								   "entity in order to get its connection permit."))))
		{
			return Handle.GetInfo().GetNetworkState()->GetConnectionPermit();
		}
		return nullptr;
	}

	/**
	 * Set the server-side connection to a client from which it is allowed
	 * to push traits to this subjective.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] Connection The connection permit to set to.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	SetConnectionPermit(UNetConnection* const Connection)
	{
		if (const auto Actor = GetActor())
		{
			AssessConditionFormat(Paradigm, Actor->GetNetConnection() == Connection, EApparatusStatus::InvalidState,
								  TEXT("Changing the connection permit of the %s actor-based subject is prohibited."),
								  *(Actor->GetName()));
		}

		AssessConditionFormat(Paradigm, Handle.IsServerSide(), EApparatusStatus::InvalidState,
							  TEXT("The '%s' subjective must be a registered server-side "
								   "entity in order to change its connection permit."),
							  *(CastChecked<UObject>(this)->GetName()));

		return Handle.template SetConnectionPermit<Paradigm>(Connection);
	}

	/**
	 * Get the server-side list of
	 * traits allowed to be received from clients.
	 * 
	 * Can only be queried on a server-side subjective version.
	 * 
	 * @return The current traitmark permit whitelist.
	 */
	FORCEINLINE const FTraitmark&
	GetTraitmarkPermit() const
	{
		if (LIKELY(Handle.IsOnline()))
		{
			return Handle.GetTraitmarkPermit();
		}
		else if (LIKELY(IsServerSide()))
		{
			return GetStandardNetworkPropertyBlock().TraitmarkPermit;
		}
		return FTraitmark::Zero;
	}

	[[deprecated("The method was renamed. Use 'GetTraitmarkPermit', please.")]]
	FORCEINLINE const FTraitmark&
	GetTraitmarkPass() const
	{
		return GetTraitmarkPermit();
	}

	/**
	 * Set the server-side list of
	 * traits allowed to be received from clients.
	 * 
	 * Can only be set for a server-side subjective version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InTraitmarkPermit The new traitmark to be used as a whitelist.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	SetTraitmarkPermit(const FTraitmark& InTraitmarkPermit)
	{
		if (LIKELY(Handle.IsOnline()))
		{
			// This also means the handle is a valid one.
			return Handle.template SetTraitmarkPermit<Paradigm>(InTraitmarkPermit);
		}
		else if (LIKELY(IsServerSide()))
		{
			// The subjective is registered yet, so change the property for now:
			return GetStandardNetworkPropertyBlock().TraitmarkPermit.template Set<Paradigm>(InTraitmarkPermit);
		}
		return EApparatusStatus::InvalidState;
	}

	template < EParadigm Paradigm = EParadigm::Default >
	[[deprecated("The method was renamed. Use 'SetTraitmarkPermit', please.")]]
	FORCEINLINE TOutcome<Paradigm>
	SetTraitmarkPass(const FTraitmark& InTraitmarkPermit)
	{
		return SetTraitmarkPermit<Paradigm>(InTraitmarkPermit);
	}

	/**
	 * Send an existing trait to a remote peer.
	 * Dynamically typed immutable version.
	 * 
	 * On a client the trait can only be sent from a current connection's
	 * owned actor, i.e. a pawn or a player controller.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to send.
	 * Must not be a @c nullptr.
	 * @param PeerRole The explicit peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should reliable channel be used
	 * for the transaction.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	PushTrait(UScriptStruct* const TraitType,
			  const EPeerRole      PeerRole = EPeerRole::Auto,
			  const bool           bReliable = true) const
	{
		return DoPushTrait(Paradigm, TraitType, PeerRole, bReliable);
	}

	/**
	 * Send an additional trait to a remote peer.
	 * Dynamically typed version.
	 * 
	 * On a client the trait can only be sent from a current connection's
	 * owned actor, i.e. a pawn or a player controller.
	 * 
	 * Executes in a safe polite paradigm.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to send.
	 * Must not be a @c nullptr.
	 * @param TraitData The data of the trait to send.
	 * Must not be a @c nullptr.
	 * @param bSetForLocal Should the trait be also
	 * set for the local version of the subject.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should reliable channel be used
	 * for the transaction.
	 * @return The outcome of the operation.
	 * 
	 * @deprecated since 1.23 The @p bSetForLocal argument is deprecated now.
	 * Set it explicitly and use the version of the method without the argument, please.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	[[deprecated("The 'bSetForLocal' argument is deprecated now. Set the trait explicitly or use the version of the method without the argument, please.")]]
	FORCEINLINE TOutcome<Paradigm>
	PushTrait(UScriptStruct* const TraitType,
			  const void* const    TraitData,
			  const bool           bSetForLocal,
			  const EPeerRole      PeerRole = EPeerRole::Auto,
			  const bool           bReliable = true)
	{
		return DoPushTrait(Paradigm, TraitType, TraitData, bSetForLocal, PeerRole, bReliable);
	}

	/**
	 * Send an additional trait to a remote peer.
	 * Dynamically typed version.
	 * 
	 * On a client the trait can only be sent from a current connection's
	 * owned actor, i.e. a pawn or a player controller.
	 * 
	 * Executes in a safe polite paradigm.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to send.
	 * Must not be a @c nullptr.
	 * @param TraitData The data of the trait to send.
	 * Must not be a @c nullptr.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should reliable channel be used
	 * for the transaction.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	PushTrait(UScriptStruct* const TraitType,
			  const void* const    TraitData,
			  const EPeerRole      PeerRole = EPeerRole::Auto,
			  const bool           bReliable = true)
	{
		return DoPushTrait(Paradigm, TraitType, TraitData, /*bSetForLocal=*/false, PeerRole, bReliable);
	}

	/**
	 * Send an extra trait to a remote peer.
	 * Statically typed version.
	 * 
	 * On a client the trait can only be send from a current connection's
	 * owned actor, i.e. a pawn or a player controller.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to push.
	 * @param Trait The trait to push.
	 * @param bSetForLocal Should the trait be also
	 * set for the local version of the subject.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should reliable channel be used
	 * for the transaction.
	 * @return The outcome of the operation.
	 *
	 * @deprecated since 1.23 The @p bSetForLocal argument is deprecated now.
	 * Set it explicitly and use the version of the method without the argument, please.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void >
	[[deprecated("The 'bSetForLocal' argument is deprecated now. Set the trait explicitly or use the version of the method without the argument, please.")]]
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	PushTrait(const T&        Trait,
			  const bool      bSetForLocal = false,
			  const EPeerRole PeerRole = EPeerRole::Auto,
			  const bool      bReliable = true)
	{
		return PushTrait<Paradigm>(T::StaticStruct(), static_cast<const void*>(&Trait),
								   bSetForLocal, PeerRole, bReliable);
	}

	/**
	 * Send an extra trait to a remote peer.
	 * Statically typed version.
	 * 
	 * On a client the trait can only be send from a current connection's
	 * owned actor, i.e. a pawn or a player controller.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to push.
	 * @param Trait The trait to push.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should reliable channel be used
	 * for the transaction.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	PushTrait(const T&        Trait,
			  const EPeerRole PeerRole = EPeerRole::Auto,
			  const bool      bReliable = true)
	{
		return PushTrait<Paradigm>(T::StaticStruct(), static_cast<const void*>(&Trait),
								   /*bSetForLocal=*/false, PeerRole, bReliable);
	}

	/**
	 * Send an extra trait to a remote peer.
	 * Statically typed version.
	 * 
	 * On a client the trait can only be send from a current connection's
	 * owned actor, i.e. a pawn or a player controller.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to push.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should reliable channel be used
	 * for the transaction.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	PushTrait(const EPeerRole PeerRole  = EPeerRole::Auto,
			  const bool      bReliable = true)
	{
		return PushTrait<Paradigm>(T::StaticStruct(), PeerRole, bReliable);
	}

	/**
	 * Send an extra trait to a remote peer.
	 * Statically typed default paradigm version.
	 * 
	 * On a client the trait can only be send from a current connection's
	 * owned actor, i.e. a pawn or a player controller.
	 * 
	 * @tparam T The type of the trait to push.
	 * @tparam Paradigm The paradigm to work under.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should reliable channel be used
	 * for the transaction.
	 * @return The outcome of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	PushTrait(const EPeerRole PeerRole  = EPeerRole::Auto,
			  const bool      bReliable = true)
	{
		return PushTrait<Paradigm, T>(PeerRole, bReliable);
	}

	/// @}
#pragma endregion Networking

	/**
	 * Get the preferred belt of the subjective (if any).
	 */
	FORCEINLINE UBelt*
	GetPreferredBelt() const
	{
		return GetStandardPropertyBlock().PreferredBelt;
	}

	/**
	 * Get the current belt slot of the subjective (if any). Constant version.
	 *
	 * @return The active belt slot of the subject or <c>nullptr</c>,
	 * if the subject is not (yet) part of any belt.
	 */
	const struct FBeltSlot*
	GetSlot() const;

	/**
	 * Check if the subjective is registered
	 * within the machine.
	 */
	FORCEINLINE bool
	IsRegistered() const
	{
		return Handle.IsValid() && (GetSlot() != nullptr);
	}

	/**
	 * Get the current slot index of the subjective.
	 */
	FORCEINLINE int32
	GetSlotIndex() const
	{
		return SlotIndex;
	}

	/**
	 * Check if the subject is booted.
	 *
	 * The subject must be booted (initialized) by the booting mechanics
	 * prior to being processed by the ticking mechanics.
	 */
	FORCEINLINE bool
	IsBooted() const 
	{
		return GetFingerprint().IsBooted();
	}

	static FORCEINLINE bool
	IsValidSubjective(const ISubjective* const InSubjective)
	{
		return InSubjective != nullptr;
	}

	/**
	 * Get the subject handle of this subjective.
	 */
	FORCEINLINE FSubjectHandle
	GetHandle()
	{
		if (UNLIKELY(!IsValidSubjective(this)))
		{
			return FSubjectHandle::Invalid;
		}
		return Handle;
	}

	/**
	 * Get the subject handle of this subjective.
	 * Constant version.
	 */
	FORCEINLINE FConstSubjectHandle
	GetHandle() const
	{
		if (UNLIKELY(!IsValidSubjective(this)))
		{
			return FConstSubjectHandle::Invalid;
		}
		return Handle;
	}

	/**
	 * Unregister the subjective from its mechanism
	 * in a manual fashion.
	 * 
	 * If the subjective is already registered,
	 * nothing is performed.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultPortable >
	TOutcome<Paradigm>
	Unregister();

	/**
	 * Calculate the hash of the subjective.
	 */
	FORCEINLINE virtual uint32
	CalcHash() const 
	{
		return GetTypeHash((const void*)this);
	}

	/**
	 * Called when the subject handle is despawned.
	 * 
	 * This method should basically self-destruct the subjective
	 * or finalize it somehow.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,
			  Meta = (DisplayName = "Handle Despawned"),
			  Category = "Apparatus|Subjective")
	void ReceiveHandleDespawned();

	/**
	 * Called when the subject handle is despawned.
	 * 
	 * This method should basically self-destruct the subjective
	 * or finalize it somehow.
	 * 
	 * This method should never fail or raise any assertions.
	 * 
	 * If the self-destruction is not possible or not available
	 * for the subjective class, just silently exit the routine.
	 * 
	 * By default the top-level implementation of the method
	 * calls the respective blueprint event and exits silently.
	 */
	FORCEINLINE virtual void
	NotifyHandleDespawned()
	{
		if (!bUnregistered)
		{
			Execute_ReceiveHandleDespawned(CastChecked<UObject>(this));
		}
	}

	/**
	 * Get an actor corresponding to the subjective (if any).
	 * 
	 * Must be overridden in the descendants if they
	 * can derive an actor from itselves.
	 * 
	 * The default implementation just returns @c nullptr.
	 * 
	 * @return An actor this subjective belongs to.
	 * @return nullptr if the subject doesn't belong to any actor.
	 */
	FORCEINLINE virtual AActor*
	GetActor() const
	{
		return nullptr;
	}

}; //-class ISubjective

FORCEINLINE uint32
GetTypeHash(const TWeakInterfacePtr<ISubjective>& Subjective)
{
	return Subjective->CalcHash();
}

FORCEINLINE bool
ISubjective::MarkBooted()
{
	return Handle.MarkBooted();
}

inline void
ISubjective::GetDetails(const bool        bIncludeDisabled,
						TArray<UDetail*>& OutDetails) const
{
	OutDetails.Reset();
	if (bIncludeDisabled)
	{
		OutDetails.Append(GetDetailsRef());
	}
	else
	{
		for (auto Detail : GetDetailsRef())
		{
			if (Detail == nullptr)
				continue;
			if (!Detail->IsEnabled())
				continue;

			OutDetails.Add(Detail);
		}
	}
}

inline bool
ISubjective::HasDetail(const TSubclassOf<UDetail> DetailClass,
					   const bool                 bIncludeDisabled) const
{
	if (UNLIKELY(!IsValidSubjective(this) || !DetailClass))
	{
		return false;
	}

	if (GetFingerprint().Contains(DetailClass))
	{
		return true;
	}

	if (!bIncludeDisabled)
	{
		// No need to do anything if we are searching
		// for active detail only:
		return false;
	}

	if (UNLIKELY(DetailClass == UDetail::StaticClass()))
	{
		// In the UDetail case we can skip the IsA checks for a slight performance
		// benefit...
		for (auto Detail : GetDetailsRef())
		{
			if (!Detail)
			{
				continue;
			}
			return true;
		}
	}
	else
	{
		for (auto Detail : GetDetailsRef())
		{
			if (!Detail)
			{
				continue;
			}
			if (Detail->IsA(DetailClass))
			{
				return true;
			}
		}
	}
	return false;
}

#pragma region Subject Info

/**
 * Get the owning net connection of the subject (if any).
 */
inline UNetConnection*
FSubjectInfo::GetNetConnection() const
{
	if (Subjective)
	{
		auto* const Actor = Subjective->GetActor();
		if (LIKELY(Actor))
		{
			return Actor->GetNetConnection();
		}
	}
	return nullptr;
}

#pragma endregion Subject Info

#pragma region Detail Inlines

FORCEINLINE ISubjective*
UDetail::GetOwner() const
{
	return Cast<ISubjective>(GetOuter());
}

FORCEINLINE AMechanism*
UDetail::GetMechanism() const
{
	auto Owner = GetOwner();
	if (UNLIKELY(!Owner))
	{
		return nullptr;	
	}
	return Owner->GetMechanism();
}

#pragma endregion Detail Inlines


/**
 * A solid-state version of the subjective.
 * 
 * This is a semantic wrapping for the same subjective
 * interface class, with some manual casts provided.
 */
class APPARATUSRUNTIME_API ISolidSubjective
  : protected ISubjective
{
  private:

	friend class ISubjective;

	template < EParadigm Paradigm,
			   typename From, typename To,
			   bool ArgumentTypeCheck >
	friend struct TSmartCastImpl;

	/**
	 * Solid subjectives can't be created.
	 * Only cast to using the SmartCast() routine.
	 */
	FORCEINLINE
	ISolidSubjective()
	{}

  public:

	enum
	{
		/**
		 * Invalid belt slot index.
		 */
		InvalidSlotIndex = ISubjective::InvalidSlotIndex
	};

	/**
	 * Get the mechanism this subjective
	 * is currently part of (if registered).
	 */
	FORCEINLINE AMechanism*
	GetMechanism() const
	{
		return ISubjective::GetMechanism();
	}

  private:

	/**
	 * Direct access for the internal details array. Constant version.
	 */
	FORCEINLINE const TArray<UDetail*>&
	GetDetailsRef() const
	{
		return ISubjective::GetDetailsRef();
	}

  public:

	/**
	 * Get the active fingerprint of the subjective. Constant version.
	 */
	FORCEINLINE const FFingerprint&
	GetFingerprint() const
	{
		return ISubjective::GetFingerprint();
	}

	/**
	 * Check if the subjective matches a supplied filter.
	 */
	FORCEINLINE bool
	Matches(const FFilter& InFilter) const
	{
		return ISubjective::Matches(InFilter);
	}

	/**
	 * Check if the subjective matches a supplied traitmark as a filter.
	 */
	FORCEINLINE bool
	Matches(const FTraitmark& InTraitmark) const
	{
		return ISubjective::Matches(InTraitmark);
	}

	/**
	 * Check if the subjective matches a supplied detailmark as a filter.
	 */
	FORCEINLINE bool
	Matches(const FDetailmark& InDetailmark) const
	{
		return ISubjective::Matches(InDetailmark);
	}

#pragma region Flagmark
	/// @name Flagmark
	/// @{

	/**
	 * Get the current flagmark of the subjective.
	 */
	FORCEINLINE EFlagmark
	GetFlagmark() const
	{
		return ISubjective::GetFlagmark();
	}

	/**
	 * Set the current flagmark for the subjective.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InFlagmark The flagmark to set.
	 * @return The previous flagmark value.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	SetFlagmark(const EFlagmark InFlagmark)
	{
		return ISubjective::template SetFlagmark<Paradigm>(InFlagmark);
	}

	/**
	 * Get the current state of a flag for the subjective.
	 * 
	 * @param Flag The flag to get the state of.
	 * @return The state of the flag.
	 */
	FORCEINLINE bool
	HasFlag(const EFlagmarkBit Flag) const
	{
		return ISubjective::HasFlag(Flag);
	}

	/**
	 * Set a flag for the subjective.
	 * 
	 * @param Flag The flag to set. Must not be a system-level one.
	 * @param bState The state of the flag to set.
	 * @return The previous state of the flag.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	SetFlag(const EFlagmarkBit Flag, const bool bState = true)
	{
		return ISubjective::template SetFlag<Paradigm>(Flag, bState);
	}

	/**
	 * Toggle a flag for the subjective.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flag The flag to toggle. Must not be a system-level one.
	 * @return The new state of the flag.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	ToggleFlag(const EFlagmarkBit Flag)
	{
		return ISubjective::template ToggleFlag<Paradigm>(Flag);
	}

	/// @}
#pragma endregion Flagmark

#pragma region Traits
	/// @name Traits
	/// @{

	/**
	 * Check if the solid subjective has a certain trait.
	 * 
	 * @param TraitType The type of the trait to check for.
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	HasTrait(UScriptStruct* const TraitType) const
	{
		return ISubjective::HasTrait(TraitType);
	}

	/**
	 * Check if there is a trait of a specific type in the solid subjective.
	 * Statically-typed version.
	 *
	 * @tparam T The type of the trait to check for.
	 * @return The state of examination.
	 */
	template < typename T >
	FORCEINLINE bool
	HasTrait() const
	{
		return ISubjective::template HasTrait<T>();
	}

	/**
	 * Get a trait from a subjective by its type.
	 * 
	 * If the trait is missing within the subject,
	 * the behavior is undefined and an assertion may be raised.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to get.
	 * @param OutTraitData The trait data receiver.
	 * @param bTraitDataInitialized Is the @p OutTraitData buffer actually initialized?
	 *
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	GetTrait(UScriptStruct* const TraitType,
			 void* const          OutTraitData,
			 const bool           bTraitDataInitialized = true) const
	{
		return ISubjective::template GetTrait<Paradigm>(TraitType, OutTraitData, bTraitDataInitialized);
	}

	/**
	 * Get the trait data from a subjective by its type.
	 * Constant version.
	 * 
	 * If the trait is missing within the subject,
	 * a @c nullptr is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to get.
	 * Must not be a @c nullptr.
	 * @return A pointer to the trait's data.
	 * @return nullptr If there is no such trait in the subjective.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, const void*>
	GetTraitPtr(UScriptStruct* const TraitType) const
	{
		return ISubjective::template GetTraitPtr<MakeUnsafe(Paradigm)>(TraitType);
	}

	/**
	 * Get the trait data from a subjective by its type.
	 * 
	 * If the trait is missing within the subject,
	 * a @c nullptr is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to get.
	 * Must not be a @c nullptr.
	 * @return A pointer to the trait's data.
	 * @return nullptr If there is no such trait in the subjective.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, void*>
	GetTraitPtr(UScriptStruct* const TraitType)
	{
		return ISubjective::template GetTraitPtr<MakeUnsafe(Paradigm)>(TraitType);
	}

	/**
	 * Get the trait data from a subjective by its type.
	 * Statically typed constant paradigm version.
	 * 
	 * If the trait is missing within the subject,
	 * a @c nullptr is returned.
	 *
	 * @tparam The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A pointer to the trait's data.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>(), const T*>
	GetTraitPtr() const
	{
		return ISubjective::template GetTraitPtr<MakeUnsafe(Paradigm), T>();
	}

	/**
	 * Get the trait data from a subjective by its type.
	 * Statically typed constant version.
	 * 
	 * If the trait is missing within the subject,
	 * a @c nullptr is returned.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam The paradigm to work under.
	 * @return A pointer to the trait's data.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	GetTraitPtr() const
	{
		return GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Get the trait data from a subjective by its type.
	 * Statically typed paradigm version.
	 * 
	 * If the trait is missing within the subject,
	 * a @c nullptr is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A pointer to the trait's data.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>(), T*>
	GetTraitPtr()
	{
		return ISubjective::GetTraitPtr<MakeUnsafe(Paradigm), T>();
	}

	/**
	 * Get the trait data from the subjective by its type.
	 * Statically typed default paradigm version.
	 * 
	 * If the trait is missing within the subject,
	 * a @c nullptr is returned.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam The paradigm to work under.
	 * @return A pointer to the trait's data.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	GetTraitPtr()
	{
		return GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Get a trait from the subjective by its type.
	 * Statically typed outputting version.
	 *
	 * If the trait is missing within the subject,
	 * the output data is unchanged and EApparatusStatus::Missing is returned.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @param[out] OutTrait The trait receiver.
	 * @param[in] bTraitDataInitialized Is the @p OutTrait actually initialized?
	 *
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void >
	FORCEINLINE auto
	GetTrait(T& OutTrait, const bool bTraitDataInitialized = true) const
	{
		return ISubjective::template GetTrait<Paradigm, T>(OutTrait, bTraitDataInitialized);
	}

	/**
	 * Get a copy of a trait from the subjective by its type.
	 * Statically typed returning version within a paradigm.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * Must be a trait type.
	 * @return The copy of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE auto
	GetTrait() const
	{
		return ISubjective::template GetTrait<Paradigm, T>();
	}

	/**
	 * Get a copy of a trait from the subjective by its type.
	 * Statically typed returning version.
	 * 
	 * @tparam T The type of the trait to get.
	 * Must be a trait type.
	 * @return The copy of the trait.
	 */
	template < typename T >
	FORCEINLINE auto
	GetTrait() const
	{
		return ISubjective::template GetTrait<T>();
	}

	/// @}
#pragma endregion Traits

#pragma region Details
	/// @name Details
	/// @{

	// TODO: Remove the deprecated method.
	/**
	 * @deprecated Find a detail by its class.
	 * 
	 * Supports searching by a base class.
	 * 
	 * @param DetailClass The class of the detail to search for.
	 * @param bIncludeDisabled Should disabled details be included?
	 * @return A detail of the specified class if found, @c nullptr otherwise.
	 */
	[[deprecated]]
	FORCEINLINE UDetail*
	FindDetail(TSubclassOf<UDetail> DetailClass,
			   const bool           bIncludeDisabled = false) const
	{
		return ISubjective::GetDetail(DetailClass, bIncludeDisabled);
	}

	// TODO: Remove the deprecated method.
	/**
	 * @deprecated Find a detail by its class. Templated version.
	 * 
	 * Supports searching by a base class.
	 * 
	 * @tparam D The class of the detail to search for.
	 * @param bIncludeDisabled Should disabled details be included?
	 * @return A detail of the specified class if found, @c nullptr otherwise.
	 */
	template < class D >
	[[deprecated]]
	FORCEINLINE D*
	FindDetail(const bool bIncludeDisabled = false) const
	{
		return ISubjective::template GetDetail<D>(bIncludeDisabled);
	}

	/**
	 * Get a detail by its class.
	 * 
	 * Asserts success. Supports searching by a base class.
	 * 
	 * @param DetailClass The class of the detail to search for.
	 * @param bIncludeDisabled Should disabled details be considered?
	 * @return A detail of the specified class.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE UDetail*
	GetDetail(TSubclassOf<UDetail> DetailClass,
			  const bool           bIncludeDisabled = false) const
	{
		return ISubjective::template GetDetail<Paradigm>(DetailClass, bIncludeDisabled);
	}

	/**
	 * Get a detail by its class.
	 * Templated version.
	 * 
	 * Asserts success. Supports searching by a base class.
	 * 
	 * @tparam D The type of the detail to get.
	 * @param bIncludeDisabled Should disabled details be included?
	 * @return A detail of the specified type.
	 */
	template < EParadigm Paradigm, class D >
	FORCEINLINE TOutcome<Paradigm, D*>
	GetDetail(const bool bIncludeDisabled = false) const
	{
		return ISubjective::template GetDetail<Paradigm, D>(bIncludeDisabled);
	}

	/**
	 * Get a detail by its class.
	 * Templated version.
	 * 
	 * Asserts success. Supports searching by a base class.
	 * 
	 * @tparam D The type of the detail to get.
	 * @param bIncludeDisabled Should disabled details be included?
	 * @return A detail of the specified type.
	 */
	template < class D, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, D*>
	GetDetail(const bool bIncludeDisabled = false) const
	{
		return GetDetail<Paradigm, D>(bIncludeDisabled);
	}

	/**
	 * Get the details of the subjective.
	 * 
	 * @tparam D The type of details to get.
	 * @param OutDetails An array to store the details in.
	 * @param bIncludeDisabled Should disabled details be included?
	 */
	template < class D >
	FORCEINLINE typename std::enable_if<IsDetailClass<D>(), void>::type
	GetDetails(TArray<D*>& OutDetails,
			   const bool  bIncludeDisabled = false) const
	{
		ISubjective::template GetDetails<D>(OutDetails, bIncludeDisabled);
	}

	/**
	 * Get the details of the subjective.
	 * 
	 * @param bIncludeDisabled Should disabled details be included?
	 * @param OutDetails An array to store the details in.
	 */
	FORCEINLINE void
	GetDetails(const bool        bIncludeDisabled,
			   TArray<UDetail*>& OutDetails) const
	{
		ISubjective::GetDetails(bIncludeDisabled, OutDetails);
	}

	/**
	 * Get all the enabled details of the subjective.
	 * 
	 * @param OutDetails An array to store the details in.
	 */
	FORCEINLINE void
	GetDetails(TArray<UDetail*>& OutDetails) const
	{
		ISubjective::GetDetails(OutDetails);
	}

	// TODO: Remove the deprecated method.
	/**
	 * @deprecated Find the details by their class.
	 * 
	 * Supports searching by a base class.
	 */
	FORCEINLINE void
	FindDetails(TSubclassOf<UDetail> DetailClass,
				TArray<UDetail*>&    OutDetails,
				const bool           bIncludeDisabled = false) const
	{
		ISubjective::GetDetails(DetailClass, OutDetails, bIncludeDisabled);
	}

	/**
	 * Get the details of a certain class.
	 * 
	 * Supports searching by a base class.
	 */
	FORCEINLINE void
	GetDetails(TSubclassOf<UDetail> DetailClass,
			   TArray<UDetail*>&    OutDetails,
			   const bool           bIncludeDisabled = false) const
	{
		ISubjective::GetDetails(DetailClass, OutDetails, bIncludeDisabled);
	}

	/**
	 * Check if there is a detail of a specific class in the subjective.
	 * 
	 * Supports examining by a base class.
	 * 
	 * @param DetailClass The class of the detail to test for.
	 * @param bIncludeDisabled Should disabled details be considered?
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	HasDetail(TSubclassOf<UDetail> DetailClass,
			  const bool           bIncludeDisabled = false) const
	{
		return ISubjective::HasDetail(DetailClass, bIncludeDisabled);
	}

	/**
	 * Check if there is a detail of a specific class in the subjective.
	 * Templated version.
	 * 
	 * Supports examining by a base class.
	 * 
	 * @tparam T The class of the detail to test for.
	 * @param bIncludeDisabled Should disabled details be considered?
	 * @return The state of examination.
	 */
	template < class T >
	FORCEINLINE bool
	HasDetail(const bool bIncludeDisabled = false) const
	{
		return ISubjective::template HasDetail<T>(bIncludeDisabled);
	}

	/// @}
#pragma endregion Details

#pragma region Networking
	/// @name Networking
	/// @{

	/**
	 * Check if the subjective class is capable of networking.
	 * 
	 * This method should be overriden by the
	 * interface implementors to explicitly indicate
	 * that they have some mandatory properties
	 * for the functionality.
	 */
	FORCEINLINE bool
	IsNetworkCapable() const
	{
		return ISubjective::IsNetworkCapable();
	}

	/**
	 * Get the unique cross-peer
	 * network identifier of the subjective (if any).
	 */
	FORCEINLINE uint32
	GetNetworkId() const
	{
		return ISubjective::GetNetworkId();
	}

	/**
	 * Check if the subjective is a server-side one.
	 */
	FORCEINLINE bool
	IsServerSide() const
	{
		return ISubjective::IsServerSide();
	}

	/**
	 * Check if the subjective is a client-side one.
	 */
	FORCEINLINE bool
	IsClientSide() const
	{
		return ISubjective::IsClientSide();
	}

	/**
	 * Get the server-side list of
	 * traits allowed to be received from clients.
	 * 
	 * Can only be queried on a server-side subjective version.
	 * 
	 * @return The current traitmark permit whitelist.
	 */
	FORCEINLINE const FTraitmark&
	GetTraitmarkPermit() const
	{
		return ISubjective::GetTraitmarkPermit();
	}

	[[deprecated("The method was renamed. Use 'GetTraitmarkPermit', please.")]]
	FORCEINLINE const FTraitmark&
	GetTraitmarkPass() const
	{
		return GetTraitmarkPermit();
	}

	/**
	 * Set the server-side list of
	 * traits allowed to be received from clients.
	 * 
	 * Can only be set for a server-side subjective version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InTraitmarkPermit The new traitmark to be used as a whitelist.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	SetTraitmarkPermit(const FTraitmark& InTraitmarkPermit)
	{
		return ISubjective::template SetTraitmarkPermit<Paradigm>(InTraitmarkPermit);
	}

	template < EParadigm Paradigm = EParadigm::Default >
	[[deprecated("The method was renamed. Use 'SetTraitmarkPermit', please.")]]
	FORCEINLINE TOutcome<Paradigm>
	SetTraitmarkPass(const FTraitmark& InTraitmarkPermit)
	{
		return SetTraitmarkPermit<Paradigm>(InTraitmarkPermit);
	}

	/**
	 * Send an existing trait to a remote peer.
	 * 
	 * On a client the trait can only be send from a current connection's
	 * owned actor, i.e. a pawn or a player controller.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to send.
	 * Must not be a @c nullptr.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should a reliable channel be used for
	 * the transaction.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	PushTrait(UScriptStruct* const TraitType,
			  const EPeerRole      PeerRole = EPeerRole::Auto,
			  const bool           bReliable = true) const
	{
		return ISubjective::template PushTrait<Paradigm>(TraitType, PeerRole, bReliable);
	}

	/**
	 * Send an additional trait to a remote peer.
	 * Statically typed version.
	 * 
	 * On a client the trait can only be send from a current connection's
	 * owned actor, i.e. a pawn or a player controller.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to send.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should a reliable channel be used for the transaction.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE auto
	PushTrait(const EPeerRole PeerRole = EPeerRole::Auto,
			  const bool      bReliable = true) const
	{
		return ISubjective::template PushTrait<Paradigm, T>(PeerRole, bReliable);
	}

	/**
	 * Send an additional trait to a remote peer.
	 * Statically typed default paradigm version.
	 * 
	 * On a client the trait can only be send from a current connection's
	 * owned actor, i.e. a pawn or a player controller.
	 * 
	 * @tparam T The type of the trait to send.
	 * @tparam Paradigm The paradigm to work under.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should a reliable channel be used for transaction.
	 * @return The outcome of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	PushTrait(const EPeerRole PeerRole = EPeerRole::Auto,
			  const bool      bReliable = true) const
	{
		return ISubjective::template PushTrait<Paradigm, T>(PeerRole, bReliable);
	}

	/// @}
#pragma endregion Networking

	/**
	 * Get the preferred belt of the subjective (if any).
	 */
	FORCEINLINE UBelt*
	GetPreferredBelt() const
	{
		return ISubjective::GetPreferredBelt();
	}

	/**
	 * Get the current belt slot of the subjective (if any). Constant version.
	 *
	 * @return The active belt slot of the subject or <c>nullptr</c>,
	 * if the subject is not (yet) part of any belt.
	 */
	const struct FBeltSlot*
	GetSlot() const
	{
		return ISubjective::GetSlot();
	}

	/**
	 * Check if the subjective is registered
	 * within the machine.
	 */
	FORCEINLINE bool
	IsRegistered() const
	{
		return ISubjective::IsRegistered();
	}

	/**
	 * Get the current belt slot index of the subjective.
	 */
	FORCEINLINE int32
	GetSlotIndex() const
	{
		return ISubjective::GetSlotIndex();
	}

	/**
	 * Check if the subject is booted.
	 *
	 * The subject must be booted (initialized) by the booting mechanics
	 * prior to being processed by the ticking mechanics.
	 */
	FORCEINLINE bool
	IsBooted() const 
	{
		return ISubjective::GetHandle();
	}

	/**
	 * Get the subject handle of this subjective.
	 */
	FORCEINLINE FSolidSubjectHandle
	GetHandle() const
	{
		return (FSolidSubjectHandle)ISubjective::GetHandle();
	}

	/**
	 * Calculate the hash of the subjective.
	 */
	FORCEINLINE uint32
	CalcHash() const 
	{
		return ISubjective::CalcHash();
	}

	/**
	 * Get an actor corresponding to the subjective (if any).
	 * 
	 * Must be overridden in the descendants if they
	 * can derive an actor from itselves.
	 * 
	 * The default implementation just returns @c nullptr.
	 * 
	 * @return An actor this subjective belongs to.
	 * @return nullptr if the subject doesn't belong to any actor.
	 */
	FORCEINLINE AActor*
	GetActor() const
	{
		return ISubjective::GetActor();
	}

}; //-class ISolidSubjective

/**
 * Check if the supplied class is actually a subjective.
 * 
 * Also supports checking the solid subjectives.
 * 
 * @tparam C The class to examine.
 * @return The state of examination.
 */
template < class C >
constexpr bool IsSubjectiveClass()
{
	// This will also check the solid subjective,
	// since it is based on the ISubjective class:
	return std::is_base_of<ISubjective, C>::value;
}

#if CPP && !defined(SKIP_MACHINE_H)
#include "Machine.h"
#endif
