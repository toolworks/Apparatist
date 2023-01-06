/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ConstSubjectHandle.h
 * Created: 2021-07-19 16:58:36
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

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define CONST_SUBJECT_HANDLE_H_SKIPPED_MACHINE_H
#endif

#include "SolidSubjectHandle.h"

#ifdef CONST_SUBJECT_HANDLE_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "ConstSubjectHandle.generated.h"


/**
 * The handle for the immutable subject.
 *
 * This is a user-level handle structure, something like a pointer.
 * The size of this structure is exactly 64-bit and may be passed by value.
 */
USTRUCT()
struct APPARATUSRUNTIME_API FConstSubjectHandle
#if CPP
  : public TSubjectHandle<false, true, false>
#else
  : public FCommonSubjectHandle
#endif
{
	GENERATED_BODY()

  public:

	/**
	 * Base generic type.
	 */
	using SuperT = TSubjectHandle<false, true, false>;

	/**
	 * A global constant for an invalid (NULL) subject handle.
	 */
	static const FConstSubjectHandle Invalid;

	/**
	 * The compatible type of the subjective used.
	 */
	typedef const ISubjective* SubjectivePtrType;

  private:

	friend struct FSubjectInfo;
	friend struct FCommonSubjectHandle;
	friend struct FSubjectHandle;
	friend struct FSolidSubjectHandle;
	friend struct FUnsafeSubjectHandle;
	friend class UChunk;
	friend class UMachine;
	friend class ISubjective;

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	/**
	 * Initialize a constant subject handle using
	 * raw identifier and a generation.
	 * 
	 * @param InId The unique identifier of the subject.
	 * @param InGeneration The generation of the subject information entry.
	 */
	FORCEINLINE
	FConstSubjectHandle(const int32 InId,
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
		return const_cast<SubjectivePtrType>(FCommonSubjectHandle::GetSubjective());
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
	 * Check if the constant subject handle is invalid
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
	 * Check if the constant subject handle is valid
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
	 * Check if the constant subject handle is valid
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
	 * Check if the constant subject handle points to the same subject
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
	 * Check if the constant subject handle points to a different subject
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
	FORCEINLINE FConstSubjectHandle&
	operator=(const FConstSubjectHandle& InHandle)
	{
		Id = InHandle.Id;
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

	/**
	 * Explicit conversion to a uniform version.
	 */
	FORCEINLINE
	explicit operator FSolidSubjectHandle() const
	{
		return FSolidSubjectHandle(Id, Generation);
	}

	/// @}
#pragma endregion Conversion

#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Construct a new invalid constant subject handle.
	 */
	FORCEINLINE
	FConstSubjectHandle()
	{}

	/**
	 * Copy-construct a new constant subject handle.
	 */
	FORCEINLINE
	FConstSubjectHandle(const FConstSubjectHandle& InHandle)
	  : SuperT(InHandle)
	{}

	/**
	 * Copy-construct a new const subject handle
	 * from a uniform version.
	 * 
	 * The conversion is always safe and thereby implicit.
	 */
	FORCEINLINE
	FConstSubjectHandle(const FSolidSubjectHandle& InHandle)
	  : SuperT(InHandle)
	{}

	/**
	 * Copy-construct a new constant subject handle from
	 * a mutable version.
	 *
	 * The conversion is always safe and thereby implicit.
	 */
	FORCEINLINE
	FConstSubjectHandle(const FSubjectHandle& InHandle)
	  : SuperT(InHandle)
	{}

	/**
	 * Copy-construct a new constant subject handle from a base version.
	 */
	explicit FORCEINLINE
	FConstSubjectHandle(const FCommonSubjectHandle& InHandle)
	  : SuperT(InHandle)
	{}

	/// @}
#pragma endregion Initialization

}; //-struct FConstSubjectHandle

template<>
struct TStructOpsTypeTraits<FConstSubjectHandle>
  : public TStructOpsTypeTraitsBase2<FConstSubjectHandle>
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
