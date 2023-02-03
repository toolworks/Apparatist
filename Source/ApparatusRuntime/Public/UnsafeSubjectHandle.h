/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: UnsafeSubjectHandle.h
 * Created: 2022-04-06 00:02:43
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
#define UNSAFE_SUBJECT_HANDLE_H_SKIPPED_MACHINE_H
#endif

#include "ConstSubjectHandle.h"

#ifdef UNSAFE_SUBJECT_HANDLE_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "UnsafeSubjectHandle.generated.h"


// Forward declarations:
class ISubjective;
class UChunk;
class AMechanism;
struct FSolidSubjectHandle;
struct FConstSubjectHandle;

template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
struct TChain;

/**
 * An unsafe subject handle with all of the functionality available.
 *
 * Exposes all of the available APIs. Use at your own risk.
 * This is a user-level handle structure, something like a pointer.
 * The size of this structure is exactly 64 bits and the instances
 * can be passed to functions by value.
 */
USTRUCT()
struct APPARATUSRUNTIME_API FUnsafeSubjectHandle
#if CPP
  : public TSubjectHandle<true, true, true>
#else
  : public FCommonSubjectHandle
#endif
{
	GENERATED_BODY()

  public:

	/**
	 * The base handle type.
	 */
	using SuperT = TSubjectHandle<true, true, true>;

	/**
	 * A global constant for an invalid (NULL) unsafe subject handle.
	 */
	static const FUnsafeSubjectHandle Invalid;

	/**
	 * The compatible type of the subjective used.
	 */
	typedef ISubjective* SubjectivePtrType;

  private:

	friend struct FSubjectInfo;
	friend struct FSolidSubjectHandle;
	friend struct FConstSubjectHandle;
	friend class UChunk;
	friend class AMechanism;
	friend class UMachine;
	friend class ISubjective;

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	template < EParadigm Paradigm,
			   typename From, typename To,
			   bool ArgumentTypeCheck >
	friend struct TSmartCastImpl;

	/**
	 * Initialize an unsafe subject handle using raw information.
	 * 
	 * @param InId The identifier of the subject.
	 * @param InGeneration The generation of the subject.
	 */
	FORCEINLINE
	FUnsafeSubjectHandle(const int32 InId,
						 const int32 InGeneration)
	  : SuperT(InId, InGeneration)
	{
		static_assert(sizeof(FCommonSubjectHandle) == sizeof(*this),
					  "The size of the unsafe subject handle must be equal to the base version.");
	}

	/**
	 * Initialize an unsafe subject handle from a common base one.
	 * 
	 * @param InHandle The handle to initialize from.
	 */
	FORCEINLINE
	FUnsafeSubjectHandle(const FCommonSubjectHandle& InHandle)
	  : SuperT(InHandle)
	{}

  public:

	/**
	 * Get a subjective this handle is associated with (if any).
	 */
	FORCEINLINE SubjectivePtrType
	GetSubjective() const
	{
		return FCommonSubjectHandle::GetSubjective();
	}

#pragma region Validity
	/// @name Validity
	/// @{

	/**
	 * Check if the unsafe subject handle is invalid
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
	 * Check if the unsafe subject handle is valid
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
	 * Check if the unsafe subject handle is valid
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
	 * Check if the unsafe subject handle points to the same subject
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
	 * Check if the unsafe subject handle points to different subject
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
	 * Copy-assign an unsafe subject handle.
	 */
	FORCEINLINE FUnsafeSubjectHandle&
	operator=(const FUnsafeSubjectHandle& InHandle)
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
	 * Implicit conversion to a mutable version.
	 */
	FORCEINLINE
	operator FSubjectHandle() const
	{
		return FSubjectHandle(Id, Generation);
	}

	/**
	 * Implicit conversion to a uniform version.
	 */
	FORCEINLINE
	operator FSolidSubjectHandle() const
	{
		return FSolidSubjectHandle(Id, Generation);
	}

	/**
	 * Implicit conversion to an immutable version.
	 */
	FORCEINLINE
	operator FConstSubjectHandle() const
	{
		return FConstSubjectHandle(Id, Generation);
	}

	/// @}
#pragma endregion Conversion

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

#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Initialize an invalid (null) unsafe subject handle.
	 */
	FUnsafeSubjectHandle()
	{};

	/**
	 * Initialize a copy of an unsafe subject handle.
	 */
	FUnsafeSubjectHandle(const FUnsafeSubjectHandle& InHandle)
	  : SuperT(InHandle)
	{};

	/**
	 * Convert from a usual subject handle.
	 */
	explicit
	FUnsafeSubjectHandle(const FSubjectHandle& InHandle)
	  : SuperT(InHandle)
	{};

	/**
	 * Convert from a solid subject handle.
	 */
	explicit
	FUnsafeSubjectHandle(const FSolidSubjectHandle& InHandle)
	  : SuperT(InHandle)
	{};

	/// @}
#pragma endregion Initialization

}; // struct FUnsafeSubjectHandle

template<>
struct TStructOpsTypeTraits<FUnsafeSubjectHandle>
  : public TStructOpsTypeTraitsBase2<FUnsafeSubjectHandle>
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
