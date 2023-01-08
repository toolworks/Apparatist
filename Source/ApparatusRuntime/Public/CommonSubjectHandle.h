/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: CommonSubjectHandle.h
 * Created: 2021-07-25 21:01:26
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

#include "More/type_traits"

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "Kismet/KismetSystemLibrary.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define COMMON_SUBJECT_HANDLE_H_SKIPPED_MACHINE_H
#endif

#include "SubjectInfo.h"

#ifdef COMMON_SUBJECT_HANDLE_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "CommonSubjectHandle.generated.h"


// Forward declarations:
class ISubjective;
class UChunk;
class UNetworkBearerComponent;
class AMechanism;
struct FSubjectHandle;
struct FSolidSubjectHandle;
struct FConstSubjectHandle;
struct FUnsafeSubjectHandle;
template < class D >
constexpr bool IsDetailClass();

/**
 * The base structure for all subject handles.
 * 
 * Shouldn't really be used by the end-user of the
 * framework.
 * 
 * The size of the handle should be exactly 64-bits and
 * can be passed to functions by value.
 * 
 * Subject handles are not meant to be serialized.
 * 
 * @see FSubjectHandle, FSolidSubjectHandle, FConstSubjectHandle
 */
USTRUCT(BlueprintType)
struct APPARATUSRUNTIME_API FCommonSubjectHandle
{
	GENERATED_BODY()

  public:

	/**
	 * The type of the unique mechanism identifier.
	 */
	typedef FSubjectInfo::MechanismIdType MechanismIdType;

	/**
	 * The type of the unique subject identifier.
	 */
	typedef FSubjectInfo::IdType IdType;

	/**
	 * The type of the subject generation counter.
	 */
	typedef FSubjectInfo::GenerationType GenerationType;

	/**
	 * The type of the network identifier.
	 */
	using NetworkIdType = FSubjectNetworkState::IdType;

	/**
	 * Invalid subject identifier.
	 */
	static constexpr IdType InvalidId = FSubjectInfo::InvalidId;

	/**
	 * A first valid subject identifier.
	 */
	static constexpr IdType FirstPlace = FSubjectInfo::FirstPlace;

	/**
	 * The maximum valid subject identifier.
	 */
	static constexpr IdType LastPlace = FSubjectInfo::LastPlace;

	/**
	 * The first valid generation.
	 */
	static constexpr GenerationType FirstGeneration = FSubjectInfo::FirstGeneration;

	/**
	 * The maximum valid generation.
	 */
	static constexpr GenerationType LastGeneration = FSubjectInfo::LastGeneration;

	/**
	 * An invalid common subject handle constant.
	 */
	static const FCommonSubjectHandle Invalid;

  protected:

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	template < typename SubjectHandleT >
	friend struct TChunkIt;
	template < typename SubjectHandleT >
	friend struct TBeltIt;

	friend struct FSubjectInfo;
	friend struct FSubjectNetworkState;
	friend struct FSubjectHandle;
	friend struct FSolidSubjectHandle;
	friend struct FConstSubjectHandle;
	friend struct FUnsafeSubjectHandle;
	friend struct FChunkSlot;
	friend struct FSubjectRecord;
	friend class UChunk;
	friend class AMechanism;
	friend class UMachine;
	friend class ISubjective;
	friend class UNetworkBearerComponent;
	friend class ASubjectiveActor;
	friend class USubjectiveActorComponent;

	/**
	 * A unique identifier of the subject within the mechanism.
	 * 
	 * If this identifier is invalid, the handle is considered
	 * to be invalid.
	 */
	UPROPERTY(VisibleAnywhere, Category="Address")
	int32 Id = InvalidId;

	/**
	 * A unique generation of the subject.
	 *
	 * Used to validate the subject against reused slots.
	 */
	UPROPERTY(VisibleAnywhere, Category="Address")
	uint32 Generation = FirstGeneration;

	/**
	 * Get the valid subject info record (if any).
	 * 
	 * The subject handle gets invalidated if the information is not found.
	 * 
	 * @return A valid subject registry information or @c nullptr,
	 * if the subject handle points to an invalid subject.
	 */
	FSubjectInfo*
	FindInfo() const;

	/**
	 * Get the subject info record associated with it.
	 */
	FSubjectInfo&
	GetInfo() const;

	/**
	 * Get the current fingerprint's reference of the subject.
	 */
	FFingerprint&
	GetFingerprintRef() const;

	/**
	 * @internal
	 * An internal method to mark the subject as booted.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @return The previous state of the booted flag.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	FORCEINLINE TOutcome<Paradigm, bool>
	MarkBooted() const;

	/**
	 * Get the current chunk of the subject.
	 *
	 * @return A chunk, this subject is currently part of.
	 * Returns @c nullptr, if there is none.
	 */
	UChunk*
	GetChunk() const;

	/**
	 * Get a subjective this handle is associated with (if any).
	 * 
	 * Subjectives are high-level UObject-compatible entities.
	 * The method is non-public, cause a cast to a specific version
	 * is actually in the child struct.
	 * 
	 * The method is private, cause the exact subjective pointer
	 * type has to be cast to.
	 */
	ISubjective*
	GetSubjective() const;

	/**
	 * Get a detail of a certain class.
	 * 
	 * The method is protected since the child classes
	 * have to provide the exact return value semantics.
	 * 
	 * @param DetailClass The class of the detail to get.
	 * @return A pointer to the detail of the designated class.
	 * @return nullptr If there is no such detail.
	 */
	UDetail* GetDetail(TSubclassOf<UDetail> DetailClass) const;

	/**
	 * Get a detail of a certain class.
	 * Templated version.
	 * 
	 * The method is protected since the child classes
	 * have to provide the exact return value semantics.
	 * 
	 * @tparam D The class of the detail to get.
	 * @return A pointer to the detail of the designated class.
	 * @return nullptr If there is no such detail.
	 */
	template < class D >
	D* GetDetail() const;

	/**
	 * Initialize a common handle with explicit id and generation specifications.
	 * 
	 * @param InId The unique subject identifier.
	 * @param InGeneration The generation of the subject.
	 */
	FORCEINLINE
	FCommonSubjectHandle(const IdType         InId,
						 const GenerationType InGeneration)
	  : Id(InId)
	  , Generation(InGeneration)
	{
		static_assert(std::is_same<decltype(Id), IdType>::value, "The type of the identifier must match.");
		static_assert(std::is_same<decltype(Generation), GenerationType>::value, "The type of the generation must match.");
	}

	/**
	 * Initialize a common handle with explicit mechanism, placement and generation specifications.
	 * 
	 * @param InMechanismId The unique mechanism identifier.
	 * @param InPlace The unique place of the subject within its mechanism.
	 * @param InGeneration The generation of the subject.
	 */
	FORCEINLINE
	FCommonSubjectHandle(const MechanismIdType InMechanismId,
						 const IdType          InPlace,
						 const GenerationType  InGeneration)
	  : FCommonSubjectHandle(FSubjectInfo::MakeId(InMechanismId, InPlace), InGeneration)
	{}

  public:

#pragma region A Pointer-Like Interface
	/// @name A Pointer-Like Interface
	/// @{

	/**
	 * Use the handle as a pointer.
	 * 
	 * This interface is needed for compatiblity 
	 */
	FORCEINLINE auto
	operator->() const
	{
		return this;
	}

	/**
	 * Dereference itself.
	 * 
	 * This interface is needed for compatiblity 
	 */
	FORCEINLINE auto&
	operator*() const
	{
		return *this;
	}

	/// @}
#pragma endregion A Pointer-Like Interface

#pragma region Identification
	/// @name Identification
	/// @{

	/**
	 * Get the unique identifier of the subject.
	 * 
	 * The identifier is unique along the lifetime
	 * of the subject. It gets reused (under a different
	 * generation) after the subject is despawned.
	 * 
	 * You can use the identifier for the purpose of
	 * ordering.
	 * 
	 * @see GetGeneration()
	 */
	FORCEINLINE IdType
	GetId() const
	{
		return Id;
	}

	/**
	 * Get the current generation of the subject.
	 *
	 * Each unique instance of a subject has a unique
	 * combination of its generation and identifier.
	 * 
	 * @see GetId()
	 */
	FORCEINLINE GenerationType
	GetGeneration() const
	{
		return Generation;
	}

	/**
	 * Get the identifier of the mechanism the subject is part of.
	 * 
	 * @note The procedure is executed without touching
	 * any global state and is derived from the identifier.
	 * 
	 * @see GetId(), GetPlace()
	 */
	FORCEINLINE auto
	GetMechanismId() const
	{
		return FSubjectInfo::ExtractMechanismId(Id);
	}

