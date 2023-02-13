/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectInfo.h
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
#include "Math/Range.h"
#include "UObject/Class.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/WeakInterfacePtr.h"
#include "Templates/SharedPointer.h"
#include "Engine/NetConnection.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define SUBJECT_INFO_H_SKIPPED_MACHINE_H
#endif

#include "Fingerprint.h"
#include "TraitRecord.h"
#include "SubjectRecord.h"

#ifdef SUBJECT_INFO_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "SubjectInfo.generated.h"


// Forward declarations:
struct FCommonSubjectHandle;
struct FSubjectHandle;
struct FSolidSubjectHandle;
struct FConstSubjectHandle;
struct FUnsafeSubjectHandle;
struct FSubjectNetworkState;
struct FSubjectRecord;
struct FChunkSlot;
class UChunk;
class AMechanism;
class UMachine;
class ISubjective;
class UNetworkBearerComponent;

template < typename SubjectHandleT >
struct TChunkIt;

template < typename SubjectHandleT >
struct TBeltIt;

template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
struct TChain;

/**
 * A peer role type.
 */
UENUM(BlueprintType)
enum class EPeerRole : uint8
{
	/**
	 * "Not a peer role" identifier.
	 * 
	 * Also used in place for an automatic 
	 * role resolution.
	 */
	None = 0 UMETA(DisplayName = "None (Auto)"),

	/**
	 * A mnemonic synonym for the automatic peer resolution.
	 */
	Auto = None UMETA(Hidden),

	/**
	 * The peer is treated as a client.
	 */
	Client,

	/**
	 * The peer is treated as a server.
	 */
	Server
};

/**
 * The type of a subject's network identifier.
 */
using SubjectNetworkIdType = uint32;

/**
 * The internal state for the main subject entity.
 * 
 * Subjects are not used directly but instead
 * are referenced through ::FSubjectHandle and ::FSolidSubjectHandle.
 */
struct APPARATUSRUNTIME_API FSubjectInfo
{
  public:

	/**
	 * The type for the unique subject
	 * identifier.
	 */
	typedef int32 IdType;

	/**
	 * The type for the mechanism identifier.
	 */
	typedef uint8 MechanismIdType;

	/**
	 * The type of the chunk slot index
	 * of the subject.
	 */
	typedef int32 SlotIndexType;

	/**
	 * Invalid subject identifier.
	 */
	static constexpr IdType InvalidId = 0;

	/**
	 * A first valid subject place.
	 */
	static constexpr IdType FirstPlace = 0x1;

	/**
	 * Invalid subject slot index within the chunk.
	 */
	static constexpr SlotIndexType InvalidSlotIndex = -1;

	/**
	 * The maximum number of subject slots per chunk.
	 */
	static constexpr auto SlotsPerChunkMax = TNumericLimits<SlotIndexType>::Max();

	/**
	 * The shift of the subject identifier
	 * to recover the mechanism identifier.
	 */
	static constexpr int MechanismIdShift = 24;

	/**
	 * The mask for the shifted mechanism identifier.
	 */
	static constexpr IdType ShiftedMechanismIdMask = 0xFF000000;

	/**
	 * The mask for the subject index
	 * within a mechanism's subject pool
	 * which resides in the machine.
	 */
	static constexpr IdType PlaceMask = 0xFFFFFF;

	/**
	 * An invalid subject place identifier.
	 */
	static constexpr IdType InvalidPlace = 0x0;

	/**
	 * The maximum number of subjects within the mechanism.
	 */
	static constexpr IdType PlacesPerMechanismMax = 0xFFFFFF;

	/**
	 * The maximum possible subject place within
	 * the mechanism.
	 */
	static constexpr IdType LastPlace = PlacesPerMechanismMax - 1;

	/**
	 * Extract a mechanism identifier from a subject
	 * identifier.
	 * 
	 * A mechanism identifier is its unique index.
	 * 
	 * @param InSubjectId The subject identifier
	 * to extract from.
	 * @return The mechanism identifier.
	 */
	static constexpr FORCEINLINE MechanismIdType
	ExtractMechanismId(const IdType InSubjectId)
	{
		return static_cast<typename std::make_unsigned<IdType>::type>(InSubjectId) >> MechanismIdShift;
	}

	/**
	 * Extract a subject place from a subject
	 * identifier.
	 * 
	 * A place is a unique subject index within
	 * its mechanism.
	 * 
	 * @param InSubjectId The subject identifier
	 * to extract from.
	 * @return The subject place.
	 */
	static constexpr FORCEINLINE IdType
	ExtractPlace(const IdType InSubjectId)
	{
		return InSubjectId & PlaceMask;
	}

	/**
	 * Make a subject identifier from a mechanism identifier
	 * and a subject placement index.
	 * 
	 * @param MechanismId The identifier of the mechanism. 
	 * @param Place The place of the subject within the mechanism.
	 * @return The global subject identifier.
	 */
	static FORCEINLINE IdType
	MakeId(const MechanismIdType MechanismId, const IdType Place)
	{
		checkf((Place & ShiftedMechanismIdMask) == 0,
			   TEXT("There must be no mechanism identifier information within the place."));
		return static_cast<IdType>(
			(static_cast<typename std::make_unsigned<IdType>::type>(MechanismId) << MechanismIdShift) | Place);
	}

	/**
	 * The type for the subject generation.
	 * For the reasons of longevity the generation
	 * actually doesn't have an invalid value but
	 * is essentially looping its value on overflow.
	 */
	typedef uint32 GenerationType;

	/**
	 * The first valid generation.
	 */
	static constexpr GenerationType FirstGeneration = 0;

	/**
	 * The maximum valid generation.
	 */
	static constexpr GenerationType LastGeneration = TNumericLimits<GenerationType>::Max();

  private:

	friend struct FCommonSubjectHandle;
	friend struct FSubjectHandle;
	friend struct FSubjectNetworkState;
	friend struct FSolidSubjectHandle;
	friend struct FConstSubjectHandle;
	friend struct FSubjectRecord;
	friend class ASubjectiveActor;
	friend class USubjectiveActorComponent;
	friend class UChunk;
	friend class UNetworkBearerComponent;

	template < typename SubjectHandleT >
	friend struct TChunkIt;
	template < typename SubjectHandleT >
	friend struct TBeltIt;

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	friend class UMachine;
	friend class AMechanism;
	friend class ISubjective;

	/**
	 * A chunk this subject currently resides in (if any).
	 *
	 * You can obtain a subject's mechanism indirectly via this field.
	 * Otherwise, it is part of the user-level subject handle ::GTSubject
	 */
	UChunk* Chunk = nullptr;

	/**
	 * The unique identifier of the subject.
	 *
	 * Consists of a mechanism identifier byte
	 * and a 24-bit place index.
	 */
	IdType Id = InvalidId;

	/**
	 * A network state of the subject (if any).
	 */
	FSubjectNetworkState* NetworkState = nullptr;

	/**
	 * A unique generation of the subject.
	 *
	 * Used for checking the actuality,
	 * in relation to a handle.
	 * 
	 * This value is actually safe to be
	 * looped without any overflow protection
	 * as it will start with the first generation.
	 * This can potentially happen if the app
	 * has been running for too long.
	 */
	std::atomic<GenerationType> Generation{FirstGeneration};

	/**
	 * An index of the subject slot within its chunk.
	 */
	SlotIndexType SlotIndex = InvalidSlotIndex;

	/**
	 * The optional high-level subjective of the subject.
	 */
	ISubjective* Subjective = nullptr;

	/**
	 * Get the chunk slot this subject belongs to. Constant version.
	 */
	const FChunkSlot&
	GetChunkSlot() const;

	/**
	 * Get the chunk slot this subject belongs to.
	 */
	FChunkSlot&
	GetChunkSlot();

	/**
	 * Get a reference to the original fingerprint of the subject.
	 */
	FORCEINLINE FFingerprint&
	GetFingerprintRef();

	/**
	 * @internal
	 * An internal method to mark the subject as booted.
	 *
	 * @tparam The paradigm to work under.
	 * @return The previous state of the booted flag.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, bool>
	MarkBooted();

	/**
	 * Get the subject base handle corresponding to
	 * this information structure.
	 * 
	 * Returns a correctly set-up handle for this info,
	 * even if the info is not completely valid.
	 * 
	 * @return The subject handle.
	 */
	FCommonSubjectHandle
	GetHandleBase() const;

