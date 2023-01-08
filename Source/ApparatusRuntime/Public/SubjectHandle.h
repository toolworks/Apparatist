/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectHandle.h
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
#include "UObject/Class.h"
#include "Kismet/KismetSystemLibrary.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define SUBJECT_HANDLE_H_SKIPPED_MACHINE_H
#endif

#include "CommonSubjectHandle.h"

#ifdef SUBJECT_HANDLE_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "SubjectHandle.generated.h"


// Forward declarations:
class ISubjective;
class UChunk;
class AMechanism;
struct FSolidSubjectHandle;
struct FConstSubjectHandle;
struct FUnsafeSubjectHandle;

template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
struct TChain;

/**
 * A subject with structural changes allowed.
 *
 * This is a user-level handle structure, something like a pointer.
 * The size of this structure is exactly 64-bit and may be passed by value.
 */
USTRUCT(BlueprintType)
struct APPARATUSRUNTIME_API FSubjectHandle
#if CPP
  : public TSubjectHandle<true, false, true>
#else
  : public FCommonSubjectHandle
#endif
{
	GENERATED_BODY()

  public:

	/**
	 * Base generic type.
	 */
	using SuperT = TSubjectHandle<true, false, true>;

	/**
	 * A global constant for an invalid (NULL) subject handle.
	 */
	static const FSubjectHandle Invalid;

	/**
	 * The compatible type of the subjective used.
	 */
	typedef ISubjective* SubjectivePtrType;

  private:

	friend struct FSubjectInfo;
	friend struct FSolidSubjectHandle;
	friend struct FConstSubjectHandle;
	friend struct FUnsafeSubjectHandle;
	friend class UChunk;
	friend class AMechanism;
	friend class UMachine;
	friend class ISubjective;

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	/**
	 * Initialize a subject handle using raw information.
	 * 
	 * @param InId The identifier of the subject.
	 * @param InGeneration The generation of the subject.
	 */
	FORCEINLINE
	FSubjectHandle(const int32 InId,
				   const int32 InGeneration)
	  : SuperT(InId, InGeneration)
	{
		static_assert(sizeof(FCommonSubjectHandle) == sizeof(*this), "The size must match the base one.");
	}

  public:

	/**
	 * Get a subjective this handle is associated with (if any).
	 */
	FORCEINLINE SubjectivePtrType
	GetSubjective() const
	{
		return (SubjectivePtrType)Super::GetSubjective();
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

	/**
	 * Check if the subject handle is valid
	 * and is pointing to an existent subject.
	 *
	 * @return Returns @c true if the subject is valid.
	 * Otherwise, @c false is returned.
	 */
	FORCEINLINE
	operator bool() const
	{
		return IsValid();
	}

	/// @}
#pragma endregion Validity

#pragma region Equality
	/// @name Equality
	/// @{

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
	 * Check if the subject handle point to a different subject
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

#pragma region Assignment
	/// @name Assignment
	/// @{

	/**
	 * Copy a subject handle.
	 */
	FORCEINLINE FSubjectHandle&
	operator=(const FSubjectHandle& InHandle)
	{
		Id         = InHandle.Id;
		Generation = InHandle.Generation;
		return *this;
	}

	/// @}
#pragma endregion Assignment

#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Construct a new invalid subject handle.
	 */
	FORCEINLINE
	FSubjectHandle()
	{}

	/**
	 * Copy-construct a new subject handle.
	 */
	FORCEINLINE
	FSubjectHandle(const FSubjectHandle& InHandle)
	  : SuperT(InHandle)
	{}

	/**
	 * Initialize a new subject handle from a base handle version.
	 * 
	 * @param InBaseHandle The base handle to initialize with.
	 */
	explicit FORCEINLINE
	FSubjectHandle(const FCommonSubjectHandle& InBaseHandle)
	  : SuperT(InBaseHandle)
	{}

	/// @}
#pragma endregion Initialization

}; //-struct FSubjectHandle

template<>
struct TStructOpsTypeTraits<FSubjectHandle>
  : public TStructOpsTypeTraitsBase2<FSubjectHandle>
{
	enum 
	{
		WithCopy = true,
		WithIdenticalViaEquality = true
		// TODO: Implement net serialization.
	}; 
};

#if CPP && !defined(SKIP_MACHINE_H)
#include "Machine.h"
#endif