	/**
	 * Get the place index of the subject.
	 * 
	 * This is the index of the subject information
	 * structure within the mechanism's subject pool.
	 * 
	 * @note The procedure is executed without touching
	 * any global state and is derived from the identifier.
	 * 
	 * @see GetId(), GetMechanismId()
	 */
	FORCEINLINE auto
	GetPlace() const
	{
		return FSubjectInfo::ExtractPlace(Id);
	}

	/// @}
#pragma endregion Identification

#pragma region Common Functionality
	/// @name Common Functionality
	/// @{

	/**
	 * Check if the subject is currently solid.
	 * 
	 * The subject is solid if it's part of a solid
	 * mechanism.
	 * 
	 * Invalid subject handles will return @c false here.
	 */
	FORCEINLINE bool
	IsSolid() const
	{
		const auto Info = FindInfo();
		if (LIKELY(Info))
		{
			return Info->IsSolid();
		}
		return false;
	}

	/**
	 * Reset the subject handle.
	 * 
	 * Only the handle itself is reset.
	 * The referenced subject (if any) will remain intact.
	 */
	FORCEINLINE void
	ResetHandle()
	{
		// Only the id is checked for validity,
		// so reset it here only:
		Id = InvalidId;
	}

	/**
	 * Clear the subject handle, essentially making it invalid.
	 */
	FORCEINLINE FCommonSubjectHandle&
	operator=(TYPE_OF_NULLPTR)
	{
		ResetHandle();
		return *this;
	}

	/**
	 * Calculate the hash-sum value for the handle.
	 */
	uint32
	CalcHash() const;

	/**
	 * Get the mechanism the subject is part of.
	 * 
	 * @return The mechanism the subject resides within.
	 * @return nullptr If the mechanism is invalid.
	 */
	AMechanism*
	GetMechanism() const;

	/**
	 * Get the current fingerprint of the subject.
	 */
	const FFingerprint&
	GetFingerprint() const;

	/**
	 * Check if the subject matches a certain filter.
	 */
	FORCEINLINE bool
	Matches(const FFilter& InFilter) const
	{
		return GetFingerprint().Matches(InFilter);
	}

	/// @}
#pragma endregion Common Functionality

#pragma region Validity
	/// @name Validity
	/// @{

	/**
	 * Check if the subject handle is valid
	 * and is pointing to an existent subject.
	 *
	 * @return Returns @c true if the subject is valid.
	 * Otherwise, @c false is returned.
	 */
	FORCEINLINE bool
	IsValid() const
	{
		return FindInfo() != nullptr;
	}

	/**
	 * Check if the subject handle is invalid
	 * and is pointing to a non-existent subject.
	 *
	 * @return Returns @c true if the subject is invalid.
	 * Otherwise, @c false is returned.
	 */
	FORCEINLINE bool
	operator==(TYPE_OF_NULLPTR) const
	{
		return !IsValid();
	}

	/**
	 * Check if the subject handle is valid
	 * and is pointing to an existent subject.
	 *
	 * @return Returns @c true if the subject is valid.
	 * Otherwise, @c false is returned.
	 */
	FORCEINLINE bool
	operator!=(TYPE_OF_NULLPTR) const
	{
		return IsValid();
	}

	/**
	 * Check if the subject handle is valid
	 * and is pointing to an existent subject.
	 *
	 * @return Returns @c true if the subject is valid.
	 * Otherwise, @c false is returned.
	 * 
	 * @see IsValid()
	 */
	FORCEINLINE operator bool() const
	{
		return IsValid();
	}

	/// @}
#pragma endregion Validity

#pragma region Comparison
	/// @name Comparison
	/// @{
	
	/**
	 * Check if two subject handles are equal byte-wise.
	 */
	FORCEINLINE bool
	EqualsRaw(const FCommonSubjectHandle& InSubject) const
	{
		return (Id == InSubject.Id) && (Generation == InSubject.Generation);
	}

	/**
	 * Check if the subject handle points to the same subject
	 * as the other one.
	 * 
	 * If both handles are invalid, returns @c true
	 * 
	 * @param InSubjectHandle A subject handle to compare with.
	 * @return true if the handles point to the same subject
	 * or both are invalid.
	 * @return false if the handles point to different
	 * subjects or one is invalid.
	 */
	FORCEINLINE bool
	Equals(const FCommonSubjectHandle& InSubjectHandle) const
	{
		if (EqualsRaw(InSubjectHandle))
		{
			return true;
		}
		return (!IsValid()) && (!InSubjectHandle.IsValid());
	}

	/**
	 * Check if the subject handle points to the same subject
	 * as the other one.
	 * 
	 * If both handles are invalid, returns @c true
	 * 
	 * @param InSubjectHandle A subject handle to compare with.
	 * @return true if the handles point to the same subject
	 * or both are invalid.
	 * @return false if the handles point to different
	 * subjects or one is invalid.
	 */
	FORCEINLINE bool
	operator==(const FCommonSubjectHandle& InSubjectHandle) const
	{
		return Equals(InSubjectHandle);
	}

	/**
	 * Check if the subject handles point to different subjects.
	 * 
	 * Two invalid subject handles are considered to be the same
	 * and this operator would return @c false.
	 * 
	 * @param InSubjectHandle A subject handle to compare with.
	 * @return true if the handles point to different subjects
	 * or one is invalid.
	 * @return false if the handles point to the same subject
	 * or both are invalid.
	 */
	FORCEINLINE bool
	operator!=(const FCommonSubjectHandle& InSubjectHandle) const
	{
		if (EqualsRaw(InSubjectHandle))
		{
			return false;
		}
		return IsValid() || InSubjectHandle.IsValid();
	}

	/// @}
#pragma endregion Comparison

#pragma region Flagmark Examination
	/// @name Flagmark Examination
	/// @{

	/**
	 * Get the flagmark of the subject.
	 */
	FORCEINLINE EFlagmark
	GetFlagmark() const
	{
		return GetInfo().GetFlagmark();
	}

	/**
	 * Get the state of a certain flag of the subject.
	 * 
	 * Always returns @c false for invalid subject handles.
	 * 
	 * @param Flag The flag to examine.
	 * @return The state of the flag.
	 */
	FORCEINLINE bool
	HasFlag(const EFlagmarkBit Flag) const
	{
		const auto Info = FindInfo();
		if (LIKELY(Info))
		{
			return Info->HasFlag(Flag);
		}
		return false;
	}

	/// @}
#pragma endregion Flagmark Examination

#pragma region Components Examination
	/// @name Components Examination
	/// @{

	/**
	 * Check if the subject has a certain trait.
	 */
	FORCEINLINE bool
	HasTrait(UScriptStruct* const TraitType) const
	{
		const auto Info = FindInfo();
		if (LIKELY(Info))
		{
			return Info->GetFingerprint().Contains(TraitType);
		}
		return false;
	}

	/**
	 * Check if the subject has a certain trait. Templated version.
	 */
	template < typename T >
	FORCEINLINE typename std::enable_if<IsTraitType<T>(), bool>::type
	HasTrait() const
	{
		return HasTrait(T::StaticStruct());
	}

	/**
	 * Check if the subject has a certain detail.
	 */
	FORCEINLINE bool
	HasDetail(TSubclassOf<UDetail> DetailClass) const
	{
		const auto Info = FindInfo();
		if (LIKELY(Info))
		{
			return Info->GetFingerprint().Contains(DetailClass);
		}
		return false;
	}

	/**
	 * Check if the subject has a certain detail.
	 * Templated version.
	 */
	template < typename D >
	FORCEINLINE typename std::enable_if<IsDetailClass<D>(), bool>::type
	HasDetail() const
	{
		return HasDetail(D::StaticClass());
	}

	/// @}
#pragma endregion Components Examination

#pragma region Traits Getting
	/// @name Traits Getting
	/// @{