	/**
	 * Get a constant subject handle from the subject information.
	 * 
	 * @param OutSubject The subject handle to initialize.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	GetHandleBase(FCommonSubjectHandle& OutSubject) const;

	/**
	 * Obtain the networking state for the subject.
	 * 
	 * This just guarantees the existence of the
	 * network state information structure.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @return The network state pointer.
	 * 
	 * @see ObtainNetworkState(const uint32)
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, FSubjectNetworkState*>
	ObtainNetworkState();

	/**
	 * Obtain the networking state for the specific subject
	 * network identifier.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @return The network state pointer.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, FSubjectNetworkState*>
	ObtainNetworkState(const uint32 InSubjectNetworkId);

	/**
	 * Get the existing network state (if any). Constant version.
	 */
	FORCEINLINE const FSubjectNetworkState*
	GetNetworkState() const
	{
		return NetworkState;
	}

	/**
	 * Get the existing network state (if any).
	 */
	FORCEINLINE FSubjectNetworkState*
	GetNetworkState()
	{
		return NetworkState;
	}

	/**
	 * Increment a subject generation.
	 * 
	 * Adds up to the subject's generation essentially making
	 * all of the existing handles to it invalid.
	 */
	FORCEINLINE void
	DoIncrementGeneration()
	{
		// No checks for overflow
		// are performed here, since
		// every generation is fine and
		// should start anew in case
		// of some very long game running.
		++Generation;
	}

  public:

	/**
	 * Check if this is an information block
	 * about some actually spawned subject.
	 * 
	 * @note The info can be valid though
	 * the subject can be already stale.
	 * 
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	IsValid() const
	{
		check(Id != InvalidId);
		check((SlotIndex == InvalidSlotIndex) || (Chunk != nullptr));
		return SlotIndex != InvalidSlotIndex;
	}

	/**
	 * Get the unique identifier of the subject.
	 */
	FORCEINLINE IdType
	GetId() const
	{
		return Id;
	}

	/**
	 * Get the identifier of the mechanism the subject is part of.
	 */
	FORCEINLINE MechanismIdType
	GetMechanismId() const
	{
		return ExtractMechanismId(Id);
	}

	/**
	 * Get the place index of the subject.
	 * 
	 * This is the index of the subject information
	 * structure within the mechanism's subject pool.
	 */
	FORCEINLINE IdType
	GetPlace() const
	{
		return ExtractPlace(Id);
	}

	/**
	 * Get the chunk this subject resides in.
	 */
	FORCEINLINE UChunk*
	GetChunk() const
	{
		return Chunk;
	}

	/**
	 * Get the Unreal world the subject resides within.
	 * 
	 * This is derived from the mechanism.
	 * 
	 * @see GetMechanism()
	 */
	UWorld*
	GetWorld() const;

	/**
	 * Get the mechanism the subject resides within.
	 * 
	 * This is derived from the current chunk.
	 * 
	 * @see SetMechanism(), GetChunk()
	 */
	AMechanism*
	GetMechanism() const;

	/**
	 * Transition the subject to a designated mechanism.
	 * 
	 * @note You may only transition a subjective's subject
	 * to a mechanism that resides within the same world as its owner.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Mechanism The mechanism to transition to.
	 * @return The outcome of the operation.
	 * 
	 * @see GetMechanism()
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	SetMechanism(AMechanism* const Mechanism);

	/**
	 * Check if the subject is in a solid state now.
	 * 
	 * The subject is solid if it's either
	 * a part of a solid chunk or a part
	 * of a concurrent environment.
	 */
	bool
	IsSolid() const;

	/**
	 * Get the active fingerprint of the subject.
	 */
	const FFingerprint&
	GetFingerprint() const;

	/**
	 * Get the subject handle corresponding to
	 * this information structure.
	 *
	 * @tparam SubjectHandleT The type of the subject handle to get.
	 * @return The subject handle.
	 */
	template < typename SubjectHandleT = FSubjectHandle >
	FORCEINLINE SubjectHandleT
	GetHandle() const
	{
		return SubjectHandleT(Id, Generation);
	}

	/**
	 * Get a constant subject handle from the subject information.
	 *
	 * @tparam SubjectHandleT The type of the subject handle to get.
	 * @param OutSubject The subject handle to initialize.
	 * @return The status of the operation.
	 */
	template < typename SubjectHandleT >
	FORCEINLINE EApparatusStatus
	GetHandle(SubjectHandleT& OutSubject) const
	{
		OutSubject = SubjectHandleT(Id, Generation);
		return EApparatusStatus::Success;
	}

	/**
	 * Get the subjective associated with a subject (if any).
	 */
	FORCEINLINE ISubjective*
	GetSubjective() const
	{
		return Subjective;
	}

#pragma region Trait Data Access
	/// @name Trait Data Access
	/// @{

	/**
	 * Get a read-only trait data pointer from a subject by its type.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to get.
	 * @return A pointer to the read-only trait data.
	 * @return nullptr If there is no such trait in the subject.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, const void*>
	GetTraitPtr(UScriptStruct* const TraitType) const;

	/**
	 * Get a trait data pointer from a subject by its type.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to get.
	 * @return A pointer to the trait data.
	 * @return nullptr If there is no such trait in the subject.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, void*>
	GetTraitPtr(UScriptStruct* const TraitType);

	/**
	 * Get a read-only trait data pointer from a subject by its type.
	 * Templated paradigm constant version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return The pointer to the read-only trait data or @c nullptr.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, const T*>
	GetTraitPtr() const
	{
		return OutcomeStaticCast<const T*>(GetTraitPtr(T::StaticStruct()));
	}

	/**
	 * Get a read-only trait data pointer from a subject by its type.
	 * Templated constant version.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The pointer to the read-only trait data or @c nullptr.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, const T*>
	GetTraitPtr() const
	{
		return GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Get a trait data pointer from a subject by its type.
	 * Templated paradigm version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return The pointer to the trait data or @c nullptr.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, T*>
	GetTraitPtr()
	{
		return OutcomeStaticCast<T*>(GetTraitPtr<Paradigm>(T::StaticStruct()));
	}

	/**
	 * Get a trait data pointer from a subject by its type.
	 * Templated version.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The pointer to the trait data or @c nullptr.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, T*>
	GetTraitPtr()
	{
		return GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Get a read-only trait reference from a subject by its type.
	 * Constant templated version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A reference to the read-only trait data.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE const T&
	GetTraitRef() const
	{
		return *GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Get a read-only trait reference from a subject by its type.
	 * Constant templated version.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @return A reference to the read-only trait data.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE const T&
	GetTraitRef() const
	{
		return GetTraitRef<Paradigm, T>();
	}

	/**
	 * Get a trait reference from a subject by its type.
	 * Templated version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A reference to the trait data.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE T&
	GetTraitRef()
	{
		return *GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Get a trait reference from a subject by its type.
	 * Templated version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A reference to the trait data.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE T&
	GetTraitRef()
	{
		return GetTraitRef<Paradigm, T>();
	}

	/// @}
#pragma endregion Trait Data Access

#pragma region Multi-Trait Data Access
	/// @name Multi-Trait Data Access
	/// @{

	/**
	 * Get a list of immutable traits data pointers.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @param TraitType The type of the trait to get.
	 * @param OutTraits The results receiver.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm   = EParadigm::DefaultInternal,
			   typename  AllocatorT = FDefaultAllocator >
	TOutcome<Paradigm>
	GetTraitsPtrs(UScriptStruct* const             TraitType,
				  TArray<const void*, AllocatorT>& OutTraits) const;

	/**
	 * Get a list of mutable traits data pointers.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @param TraitType The type of the trait to get.
	 * @param OutTraits The results receiver.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm   = EParadigm::DefaultInternal,
			   typename  AllocatorT = FDefaultAllocator >
	TOutcome<Paradigm>
	GetTraitsPtrs(UScriptStruct* const       TraitType,
				  TArray<void*, AllocatorT>& OutTraits);

	/**
	 * Get a list of immutable traits data pointers.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @param OutTraits The results receiver.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm   = EParadigm::DefaultInternal,
			   typename  T          = void,
			   typename  AllocatorT = FDefaultAllocator,
			   TTraitTypeSecurity<T> = true >
	TOutcome<Paradigm>
	GetTraitsPtrs(TArray<const T*, AllocatorT>& OutTraits) const;

	/**
	 * Get a list of mutable traits data pointers.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @param OutTraits The results receiver.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm   = EParadigm::DefaultInternal,
			   typename  T          = void,
			   typename  AllocatorT = FDefaultAllocator,
			   TTraitTypeSecurity<T> = true >
	TOutcome<Paradigm>
	GetTraitsPtrs(TArray<T*, AllocatorT>& OutTraits);

	/**
	 * Get a list of immutable traits data pointers.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @param TraitType The type of the trait to get.
	 * @return The resulting list of traits.
	 */
	template < EParadigm Paradigm   = EParadigm::DefaultInternal,
			   typename  AllocatorT = FDefaultAllocator >
	FORCEINLINE TOutcome<Paradigm, TArray<const void*, AllocatorT>>
	GetTraitsPtrs(UScriptStruct* const TraitType) const
	{
		TArray<const void*, AllocatorT> TraitsTemp;
		const auto Outcome = GetTraitsPtrs<Paradigm>(TraitType, TraitsTemp);
		return MakeOutcome<Paradigm, TArray<const void*, AllocatorT>>(Outcome, MoveTemp(TraitsTemp));
	}

