/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ChunkSlot.h
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

#include <atomic>
#include "More/type_traits"

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "More/HAL/UnrealMemory.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define CHUNK_SLOT_H_SKIPPED_MACHINE_H
#endif

#include "Fingerprint.h"

#ifdef CHUNK_SLOT_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif


// Forward declarations:
struct FCommonSubjectHandle;
class UChunk;
class AMechanism;
class UMachine;

/**
 * A single subject entry within the chunk.
 * 
 * @see ::UChunk
 */
struct APPARATUSRUNTIME_API FChunkSlot
{
  private:

	friend struct FSubjectInfo;
	friend class UChunk;
	friend class AMechanism;
	friend class UMachine;

	template < typename SubjectHandleT >
	friend struct TChunkIt;

	/**
	 * An identifier of the original
	 * subject, residing withing the slot.
	 * 
	 * The identifier is the
	 * most reliable reference here,
	 * since the subject itself can actually
	 * move to somewhere else.
	 * 
	 * We don't need a generation here,
	 * since this identifier will be reset,
	 * when the subject is despawned.
	 */
	FSubjectInfo::IdType SubjectId = FSubjectInfo::InvalidId;

	/**
	 * The fingerprint of the subject.
	 * 
	 * The fingerprint is stale by default.
	 * It should not be initialized until the subject FChunkSlot::Handle is set.
	 * 
	 * We store it in here and not in
	 * ::FSubjectInfo for cache friendliness
	 * during the iterating and also for the
	 * EFlagmarkBit::Stale bit to be set locally.
	 */
	FFingerprint Fingerprint{FM_Stale};

	/**
	 * Get a reference to the original fingerprint of the subject.
	 */
	FORCEINLINE FFingerprint&
	GetFingerprintRef()
	{
		return Fingerprint;
	}

	/**
	 * Mark the slot for removal.
	 *
	 * Sets the corresponding flag internally.
	 *
	 * The method is atomic and thread-safe.
	 */
	template < EParadigm Paradigm = EParadigm::Internal >
	FORCEINLINE std::enable_if_t<Paradigm <= EParadigm::Internal, bool>
	SetStale()
	{
		return Fingerprint.SetFlag(EFlagmarkBit::Stale);
	}

	/**
	 * Reset the slot,
	 * clearing the handle and the fingerprint
	 * essentially making this slot a stale one.
	 */
	template < EParadigm Paradigm = EParadigm::Internal >
	FORCEINLINE std::enable_if_t<Paradigm <= EParadigm::Internal, void>
	Reset()
	{
		SubjectId = FSubjectInfo::InvalidId;
		Fingerprint.Reset(FM_Stale);
	}

	/**
	 * Get the subject information structure.
	 * Constant version.
	 */ 
	const FSubjectInfo&
	GetInfo() const;

	/**
	 * Get the subject information structure.
	 */ 
	FSubjectInfo&
	GetInfo();

	/**
	 * Find the subject information structure.
	 * Constant version.
	 */ 
	const FSubjectInfo*
	FindInfo() const;

	/**
	 * Find the subject information structure.
	 */ 
	FSubjectInfo*
	FindInfo();

	/**
	 * This assignment operator should be used
	 * to occupy the entry with a subject.
	 * 
	 * This does not modify the fingerprint traits/details
	 * though.
	 */
	FChunkSlot&
	operator=(FSubjectInfo* const InInfo);

	/**
	 * This assignment operator should be used
	 * to occupy the entry with a subject handle.
	 * 
	 * This does not modify the fingerprint traits/details
	 * though.
	 */
	FChunkSlot&
	operator=(const FCommonSubjectHandle& InHandle);

  public:

#pragma region Flagmark
	/// @name Flagmark

	/**
	 * Get the flagmark of the subject.
	 *
	 * This examination is atomic and thread-safe.
	 *
	 * @return The flagmark of the subject.
	 */
	FORCEINLINE EFlagmark
	GetFlagmark() const
	{
		return Fingerprint.GetFlagmark();
	}

	/**
	 * Check if a certain flag is set for the slot.
	 *
	 * This examination is atomic and thread-safe.
	 *
	 * @param Flag The flag to examine.
	 * @return The state of the flag.
	 */
	FORCEINLINE bool
	HasFlag(const EFlagmarkBit Flag) const
	{
		return Fingerprint.HasFlag(Flag);
	}