	/**
	 * Get a trait from the subject by its type.
	 * Dynamic outputting version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait data to receive the trait.
	 * Must match the one specified within the @p TraitType or be @c void.
	 * @param[in] TraitType The type of the trait to get.
	 * Must match that of the @p T.
	 * @param[out] OutTraitData The trait data receiver.
	 * Must not be a @c nullptr if the @c TraitType is set.
	 * @param[in] bTraitDataInitialized Is the @p OutTraitData buffer actually initialized?
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Missing If there is no such trait in the subject.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	GetTrait(UScriptStruct* const TraitType,
			 void* const          OutTraitData,
			 const bool           bTraitDataInitialized = true) const
	{
		const auto Info = FindInfo();
		if (Avoid(Paradigm, Info == nullptr))
		{
			return EApparatusStatus::InvalidState;
		}
		return Info->template GetTrait<Paradigm>(TraitType, OutTraitData, bTraitDataInitialized);
	}

	/**
	 * Get a trait from a subject by its type.
	 * Compile-time outputting version.
	 *
	 * @tparam T The type of the trait to get.
	 * @param[out] OutTrait The trait receiver.
	 * @param[out] bTraitDataInitialized Is the @p OutTrait actually initialized?
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Missing If there is no such trait in the subject.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void >
	FORCEINLINE TOutcome<Paradigm>
	GetTrait(T& OutTrait, const bool bTraitDataInitialized = true) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template GetTrait<Paradigm, T>(OutTrait, bTraitDataInitialized);
	}

	/**
	 * Get a trait copy from the subject by its type.
	 * Compile-time paradigm version.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A copy of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm, T>
	GetTrait() const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, (MakeOutcome<Paradigm, T>(EApparatusStatus::InvalidState, T())));
		return Info->template GetTrait<Paradigm, T>();
	}

	/**
	 * Get a trait copy from the subject by its type.
	 * Compile-time version.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @return A copy of the trait.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, T>
	GetTrait() const
	{
		return GetTrait<Paradigm, T>();
	}

	/// @}
#pragma endregion Traits Getting

#pragma region Common Networking
	/// @name Common Networking
	/// @{

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
	NetworkIdType
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
	FORCEINLINE bool
	IsInNetworkMode(const ENetMode Mode) const
	{
		const auto Info = FindInfo();
		if (UNLIKELY(Info == nullptr))
		{
			return false;
		}
		return Info->IsInNetworkMode(Mode);
	}

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
	 * to push to the server's version of the subject.
	 * 
	 * Can only be queried from a server-side subject.
	 * 
	 * @see SetConnectionPermit()
	 */
	UNetConnection*
	GetConnectionPermit() const;

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

	[[deprecated("The method was renamed. Use 'GetTraitmarkPermit', please.")]]
	FORCEINLINE const FTraitmark&
	GetTraitmarkPass() const
	{
		return GetTraitmarkPermit();
	}

	/// @}
#pragma endregion Common Networking

#pragma region Assignment
	/// @name Assignment
	/// @{

	/**
	 * Assign to be a copy of an another subject handle.
	 * 
	 * @param InSubjectHandle The subject handle to copy.
	 * @return Returns itself as a result.
	 */
	FORCEINLINE FCommonSubjectHandle&
	operator=(const FCommonSubjectHandle& InSubjectHandle)
	{
		Id         = InSubjectHandle.Id;
		Generation = InSubjectHandle.Generation;
		return *this;
	}

	/// @}
#pragma endregion Assignment

#pragma region Initialization

	/**
	 * Initialize an invalid (null) handle.
	 */
	FORCEINLINE
	FCommonSubjectHandle()
	{}

	/**
	 * Initialize as a copy of an another subject handle.
	 * 
	 * @param InSubjectHandle The subject handle to copy.
	 */
	FORCEINLINE
	FCommonSubjectHandle(const FCommonSubjectHandle& InSubjectHandle)
	  : FCommonSubjectHandle(InSubjectHandle.Id, InSubjectHandle.Generation)
	{}

#pragma endregion Initialization

}; //-struct FCommonSubjectHandle

FORCEINLINE uint32
GetTypeHash(const FCommonSubjectHandle& SubjectHandle)
{
	return SubjectHandle.CalcHash();
}

template<>
struct TStructOpsTypeTraits<FCommonSubjectHandle>
  : public TStructOpsTypeTraitsBase2<FCommonSubjectHandle>
{
	enum 
	{
		WithCopy = true,
		WithIdenticalViaEquality = true
		// TODO: Implement net serialization.
	}; 
};

/**
 * The main entity in the mechanism, consisting of traits.
 *
 * This is a user-level handle structure, something like a pointer.
 * The size of this structure is exactly 64-bit and may efficiently be passed by value
 * to the methods and functions.
 * 
 * The base version is the most generic and unsafe one, that
 * allows all sorts of access and functionality.
 *
 * @tparam bAllowChanges Is the referenced subject mutable (non-constant)?
 * @tparam bAllowDirectTraitAccess Should direct access provided to the trait data.
 * @tparam bAllowStructuralChanges Should structural changes (adding/removing traits) be allowed.
 */
template < bool bAllowChanges,
		   bool bAllowDirectTraitAccess,
		   bool bAllowStructuralChanges >
struct TSubjectHandle
  : public FCommonSubjectHandle
{
  public:

	static_assert(!bAllowStructuralChanges || bAllowChanges, "Only mutable subjects are allowed to have structural changes.");

	/**
	 * Is the target subject mutable (non-constant)?
	 */
	static constexpr bool AllowsChanges = bAllowChanges;

	/**
	 * Is the direct (trait) data access possible?
	 */
	static constexpr bool AllowsDirectTraitAccess = bAllowDirectTraitAccess;

	/**
	 * Is changing of the subject's structure (adding/removing traits) allowed.
	 */
	static constexpr bool AllowsStructuralChanges = bAllowStructuralChanges;

	/**
	 * The base handle type.
	 */
	using Super = FCommonSubjectHandle;

	/**
	 * Check if the handle itself a solid one.
	 */
	static constexpr bool
	IsHandleSolid()
	{
		return bAllowDirectTraitAccess && !bAllowStructuralChanges;
	}

	/**
	 * The type of a trait void pointer returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 */
	template < EParadigm Paradigm >
	using TTraitVoidPtrResultSecurity = typename std::enable_if<bAllowDirectTraitAccess || IsUnsafe(Paradigm), bool>::type;

	/**
	 * The type of a trait void pointer returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 */
	template < EParadigm Paradigm >
	using TTraitVoidPtrResult = typename std::conditional<AllowsChanges, void*, const void*>::type;

	/**
	 * Assess the availability of a trait pointer result.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitPtrResultSecurity = typename std::enable_if<(bAllowDirectTraitAccess || IsUnsafe(Paradigm)) && IsTraitType<std::remove_cv_t<T>>(), bool>::type;

	/**
	 * The type of a trait pointer returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitPtrResult = typename std::conditional<AllowsChanges,
													  std::add_pointer_t<T>,
													  std::add_pointer_t<std::add_const_t<T>>>::type;

	/**
	 * Check the type of a trait reference returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitRefResultSecurity = typename std::enable_if<(bAllowDirectTraitAccess || IsUnsafe(Paradigm)) && IsTraitType<std::remove_cv_t<T>>(), bool>::type;

	/**
	 * The type of a trait reference returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitRefResult = typename std::conditional<bAllowChanges,
													  std::add_lvalue_reference_t<T>,
													  std::add_lvalue_reference_t<std::add_const_t<T>>>::type;

	/**
	 * Check the class of a detail.
	 * 
	 * @tparam D The class of the detail. Must be a @c UDetail subclass.
	 */
	template < class D >
	using TDetailPtrResultSecurity = typename std::enable_if<IsDetailClass<std::remove_cv_t<D>>(), bool>::type;

	/**
	 * The type of a detail pointer returned by the methods.
	 * 
	 * @tparam D The class of the detail. Must be a @c UDetail subclass.
	 */
	template < class D >
	using TDetailPtrResult = typename std::conditional<bAllowChanges,
													   std::add_pointer_t<D>,
													   std::add_pointer_t<std::add_const_t<D>>>::type;

  protected:

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;
	template < typename SubjectHandleT >
	friend struct TChunkIt;
	template < typename SubjectHandleT >
	friend struct TBeltIt;

	friend struct FSubjectInfo;
	friend struct FSubjectNetworkState;
	friend struct FSolidSubjectHandle;
	friend struct FConstSubjectHandle;
	friend struct FChunkSlot;
	friend struct FSubjectRecord;
	friend class UChunk;
	friend class AMechanism;
	friend class UMachine;
	friend class ISubjective;
	friend class UNetworkBearerComponent;
	friend class ASubjectiveActor;
	friend class USubjectiveActorComponent;

	/**
	 * @internal Initialize a subject handle by an explicit id and generation specifications.
	 * 
	 * @param InId The unique subject identifier.
	 * @param InGeneration The generation of the subject.
	 */
	FORCEINLINE
	TSubjectHandle(const int32          InId,
				   const GenerationType InGeneration)
	  : FCommonSubjectHandle(InId, InGeneration)
	{}

	/**
	 * @internal Initialize a subject handle with explicit mechanism, placement and generation specifications.
	 * 
	 * @param InMechanismId The unique mechanism identifier.
	 * @param InPlace The unique place of the subject within its mechanism.
	 * @param InGeneration The generation of the subject.
	 */
	FORCEINLINE
	TSubjectHandle(const MechanismIdType InMechanismId,
				   const IdType          InPlace,
				   const GenerationType  InGeneration)
	  : FCommonSubjectHandle(InMechanismId, InPlace, InGeneration)
	{}

	/**
	 * @internal Convert from a base handle type.
	 * 
	 * @param InHandle The base handle version.
	 */
	FORCEINLINE
	TSubjectHandle(const FCommonSubjectHandle& InHandle)
	  : FCommonSubjectHandle(InHandle)
	{}

  public:

	/**
	 * Clear the subject handle, essentially making it invalid.
	 */
	FORCEINLINE TSubjectHandle&
	operator=(TYPE_OF_NULLPTR)
	{
		ResetHandle();
		return *this;
	}

#pragma region A Pointer-Like Interface
	/// @name A Pointer-Like Interface
	/// @{

	/**
	 * Use the handle as a pointer.
	 * 
	 * This interface is needed for compatiblity 
	 */
	FORCEINLINE auto
	operator->() const
	{
		return this;
	}

	/**
	 * Dereference itself.
	 * 
	 * This interface is needed for compatiblity 
	 */
	FORCEINLINE auto&
	operator*() const
	{
		return *this;
	}

	/// @}
#pragma endregion A Pointer-Like Interface

#pragma region Validity
	/// @name Validity
	/// @{

	/**
	 * Check if the subject handle is invalid
	 * and is pointing to a non-existent subject.
	 *
	 * @return Returns @c true if the subject is invalid.
	 * Otherwise, @c false is returned.
	 */
	FORCEINLINE bool
	operator==(TYPE_OF_NULLPTR) const
	{
		return !IsValid();
	}

	/**
	 * Check if the subject handle is valid
	 * and is pointing to an existent subject.
	 *
	 * @return Returns @c true if the subject is valid.
	 * Otherwise, @c false is returned.
	 */
	FORCEINLINE bool
	operator!=(TYPE_OF_NULLPTR) const
	{
		return IsValid();
	}

	/// @}
#pragma endregion Validity

#pragma region Context
	/// @name Context
	/// @{

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
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsStructuralChanges>
	SetMechanism(AMechanism* const Mechanism)
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template SetMechanism<Paradigm>(Mechanism);
	}

	/// @}