	/**
	 * Get a list of mutable traits data pointers.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @param TraitType The type of the trait to get.
	 * @return The resulting list of traits.
	 */
	template < EParadigm Paradigm   = EParadigm::DefaultInternal,
			   typename  AllocatorT = FDefaultAllocator >
	FORCEINLINE TOutcome<Paradigm, TArray<void*, AllocatorT>>
	GetTraitsPtrs(UScriptStruct* const TraitType)
	{
		using ArrayType = TArray<void*, AllocatorT>;
		ArrayType TraitsTemp;
		const auto Outcome = GetTraitsPtrs<Paradigm>(TraitType, TraitsTemp);
		return MakeOutcome<Paradigm, ArrayType>(Outcome, MoveTemp(TraitsTemp));
	}

	/**
	 * Get a list of immutable traits data pointers.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of traits to get.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @return The resulting list of traits.
	 */
	template < EParadigm Paradigm   = EParadigm::DefaultInternal,
			   typename  T          = void,
			   typename  AllocatorT = FDefaultAllocator,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, TArray<const T*, AllocatorT>>
	GetTraitsPtrs() const
	{
		using ArrayType = TArray<const T*, AllocatorT>;
		ArrayType TraitsTemp;
		const auto Outcome = GetTraitsPtrs<Paradigm>(TraitsTemp);
		return MakeOutcome<Paradigm, ArrayType>(Outcome, MoveTemp(TraitsTemp));
	}

	/**
	 * Get a list of mutable traits data pointers.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of traits to get.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @return The resulting list of traits.
	 */
	template < EParadigm Paradigm   = EParadigm::DefaultInternal,
			   typename  T          = void,
			   typename  AllocatorT = FDefaultAllocator,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, TArray<T*, AllocatorT>>
	GetTraitsPtrs()
	{
		using ArrayType = TArray<T*, AllocatorT>;
		ArrayType TraitsTemp;
		const auto Outcome = GetTraitsPtrs<Paradigm>(TraitsTemp);
		return MakeOutcome<Paradigm, ArrayType>(Outcome, MoveTemp(TraitsTemp));
	}

	/**
	 * Get a list of immutable traits data pointers.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam T The type of traits to get.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The resulting list of traits.
	 */
	template < typename  T,
			   typename  AllocatorT = FDefaultAllocator,
			   EParadigm Paradigm   = EParadigm::DefaultInternal,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, TArray<const T*, AllocatorT>>
	GetTraitsPtrs() const
	{
		return GetTraitsPtrs<Paradigm, T, AllocatorT>();
	}

	/**
	 * Get a list of mutable traits data pointers.
	 * Default paradigm version.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam T The type of traits to get.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The resulting list of traits.
	 */
	template < typename  T,
			   typename  AllocatorT = FDefaultAllocator,
			   EParadigm Paradigm   = EParadigm::DefaultInternal,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, TArray<T*, AllocatorT>>
	GetTraitsPtrs()
	{
		return GetTraitsPtrs<Paradigm, T, AllocatorT>();
	}

	/// @}
#pragma endregion Multi-Trait Data Access

#pragma region Trait Setting
	/// @name Trait Setting
	/// @{

	/**
	 * Set a trait of the subject by its type.
	 * Dynamically typed version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and set accordingly.
	 * 
	 * @note This also supports passing a dynamic trait record
	 * and will cast it appropriately.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to add.
	 * May be a @c nullptr and the method do nothing in that case.
	 * @param[in] TraitData The trait data to initialize with.
	 * Must not be a @c nullptr, if the @p TraitType is not a @c nullptr.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	SetTrait(UScriptStruct* const TraitType,
			 const void* const    TraitData);

	/**
	 * Set a trait of the subject to the record contents.
	 * 
	 * If the trait of the corresponding type is not
	 * currently within the subject,
	 * it gets created anew and set accordingly.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitRecord The trait record to set to.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	SetTrait(const FTraitRecord& TraitRecord);

#pragma region Multi-Trait Setting
	/// @name Multi-Trait Setting
	/// @{

	/**
	 * Set multiple subject traits at once.
	 * 
	 * This is generally more performant than setting traits sequentially, one by one.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Ts The types of the traits to set.
	 * @param Traits The traits to set to.
	 * @return The outcome of the operation.
	 */
	template< EParadigm Paradigm, typename... Ts,
			  TTraitTypesSecurity<Ts...> = true >
	TOutcome<Paradigm>
	SetTraits(Ts&&... Traits);

	/**
	 * Set multiple subject traits at once.
	 * 
	 * This is generally more performant than setting traits sequentially, one by one.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Ts The types of the traits to set.
	 * @param Traits The traits to set to.
	 * @param bLeaveRedundant Should redundant existing traits
	 * be left in the subject.
	 * @return The outcome of the operation.
	 */
	template< EParadigm Paradigm, typename... Ts,
			  TTraitTypesSecurity<Ts...> = true >
	TOutcome<Paradigm>
	SetTraits(Ts&&... Traits, const bool bLeaveRedundant);

	/**
	 * Set multiple subject traits at once.
	 * 
	 * This is generally more performant than setting traits sequentially, one by one.
	 *
	 * @tparam Ts The types of the traits to set.
	 * @tparam Paradigm The paradigm to work under.
	 * @param Traits The traits to set to.
	 * @return The outcome of the operation.
	 */
	template< typename... Ts,
			  EParadigm Paradigm = EParadigm::DefaultInternal,
			  TTraitTypesSecurity<Ts...> = true >
	FORCEINLINE TOutcome<Paradigm>
	SetTraits(Ts&&... Traits)
	{
		return SetTraits<Paradigm>(std::forward<Ts>(Traits)...);
	}

	/**
	 * Set multiple subject traits at once.
	 * 
	 * This is generally more performant than setting traits sequentially, one by one.
	 *
	 * @tparam Ts The types of the traits to set.
	 * @tparam Paradigm The paradigm to work under.
	 * @param Traits The traits to set to.
	 * @param bLeaveRedundant Should redundant existing traits
	 * be left in the subject.
	 * @return The outcome of the operation.
	 */
	template< typename... Ts,
			  EParadigm Paradigm = EParadigm::DefaultInternal,
			  TTraitTypesSecurity<Ts...> = true >
	FORCEINLINE TOutcome<Paradigm>
	SetTraits(Ts&&... Traits, const bool bLeaveRedundant)
	{
		return SetTraits<Paradigm>(std::forward<Ts>(Traits)..., bLeaveRedundant);
	}

	/**
	 * Set multiple subject traits equal to a supplied list.
	 * 
	 * This is generally more performant than setting traits sequentially, one by one.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam TraitsExtractorT The type of the traits-extracting container.
	 * @param TraitsExtractor The types of the traits to add
	 * supplied via an extractor.
	 * @param bLeaveRedundant Should redundant existing traits
	 * be left in the subject.
	 * @return The outcome of the operation.
	 */
	template< EParadigm Paradigm = EParadigm::DefaultInternal,
			  typename TraitsExtractorT = void >
	TOutcome<Paradigm>
	SetTraits(const TraitsExtractorT& TraitsExtractor,
			  const bool              bLeaveRedundant = true);