	/**
	 * Check if the slot is stale, i.e. marked for removal from the chunk.
	 * 
	 * This examination is atomic and thread-safe.
	 */
	FORCEINLINE bool
	IsStale() const
	{
		return Fingerprint.IsStale();
	}

	/**
	 * Set the flagmark of the subject.
	 *
	 * This operation is atomic and thread-safe.
	 * 
	 * All of the system-level flags get removed from the argument
	 * prior to application.
	 *
	 * @tparam Paradigm The safety paradigm to utilize.
	 * @param InFlagmark The new flagmark to set to.
	 * @return The previous flagmark of the subject.
	 */
	template < EParadigm Paradigm = EParadigm::Safe >
	FORCEINLINE EFlagmark
	SetFlagmark(const EFlagmark InFlagmark)
	{
		if (Paradigm <= EParadigm::Internal) // Compile-time branch.
		{
			return Fingerprint.SetFlagmark(InFlagmark);
		}
		else
		{
			return Fingerprint.SetFlagmarkMasked(InFlagmark, FM_AllUserLevel);
		}
	}

	/**
	 * Set the masked flagmark for the subject.
	 *
	 * This operation is atomic and thread-safe.
	 * 
	 * All of the system-level flags get removed from the mask argument
	 * prior to application.
	 *
	 * @tparam Paradigm The safety paradigm to utilize.
	 * @param InFlagmark The new flagmark to set to.
	 * @param Mask The mask to apply on the flagmark bits to set.
	 * @return The previous flagmark of the subject.
	 */
	template < EParadigm Paradigm = EParadigm::Safe >
	FORCEINLINE EFlagmark
	SetFlagmarkMasked(const EFlagmark InFlagmark, const EFlagmark Mask)
	{
		if (Paradigm <= EParadigm::Internal) // Compile-time branch.
		{
			return Fingerprint.SetFlagmarkMasked(InFlagmark, Mask);
		}
		else
		{
			return Fingerprint.SetFlagmarkMasked(InFlagmark, FM_AllUserLevel & Mask);
		}
	}

	/**
	 * Set a flag of the subject.
	 *
	 * This operation is atomic and thread-safe.
	 *
	 * @tparam Paradigm The safety paradigm to utilize.
	 * @param InFlag The flag to set.
	 * @param bState The state to set to.
	 * @return The previous flagmark of the subject.
	 */
	template < EParadigm Paradigm = EParadigm::Safe >
	FORCEINLINE bool
	SetFlag(const EFlagmarkBit InFlag, const bool bState = true)
	{
		if ((Paradigm <= EParadigm::Internal) || IsUserLevel(InFlag)) // Semi-compile-time branch.
		{
			return Fingerprint.SetFlag(InFlag, bState);
		}
		checkNoEntry();
		return false;
	}

	/**
	 * Toggle a flag of the subject.
	 *
	 * This operation is atomic and thread-safe.
	 *
	 * @tparam Paradigm The safety paradigm to utilize.
	 * @param InFlag The flag to set.
	 * @return The previous flagmark of the subject.
	 */
	template < EParadigm Paradigm = EParadigm::Safe >
	FORCEINLINE bool
	ToggleFlag(const EFlagmarkBit InFlag)
	{
		if ((Paradigm <= EParadigm::Internal) || IsUserLevel(InFlag)) // Semi-compile-time branch.
		{
			return Fingerprint.ToggleFlag(InFlag);
		}
		checkNoEntry();
		return false;
	}

	/// @}
#pragma endregion Flagmark

#pragma region Fingerprint

	/**
	 * Get the fingerprint of the subject.
	 */
	FORCEINLINE const FFingerprint&
	GetFingerprint() const
	{
		return Fingerprint;
	}

#pragma endregion Fingerprint

	/**
	 * Check if the slot is for
	 * a completely despawned subject.
	 */
	FORCEINLINE bool
	IsDespawned() const
	{
		return SubjectId == FSubjectInfo::InvalidId;
	}

	/**
	 * Check if the slot points to a valid
	 * subject but which should be moved to some
	 * other chunk.
	 */
	bool
	IsMoved(UChunk* const OrigChunk) const;

	/**
	 * Get the subject handle.
	 *
	 * @tparam SubjectHandleT The type of the
	 * subject handle to get.
	 */
	template < typename SubjectHandleT = FSubjectHandle >
	SubjectHandleT
	GetHandle() const;

#pragma region Initialization

	/**
	 * Initialize a new empty chunk slot.
	 */
	FORCEINLINE
	FChunkSlot() {}

#pragma endregion Initialization

}; //-struct FChunkSlot