#pragma endregion Context

#pragma region Despawning
	/// @name Despawning
	/// @{

	/**
	 * Destroy the subject.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @return The status of the operation.
	 * @return EApparatusStatus::Noop If the subject handle is
	 * already invalid/despawned.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsStructuralChanges>
	Despawn() const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template Despawn<Paradigm>();
	}

	/**
	 * Destroy the subject.
	 * Deferred version.
	 *
	 * The actual entity destruction is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 *
	 * @tparam Paradigm The security paradigm to work under.
	 * @param bHard Should the hard version of the operation be performed, i.e.
	 * the entity gets marked and is skipped during the iteratings from now on.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop If the subject handle is
	 * already invalid/despawned.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcomeIfUnsafeOr<Paradigm, AllowsChanges>
	DespawnDeferred(const bool bHard = true) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template DespawnDeferred<Paradigm>(bHard);
	}

	/// @}
#pragma endregion Despawning

#pragma region Flagmark
	/// @name Flagmark
	/// @{

	/**
	 * Set the flagmark of the subject.
	 * 
	 * This is a user-level method that prevents changing
	 * the system-level flags by default.
	 * 
	 * This operation is atomic and thread-safe.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flagmark The flagmark to set.
	 * The system-level bits are silently ignored.
	 * @return The previous flagmark of the subject.
	 * 
	 * @see SetFlagmarkMasked(), GetFlagmark()
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsChanges, EFlagmark>
	SetFlagmark(const EFlagmark Flagmark) const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			return MakeOutcome<Paradigm, EFlagmark>(EApparatusStatus::InvalidState, FM_None);
		}
		return Info->template SetFlagmark<Paradigm>(Flagmark);
	}

	/**
	 * Set the flagmark of the subject.
	 * Status version.
	 * 
	 * This is a user-level method that prevents setting
	 * the system-level flags.
	 * 
	 * This operation is atomic and thread-safe.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flagmark The flagmark to set.
	 * The system-level bits are silently ignored.
	 * @return The status of the operation.
	 * 
	 * @see SetFlagmark(), SetFlagmarkMasked()
	 * 
	 * @deprecated since 1.23. Use the SetFlagmark() method within a polite paradigm, please.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	[[deprecated("The method is deprecated. Use the 'SetFlagmark' method within a polite paradigm, please.")]]
	FORCEINLINE typename std::enable_if<AllowsChanges || IsUnsafe(Paradigm), EApparatusStatus>::type
	SetFlagmark_Status(const EFlagmark Flagmark) const
	{
		const auto Info = FindInfo();
		if (LIKELY(Info != nullptr))
		{
			return Info->template SetFlagmark<MakePolite(Paradigm)>(Flagmark);
		}
		return EApparatusStatus::InvalidState;
	}

	/**
	 * Set the flagmark of the subject.
	 *
	 * This is a user-level method that prevents setting
	 * the system-level flags.
	 * 
	 * This operation is atomic and thread-safe.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @param Flagmark The flagmark to set.
	 * @param Mask The mask to use.
	 * @return The previous flagmark of the subject
	 * masked with the passed-in mask.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsChanges, EFlagmark>
	SetFlagmarkMasked(const EFlagmark Flagmark,
					  const EFlagmark Mask) const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			return MakeOutcome<Paradigm, EFlagmark>(EApparatusStatus::InvalidState, FM_None);
		}
		return Info->template SetFlagmarkMasked<Paradigm>(Flagmark, Mask);
	}

	/**
	 * Set the flagmark of the subject.
	 * Status version.
	 * 
	 * This is a user-level method that prevents setting
	 * the system-level flags.
	 * 
	 * This operation is atomic and thread-safe.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @param Flagmark The flagmark to set.
	 * @param Mask The mask to use.
	 * @return The status of the operation.
	 * 
	 * @deprecated since 1.23. Use the SetFlagmarkMasked() method within a polite paradigm, please.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	[[deprecated("The method is deprecated. Use the 'SetFlagmark' method within a polite paradigm, please.")]]
	FORCEINLINE typename std::enable_if<AllowsChanges || (Paradigm <= EParadigm::Unsafe), EApparatusStatus>::type
	SetFlagmarkMasked_Status(const EFlagmark Flagmark,
							 const EFlagmark Mask) const
	{
		const auto Info = FindInfo();
		if (LIKELY(Info))
		{
			return Info->template SetFlagmarkMasked<MakePolite(Paradigm)>(Flagmark, Mask);
		}
		return EApparatusStatus::InvalidState;
	}

	/**
	 * Set a single flag for the subject.
	 * 
	 * This is a user-level method. Setting of the system-level
	 * flags is prohibited by default.
	 * 
	 * The operation is atomic and thread-safe.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flag The flag to set.
	 * @param bState The state to set to.
	 * @return The previous state of the flag.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsChanges, bool>
	SetFlag(const EFlagmarkBit Flag,
			const bool         bState = true) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr,
					(MakeOutcome<Paradigm, bool>(EApparatusStatus::InvalidState, false)));
		return Info->template SetFlag<Paradigm>(Flag, bState);
	}

	/**
	 * Set a single flag for the subject.
	 * Status version.
	 * 
	 * This is a user-level method. Setting of the system-level
	 * flags is strictly prohibited.
	 * 
	 * The operation is atomic and thread-safe.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @param Flag The flag to set.
	 * @param bState The state to set to.
	 * @return The status of the operation.
	 * 
	 * @deprecated since 1.23. Use the SetFlag() method within a polite paradigm, please.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	[[deprecated("The status version of the method is deprecated. Use the 'SetFlag' method within a polite paradigm, please.")]]
	FORCEINLINE typename std::enable_if<AllowsChanges || (Paradigm <= EParadigm::Unsafe), EApparatusStatus>::type
	SetFlag_Status(const EFlagmarkBit Flag,
				   const bool         bState = true) const
	{
		const auto Info = FindInfo();
		if (LIKELY(Info))
		{
			return Info->template SetFlag<MakePolite(Paradigm)>(Flag, bState);
		}
		return EApparatusStatus::InvalidState;
	}

	/**
	 * Add flags to the flagmark of the subject.
	 *
	 * This is a user-level method that prevents adding
	 * the system-level flags.
	 * 
	 * This operation is atomic and thread-safe.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @param Flagmark The flagmark to set.
	 * @return The previous flagmark of the subject.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsChanges, EFlagmark>
	AddToFlagmark(const EFlagmark Flagmark) const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			return MakeOutcome<Paradigm, EFlagmark>(EApparatusStatus::InvalidState, FM_None);
		}
		return Info->template AddToFlagmark<Paradigm>(Flagmark);
	}

	/**
	 * Toggle a single flag for the subject.
	 * 
	 * This is a user-level method. Changing a system-level
	 * flag is prohibited by default.
	 * 
	 * The operation is atomic and thread-safe.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param Flag The flag to toggle.
	 * @return The new state of the corresponding flag.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsChanges, bool>
	ToggleFlag(const EFlagmarkBit Flag) const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			return MakeOutcome<Paradigm, bool>(EApparatusStatus::InvalidState, false);
		}
		return Info->template ToggleFlag<Paradigm>(Flag);
	}

	/// @}
#pragma endregion Flagmark

#pragma region Equality
	/// @name Equality
	/// @{

	/**
	 * Check if the subject handle points to the same subject
	 * as the other one.
	 * 
	 * If both handles are invalid, returns @c true.
	 * 
	 * @param InSubjectHandle A subject handle to compare with.
	 * @return true if the handles point to the same subject
	 * or are both invalid.
	 * @return false if the handles point to different
	 * subjects or one is invalid.
	 * 
	 * @see Equals()
	 */
	FORCEINLINE bool
	operator==(const FCommonSubjectHandle& InSubjectHandle) const
	{
		return Equals(InSubjectHandle);
	}

	/**
	 * Check if the subject handle points to a different subject
	 * than the other one.
	 * 
	 * Two invalid subject handles are considered to be the same
	 * and this operator would return @c false.
	 * 
	 * @param InSubjectHandle A subject handle to compare with.
	 * @return true if the handles point to different subjects
	 * or one is invalid.
	 * @return false if the handles point to the same subject
	 * or both are invalid.
	 * 
	 * @see Equals()
	 */
	FORCEINLINE bool
	operator!=(const FCommonSubjectHandle& InSubjectHandle) const
	{
		if (EqualsRaw(InSubjectHandle))
		{
			return false;
		}
		return IsValid() || InSubjectHandle.IsValid();
	}

	/// @}