	/**
	 * Set multiple subject traits equal to the supplied list.
	 * 
	 * This is generally more performant than setting traits sequentially, one by one.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam A1 The allocator type of the types array.
	 * @tparam A2 The allocator type of the traits data array.
	 * @tparam T The type of the traits data elements. May be a @c void.
	 * @param TraitsTypes The types of traits to add.
	 * @param TraitsData The corresponding traits data to initialize with.
	 * @param bLeaveRedundant Should redundant existing traits
	 * be left in the subject.
	 * @return The status of the operation.
	 */
	template< EParadigm Paradigm = EParadigm::DefaultInternal,
			  typename A1 = FDefaultAllocator, typename A2 = FDefaultAllocator,
			  typename T = void >
	TOutcome<Paradigm>
	SetTraits(const TArray<UScriptStruct*, A1>& TraitsTypes,
			  const TArray<const T*, A2>&       TraitsData,
			  const bool                        bLeaveRedundant = true);

	/**
	 * Set multiple subject traits from a subject record.
	 * 
	 * The traits essentially get copied from the record.
	 * 
	 * This is generally more performant than setting traits sequentially, one by one.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectRecord The subject traits pack to set from.
	 * @param bLeaveRedundant Should redundant existing traits
	 * (not present in the record) be left in the subject.
	 * @return The outcome of the operation.
	 */
	template< EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	SetTraits(const FSubjectRecord& SubjectRecord,
			  const bool            bLeaveRedundant = true);

	/// @}
#pragma endregion Multi-Trait Setting

	/**
	 * Set a trait of the subject with a pointer it.
	 * Statically-typed default paradigm version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and set accordingly.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to set.
	 * @param Trait The trait to initialize with.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	SetTrait(const T* const Trait)
	{
		return SetTrait(T::StaticStruct(),
						static_cast<const void*>(Trait));
	}

	/**
	 * Set a trait of the subject with a pointer to it.
	 * Statically-typed default paradigm version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and set accordingly.
	 *
	 * @tparam T The type of the trait to set.
	 * @tparam Paradigm The paradigm to work under.
	 * @param Trait A pointer to the trait to initialize with.
	 * @return The outcome of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE auto
	SetTrait(const T* const Trait)
	{
		return SetTrait<Paradigm, T>(Trait);
	}

	/**
	 * Set a trait of the subject.
	 * Templated paradigm version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and set accordingly.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to set.
	 * @param Trait The trait to initialize with.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	SetTrait(const T& Trait)
	{
		return SetTrait(&Trait);
	}

	/**
	 * Set a trait of the subject.
	 * Templated version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and set accordingly.
	 *
	 * @tparam T The type of the trait to set.
	 * @tparam Paradigm The paradigm to work under.
	 * @param Trait The trait to initialize with.
	 * @return The outcome of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE auto
	SetTrait(const T& Trait)
	{
		return SetTrait<Paradigm, T>(Trait);
	}

	/**
	 * Overwrite existing traits from a generic extractor.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam TraitsExtractorT The type of the traits-extracting container.
	 * @param TraitsExtractor The types of the traits to add
	 * supplied via an extractor.
	 * @return The outcome of the operation.
	 */
	template< EParadigm Paradigm = EParadigm::DefaultInternal, typename TraitsExtractorT = void >
	TOutcome<Paradigm>
	OverwriteTraits(const TraitsExtractorT& TraitsExtractor);

	/**
	 * Overwrite existing subject traits from the array.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam A1 The allocator type of the types array.
	 * @tparam A2 The allocator type of the traits data array.
	 * @tparam T The type of the traits data elements. May be a @c void.
	 * @param TraitsTypes The types of traits to overwrite.
	 * @param TraitsData The corresponding traits data to overwrite with.
	 * @return The status of the operation.
	 */
	template< EParadigm Paradigm = EParadigm::DefaultInternal,
			  typename A1 = FDefaultAllocator, typename A2 = FDefaultAllocator,
			  typename T = void >
	TOutcome<Paradigm>
	OverwriteTraits(const TArray<UScriptStruct*, A1>& TraitsTypes,
					const TArray<const T*, A2>&       TraitsData);

	/**
	 * Overwrite existing traits of the subject to the record contents.
	 * 
	 * If the trait of the corresponding type is not
	 * currently within the subject,
	 * it gets created anew and set accordingly.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectRecord The subject record to overwrite with.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	OverwriteTraits(const FSubjectRecord& SubjectRecord);

	/// @}
#pragma endregion Trait Setting

#pragma region Deferred Trait Setting
	/// @name Deferred Trait Setting
	/// @{

	/**
	 * Set a trait of the subject by its type.
	 * Deferred version.
	 * 
	 * The actual change is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 * 
	 * At the time of performing the operation,
	 * if the trait is not currently within the subject,
	 * it gets created anew and set accordingly.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] TraitType The type of the trait to add.
	 * @param[in] TraitData The trait data to initialize with.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	SetTraitDeferred(UScriptStruct* const TraitType,
					 const void* const    TraitData);

	/**
	 * Set a trait of the subject by its type.
	 * Deferred templated paradigm version.
	 * 
	 * The actual change is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 * 
	 * At the time of performing the operation,
	 * if the trait is not currently within the subject,
	 * it gets created anew and set accordingly.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to set.
	 * @param Trait The trait to initialize with.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm>
	SetTraitDeferred(const T& Trait)
	{
		return SetTraitDeferred<Paradigm>(T::StaticStruct(), static_cast<const void*>(&Trait));
	}

	/**
	 * Set a trait of the subject by its type.
	 * Deferred templated version.
	 * 
	 * The actual change is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 * 
	 * At the time of performing the operation,
	 * if the trait is not currently within the subject,
	 * it gets created anew and set accordingly.
	 *
	 * @tparam T The type of the trait to set.
	 * @param Trait The trait to initialize with.
	 * @return The outcome of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE auto
	SetTraitDeferred(const T& Trait)
	{
		return SetTraitDeferred<Paradigm, T>(Trait);
	}

	/// @}
#pragma endregion Deferred Trait Setting

#pragma region Trait Getting
	/// @name Trait Getting
	/// @{

	/**
	 * Get a trait from the subject by its type.
	 *
	 * @param[in] TraitType The type of the trait to get.
	 * @param[out] OutTraitData The trait data receiver.
	 * Must have enough bytes for the trait to be copied.
	 * @param[in] bTraitDataInitialized Is the @p OutTraitData buffer actually initialized?
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Missing If there is no such trait within the subject.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	GetTrait(UScriptStruct* const TraitType,
			 void* const          OutTraitData,
			 const bool           bTraitDataInitialized = true) const;

	/**
	 * Get a trait from the subject into
	 * a trait record.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param[out] OutTraitRecord The trait record to receive the trait.
	 * Its type must be set accordingly beforehand.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Missing If the subject is not within the subject.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	GetTrait(FTraitRecord& OutTraitRecord) const;

	/**
	 * Get a trait from a subject by its type. Templated version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @param OutTrait The trait receiver.
	 * @param bTraitDataInitialized Is the @p OutTrait actually initialized?
	 * @return The status of the operation.
	 * @return EApparatusStatus::Missing If the subject is not within the subject.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm>
	GetTrait(T& OutTrait, const bool bTraitDataInitialized = true) const
	{
		return GetTrait<Paradigm>(T::StaticStruct(),
								  static_cast<void*>(&OutTrait),
								  bTraitDataInitialized);
	}

	/**
	 * Get a trait from a subject by its type. Templated version.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @param OutTrait The trait receiver.
	 * @param bTraitDataInitialized Is the @p OutTrait actually initialized?
	 * @return The status of the operation.
	 * @return EApparatusStatus::Missing If the subject is not within the subject.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm>
	GetTrait(T& OutTrait, const bool bTraitDataInitialized = true) const
	{
		return GetTrait<Paradigm>(OutTrait, bTraitDataInitialized);
	}

	/**
	 * Get a trait copy from a subject by its type. Templated version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A copy of the trait.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, T>
	GetTrait() const
	{
		T Trait;
		if (AvoidError(Paradigm, GetTrait(Trait)))
		{
			return MakeOutcome<Paradigm, T>(FApparatusStatus::GetLastError(), T());
		}
		return MoveTempIfPossible(Trait);
	}

	/**
	 * Get a trait copy from a subject by its type. Templated version.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @return A copy of the trait.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal,
			   TTraitTypeSecurity<T> = true >
	FORCEINLINE TOutcome<Paradigm, T>
	GetTrait() const
	{
		return GetTrait<Paradigm, T>();
	}

	/// @}
#pragma endregion Trait Getting

#pragma region Trait Obtainment
	/// @name Trait Obtainment
	/// @{

	/**
	 * Obtain a trait data pointer from a subject by its type.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and returned
	 * to the receiver.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to obtain.
	 * @return The obtained trait data pointer.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, void*>
	ObtainTraitPtr(UScriptStruct* const TraitType);

	/**
	 * Obtain a trait for a subject by its type.
	 * Plain ensuring version without return value.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew for the subject.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to obtain.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	ObtainTrait(UScriptStruct* const TraitType);

	/**
	 * Obtain a trait data pointer from the subject by its type.
	 * Templated paradigm version
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and returned
	 * to the receiver.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to obtain.
	 * @return The obtained trait pointer.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>(), T*>
	ObtainTraitPtr()
	{
		return static_cast<T*>(ObtainTraitPtr<Paradigm>(T::StaticStruct()));
	}

	/**
	 * Obtain a trait data pointer from the subject by its type.
	 * Statically-typed default paradigm version
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and returned
	 * to the receiver.
	 *
	 * @tparam T The type of the trait to obtain.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The obtained trait pointer.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE auto
	ObtainTraitPtr()
	{
		return ObtainTraitPtr<Paradigm, T>();
	}

	/**
	 * Obtain a trait reference from the subject by its type.
	 * Templated paradigm version
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and its reference is returned
	 * to the user.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to obtain.
	 * @return The obtained reference to the trait.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>(), T&>
	ObtainTraitRef()
	{
		return *ObtainTraitPtr<Paradigm, T>();
	}

	/**
	 * Obtain a trait reference from the subject by its type.
	 * Templated version
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and its reference is returned
	 * to the user.
	 *
	 * @tparam T The type of the trait to obtain.
	 * @return The obtained trait reference.
	 */
	template < typename T >
	FORCEINLINE auto
	ObtainTraitRef()
	{
		return ObtainTraitRef<EParadigm::DefaultInternal, T>();
	}

