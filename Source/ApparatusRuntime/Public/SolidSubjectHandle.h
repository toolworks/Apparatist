/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SolidSubjectHandle.h
 * Created: 2021-07-24 18:32:45
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

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define SOLID_SUBJECT_HANDLE_H_SKIPPED_MACHINE_H
#endif

#include "SubjectHandle.h"

#ifdef SOLID_SUBJECT_HANDLE_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "SolidSubjectHandle.generated.h"


// Forward declarations:
struct FConstSubjectHandle;
struct FUnsafeSubjectHandle;
class ISubjective;
class ISolidSubjective;

/**
 * The handle for the uniform (homogenous) subject.
 *
 * The handle disallows any structural changes on the subject.
 * You can only queue the deferred changes.
 *
 * This is a user-level handle structure, something like a pointer.
 * The size of this structure is exactly 64-bit and may be passed by value.
 */
USTRUCT()
struct APPARATUSRUNTIME_API FSolidSubjectHandle
#if CPP
  : public TSubjectHandle<true, true, false>
#else
  : public FCommonSubjectHandle
#endif
{
	GENERATED_BODY()

  public:

	/**
	 * Base generic type.
	 */
	using SuperT = TSubjectHandle<true, true, false>;

	/**
	 * A global constant for an invalid (NULL) subject handle.
	 */
	static const FSolidSubjectHandle Invalid;

	/**
	 * The compatible type of the subjective used.
	 */
	typedef ISolidSubjective* SubjectivePtrType;

  private:

	friend struct FSubjectInfo;
	friend struct FCommonSubjectHandle;
	friend struct FSubjectHandle;
	friend struct FConstSubjectHandle;
	friend struct FUnsafeSubjectHandle;
	friend class UChunk;
	friend class UMachine;
	friend class ISubjective;

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	FORCEINLINE
	FSolidSubjectHandle(const int32 InId,
						const int32 InGeneration)
	  : SuperT(InId, InGeneration)
	{
		static_assert(sizeof(FCommonSubjectHandle) == sizeof(*this), "The size must match the base one.");
	}

  public:

	/**
	 * Get a subjective this handle is associated with (if any).
	 */
	SubjectivePtrType
	GetSubjective() const;

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
	 * Check if the solid subject handle is invalid
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
	 * Check if the solid subject handle is valid
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
	 * Check if the solid subject handle is valid
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
	 * Check if the solid subject handle points to the same subject
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
	 * Check if the solid subject handle points to a different subject
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
	 * Copy a solid subject handle.
	 */
	FORCEINLINE FSolidSubjectHandle&
	operator=(const FSolidSubjectHandle& InHandle)
	{
		Id         = InHandle.Id;
		Generation = InHandle.Generation;
		return *this;
	}

	/// @}
#pragma endregion Assignment

#pragma region Conversion
	/// @name Conversion
	/// @{

	/**
	 * Explicit conversion to a mutable version.
	 */
	FORCEINLINE
	explicit operator FSubjectHandle() const
	{
		return FSubjectHandle(Id, Generation);
	}

	/// @}
#pragma endregion Conversion

#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Construct a new invalid solid subject handle.
	 */
	FORCEINLINE
	FSolidSubjectHandle()
	{}

	/**
	 * Copy-construct a new solid subject handle.
	 */
	FORCEINLINE
	FSolidSubjectHandle(const FSolidSubjectHandle& InHandle)
	  : SuperT(InHandle)
	{}

	/**
	 * Copy-construct a new solid subject handle from a non-solid one.
	 * 
	 * The conversion is implicit cause the solid handle is defined
	 * to have a lesser functionality scope.
	 */
	FORCEINLINE
	FSolidSubjectHandle(const FSubjectHandle& InHandle)
	  : FSolidSubjectHandle(InHandle.Id, InHandle.Generation)
	{}

	/**
	 * Copy-construct a new subject handle from a base version.
	 * 
	 * @param InBaseHandle The base handle to initialize with.
	 */
	explicit FORCEINLINE
	FSolidSubjectHandle(const FCommonSubjectHandle& InBaseHandle)
	  : SuperT(InBaseHandle)
	{}

	/// @}
#pragma endregion Initialization

}; //-struct FSolidSubjectHandle

template<>
struct TStructOpsTypeTraits<FSolidSubjectHandle>
  : public TStructOpsTypeTraitsBase2<FSolidSubjectHandle>
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