#pragma endregion Equality

#pragma region Traits Data Access
	/// @name Traits Data Access
	/// @{

	/**
	 * Get a pointer to a trait of the subject of a certain type.
	 * Dynamic type version.
	 * 
	 * The operation is secure only for subject handles
	 * allowing direct access to the traits data.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @param TraitType The type of the trait to get.
	 * May be a @c nullptr and a @c nullptr is returned in this case.
	 * @return A pointer to the trait data.
	 * @return nullptr If there is no such trait in the subject
	 * or @p TraitType is a @c nullptr.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>
	GetTraitPtr(UScriptStruct* const TraitType) const
	{
		const auto Info = FindInfo();
		if (Avoid(Paradigm, Info == nullptr))
		{
			return MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::InvalidState, nullptr);
		}
		return Info->template GetTraitPtr<Paradigm>(TraitType);	
	}

	/**
	 * Get a pointer to a trait of a certain type.
	 * Static type paradigm version.
	 * 
	 * The operation is secure only for subject handles
	 * allowing direct access to the traits data.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A pointer to the trait data.
	 * @return nullptr If there is no such trait in the subject.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitPtrResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtr() const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr,
					(MakeOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>(EApparatusStatus::InvalidState, nullptr)));
		return Info->template GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Get a pointer to a trait of a certain type.
	 * Statically typed default paradigm version.
	 * 
	 * The operation is secure only for subject handles
	 * allowing direct access to the traits data.
	 *
	 * @tparam T The type of the trait to get.
	 * @return A pointer to the trait data.
	 * @return nullptr If there is no such trait in the subject.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default,
			   TTraitPtrResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtr() const
	{
		return GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Get a trait from a subject by its type.
	 * Static type paradigm version.
	 * 
	 * The operation is secure only for subject handles
	 * allowing direct access to the traits data.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A reference to the trait data.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitRefResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRef() const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			static thread_local T TraitTemp;
			return MakeOutcome<Paradigm, TTraitRefResult<Paradigm, T>>(EApparatusStatus::InvalidState, TraitTemp);
		}
		return Info->template GetTraitRef<Paradigm, T>();
	}

	/**
	 * Get a trait from a subject by its type.
	 * Static type version.
	 * 
	 * The operation is secure only for subject handles
	 * allowing direct access to the traits data.
	 *
	 * @tparam T The type of the trait to get.
	 * @return A reference to the trait data.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default,
			   TTraitRefResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRef() const
	{
		return GetTraitRef<Paradigm, T>();
	}

	/// @}
#pragma endregion Traits Data Access

#pragma region Traits Setting
	/// @name Traits Setting
	/// @{

	/**
	 * Set a trait of the subject by its type.
	 * Runtime type specification version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and set accordingly
	 * 
	 * @note This also supports passing a dynamic trait record
	 * and will cast it appropriately.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] TraitType The type of the trait to add.
	 * @param[in] TraitData The trait data to initialize with.
	 * May not be a @c nullptr, if the @p TraitType is set.
	 * @return The outcome of the operation.
	 * 
	 * @see GetTrait()
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsStructuralChanges>
	SetTrait(UScriptStruct* const TraitType,
			 const void* const    TraitData) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template SetTrait<Paradigm>(TraitType, TraitData);
	}

	/**
	 * Set a trait of the subject by its type.
	 * Statically typed pointer version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and set accordingly
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] TraitData The trait data to initialize with.
	 * May not be a @c nullptr.
	 * @return The outcome of the operation.
	 * 
	 * @see GetTrait()
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsStructuralChanges>
	SetTrait(const T* const TraitData) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template SetTrait<Paradigm>(TraitData);
	}

	/**
	 * Set a trait of the subject to a trait record.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and set accordingly
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @param TraitRecord The trait record to set to.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsStructuralChanges>
	SetTrait(const FTraitRecord& TraitRecord) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template SetTrait<Paradigm>(TraitRecord);
	}
	
	/**
	 * Set a trait of the subject. Statically typed version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and set accordingly.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The type of the trait to add.
	 * Must be a trait type.
	 * @param Trait The trait to initialize with.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   typename T = void,
			   typename std::enable_if<IsTraitType<T>(), bool>::type = true >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsStructuralChanges>
	SetTrait(const T& Trait) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template SetTrait<Paradigm>(Trait);
	}

	/**
	 * Add a flagmark to the subject.
	 * Statically typed flagmark-compatiblity version.
	 * 
	 * The flags are actually being added here.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The flagmark type.
	 * @param Flagmark The flagmark to add.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   typename T = void,
			   typename std::enable_if<IsFlagmarkType<T>(), bool>::type = true >
	FORCEINLINE auto
	SetTrait(const T& Flagmark) const
	{
		return AddToFlagmark<Paradigm>(Flagmark);
	}

	/**
	 * Set multiple subject traits equal to the supplied list.
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
	template< EParadigm Paradigm = EParadigm::Default,
			  typename A1 = FDefaultAllocator, typename A2 = FDefaultAllocator,
			  typename T = void >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsStructuralChanges>
	SetTraits(const TArray<UScriptStruct*, A1>& TraitsTypes,
			  const TArray<const T*, A2>&       TraitsData,
			  const bool                        bLeaveRedundant = true) const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			return EApparatusStatus::InvalidState;
		}
		return Info->template SetTraits<Paradigm>(TraitsTypes, TraitsData, bLeaveRedundant);
	}

	/**
	 * Set multiple subject traits from a subject record.
	 * 
	 * The traits essentially get copied from the record.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectRecord The subject traits pack to set from.
	 * @param bLeaveRedundant Should redundant existing traits
	 * (not present in the record) be left in the subject.
	 * @return The outcome of the operation.
	 */
	template< EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsStructuralChanges>
	SetTraits(const FSubjectRecord& SubjectRecord,
			  const bool            bLeaveRedundant = true) const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			return EApparatusStatus::InvalidState;
		}
		return Info->template SetTraits<Paradigm>(SubjectRecord);
	}

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
	template< EParadigm Paradigm = EParadigm::Default,
			  typename A1 = FDefaultAllocator, typename A2 = FDefaultAllocator,
			  typename T = void >
	FORCEINLINE TOutcome<Paradigm>
	OverwriteTraits(const TArray<UScriptStruct*, A1>& TraitsTypes,
					const TArray<const T*, A2>&       TraitsData) const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			return EApparatusStatus::InvalidState;
		}
		return Info->template OverwriteTraits<Paradigm>(TraitsTypes, TraitsData);
	}

	/**
	 * Overwrite existing traits from a subject record.
	 * 
	 * Only the existing traits get overwritten.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param SubjectRecord The subject traits pack to overwrite from.
	 * @return The outcome of the operation.
	 */
	template< EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsChanges>
	OverwriteTraits(const FSubjectRecord& SubjectRecord) const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			return EApparatusStatus::InvalidState;
		}
		return Info->template OverwriteTraits<Paradigm>(SubjectRecord);
	}

	/**
	 * Copy all of the traits of the subject to some other subject.
	 * 
	 * The existing traits of the destination subject are not removed
	 * but the new ones are added and/or override the existing matching ones.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AnyDirectAccess Basically, any trait access is supported by the method.
	 * @param DestinationSubjectHandle The destination subject to copy to.
	 * Must denote a valid subject.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, bool AnyDirectAccess = false >
	FORCEINLINE TOutcome<Paradigm>
	CopyTraitsTo(const TSubjectHandle<true, AnyDirectAccess, true>& DestinationSubjectHandle) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		const auto OtherInfo = DestinationSubjectHandle.FindInfo();
		AssessCondition(Paradigm, OtherInfo != nullptr, EApparatusStatus::InvalidArgument);
		return Info->template CopyTraitsTo<Paradigm>(OtherInfo);
	}

	/// @}
#pragma endregion Traits Setting

#pragma region Trait Obtainment
	/// @name Trait Obtainment
	/// @{

	/**
	 * Obtain a trait from the subject by its type.
	 * Outputting version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and the default value gets copied
	 * to the receiver.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the data to receive the trait.
	 * Must match the @p TraitType.
	 * @param TraitType The type of the trait to obtain.
	 * Must match that of the @p T parameter.
	 * @param OutTraitData The trait data receiver.
	 * @param bTraitDataInitialized Is the @p OutTraitData buffer actually initialized?
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void,
			   more::enable_if_t<AllowsStructuralChanges || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm>
	ObtainTrait(UScriptStruct* const TraitType,
				T* const             OutTraitData,
				const bool           bTraitDataInitialized = true) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template ObtainTrait<Paradigm>(TraitType, OutTraitData, bTraitDataInitialized);
	}

	/**
	 * Obtain a trait for the subject by its type.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to obtain.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   more::enable_if_t<AllowsStructuralChanges || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm>
	ObtainTrait(UScriptStruct* const TraitType) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template ObtainTrait<Paradigm>(TraitType);
	}

	/**
	 * Obtain a trait from the subject.
	 * Templated outputting version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and the default value gets copied
	 * to the receiver.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The type of the trait to obtain.
	 * @param[out] OutTrait The trait receiver.
	 * @param[in] bTraitDataInitialized Is the @p OutTrait actually initialized?
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void,
			   more::enable_if_t<AllowsStructuralChanges || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm>
	ObtainTrait(T&         OutTrait,
				const bool bTraitDataInitialized = true) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template ObtainTrait<Paradigm>(OutTrait,
													bTraitDataInitialized);
	}

	/**
	 * Obtain a trait copy from the subject.
	 * Statically typed paradigm version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and the default value is returned.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The type of the trait to obtain.
	 * @return A copy of the trait.
	 */
	template < EParadigm Paradigm, typename T,
			   more::enable_if_t<AllowsStructuralChanges || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm, T>
	ObtainTrait() const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			return MakeOutcome<Paradigm, T>(EApparatusStatus::InvalidState, T());
		}
		return Info->template ObtainTrait<Paradigm, T>();
	}

	/**
	 * Obtain a trait copy from the subject.
	 * Statically typed version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and the default value is returned.
	 *
	 * @tparam T The type of the trait to obtain.
	 * @return The copy of the trait.
	 */
	template < typename T >
	FORCEINLINE auto
	ObtainTrait() const
	{
		return ObtainTrait<EParadigm::Default, T>();
	}

	/**
	 * Obtain a trait data pointer from the subject by its type.
	 * Pointer version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and returned
	 * to the receiver.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to obtain.
	 * May be a @c nullptr and will return a @c nullptr in this case.
	 * @return The obtained trait data pointer.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   more::enable_if_t<(AllowsStructuralChanges && AllowsDirectTraitAccess) || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>
	ObtainTraitPtr(UScriptStruct* const TraitType) const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			return MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::InvalidState, nullptr);
		}
		return Info->template ObtainTraitPtr<Paradigm>();
	}

	/**
	 * Obtain a trait pointer from the subject.
	 * Statically typed pointer paradigm version.
	 *
	 * If the trait is not currently within the subject,
	 * it gets created anew and a pointer to it is returned.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The type of the trait to obtain.
	 * @return A pointer to the trait.
	 */
	template < EParadigm Paradigm, typename T,
			   more::enable_if_t<(AllowsStructuralChanges && AllowsDirectTraitAccess) || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	ObtainTraitPtr() const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			return MakeOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>(EApparatusStatus::InvalidState, nullptr);
		}
		return Info->template ObtainTraitPtr<Paradigm, T>();
	}

	/**
	 * Obtain a trait pointer from the subject.
	 * Statically typed pointer version.
	 *
	 * If the trait is not currently within the subject,
	 * it gets created anew and a pointer to it is returned.
	 *
	 * @tparam T The type of the trait to obtain.
	 * @return The pointer to the trait of
	 * the specified type.
	 */
	template < typename T >
	FORCEINLINE auto
	ObtainTraitPtr() const
	{
		return ObtainTraitPtr<EParadigm::Default, T>();
	}

	/**
	 * Obtain a trait reference from the subject.
	 * Templated paradigm reference version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and its reference is returned.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The type of the trait to obtain.
	 * @return A reference to the trait.
	 */
	template < EParadigm Paradigm, typename T,
			   more::enable_if_t<(AllowsStructuralChanges && AllowsDirectTraitAccess) || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	ObtainTraitRef() const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			static T TraitTemp;
			return MakeOutcome<Paradigm, TTraitRefResult<Paradigm, T>>(EApparatusStatus::InvalidState, TraitTemp);
		}
		return Info->template ObtainTraitRef<Paradigm, T>();
	}

	/**
	 * Obtain a trait reference from the subject.
	 * Templated reference version.
	 * 
	 * If the trait is not currently within the subject,
	 * it gets created anew and its reference is returned.
	 *
	 * @tparam T The type of the trait to obtain.
	 * @return A reference to the trait.
	 */
	template < typename T >
	FORCEINLINE auto
	ObtainTraitRef() const
	{
		return ObtainTraitRef<EParadigm::Default, T>();
	}

	/**
	 * Obtain a trait for the subject by its type.
	 * Dynamically-typed deferred version.
	 * 
	 * The actual change (if any) is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @param TraitType The type of the trait to obtain.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   more::enable_if_t<AllowsChanges || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm>
	ObtainTraitDeferred(UScriptStruct* const TraitType) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template ObtainTraitDeferred<Paradigm>(TraitType);
	}

	/**
	 * Obtain a trait copy from the subject.
	 * Templated paradigm deferred version.
	 * 
	 * If the trait is currently missing in the
	 * subject, its default value is returned.
	 * 
	 * The actual change (if any) is deferred until the
	 * deferreds are applied either automatically
	 * or manually.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to obtain.
	 * @tparam Paradigm The security paradigm to use.
	 * @return A copy of the trait.
	 */
	template < EParadigm Paradigm, typename T,
			   more::enable_if_t<AllowsChanges || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm, T>
	ObtainTraitDeferred() const
	{
		const auto Info = FindInfo();
		if (AvoidCondition(Paradigm, Info == nullptr))
		{
			return MakeOutcome<Paradigm, T>(EApparatusStatus::InvalidState, T());
		}
		return Info->template ObtainTraitDeferred<Paradigm, T>();
	}

	/**
	 * Obtain a trait copy from the subject.
	 * Templated deferred version.
	 * 
	 * If the trait is currently missing in the
	 * subject, its default value is returned.
	 * 
	 * The actual change (if any) is deferred until the
	 * deferreds are applied either automatically
	 * or manually.
	 *
	 * @tparam T The type of the trait to obtain.
	 * @tparam Paradigm The paradigm to work under.
	 * @return A copy of the trait.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	ObtainTraitDeferred() const
	{
		return ObtainTraitDeferred<Paradigm, T>();
	}

	/// @}
#pragma endregion Trait Obtainment

#pragma region Deferred Trait Setting
	/// @name Deferred Trait Settings
	/// @{

	/**
	 * Set a trait of the subject by its type.
	 * Dynamically typed deferred version.
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
	 * @param[in] TraitType The type of the trait to set.
	 * May be a @c nullptr and @p TraitData is not used
	 * in that case.
	 * @param[in] TraitData The trait data to initialize with.
	 * May not be a @c nullptr if the @p TraitType is specified.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void,
			   more::enable_if_t<AllowsChanges || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm>
	SetTraitDeferred(UScriptStruct* const TraitType,
					 const void* const    TraitData) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template SetTraitDeferred<Paradigm>(TraitType, TraitData);
	}

	/**
	 * Set a trait of the subject by its type.
	 * Statically typed deferred pointer version.
	 * 
	 * The actual change is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 * 
	 * At the time of performing the operation,
	 * if the trait is not currently within the subject,
	 * it gets created anew and set accordingly.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The type of the trait to set.
	 * @param Trait The trait data to initialize with.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void,
			   more::enable_if_t<AllowsChanges || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm>
	SetTraitDeferred(const T* Trait) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template SetTraitDeferred<Paradigm>(Trait);
	}

	/**
	 * Set a trait of the subject by its type.
	 * Statically typed deferred version.
	 * 
	 * The actual change is deferred until the 
	 * deferreds are applied either automatically
	 * or manually.
	 * 
	 * At the time of performing the operation,
	 * if the trait is not currently within the subject,
	 * it gets created anew and set accordingly.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The type of the trait to set.
	 * @param Trait The trait to initialize with.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void,
			   more::enable_if_t<AllowsChanges || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm>
	SetTraitDeferred(const T& Trait) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template SetTraitDeferred<Paradigm, T>(Trait);
	}

	/// @}
#pragma endregion Deferred Trait Setting

#pragma region Trait Removal
	/// @name Trait Removal
	/// @{

	/**
	 * Remove a trait from the subject.
	 * Dynamic type version.
	 * 
	 * If there is no such trait in the subject,
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @param TraitType The type of the trait to remove.
	 * If is a @c nullptr, nothing is performed.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop if there was no such
	 * trait in the first place.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   more::enable_if_t<AllowsStructuralChanges || IsUnsafe(Paradigm), bool> = true  >
	FORCEINLINE TOutcome<Paradigm>
	RemoveTrait(UScriptStruct* const TraitType) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template RemoveTrait<Paradigm>(TraitType);
	}

	/**
	 * Remove a trait from the subject.
	 * Templated paradigm version.
	 * 
	 * If there is no such trait in the subject,
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The type of the trait to remove.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T,
			   more::enable_if_t<AllowsStructuralChanges || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm>
	RemoveTrait() const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template RemoveTrait<Paradigm, T>();
	}

	/**
	 * Remove a trait from the subject.
	 * Templated version.
	 * 
	 * If there is no such trait in the subject,
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 *
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The type of the trait to remove.
	 * @return The outcome of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	RemoveTrait() const
	{
		return RemoveTrait<Paradigm, T>();
	}

	/**
	 * Remove all of the traits from the subject (if any).
	 * 
	 * If there are no traits in the subject,
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @return The outcome of the operation.
	 * @return EApparatusStatus::Noop if there are no traits
	 * currently within the subject.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   more::enable_if_t<AllowsStructuralChanges || IsUnsafe(Paradigm), bool> = true >
	FORCEINLINE TOutcome<Paradigm>
	RemoveAllTraits() const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template RemoveAllTraits<Paradigm>();
	}

	/// @}
#pragma endregion Trait Removal

#pragma region Deferred Trait Removal
	/// @name Deferred Trait Removal
	/// @{

	/**
	 * Remove a trait from the subject.
	 * Deferred version.
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
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   more::enable_if_t<IsUnsafe(Paradigm) || AllowsChanges, int> = 0 >
	FORCEINLINE TOutcome<Paradigm>
	RemoveTraitDeferred(UScriptStruct* const TraitType) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template RemoveTraitDeferred<Paradigm>(TraitType);
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
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The type of the trait to remove.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T,
			   more::enable_if_t<IsUnsafe(Paradigm) || AllowsChanges, int> = 0 >
	FORCEINLINE TOutcome<Paradigm>
	RemoveTraitDeferred() const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template RemoveTraitDeferred<Paradigm, T>();
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
	 * @tparam Paradigm The security paradigm to use.
	 * @return The outcome of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	RemoveTraitDeferred() const
	{
		return RemoveTraitDeferred<Paradigm, T>();
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
	 * @return EApparatusStatus::Deferred If the operation was successfully queued
	 * to be executed at a later time.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsChanges>
	RemoveAllTraitsDeferred(const bool bForceEnqueue = false) const
	{
		const auto Info = FindInfo();
		AssessCondition(Paradigm, Info != nullptr, EApparatusStatus::InvalidState);
		return Info->template RemoveAllTraitsDeferred<Paradigm>(bForceEnqueue);
	}

	/// @}
#pragma endregion Deferred Trait Removal

#pragma region Details Getting
	/// @name Details Getting
	/// @{

	/**
	 * Get a detail of a certain class.
	 * 
	 * @param DetailClass The class of the detail to get.
	 * @return A pointer to the detail of the designated class.
	 * @return nullptr If there is no such detail.
	 */
	FORCEINLINE TDetailPtrResult<UDetail>
	GetDetail(TSubclassOf<UDetail> DetailClass) const
	{
		return Super::GetDetail(DetailClass);
	}

	/**
	 * Get a detail of a certain class.
	 * Templated version.
	 * 
	 * @tparam D The class of the detail to get.
	 * @return A pointer to the detail of the designated class.
	 * @return nullptr If there is no such detail.
	 */
	template < class D >
	FORCEINLINE TDetailPtrResult<D>
	GetDetail() const
	{
		return Super::template GetDetail<D>();
	}

	/// @}