	/**
	 * Obtain a trait from the subject by its type.
	 * Outputting version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and the default value gets copied
	 * to the receiver.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] TraitType The type of the trait to obtain.
	 * @param[out] OutTraitData The trait data receiver pointer.
	 * Must be enough to hold the @p TraitType instance.
	 * @param[in] bTraitDataInitialized Is the @p TraitData buffer actually initialized?
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	ObtainTrait(UScriptStruct* const TraitType,
				void* const          OutTraitData,
				const bool           bTraitDataInitialized = true);

	/**
	 * Obtain a trait from the subject by its type.
	 * Outputting to pointer version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and the default value gets copied
	 * to the receiver.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to obtain.
	 * @param[out] OutTraitData The trait data receiver pointer.
	 * @param[in] bTraitDataInitialized Is the @p TraitData buffer actually initialized?
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal, typename T = void >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	ObtainTrait(T* const   OutTraitData,
				const bool bTraitDataInitialized = true)
	{
		return ObtainTrait(T::StaticStruct(), static_cast<void*>(OutTraitData), bTraitDataInitialized);
	}

	/**
	 * Obtain a trait from the subject.
	 * Templated paradigm outputting version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and the default value gets copied
	 * to the receiver.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to obtain.
	 * @param[out] OutTrait The trait receiver.
	 * @param[in] bTraitDataInitialized Is the @p OutTrait actually initialized?
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE auto
	ObtainTrait(T&         OutTrait,
				const bool bTraitDataInitialized = true)
	{
		return ObtainTrait<Paradigm>(&OutTrait, bTraitDataInitialized);
	}

	/**
	 * Obtain a trait from the subject.
	 * Templated copying to destination version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and the default value gets copied
	 * to the receiver.
	 *
	 * @tparam T The type of the trait to obtain.
	 * @param[out] OutTrait The trait receiver.
	 * @param[in] bTraitDataInitialized Is the @p OutTrait actually initialized?
	 * @return The outcome of the operation.
	 */
	template < typename T >
	FORCEINLINE auto
	ObtainTrait(T&         OutTrait,
				const bool bTraitDataInitialized = true)
	{
		return ObtainTrait<EParadigm::DefaultInternal, T>(OutTrait,
														  bTraitDataInitialized);
	}

	/**
	 * Obtain a trait from the subject.
	 * Templated paradigm copy-returning version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to obtain.
	 * @return A copy of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>(), T>
	ObtainTrait()
	{
		T TraitTemp;
		const auto Outcome = ObtainTrait<Paradigm, T>(TraitTemp);
		return MakeOutcome<Paradigm, T>(Outcome, MoveTempIfPossible(TraitTemp));
	}

	/**
	 * Obtain a trait from the subject.
	 * Templated copy-returning version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and returned.
	 *
	 * @tparam T The type of the trait to obtain.
	 * @return A copy of the trait.
	 */
	template < typename T >
	FORCEINLINE auto
	ObtainTrait()
	{
		return ObtainTrait<EParadigm::DefaultInternal, T>();
	}

	/**
	 * Obtain a trait from a subject.
	 * Deferred version.
	 * 
	 * If the trait is not currently within the subject,
	 * a EApparatusStatus::Deferred is returned.
	 * 
	 * The actual change (if any) will be
	 * deferred until the deferreds are applied
	 * either manually or automatically.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to obtain.
	 * Must not be a @c nullptr.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	ObtainTraitDeferred(UScriptStruct* const TraitType);

	/**
	 * Obtain a trait from a subject.
	 * Templated paradigm deferred version.
	 * 
	 * If the trait is not currently within the subject,
	 * a default trait is returned.
	 * 
	 * The actual change (if any) will be
	 * deferred until the deferreds are applied
	 * either manually or automatically.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to obtain.
	 * @return A copy of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	TOutcomeIf<Paradigm, IsTraitType<T>(), T>
	ObtainTraitDeferred();

	/**
	 * Obtain a trait from a subject.
	 * Templated deferred version.
	 * 
	 * If the trait is not currently within the subject,
	 * a default trait is returned.
	 * 
	 * The actual change (if any) will be
	 * deferred until the deferreds are applied
	 * either manually or automatically.
	 *
	 * @tparam T The type of the trait to obtain.
	 * @return A copy of the trait.
	 */
	template < typename T >
	FORCEINLINE auto
	ObtainTraitDeferred()
	{
		return ObtainTraitDeferred<EParadigm::DefaultInternal, T>();
	}

	/// @}
#pragma endregion Trait Obtainment

#pragma region Traits Removal
	/// @name Traits Removal
	/// @{

	/**
	 * Remove a trait from the subject.
	 * 
	 * If there is no such trait in the subject,
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to remove.
	 * @param bExact Should the exact trait matching be used instead of a hereditary one.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop If there is no such trait
	 * in the subject.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	RemoveTrait(UScriptStruct* const TraitType,
				const bool           bExact = false);

	/**
	 * Remove a trait from the subject.
	 * Templated paradigm version.
	 * 
	 * If there is no such trait in the subject,
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to remove.
	 * @param bExact Should the exact trait matching be used instead of a hereditary one.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	RemoveTrait(const bool bExact = false)
	{
		return RemoveTrait(T::StaticStruct(), bExact);
	}

	/**
	 * Remove a trait from the subject.
	 * Templated paradigm version.
	 * 
	 * If there is no such trait in the subject,
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 *
	 * @tparam T The type of the trait to remove.
	 * @param bExact Should the exact trait matching be used instead of a hereditary one.
	 * @return The outcome of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE auto
	RemoveTrait(const bool bExact = false)
	{
		return RemoveTrait<Paradigm, T>(bExact);
	}

	/**
	 * Remove traits from the subject.
	 * 
	 * If there is no such trait in the subject,
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The base type of the traits to remove.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop If there were no such traits
	 * in the subject.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	RemoveTraits(UScriptStruct* const TraitType);

	/**
	 * Remove traits from the subject based on a type.
	 * Statically-typed version.
	 * 
	 * If there is no such trait in the subject,
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The base type of the traits to remove.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop If there were no such traits
	 * in the subject.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm>
	RemoveTraits()
	{
		return RemoveTraits(T::StaticStruct());
	}

	/**
	 * Remove traits from the subject based on a type.
	 * Statically-typed default paradigm version.
	 * 
	 * If there is no such trait in the subject,
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 *
	 * @tparam T The base type of the traits to remove.
	 * @tparam Paradigm The paradigm to work under.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop If there were no such traits
	 * in the subject.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE TOutcome<Paradigm>
	RemoveTraits()
	{
		return RemoveTraits<Paradigm, T>();
	}

	/**
	 * Remove all of the traits from the subject.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop if there are no traits in the subject.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	RemoveAllTraits();

	/// @}
#pragma endregion Traits Removal

#pragma region Deferred Trait Removal
	/// @name Deferred Trait Removal
	/// @{

	/**
	 * Remove a trait from the subject.
	 * Dynamically typed deferred version.
	 *
	 * The actual change is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 * 
	 * If at the time of performing the operation,
	 * the trait is not within the subject,
	 * nothing gets performed.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to remove.
	 * If @c nullptr, nothing is performed.
	 * @param bForceEnqueue Should the removal be enqueued even
	 * if there's currently no such trait within the subject.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	RemoveTraitDeferred(UScriptStruct* const TraitType,
						const bool           bForceEnqueue = false);

	/**
	 * Remove a trait from the subject.
	 * Statically typed paradigm deferred version.
	 * 
	 * The actual change is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 * 
	 * If at the time of performing the operation,
	 * the trait is not within the subject,
	 * nothing gets performed.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to remove.
	 * @param bForceEnqueue Should the removal be enqueued even
	 * if there's currently no such trait within the subject.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	RemoveTraitDeferred(const bool bForceEnqueue = false)
	{
		return RemoveTraitDeferred<Paradigm>(T::StaticStruct(), bForceEnqueue);
	}

	/**
	 * Remove a trait from the subject.
	 * Statically typed paradigm deferred version.
	 * 
	 * The actual change is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 * 
	 * If at the time of performing the operation,
	 * the trait is not within the subject,
	 * nothing gets performed.
	 *
	 * @tparam T The type of the trait to remove.
	 * @param bForceEnqueue Should the removal be enqueued even
	 * if there's currently no such trait within the subject.
	 * @return The status of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE auto
	RemoveTraitDeferred(const bool bForceEnqueue = false)
	{
		return RemoveTraitDeferred<Paradigm, T>(bForceEnqueue);
	}

	/**
	 * Remove all of the traits from the subject.
	 * Deferred version.
	 * 
	 * The actual change is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param bForceEnqueue Should the traits be enqueued
	 * to be removed even if there are none currently.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop if there are no traits in the subject,
	 * unless @p bForceEnqueue is set to @c true.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	RemoveAllTraitsDeferred(const bool bForceEnqueue = false);

	/// @}
#pragma endregion Deferred Trait Removal

#pragma region Flagmark
	/// @name Flagmark
	/// @{

	/**
	 * Get the flagmark of the subject.
	 *
	 * The examination is atomic and thread-safe.
	 */
	EFlagmark
	GetFlagmark() const;

	/**
	 * Set the flagmark of the subject.
	 * 
	 * @note Residing within the info, this is
	 * a low-level method with no checks on the argument.
	 * 
	 * This operation is atomic and thread-safe.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flagmark The flagmark to set.
	 * @return The previous flagmark value.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, EFlagmark>
	SetFlagmark(const EFlagmark Flagmark);

	/**
	 * Set the flagmark of the subject.
	 * 
	 * @note Residing within the info, this is
	 * a low-level method with no checks on the argument.
	 * 
	 * This operation is atomic and thread-safe.
	 * 
	 * @param Flagmark The flagmark to set.
	 * @return The status of the operation.
	 */
	[[deprecated("The method is deprecated. Use the 'SetFlagmark' method with a polite outcome.")]]
	EApparatusStatus
	SetFlagmark_Status(const EFlagmark Flagmark);

	/**
	 * Set the flagmark of the subject to a masked flagmark.
	 *
	 * @note Residing within the info, this is
	 * a low-level method with no checks on the arguments.
	 * 
	 * The operations is atomic and thread-safe.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flagmark The flagmark to set.
	 * @param Mask The mask to apply.
	 * @return The previous flagmark masked with the passed-in mask.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, EFlagmark>
	SetFlagmarkMasked(const EFlagmark Flagmark,
					  const EFlagmark Mask);

	/**
	 * Set the flagmark of the subject to a masked flagmark.
	 * Status version.
	 *
	 * @note Residing within the info, this is
	 * a low-level method with no checks on the arguments.
	 * 
	 * The operations is atomic and thread-safe.
	 * 
	 * @param Flagmark The flagmark to set.
	 * @param Mask The mask to apply.
	 * @return The status of the operation.
	 */
	[[deprecated("The method is deprecated. Use the 'SetFlagmarkMasked' method with a polite outcome.")]]
	EApparatusStatus
	SetFlagmarkMasked_Status(const EFlagmark Flagmark,
							 const EFlagmark Mask);

	/**
	 * Add flags to the subject's flagmark.
	 * 
	 * The operations is atomic and thread-safe.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flagmark The flagmark to add.
	 * @return The previous flagmark.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, EFlagmark>
	AddToFlagmark(const EFlagmark Flagmark);

	/**
	 * Get the state of a subject flag.
	 * 
	 * The examination is atomic and thread-safe.
	 * 
	 * @param Flag The flag to get the state of.
	 * @return The state of the flag.
	 */
	bool
	HasFlag(const EFlagmarkBit Flag) const;

	/**
	 * Set the state of a subject flag.
	 * 
	 * This method doesn't perform any additional
	 * checks on the argument since it's in an
	 * internal subject info structure.
	 * 
	 * The operation is atomic and thread-safe.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flag The flag to set the state of.
	 * @param State The state to set the flag to.
	 * @return The previous state of the flag.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, bool>
	SetFlag(const EFlagmarkBit Flag,
			const bool         State = true);

	/**
	 * Set the state of a subject flag.
	 * Status version.
	 * 
	 * This method doesn't perform any additional
	 * checks on the argument since it's in an
	 * internal subject info structure.
	 * 
	 * The operation is atomic and thread-safe.
	 * 
	 * @param Flag The flag to set the state of.
	 * @param State The state to set the flag to.
	 * @return The status of the operation.
	 */
	[[deprecated("The method is deprecated. Use the 'SetFlag' method with a polite outcome.")]]
	EApparatusStatus
	SetFlag_Status(const EFlagmarkBit Flag,
				   const bool         State = true);

	/**
	 * Toggle the state of a subject flag.
	 * 
	 * This method doesn't perform any additional
	 * checks on the argument since it's in an
	 * internal subject info structure.
	 * 
	 * The operation is atomic and thread-safe.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flag The flag to toggle.
	 * @return The new state of the flag.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, bool>
	ToggleFlag(const EFlagmarkBit Flag);

	/// @}
#pragma endregion Flagmark

#pragma region Networking
	/// @name Networking
	/// @{

	/**
	 * Get the owning net connection of the subject (if any).
	 */
	UNetConnection*
	GetNetConnection() const;

	/**
	 * Check if the subject currently networked.
	 * 
	 * Networked subject can receive and push traits
	 * over the network to the remote versions of themselves.
	 * 
	 * @see GetNetworkId() BringOnline()
	 */
	bool
	IsOnline() const;

	/**
	 * Make the subject networked. Server-only method.
	 * 
	 * This assigns a new, unique, cross-peer identifier
	 * to the subject and spawns an empty instance of it
	 * on the client(s).
	 * 
	 * If the subject is already online, nothing is performed.
	 * 
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop if the subject is already online.
	 * 
	 * @see IsOnline() GetNetworkId()
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	BringOnline();

	/**
	 * Make the subject networked while using a custom network identifiers range. Server-only method.
	 * 
	 * This assigns a new, unique, cross-peer identifier
	 * to the subject and spawns an empty instance of it
	 * on the client(s).
	 * 
	 * If the subject is already online, nothing is performed.
	 * 
	 * @param IdRange The range of identifiers to allocate within.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop if the subject is already online.
	 * 
	 * @see IsOnline() GetNetworkId()
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	BringOnline(const TRange<SubjectNetworkIdType>& IdRange);

	/**
	 * Get a unique network identifier of a subject (if any).
	 * 
	 * This unique identifier clearly and unequivocally identifies
	 * a subject within multiple peers (client(s) and server)
	 * and is used internally to address the subject in the
	 * networking environment.
	 * 
	 * @return The unique cross-peer identifier of the subject
	 * or FSubjectNetworkState::InvalidId, if the subject
	 * is not online just yet.
	 * 
	 * @see IsOnline() BringOnline()
	 */
	SubjectNetworkIdType
	GetNetworkId() const;