#pragma endregion Details Getting

#pragma region Networking
	/// @name Networking
	/// @{

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
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsChanges>
	BringOnline() const
	{
		const auto Info = FindInfo();
		AssessConditionFormat(Paradigm, Info != nullptr, EApparatusStatus::InvalidState,
							  TEXT("A subject must be valid to bring it online. "
								   "Was it already despawned?"));
		return Info->template BringOnline<Paradigm>();
	}

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
	template < EParadigm Paradigm = EParadigm::Default >
	TOutcome<Paradigm>
	BringOnline(const TRange<NetworkIdType>& IdRange) const
	{
		const auto Info = FindInfo();
		AssessConditionFormat(Paradigm, Info != nullptr, EApparatusStatus::InvalidState,
							  TEXT("A subject must be valid to bring it online. "
								   "Was it already despawned?"));
		return Info->template BringOnline<Paradigm>(IdRange);
	}

	/**
	 * Set the server-side connection to a client from which it is allowed
	 * to push traits to this subject.
	 * 
	 * @tparam Paradigm The safety paradigm to invoke under.
	 * @param[in] Connection The connection permit to set.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsChanges>
	SetConnectionPermit(UNetConnection* const Connection) const
	{
		const auto Info = FindInfo();
		AssessConditionFormat(Paradigm, Info != nullptr, EApparatusStatus::InvalidState,
							  TEXT("A subject must be valid to set its connection permit. "
								   "Was it already despawned?"));
		return Info->template SetConnectionPermit<Paradigm>(Connection);
	}

	/**
	 * Set the server-side list
	 * of traits allowed to be pushed from clients.
	 * 
	 * @tparam Paradigm The safety paradigm to invoke under.
	 * @param[in] InTraitmarkPermit The traitmark permit to set.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsChanges>
	SetTraitmarkPermit(const FTraitmark& InTraitmarkPermit) const
	{
		const auto Info = FindInfo();
		AssessConditionFormat(Paradigm, Info != nullptr, EApparatusStatus::InvalidState,
							  TEXT("A subject must be valid to set its traitmark permit. "
								   "Was it already despawned?"));
		return Info->template SetTraitmarkPermit<Paradigm>(InTraitmarkPermit);
	}

	template < EParadigm Paradigm = EParadigm::Default >
	[[deprecated("The method was renamed. Use 'SetTraitmarkPermit', please.")]]
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsChanges>
	SetTraitmarkPass(const FTraitmark& InTraitmarkPermit) const
	{
		return SetTraitmarkPermit<Paradigm>(InTraitmarkPermit);
	}

	/**
	 * Send an additional trait to the remote version of the subject.
	 * Templated version.
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
	 * @tparam Paradigm The safety paradigm to invoke under.
	 * @param[in] TraitType The type of the trait to send.
	 * @param[in] TraitData The data of the trait to send.
	 * @param[in] PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param[in] bReliable Should the trait be sent using a reliable channel.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsStructuralChanges>
	PushTrait(UScriptStruct* const TraitType,
			  const void* const    TraitData,
			  const EPeerRole      PeerRole  = EPeerRole::Auto,
			  const bool           bReliable = true) const
	{
		const auto Info = FindInfo();
		AssessConditionFormat(Paradigm, Info != nullptr, EApparatusStatus::InvalidState, TEXT("A subject must be valid to push the trait for."));
		return Info->template PushTrait<Paradigm>(TraitType, TraitData, PeerRole, bReliable);
	}

	/**
	 * Send an additional trait to the remote version of the subject.
	 * Templated version.
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
	 * @tparam Paradigm The safety paradigm to invoke under.
	 * @param[in] TraitType The type of the trait to send.
	 * @param[in] TraitData The data of the trait to send.
	 * @param[in] bSetForLocal Should the trait also be set for the local version
	 * of the subject.
	 * @param[in] PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param[in] bReliable Should the trait be sent using a reliable channel.
	 * @return The outcome of the operation.
	 * 
	 * @deprecated since 1.23 The @p bSetForLocal argument is deprecated now.
	 * Set the trait explicitly or use the version of the method without the argument, please.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	[[deprecated("The 'bSetForLocal' argument is deprecated now. Set the trait explicitly or use the version of the method without the argument, please.")]]
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsStructuralChanges>
	PushTrait(UScriptStruct* const TraitType,
			  const void* const    TraitData,
			  const bool           bSetForLocal = false,
			  const EPeerRole      PeerRole     = EPeerRole::Auto,
			  const bool           bReliable    = true) const
	{
		AssessConditionFormat(Paradigm, !bSetForLocal, EApparatusStatus::InvalidArgument, TEXT("The 'bSetForLocal' argument is deprecated. Set the trait explicitly and use the method without it, please."))
		const auto Info = FindInfo();
		AssessConditionFormat(Paradigm, Info != nullptr, EApparatusStatus::InvalidState, TEXT("A subject must be valid to push an extra trait for."));
		return Info->template PushTrait<Paradigm>(TraitType, TraitData, PeerRole, bReliable);
	}

	/**
	 * Send an additional trait to the remote version of the subject.
	 * Templated version.
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
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The type of the trait to send.
	 * @param[in] Trait The trait to push.
	 * @param[in] PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you should provide one explicitly sometimes.
	 * @param[in] bReliable Should the trait be sent using a reliable channel.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void >
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsStructuralChanges>
	PushTrait(const T&        Trait,
			  const EPeerRole PeerRole = EPeerRole::Auto,
			  const bool      bReliable = true) const
	{
		const auto Info = FindInfo();
		AssessConditionFormat(Paradigm, Info != nullptr, EApparatusStatus::InvalidState, TEXT("A subject must be valid to push the trait for."));
		return Info->template PushTrait<Paradigm>(Trait, PeerRole, bReliable);
	}

	/**
	 * Send an additional trait to the remote version of the subject.
	 * Templated version.
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
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The type of the trait to send.
	 * @param[in] Trait The trait to push.
	 * @param[in] bSetForLocal Should the trait be also set on the local version
	 * of the subject.
	 * @param[in] PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you should provide one explicitly sometimes.
	 * @param[in] bReliable Should the trait be sent using a reliable channel.
	 * @return The outcome of the operation.
	 * 
	 * @deprecated since 1.23 The @p bSetForLocal argument is deprecated now.
	 * Set the trait explicitly or use the version of the method without the argument, please.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void >
	[[deprecated("The 'bSetForLocal' argument is deprecated now. Set the trait explicitly or use the version of the method without the argument, please.")]]
	FORCEINLINE TOutcomeIfUnsafeOr<Paradigm, AllowsStructuralChanges>
	PushTrait(const T&        Trait,
			  const bool      bSetForLocal = false,
			  const EPeerRole PeerRole = EPeerRole::Auto,
			  const bool      bReliable = true) const
	{
		AssessConditionFormat(Paradigm, !bSetForLocal, EApparatusStatus::InvalidArgument, TEXT("The 'bSetForLocal' argument is deprecated. Set the trait explicitly and use the method without it, please."))
		const auto Info = FindInfo();
		AssessConditionFormat(Paradigm, Info != nullptr, EApparatusStatus::InvalidState, TEXT("A subject must be valid to push the trait for."));
		return Info->template PushTrait<Paradigm>(Trait, PeerRole, bReliable);
	}

	/**
	 * Send an additional trait to the remote version of the subject.
	 * Statically typed immutable version.
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
	 * @tparam Paradigm The security paradigm to use.
	 * @tparam T The type of the trait to send.
	 * @param[in] Trait The trait to push.
	 * @param[in] PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you should provide one explicitly sometimes.
	 * @param[in] bReliable Should the trait be sent using a reliable channel.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T = void >
	FORCEINLINE TOutcome<Paradigm>
	PushTrait(const T&        Trait,
			  const EPeerRole PeerRole = EPeerRole::Auto,
			  const bool      bReliable = true) const
	{
		const auto Info = FindInfo();
		AssessConditionFormat(Paradigm, Info != nullptr, EApparatusStatus::InvalidState,
							  TEXT("A subject must be valid to push an additional trait for it."));
		return Info->template PushTrait<Paradigm>(Trait, PeerRole, bReliable);
	}

	/**
	 * Send an existing trait to the remote version of the subject.
	 * Dynamically typed immutable version.
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
	 * @tparam Paradigm The safety paradigm to invoke under.
	 * @param[in] TraitType The type of the trait to send.
	 * The type must already exist within the subject or be a @c nullptr.
	 * @param[in] PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param[in] bReliable Should the trait be sent using a reliable channel.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	PushTrait(UScriptStruct* const TraitType,
			  const EPeerRole      PeerRole = EPeerRole::Auto,
			  const bool           bReliable = true) const
	{
		const auto Info = FindInfo();
		AssessConditionFormat(Paradigm, Info != nullptr, EApparatusStatus::InvalidState, TEXT("A subject must be valid to push an existing trait for."));
		return Info->template PushTrait<Paradigm>(TraitType, PeerRole, bReliable);
	}

	/**
	 * Send an existing trait to the remote version of the subject.
	 * Dynamically typed immutable version.
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
	 * @tparam Paradigm The safety paradigm to invoke under.
	 * @param[in] TraitType The type of the trait to send.
	 * The type must already exist within the subject or be a @c nullptr.
	 * @param[in] bReliable Should the trait be sent using a reliable channel.
	 * @param[in] PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	PushTrait(UScriptStruct* const TraitType,
			  const bool           bReliable,
			  const EPeerRole      PeerRole = EPeerRole::Auto) const
	{
		const auto Info = FindInfo();
		AssessConditionFormat(Paradigm, Info != nullptr, EApparatusStatus::InvalidState, TEXT("A subject must be valid to push an existing trait for."));
		return Info->template PushTrait<Paradigm>(TraitType, PeerRole, bReliable);
	}

	/**
	 * Send an existing trait to the remote version of the subject.
	 * Statically typed immutable version.
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
	 * @tparam T The type of the trait to send.
	 * @tparam Paradigm The safety paradigm to invoke under.
	 * @param[in] PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param[in] bReliable Should a reliable channel be used for the transaction.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm>
	PushTrait(const EPeerRole PeerRole  = EPeerRole::Auto,
			  const bool      bReliable = true) const
	{
		const auto Info = FindInfo();
		AssessConditionFormat(Paradigm, Info != nullptr, EApparatusStatus::InvalidState, TEXT("A subject must be valid to push the trait for."));
		return Info->template PushTrait<Paradigm, T>(PeerRole, bReliable);
	}

	/**
	 * Send an existing trait to the remote version of the subject.
	 * Statically typed default paradigm immutable version.
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
	 * @tparam T The type of the trait to send.
	 * @tparam Paradigm The safety paradigm to invoke under.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @param bReliable Should a reliable channel be used for the transaction.
	 * @return The outcome of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	PushTrait(const EPeerRole PeerRole  = EPeerRole::Auto,
			  const bool      bReliable = true) const
	{
		return PushTrait<Paradigm, T>(PeerRole, bReliable);
	}

	/**
	 * Send an existing trait to the remote version of the subject.
	 * Statically typed default paradigm immutable version.
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
	 * @tparam T The type of the trait to send.
	 * @tparam Paradigm The safety paradigm to invoke under.
	 * @param bReliable Should a reliable channel be used for the transaction.
	 * @param PeerRole The peer role specification. If left as EPeerRole::Auto,
	 * the peer role is tried to be derived automatically. This may be ambiguous
	 * on a listen server, so you can provide one explicitly.
	 * @return The outcome of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	PushTrait(const bool bReliable, const EPeerRole PeerRole = EPeerRole::Auto) const
	{
		return PushTrait<Paradigm, T>(PeerRole, bReliable);
	}

	/// @}
#pragma endregion Networking



#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Construct a new invalid subject handle.
	 */
	FORCEINLINE
	TSubjectHandle()
	{}

	/**
	 * Copy-construct a new subject handle.
	 */
	FORCEINLINE
	TSubjectHandle(const TSubjectHandle& InHandle)
	  : FCommonSubjectHandle(InHandle)
	{}

	/// @}
#pragma endregion Initialization

}; //-struct TSubjectHandle

#pragma region Subject Info

FORCEINLINE EApparatusStatus
FSubjectInfo::GetHandleBase(FCommonSubjectHandle& Subject) const
{
	if (UNLIKELY(!IsValid()))
	{
		Subject.Id = InvalidId;
		return EApparatusStatus::InvalidState;
	}

	Subject.Id         = Id;
	Subject.Generation = Generation;

	return EApparatusStatus::Success;
}

FORCEINLINE FCommonSubjectHandle
FSubjectInfo::GetHandleBase() const
{
	return FCommonSubjectHandle(Id, Generation);
}

#pragma endregion Subject Info

/**
 * Check if the supplied type is actually a subject handle.
 * 
 * Supports checking the solid subjects.
 * 
 * @tparam T The type to examine.
 * @return The state of examination.
 */
template < typename T >
constexpr bool IsSubjectHandleType()
{
	return std::is_base_of<FCommonSubjectHandle, T>::value;
}

#if CPP && !defined(SKIP_MACHINE_H)
#include "Machine.h"
#endif