	/**
	 * @brief Check if the subject is of a certain network mode.
	 * 
	 * The subject's world is used to determine the mode (if any),
	 * otherwise @c false is returned, unless the mode is ENetMode::Standalone.
	 * 
	 * @param Mode The network mode to check against.
	 * @return The state of examination.
	 */
	bool
	IsInNetworkMode(const ENetMode Mode) const;

	/**
	 * Check if this is a server-side version of the subject.
	 * 
	 * A subject must be both online and be part
	 * of a server world's mechanism in order to be considered
	 * server-side.
	 * 
	 * @see IsOnline() IsClientSide()
	 */
	bool
	IsServerSide() const;

	/**
	 * Check if this is a client-side version of the subject.
	 * 
	 * A subject must be both online and be part
	 * of a client world's mechanism in order to be considered
	 * client-side.
	 * 
	 * @see IsOnline() IsServerSide()
	 */
	bool
	IsClientSide() const;

	/**
	 * Get the server-side connection that is allowed
	 * to push traits to the server's version of the subject.
	 * 
	 * Can only be queried from a server-side subject.
	 * 
	 * @see SetConnectionPermit()
	 */
	UNetConnection*
	GetConnectionPermit() const;

	/**
	 * Set the server-side connection that is allowed
	 * to push traits to the server's version of the subject.
	 * 
	 * Can only be set for a server-side subject.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * 
	 * @see GetConnectionPermit()
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	SetConnectionPermit(UNetConnection* const Connection);

	/**
	 * Get the server-side list of
	 * traits allowed to be received from clients.
	 * 
	 * Can only be queried from a server-side subject.
	 * 
	 * @see SetTraitmarkPermit()
	 */
	const FTraitmark&
	GetTraitmarkPermit() const;

	/**
	 * Set the list of traits allowed to be
	 * received from clients on the server.
	 * 
	 * Can only be set for a server-side subject.
	 * 
	 * @note The traitmark permit is also used to auto-detect the
	 * peer role during the trait pushing on a listen-server, i.e.
	 * if a trait is included within the permit the role is considered
	 * to be a client. This, of course, can be overriden by an explicit
	 * peer role specification during the push.
	 * 
	 * @param InTraitmarkPermit The traitmark permit
	 * to set to.
	 * @return The outcome of the operation.
	 * 
	 * @see GetTraitmarkPermit()
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	SetTraitmarkPermit(const FTraitmark& InTraitmarkPermit);

	/**
	 * Send an existing trait to the remote version of the subject.
	 * 
	 * The trait can be sent both from the client's version to
	 * the server's and from the server's version to the client(s)'.
	 * 
	 * When pushing from the server, the trait gets broadcasted to all
	 * of the available clients (if any).
	 * 
	 * When pushing from the client, the server will apply the
	 * trait only if it's in the traitmark permit of the server's version.
	 * 
	 * @warning The subject is not brought online automatically in this method,
	 * since it's working on an immutable version of the subject.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to send.
	 * May be a @c nullptr and will return EApparatusStatus::Noop in this case.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should the reliable channel be used for the transaction.
	 * @return The outcome of the operation.
	 * 
	 * @see SetTraitmarkPermit()
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	PushTrait(UScriptStruct* const TraitType,
			  EPeerRole            PeerRole = EPeerRole::Auto,
			  const bool           bReliable = true) const;

	/**
	 * Send an additional trait to the remote version of the subject.
	 * Dynamically typed version.
	 * 
	 * The trait can be sent both from the client's version to
	 * the server's and from the server's version to the client(s)'.
	 * 
	 * @note This method is mutable, cause a listen server can send a trait
	 * to itself by adding it to the subject.
	 * 
	 * When pushing from the server, the trait gets broadcasted to all
	 * of the available clients (if any). The subject is brought online
	 * automatically, if needed.
	 * 
	 * When pushing from the client, the server will apply the
	 * trait only if it's in the traitmark permit of the server's version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to send.
	 * May be a @c nullptr and EApparatusStatus::Noop will be returned in such case.
	 * @param TraitData The data of the trait to send.
	 * Must not be a @c nullptr if @p TraitType is specified.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should the reliable channel be used for the transaction.
	 * @return The outcome of the operation.
	 * 
	 * @deprecated since 1.23 The @p bSetForLocal argument is deprecated now. Set the trait explicitly or use the version of the method without the argument, please.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	PushTrait(UScriptStruct* const TraitType,
			  const void* const    TraitData,
			  EPeerRole            PeerRole = EPeerRole::Auto,
			  const bool           bReliable = true);

	/**
	 * Send an additional trait to the remote version of the subject.
	 * Dynamically typed version.
	 * 
	 * The trait can be sent both from the client's version to
	 * the server's and from the server's version to the client(s)'.
	 * 
	 * @note This method is mutable, cause a listen server can send a trait
	 * to itself by adding it to the subject.
	 * 
	 * When pushing from the server, the trait gets broadcasted to all
	 * of the available clients (if any). The subject is brought online
	 * automatically, if needed.
	 * 
	 * When pushing from the client, the server will apply the
	 * trait only if it's in the traitmark permit of the server's version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to send.
	 * May be a @c nullptr and EApparatusStatus::Noop will be returned in such case.
	 * @param TraitData The data of the trait to send.
	 * Must not be a @c nullptr if @p TraitType is specified.
	 * @param bSetForLocal Should the trait also be set on the local subject.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should the reliable channel be used for the transaction.
	 * @return The outcome of the operation.
	 * 
	 * @deprecated since 1.23 The @p bSetForLocal argument is deprecated now. Set the trait explicitly or use the version of the method without the argument, please.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	[[deprecated("The 'bSetForLocal' argument is deprecated now. Set the trait explicitly or use the version of the method without the argument, please.")]]
	TOutcome<Paradigm>
	PushTrait(UScriptStruct* const TraitType,
			  const void* const    TraitData,
			  const bool           bSetForLocal,
			  EPeerRole            PeerRole = EPeerRole::Auto,
			  const bool           bReliable = true)
	{
		if (AvoidCondition(Paradigm, bSetForLocal))
		{
			return EApparatusStatus::InvalidArgument;
		}
		return PushTrait<Paradigm>(TraitType, TraitData, PeerRole, bReliable);
	}

	/**
	 * Send an additional trait to the remote version of the subject.
	 * Statically typed version.
	 * 
	 * The trait can be sent both from the client's version to
	 * the server's and from the server's version to the client(s)'.
	 * 
	 * @note This method is mutable, cause a listen server can send a trait
	 * to itself by adding it to the subject.
	 * 
	 * When pushing from the server, the trait gets broadcasted to all
	 * of the available clients (if any). The subject is brought online
	 * automatically, if needed.
	 * 
	 * When pushing from the client, the server will apply the
	 * trait only if it's in the traitmark permit of the server's version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to send.
	 * @param Trait The trait to send.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should the reliable channel be used for the transaction.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal, typename T = void >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	PushTrait(const T&        Trait,
			  const EPeerRole PeerRole = EPeerRole::Auto,
			  const bool      bReliable = true)
	{
		return PushTrait<Paradigm>(T::StaticStruct(),
								   static_cast<const void*>(&Trait),
								   PeerRole, bReliable);
	}

	/**
	 * Send an additional trait to the remote version of the subject.
	 * Statically typed version.
	 * 
	 * The trait can be sent both from the client's version to
	 * the server's and from the server's version to the client(s)'.
	 * 
	 * When pushing from the server, the trait gets broadcasted to all
	 * of the available clients (if any). The subject is brought online
	 * automatically, if needed.
	 * 
	 * When pushing from the client, the server will apply the
	 * trait only if it's in the traitmark permit of the server's version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to send.
	 * @param Trait The trait to send.
	 * @param bSetForLocal Should the trait also be set on the local subject.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should the reliable channel be used for the transaction.
	 * @return The outcome of the operation.
	 * 
	 * @deprecated since 1.23 The @p bSetForLocal argument is deprecated now.
	 * Set the trait explicitly or use the version of the method without the argument, please.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal, typename T = void >
	[[deprecated("The 'bSetForLocal' argument is deprecated now. Set the trait explicitly or use the version of the method without the argument, please.")]]
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	PushTrait(const T&        Trait,
			  const bool      bSetForLocal = false,
			  const EPeerRole PeerRole = EPeerRole::Auto,
			  const bool      bReliable = true)
	{
		if (AvoidCondition(Paradigm, bSetForLocal))
		{
			return EApparatusStatus::InvalidArgument;
		}
		return PushTrait<Paradigm>(T::StaticStruct(),
								   static_cast<const void*>(&Trait),
								   PeerRole, bReliable);
	}

	/**
	 * Send an existing trait to the remote version of the subject.
	 * Statically typed version.
	 * 
	 * If the trait is not within the subject, it is added automatically.
	 * The trait can be sent both from the client's version to
	 * the server's and from the server's version to the client(s)'.
	 * 
	 * When pushing from the server, the trait gets broadcasted to all
	 * of the available clients (if any).
	 * 
	 * When pushing from the client, the server will apply the
	 * trait only if it's in the traitmark permit of the server's version.
	 * 
	 * @warning The subject is not brought online automatically in this method,
	 * since it's working on an immutable version of the subject.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to push.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should the reliable channel be used for the transaction.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	PushTrait(const EPeerRole PeerRole = EPeerRole::Auto,
			  const bool      bReliable = true) const
	{
		return PushTrait<Paradigm>(T::StaticStruct(), PeerRole, bReliable);
	}

	/**
	 * Send an existing trait to the remote version of the subject.
	 * Statically typed default paradigm version.
	 * 
	 * If the trait is not within the subject, it is added automatically.
	 * The trait can be sent both from the client's version to
	 * the server's and from the server's version to the client(s)'.
	 * 
	 * When pushing from the server, the trait gets broadcasted to all
	 * of the available clients (if any).
	 * 
	 * When pushing from the client, the server will apply the
	 * trait only if it's in the traitmark permit of the server's version.
	 * 
	 * @warning The subject is not brought online automatically in this method,
	 * since it's working on an immutable version of the subject.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to push.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should the reliable channel be used for the transaction.
	 * @return The outcome of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE TOutcomeIf<Paradigm, IsTraitType<T>()>
	PushTrait(const EPeerRole PeerRole = EPeerRole::Auto,
			  const bool      bReliable = true) const
	{
		return PushTrait<Paradigm, T>(PeerRole, bReliable);
	}

	/// @}
#pragma endregion Networking

	/**
	 * Copy all of the traits from the subject
	 * to some other subject.
	 * 
	 * The existing traits of the destination subject are not removed
	 * but the new ones are added and/or override the existing matching.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param Destination The destination subject to copy to.
	 * May not be a @c nullptr.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	CopyTraitsTo(FSubjectInfo* const Destination) const;

#pragma region Despawning
	/// @name Despawning
	/// @{

	/**
	 * Destroy the subject.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	Despawn();

	/**
	 * Destroy the subject.
	 * Deferred version.
	 *
	 * The actual entity destruction is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param bHard Should the hard version of the operation be performed.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	DespawnDeferred(const bool bHard = true);

	/// @}
#pragma endregion Despawning

#pragma region Copying

	/**
	 * Copying of the subject information structures
	 * is not supported.
	 */
	FSubjectInfo&
	operator=(const FSubjectInfo&) = delete;

#pragma endregion Copying

#pragma region Initialization

	/**
	 * Default constructor.
	 */
	FSubjectInfo()
	{}

	/**
	 * Copying of the subject information structures
	 * is not supported.
	 */
	FSubjectInfo(const FSubjectInfo&) = delete;

#pragma endregion Initialization

}; //-struct FSubjectInfo


/**
 * The internal state of a networked subject.
 */
struct APPARATUSRUNTIME_API FSubjectNetworkState
{
  public:

	/**
	 * The type of a networked identifier.
	 */
	using IdType = SubjectNetworkIdType;

	/**
	 * Invalid network identifier.
	 */
	static constexpr IdType InvalidId = 0x0;

	/**
	 * A first valid network identifier.
	 */
	static constexpr IdType FirstId = 0x1;

	/**
	 * The maximum network identifier possible to be
	 * allocated.
	 */
	static constexpr IdType LastId = TNumericLimits<IdType>::Max() - 1;

  private:

	friend struct FSubjectInfo;
	friend struct FSubjectRecord;
	friend class ISubjective;
	friend class ASubjectiveActor;
	friend class USubjectiveActorComponent;
	friend class UNetworkBearerComponent;
	friend class AMechanism;
	friend class UMachine;

	/**
	 * The list of traits allowed to be received
	 * by the subject on the server.
	 */
	FTraitmark TraitmarkPermit;

	/**
	 * A connection to be satisfied in order to push
	 * to this subject.
	 */
	TWeakObjectPtr<UNetConnection> ConnectionPass;

	/**
	 * The corresponding subject's unique identifier.
	 */
	FSubjectInfo::IdType SubjectId = FSubjectInfo::InvalidId;

	/**
	 * Unique network identifier.
	 */
	IdType Id = InvalidId;

	/**
	 * Assign a network identifier on a client.
	 */
	FORCEINLINE void
	ClientAssignId(const IdType InId)
	{
		check(Id == InvalidId || Id == InId);
		if (UNLIKELY(Id == InId)) return;
		Id = InId;
	}

	/**
	 * Assign a network identifier for the subject on the server.
	 */
	IdType
	ServerObtainId();

	/**
	 * Assign a network identifier for the subject on the server
	 * within a custom range.
	 * 
	 * @param IdRange The custom identifier range to utilize.
	 */
	IdType
	ServerObtainId(const TRange<SubjectNetworkIdType>& IdRange);

	/**
	 * Construct a new instance of subject network state.
	 */
	FORCEINLINE
	FSubjectNetworkState(const FSubjectInfo::IdType OwnerSubjectId)
	  : SubjectId(OwnerSubjectId)
	{
		check(SubjectId != FSubjectInfo::InvalidId);
	}

  public:

	/**
	 * The traits allowed to be pushed to this
	 * subject's server version.
	 */
	FORCEINLINE const FTraitmark&
	GetTraitmarkPermit() const
	{
		return TraitmarkPermit;
	}

	/**
	 * Set the server-side list
	 * of traits allowed to be pushed from clients.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] InTraitmarkPermit The traitmark permit to set.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	SetTraitmarkPermit(const FTraitmark& InTraitmarkPermit)
	{
		return TraitmarkPermit.template Set<Paradigm>(InTraitmarkPermit);
	}

	/**
	 * Get the connection allowed to push
	 * to this subject on the server.
	 */
	FORCEINLINE UNetConnection*
	GetConnectionPermit() const
	{
		return ConnectionPass.Get();
	}

	/**
	 * Set the client connection allowed to push
	 * to this subject on the server.
	 * 
	 * @param InConnection The connection to set to.
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	SetConnectionPermit(UNetConnection* const InConnection)
	{
		if (UNLIKELY(ConnectionPass == InConnection))
		{
			return EApparatusStatus::Noop;
		}
		ConnectionPass = InConnection;
		return EApparatusStatus::Success;
	}

	/**
	 * Get the universal network identifier.
	 */
	FORCEINLINE uint32
	GetId() const
	{
		return Id;
	}

	/**
	 * Get the subject identifier.
	 */
	FORCEINLINE FSubjectInfo::IdType
	GetSubjectId() const
	{
		return SubjectId;
	}

	/**
	 * Check if this network information is actually valid.
	 */
	FORCEINLINE bool
	IsValid() const
	{
		check(SubjectId != FSubjectInfo::InvalidId);
		return Id != InvalidId;
	}

}; //-struct FSubjectNetworkState

template < EParadigm Paradigm /*=EParadigm::DefaultInternal*/ >
FORCEINLINE TOutcome<Paradigm, FSubjectNetworkState*>
FSubjectInfo::ObtainNetworkState()
{
	if (LIKELY(NetworkState != nullptr))
	{
		return MakeOutcome<Paradigm, FSubjectNetworkState*>(EApparatusStatus::Noop, NetworkState);
	}
	return NetworkState = new FSubjectNetworkState(Id);
}

FORCEINLINE SubjectNetworkIdType
FSubjectInfo::GetNetworkId() const
{
	if (LIKELY(NetworkState))
	{
		return NetworkState->Id;
	}
	return FSubjectNetworkState::InvalidId;
}

#if CPP && !defined(SKIP_MACHINE_H)
#include "Machine.h"
#endif
