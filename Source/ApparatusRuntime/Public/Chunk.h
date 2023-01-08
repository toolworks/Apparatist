/*
 * ░▒▓ APPARATUS ▓▒░
 *
 * File: Chunk.h
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

#include "CoreMinimal.h"
#include "HAL/UnrealMemory.h"
#include "UObject/Class.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define CHUNK_H_SKIPPED_MACHINE_H
#endif

#include "ApparatusStatus.h"
#include "ScriptStructArray.h"
#include "ChunkAdjectiveEntry.h"
#include "Traitmark.h"
#include "MechanicInfo.h"
#include "Iterable.h"
#include "ChunkSlot.h"

#ifdef CHUNK_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "Chunk.generated.h"


#define LOCTEXT_NAMESPACE "Apparatus.UChunk"

// Forward declarations:
template < typename SubjectHandleT >
struct TChunkIt;
class AMechanism;
class UMachine;

/**
 * The main container for the subjects' traits.
 */
UCLASS(BlueprintType)
class APPARATUSRUNTIME_API UChunk
  : public UObject
  , public IIterable
{
	GENERATED_BODY()

  public:

	enum
	{
		/**
		 * Invalid chunk identifier.
		 */
		InvalidId = 0,

		/**
		 * First valid chunk identifier.
		 */
		FirstId = 1,

		/**
		 * Invalid trait line index.
		 */
		InvalidTraitLineIndex = -1,

		/**
		 * Invalid subject index.
		 */
		InvalidSlotIndex = FSubjectInfo::InvalidSlotIndex,
	};

  private:

	friend struct FSubjectInfo;
	friend struct FCommonSubjectHandle;
	friend struct FMechanicInfo;
	friend struct FFingerprint;
	friend struct FSubjectInfo;
	friend class AMechanism;
	friend class UMachine;

	template < typename SubjectHandleT >
	friend struct TChunkIt;

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	/**
	 * A unique identifier of the chunk.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk,
			  Meta = (AllowPrivateAccess = "true"))
	int32 Id = InvalidId;

	/**
	 * The traitmark of the chunk.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chunk,
			  Meta = (AllowPrivateAccess = "true"))
	FTraitmark Traitmark;

	/**
	 * The lines in the chunk, consisting of traits data.
	 */
	TArray<FScriptStructArray> Lines;

	/**
	 * The subjects for the respective trait rows
	 * within the lines.
	 */
	TArray<FChunkSlot> Slots;

	/**
	 * The adjectives of the chunk.
	 */
	TArray<FChunkAdjectiveEntry> Adjectives;

	/**
	 * An array used to store a subject's traits
	 * for the mechanic execution.
	 * 
	 * Stores only pointers to traits, not copies.
	 */
	void** TraitsTemp = nullptr;

	/**
	 * Replace a subject with another one, given its index.
	 */
	EApparatusStatus
	DoReplaceSlot(const int32 DstSlotIndex,
				  const int32 SrcSlotIndex);

	/**
	 * Set a chunk's traitmark, initializing its internal structures.
	 */
	EApparatusStatus
	Initialize(const int32       InId,
			   const FTraitmark& InTraitmark,
			   const int32       Capacity = 32);

	/**
	 * Reserve a new subject place (index) within a chunk.
	 *
	 * @note The exact subject id has to be set manually on the UChunk::Subjects array appropriately
	 * after calling this method. Otherwise, the state of the chunk will be
	 * incomplete and invalid.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @return The new prepared subject index or FSubjectInfo::InvalidSlotIndex if
	 * the reservation has failed.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm, FSubjectInfo::SlotIndexType>
	ReserveSubjectSlot();

	/**
	 * Release a subject at a specified index within the chunk. Low-level version.
	 *
	 * @param SubjectIndex The index of the subject to release.
	 *
	 * @return The status of the operation.
	 */
	EApparatusStatus
	DoReleaseSlot(const int32 SubjectIndex);

	/**
	 * Release a subject at a specified index within the chunk.
	 *
	 * @param SlotIndex The index of the subject to release.
	 * @param bHard Should the subject reference
	 * be cleared from the slot also?
	 * @return The status of the operation.
	 */
	EApparatusStatus
	ReleaseSlot(const int32 SlotIndex,
				const bool  bHard);

	/**
	 * Add an adjective to the chunk.
	 *
	 * @param Adjective The adjective to add.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	AddAdjective(struct FAdjectiveInfo* Adjective);

	/**
	 * Apply all of the chunk adjectives to a single subject within the chunk.
	 *
	 * @param Subject The internal subject information from the mechanism.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	ApplyAdjectives(struct FSubjectInfo* Subject);

	/**
	 * A low-level method to pop some last subjects
	 * from the chunk along with their traits.
	 * 
	 * @param InCount The number of elements to pop.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	DoPop(const int32 InCount = 1)
	{
		check((InCount) >= 0 && (InCount <= Slots.Num()));
		if (UNLIKELY(InCount == 0))
		{
			return EApparatusStatus::Noop;
		}
		for (int32 i = 1; i <= InCount; ++i)
		{
			const auto SlotIndex = Slots.Num() - i;
			auto& Slot = Slots[SlotIndex];
			if ((Slot.SubjectId != FSubjectInfo::InvalidId)
				&& Slot.HasFlag(EFlagmarkBit::DeferredDespawn))
			{
				const auto Info = Slot.FindInfo();
				if (LIKELY(Info && (Info->Chunk == this)))
				{
					check(Info->SlotIndex == SlotIndex);
					// This can actually be a deferred-removed
					// subject, so we have to make sure its info
					// is no longer referencing this chunk.
					Info->Chunk     = nullptr;
					Info->SlotIndex = InvalidSlotIndex;
				}
			}
		}
		Slots.RemoveAt(Slots.Num() - InCount, InCount);
		for (int32 i = 0; i < Lines.Num(); ++i)
		{
			Lines[i].Pop(InCount);
			check(Slots.Num() == Lines[i].Num());
		}
		Count = Slots.Num();
		return EApparatusStatus::Success;
	}

	/**
	 * A low-level method to clear the subject along with its
	 * traits from the chunk.
	 * 
	 * @param SlotIndex The index of a subject to clear.
	 * @param bHard Should the slot's subject reference
	 * be cleared also?
	 * @return The status of the operation. 
	 */
	FORCEINLINE EApparatusStatus
	ClearSubjectAt(const int32 SlotIndex,
				   const bool  bHard)
	{
		check(SlotIndex > FSubjectInfo::InvalidSlotIndex);
		check(SlotIndex < Slots.Num());

		for (int32 i = 0; i < Lines.Num(); ++i)
		{
			Lines[i].ClearElementAt(SlotIndex);
		}

		// Mark the slot as missing:
		Slots[SlotIndex].SetStale();
		if (bHard)
		{
			// If we are actually despawning,
			// the subject must be cleared explicitly.
			Slots[SlotIndex].SubjectId = FSubjectInfo::InvalidId;
		}

		return EApparatusStatus::Success;
	}

	/**
	 * Clone a subject at a specified index.
	 * 
	 * @param SlotIndex The index of a subject to clone.
	 * @param SubjectInfo The subject information to fill
	 * with the new subject.
	 * @return The newly created slot reference.
	 */
	FORCEINLINE FChunkSlot&
	CloneSubjectAt(const int32   SlotIndex,
				   FSubjectInfo& SubjectInfo)
	{
		check(SlotIndex > FSubjectInfo::InvalidSlotIndex);
		check(SlotIndex < Slots.Num());
		auto& Slot = Slots[SlotIndex];
		checkf(!Slot.IsStale(),
			   TEXT("Cloning stale slots is not supported."));

		for (int32 i = 0; i < Lines.Num(); ++i)
		{
			verify(Lines[i].CloneElementAt(SlotIndex) == Slots.Num());
		}

		SubjectInfo.SlotIndex = Slots.Num();
		SubjectInfo.Chunk     = this;
		check(SubjectInfo.IsValid());
		
		auto& NewSlot = Slots.AddDefaulted_GetRef();
		NewSlot.Fingerprint = Slot.Fingerprint;
		// The networking and boot states should not be copied by default:
		NewSlot.Fingerprint.SetFlagmark(Slot.Fingerprint.GetFlagmark() - 
										EFlagmarkBit::Booted - 
										EFlagmarkBit::Online);
		return NewSlot;
	}

  protected:

	/**
	 * Finalize the chunk, freeing all of its resources.
	 */
	void
	BeginDestroy() override;

	EApparatusStatus
	DoUnlock(const bool bWasSolid) const override;

  public:

	/**
	 * Get the traitmark of the chunk.
	 */
	FORCEINLINE const FTraitmark&
	GetTraitmark() const
	{
		return Traitmark;
	}

	/**
	 * Get the owning mechanism of the chunk.
	 */
	AMechanism*
	GetOwner() const;

	/**
	 * Create a new chunk instance.
	 */
	static FORCEINLINE UChunk*
	New(AMechanism* const Owner,
		const int32       Id,
		const FTraitmark& InTraitmark);

#pragma region Slots
	/// @name Slots
	/// @{

	/**
	 * Get a slot at a certain position.
	 * Constant version.
	 */
	FORCEINLINE const FChunkSlot&
	At(const int32 SlotIndex) const
	{
		return Slots[SlotIndex];
	}

	/**
	 * Get a slot at a certain position.
	 */
	FORCEINLINE FChunkSlot&
	At(const int32 SlotIndex)
	{
		return Slots[SlotIndex];
	}

	/**
	 * Get a slot at a certain position.
	 * Constant version.
	 */
	FORCEINLINE const FChunkSlot&
	operator[](const int32 SlotIndex) const
	{
		return At(SlotIndex);
	}

	/**
	 * Get a slot at a certain position.
	 */
	FORCEINLINE FChunkSlot&
	operator[](const int32 SlotIndex)
	{
		return At(SlotIndex);
	}

	/**
	 * Check if a slot is stale at a certain position.
	 * 
	 * Stale slots should generally be skipped during the iterating
	 * process.
	 * 
	 * @param SlotIndex The index of the subject slot to examine.
	 * @return The status of the examination.
	 */
	FORCEINLINE bool
	IsStaleAt(const int32 SlotIndex) const
	{
		return At(SlotIndex).IsStale();
	}

	/// @}
#pragma endregion Slots

#pragma region Subjects
	/// @name Subjects
	/// @{

	/**
	 * Get a subject at a specific slot index.
	 * 
	 * @tparam SubjectHandleT The type of the handle to acquire.
	 * @param SlotIndex The index of the subject to get.
	 * @return The subject at the specified index.
	 */
	template < typename SubjectHandleT = FSubjectHandle >
	FORCEINLINE SubjectHandleT
	SubjectAt(const int32 SlotIndex) const
	{
		return (SubjectHandleT)At(SlotIndex).GetHandle();
	}

	/**
	 * Get a subject's fingerprint at a specific slot index.
	 *
	 * The method is cache efficient during the iterating process,
	 * cause the fingerprints are stored within the chunk slots.
	 *
	 * @param SlotIndex The index of the subject slot to examine.
	 * @return The fingerprint of the subject.
	 */
	FORCEINLINE const FFingerprint&
	SubjectFingerprintAt(const int32 SlotIndex) const
	{
		return At(SlotIndex).GetFingerprint();
	}

	/**
	 * Get a subject's flagmark at a specific slot index.
	 *
	 * The operation is thread-safe and atomic.
	 *
	 * The method is cache efficient during the iterating process,
	 * cause the flagmarks are stored within the chunk slots.
	 *
	 * @param SlotIndex The index of the subject slot to examine.
	 * @return The flagmark of the subject.
	 */
	FORCEINLINE EFlagmark
	SubjectFlagmarkAt(const int32 SlotIndex) const
	{
		return SubjectFingerprintAt(SlotIndex).GetFlagmark();
	}

	/**
	 * Check a subject's flag at a specific slot index.
	 *
	 * The operation is thread-safe and atomic.
	 *
	 * The method is cache efficient during the iterating process,
	 * cause the flagmarks are stored within the chunk slots.
	 *
	 * @param SlotIndex The index of the subject slot to examine.
	 * @param Flag The flag to test.
	 * @return The state of the flag.
	 * 
	 * @see SetSubjectFlagAt(const int32, const EFlagmarkBit, const bool)
	 */
	FORCEINLINE bool
	HasSubjectFlagAt(const int32        SlotIndex,
					 const EFlagmarkBit Flag) const
	{
		return At(SlotIndex).HasFlag(Flag);
	}

	/**
	 * Set a subject's flagmark at a specific slot index.
	 *
	 * The operation is thread-safe and atomic.
	 *
	 * The method is cache efficient during the iterating process,
	 * cause the flagmarks are stored within the chunk slots.
	 *
	 * @tparam Paradigm The safety paradigm to utilize.
	 * @param SlotIndex The index of the subject slot to examine.
	 * @param Flagmark The flagmark to set to.
	 * @return The flagmark of the subject.
	 */
	template < EParadigm Paradigm = EParadigm::Safe >
	FORCEINLINE EFlagmark
	SetSubjectFlagmarkAt(const int32 SlotIndex, const EFlagmark Flagmark) const
	{
		return At(SlotIndex).template SetFlagmark<Paradigm>(Flagmark);
	}

	/**
	 * Set the masked flagmark for the subject.
	 *
	 * This operation is atomic and thread-safe.
	 * 
	 * All of the system-level flags get removed from the mask argument
	 * prior to application.
	 *
	 * The method is cache efficient during the iterating process,
	 * cause the flagmarks are stored within the chunk slots.
	 *
	 * @tparam Paradigm The safety paradigm to utilize.
	 * @param SlotIndex The index of the subject slot to examine.
	 * @param Flagmark The new flagmark to set to.
	 * @param Mask The mask to apply on the flagmark bits to set.
	 * @return The previous flagmark of the subject.
	 * 
	 * @see SetSubjectFlagmarkAt(const int32, const EFlagmark)
	 */
	template < EParadigm Paradigm = EParadigm::Safe >
	FORCEINLINE EFlagmark
	SetSubjectFlagmarkMaskedAt(const int32     SlotIndex,
							   const EFlagmark Flagmark,
							   const EFlagmark Mask)
	{
		return At(SlotIndex).template SetFlagmarkMasked<Paradigm>(Flagmark, Mask);
	}

	/**
	 * Set a subject's flag at a specific slot index.
	 *
	 * The operation is thread-safe and atomic.
	 *
	 * The method is cache efficient during the iterating process,
	 * cause the flagmarks are stored within the chunk slots.
	 *
	 * @tparam Paradigm The safety paradigm to utilize.
	 * @param SlotIndex The index of the subject slot to examine.
	 * @param Flag The flag to set or reset.
	 * @param bState The state of the flag to set to.
	 * @return The previous state of the flag.
	 * 
	 * @see HasSubjectFlagAt(const int32, const EFlagmarkBit)
	 */
	template < EParadigm Paradigm = EParadigm::Safe >
	FORCEINLINE bool
	SetSubjectFlagAt(const int32        SlotIndex,
					 const EFlagmarkBit Flag,
					 const bool         bState = true) const
	{
		return At(SlotIndex).template SetFlag<Paradigm>(Flag, bState);
	}

	/**
	 * Toggle a subject's flag at a specific slot index.
	 *
	 * The operation is thread-safe and atomic.
	 *
	 * The method is cache efficient during the iterating process,
	 * cause the flagmarks are stored within the chunk slots.
	 *
	 * @tparam Paradigm The safety paradigm to utilize.
	 * @param SlotIndex The index of the subject slot to examine.
	 * @param Flag The flag to toggle.
	 * @return The previous state of the flag.
	 * 
	 * @see SetSubjectFlagAt(const int32, const EFlagmarkBit, const bool)
	 */
	template < EParadigm Paradigm = EParadigm::Safe >
	FORCEINLINE bool
	ToggleSubjectFlagAt(const int32        SlotIndex,
						const EFlagmarkBit Flag) const
	{
		return At(SlotIndex).template ToggleFlag<Paradigm>(Flag);
	}

	/// @}
#pragma endregion Subjects

#pragma region Trait Lines
	/// @name Trait Lines
	/// @{

	/**
	 * Get the type of a trait line.
	 * 
	 * @param LineIndex An index of the line.
	 * @return The trait type of the line.
	 */
	FORCEINLINE UScriptStruct*
	TraitLineTypeAt(const int32 LineIndex) const
	{
		check(LineIndex > InvalidTraitLineIndex);
		return Traitmark.TraitAt(LineIndex);
	}

	/**
	 * The number of trait lines in the chunk.
	 *
	 * @return The total number of trait lines in the chunk.
	 */
	FORCEINLINE int32
	TraitLinesNum() const
	{
		return Traitmark.TraitsNum();
	}

	/**
	 * Get the size of a trait given its line index.
	 * 
	 * @param LineIndex The index of the line.
	 * @return The size of the trait structure corresponding to the line.
	 */
	FORCEINLINE int32
	TraitLineTraitSizeAt(const int32 LineIndex) const
	{
		return TraitLineTypeAt(LineIndex)->GetStructureSize();
	}

	/**
	 * Get a trait line by the type of its element.
	 * 
	 * @param TraitType The type of the trait to get by.
	 * @return The trait line of the specified type.
	 */
	FORCEINLINE const FScriptStructArray& 
	GetTraitLine(UScriptStruct* const TraitType) const
	{
		const auto LineIndex = Traitmark.IndexOf(TraitType);
		return TraitLineAt(LineIndex);
	}

	/**
	 * Get a trait line by the type of its element.
	 * Templated version.
	 * 
	 * @tparam T The type of the trait to get by.
	 * @return The trait line of the specified type.
	 */
	template < typename T >
	FORCEINLINE const FScriptStructArray& 
	GetTraitLine() const
	{
		return GetTraitLine(T::StaticStruct());
	}

	/**
	 * Get a trait line at a specific index.
	 * Constant version.
	 * 
	 * @param LineIndex An index of the line.
	 * @return The trait line at the specified index.
	 */
	FORCEINLINE const FScriptStructArray& 
	TraitLineAt(const int32 LineIndex) const
	{
		check(LineIndex > InvalidTraitLineIndex);
		return Lines[LineIndex];
	}

	/**
	 * Get a trait line at a specific index.
	 * 
	 * @param LineIndex An index of the line.
	 * @return The trait line at the specified index.
	 */
	FORCEINLINE FScriptStructArray& 
	TraitLineAt(const int32 LineIndex)
	{
		check(LineIndex > InvalidTraitLineIndex);
		return Lines[LineIndex];
	}

	/**
	 * Check if a trait line is within the chunk.
	 * 
	 * @param TraitType The type of the trait to check against.
	 * May not be a @c nullptr.
	 * @return The state of examination. 
	 */
	FORCEINLINE bool
	HasTraitLine(UScriptStruct* const TraitType) const
	{
		return Traitmark.Contains(TraitType);
	}

	/**
	 * Find a trait line by the type of its element.
	 * Constant version.
	 * 
	 * @param TraitType The type of the traits to search for.
	 * @return The trait line of the specified type.
	 * @return nullptr If there is no such line in the chunk.
	 */
	FORCEINLINE const FScriptStructArray*
	FindTraitLine(UScriptStruct* const TraitType) const
	{
		const auto LineIndex = Traitmark.IndexOf(TraitType);
		if (LineIndex == INDEX_NONE)
		{
			return nullptr;
		}
		return &TraitLineAt(LineIndex);
	}

	/**
	 * Find a trait line by the type of its element.
	 * 
	 * @param TraitType The type of the traits to search for.
	 * @return The trait line of the specified type.
	 * @return nullptr If there is no such line in the chunk.
	 */
	FORCEINLINE FScriptStructArray*
	FindTraitLine(UScriptStruct* const TraitType)
	{
		const auto LineIndex = Traitmark.IndexOf(TraitType);
		if (LineIndex <= INDEX_NONE)
		{
			return nullptr;
		}
		return &TraitLineAt(LineIndex);
	}

	/**
	 * Find a trait line by the type of its element.
	 * Templated constant version.
	 * 
	 * @tparam T The type of the traits to search for.
	 * @return The trait line of the specified type.
	 * @return nullptr If there is no such trait line in the chunk.
	 */
	template < typename T >
	FORCEINLINE typename std::enable_if<IsTraitType<T>(), const FScriptStructArray*>::type
	FindTraitLine() const
	{
		return FindTraitLine(T::StaticStruct());
	}

	/**
	 * Find a trait line by the type of its element.
	 * Templated version.
	 * 
	 * @tparam T The type of the traits to search for.
	 * @return The trait line of the specified type.
	 * @return nullptr If there is no such trait line in the chunk.
	 */
	template < typename T >
	FORCEINLINE typename std::enable_if<IsTraitType<T>(), FScriptStructArray*>::type
	FindTraitLine()
	{
		return FindTraitLine(T::StaticStruct());
	}

	/**
	 * Get the trait line index of a trait.
	 * 
	 * @param TraitType The type of the trait to find.
	 * @return The index of the trait, or @c INDEX_NONE, if there
	 * is no such trait within the chunk.
	 */
	FORCEINLINE int32
	TraitLineIndexOf(UScriptStruct* const TraitType) const
	{
		return Traitmark.IndexOf(TraitType);
	}

	/**
	 * Get the trait line index of a trait.
	 * 
	 * @note This method actually supports non-trait types and @c INDEX_NONE
	 * will be returned in such case.
	 * 
	 * @tparam T The type of the trait to find.
	 * @return The index of the trait, or @c INDEX_NONE, if there
	 * is no such trait within the chunk.
	 */
	template < typename T >
	FORCEINLINE constexpr int32
	TraitLineIndexOf() const
	{
		return Traitmark.template IndexOf<T>();
	}

	/// @}
#pragma endregion Trait Lines

	/**
	 * Check if the subject at specified place is locked.
	 *
	 * @return The status of the examination.
	 */
	FORCEINLINE bool
	IsSlotLocked(const int32 SlotIndex) const
	{
		check(SlotIndex > InvalidSlotIndex);
		// We can just use iterable count here, since
		// it's -1 when the chunk is unlocked.
		return SlotIndex < IterableCount;
	}

	/**
	 * Reserve a certain capacity within the chunk.
	 */
	void
	Reserve(const int32 InCapacity);

	/**
	 * Begin iterating the chunk.
	 *
	 * @tparam ChunkItT The type of the iterator to use.
	 * @param Filter The filter to iterate under.
	 * @param Offset The slot offset to begin with.
	 * @return The iterator for the chunk.
	 */
	template < typename ChunkItT >
	FORCEINLINE ChunkItT
	Begin(const FFilter& Filter,
		  const int32    Offset = 0)
	{
		if (UNLIKELY((Slots.Num() <= Offset) || 
					 (IsLocked() && (IterableCount <= Offset))))
		{
			// No subjects currently in the chunk to be iterated.
			return End<ChunkItT>();
		}

		return ChunkItT(this, Filter, Offset);
	}

	/**
	 * The ending iterator.
	 *
	 * @tparam ChunkItT The type of the chunk iterator.
	 * @return The ending iterator.
	 */
	template < typename ChunkItT >
	FORCEINLINE ChunkItT
	End() const
	{
		return ChunkItT(this); 
	}

#pragma region Traits Access
	/// @name Traits Access
	/// @{

	/**
	 * Copy a trait's data given a subject and line indices
	 * to a supplied data buffer.
	 *
	 * @param[in] SlotIndex The index of the subject which trait to copy.
	 * @param[in] LineIndex The index of the trait line.
	 * @param[out] OutTraitData The output buffer to copy to.
	 * @param[in] bTraitDataInitialized Is the trait data initialized or a raw buffer?
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	TraitAt(const int32 SlotIndex,
			const int32 LineIndex,
			void* const OutTraitData,
			bool        bTraitDataInitialized = true) const
	{
		check(SlotIndex != InvalidSlotIndex);
		check(LineIndex != InvalidTraitLineIndex);
		return Lines[LineIndex].ElementAt(SlotIndex, OutTraitData, bTraitDataInitialized);
	}

	/**
	 * Get a copy of a trait given subject's index and a line index.
	 * Statically typed version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex An index of the subject.
	 * @param LineIndex An index of the line.
	 * @return The copy of the trait.
	 */
	template < typename T >
	FORCEINLINE T
	TraitAt(const int32 SlotIndex,
			const int32 LineIndex) const
	{
		check(SlotIndex != InvalidSlotIndex);
		check(LineIndex != InvalidTraitLineIndex);
		return Lines[LineIndex].template ElementAt<T>(SlotIndex);
	}

	/**
	 * Copy a trait from the chunk's subject by its type identifier.
	 *
	 * @param[in] SlotIndex The index of the subject within the chunk.
	 * @param[in] TraitType The trait type identifier.
	 * @param[out] OutTraitData The trait data receiver.
	 * @param[in] bTraitDataInitialized Is the @p OutTraitData actually initialized?
	 * @return The status of the operation.
	 * @return EApparatusStatus::Missing If there's no such trait type in the chunk.
	 */
	EApparatusStatus
	TraitAt(const int32                SlotIndex,
			const UScriptStruct* const TraitType,
			void* const                OutTraitData,
			const bool                 bTraitDataInitialized = true) const;

	/**
	 * Get a copy of a trait given subject's index and a
	 * line index hint.
	 * 
	 * This is a low-level method without the remapping.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SubjectIndex The index of the subject.
	 * Must be a valid one.
	 * @param LineIndexHint The hinting index of the trait line.
	 * May be a UChunk::InvalidTraitLineIndex for automatic line search.
	 * @return The copy of the trait.
	 */
	template < typename T >
	FORCEINLINE T
	TraitAtHinted(const int32 SubjectIndex,
				  int32       LineIndexHint) const
	{
		if (UNLIKELY((LineIndexHint <= InvalidTraitLineIndex) || 
					 (LineIndexHint >= TraitLinesNum()) ||
					 (TraitLineTypeAt(LineIndexHint) != T::StaticStruct())))
		{
			LineIndexHint = Traitmark.template IndexOf<T>();
			checkf(LineIndexHint > InvalidTraitLineIndex,
				   TEXT("Trait was not found: %s"), *T::StaticStruct()->GetName());
		}
		return TraitAt<T>(SubjectIndex, LineIndexHint);
	}

	/**
	 * Copy a trait's data given a subject and line indices
	 * to a supplied data buffer.
	 *
	 * @param SubjectIndex The index of the subject which trait to copy.
	 * Must be a valid one.
	 * @param TraitType The type of the trait to get.
	 * @param LineIndexHint The hinting index of the trait line.
	 * May be a UChunk::InvalidTraitLineIndex for automatic line search.
	 * @param OutTraitData The output buffer to copy to.
	 * @param bTraitDataInitialized Is the trait data initialized or a raw buffer?
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	TraitAtHinted(const int32          SubjectIndex,
				  UScriptStruct* const TraitType,
				  int32                LineIndexHint,
				  void* const          OutTraitData,
				  bool                 bTraitDataInitialized = true) const
	{
		checkf(TraitType, TEXT("The type of the trait to get hinted is invalid."));
		if (UNLIKELY((LineIndexHint == InvalidTraitLineIndex) || 
					 (LineIndexHint >= TraitLinesNum()) ||
					 (TraitLineTypeAt(LineIndexHint) != TraitType)))
		{
			LineIndexHint = Traitmark.IndexOf(TraitType);
			if (UNLIKELY(LineIndexHint == InvalidTraitLineIndex))
			{
				return EApparatusStatus::Missing;
			}
		}
		return TraitAt(SubjectIndex, LineIndexHint, OutTraitData, bTraitDataInitialized);
	}

	/**
	 * Get a copy of a trait from the chunk's subject by its type identifier.
	 *
	 * @tparam T The type of the trait to copy.
	 *
	 * @return The copy of the trait.
	 */
	template < typename T >
	FORCEINLINE T
	TraitAt(const int32 SlotIndex) const
	{
		T TraitTemp;
		verify(OK(TraitAt(SlotIndex, T::StaticStruct(), static_cast<void*>(&TraitTemp))));
		return MoveTempIfPossible(TraitTemp);
	}

	/// @}
#pragma endregion Traits Access

#pragma region Traits Data Access
	/// @name Traits Data Access
	/// @{

	/**
	 * Get the trait data pointer given subject's index and a line index.
	 * Generic constant data version.
	 * 
	 * @param SlotIndex An index of the subject.
	 * @param LineIndex An index of the line.
	 * @return A read-only data of the trait.
	 */
	FORCEINLINE const void*
	TraitPtrAt(const int32 SlotIndex,
			   const int32 LineIndex) const
	{
		check(SlotIndex != InvalidSlotIndex);
		check(LineIndex != InvalidTraitLineIndex);
		return Lines[LineIndex][SlotIndex];
	}

	/**
	 * Get the trait data pointer given subject's index and a line index.
	 * Generic mutable data version.
	 * 
	 * @param SlotIndex An index of the subject slot.
	 * @param LineIndex An index of the line.
	 * @return The data of the trait.
	 */
	FORCEINLINE void*
	TraitPtrAt(const int32 SlotIndex,
			   const int32 LineIndex)
	{
		check(SlotIndex != InvalidSlotIndex);
		check(LineIndex != InvalidTraitLineIndex);
		return Lines[LineIndex][SlotIndex];
	}

/**
	 * Get the trait data pointer given subject's index and a line index.
	 * Generic mutable data version.
	 * 
	 * @param TraitTypeCheck The type to check.
	 * @param SlotIndex An index of the subject slot.
	 * @param LineIndex An index of the line.
	 * @return The data of the trait.
	 */
	FORCEINLINE void*
	TraitPtrAt(UScriptStruct* const TraitTypeCheck,
			   const int32          SlotIndex,
			   const int32          LineIndex)
	{
		check(TraitLineTypeAt(LineIndex) == TraitTypeCheck);
		check(SlotIndex != InvalidSlotIndex);
		check(LineIndex != InvalidTraitLineIndex);
		return Lines[LineIndex][SlotIndex];
	}

	/**
	 * Get a trait reference given subject's index and a line index.
	 * Statically typed constant version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex An index of the subject.
	 * @param LineIndex An index of the line.
	 * @return The reference for the read-only trait.
	 */
	template < typename T >
	FORCEINLINE const T&
	TraitRefAt(const int32 SlotIndex,
			   const int32 LineIndex) const
	{
		check(SlotIndex != InvalidSlotIndex);
		check(LineIndex != InvalidTraitLineIndex);
		return Lines[LineIndex].template RefAt<T>(SlotIndex);
	}

	/**
	 * Get the trait reference given subject's index and a line index.
	 * Statically typed mutable version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex An index of the subject.
	 * @param LineIndex An index of the line.
	 * @return The reference for the trait.
	 */
	template < typename T >
	FORCEINLINE T&
	TraitRefAt(const int32 SlotIndex,
			   const int32 LineIndex)
	{
		check(SlotIndex != InvalidSlotIndex);
		check(LineIndex != InvalidTraitLineIndex);
		return Lines[LineIndex].template RefAt<T>(SlotIndex);
	}

	/**
	 * Get the trait reference given subject's index and a line index hint.
	 * Constant version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex The index of the subject.
	 * @param LineIndexHint The hint for the index of the line.
	 * @return The reference for the trait.
	 */
	template < typename T >
	FORCEINLINE const T&
	TraitRefAtHinted(const int32 SlotIndex,
					 int32       LineIndexHint) const
	{
		if (UNLIKELY((LineIndexHint == InvalidTraitLineIndex) || 
					 (LineIndexHint >= TraitLinesNum()) ||
					 (TraitLineTypeAt(LineIndexHint) != T::StaticStruct())))
		{
			LineIndexHint = Traitmark.template IndexOf<T>();
			checkf(LineIndexHint != InvalidTraitLineIndex,
				   TEXT("A trait reference could not found: %s"), *(T::StaticStruct()->GetName()));
		}
		return TraitRefAt<T>(SlotIndex, LineIndexHint);
	}

	/**
	 * Get the trait reference given subject's index and a line index hint.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex The index of the subject.
	 * @param LineIndexHint The hint for the index of the line.
	 * @return The reference for the trait.
	 */
	template < typename T >
	FORCEINLINE T&
	TraitRefAtHinted(const int32 SlotIndex,
					 int32       LineIndexHint)
	{
		if (UNLIKELY((LineIndexHint == InvalidTraitLineIndex) || 
					 (LineIndexHint >= TraitLinesNum()) ||
					 (TraitLineTypeAt(LineIndexHint) != T::StaticStruct())))
		{
			LineIndexHint = Traitmark.template IndexOf<T>();
			checkf(LineIndexHint != InvalidTraitLineIndex,
				   TEXT("A trait reference could not be found: %s"), *(T::StaticStruct()->GetName()));
		}
		return TraitRefAt<T>(SlotIndex, LineIndexHint);
	}

	/**
	 * Get the read-only trait data pointer from the chunk's subject by its type identifier.
	 * Dynamically-typed constant data version.
	 *
	 * @param SlotIndex The index of the subject within the chunk.
	 * @param TraitType The trait type identifier.	 *
	 * @return A pointer to the read-only trait data.
	 * @return nullptr If there's no such trait in the chunk.
	 */
	const void*
	TraitPtrAt(const int32                SlotIndex,
			   const UScriptStruct* const TraitType) const
	{
		check((SlotIndex != FSubjectInfo::InvalidSlotIndex) &&
			  (SlotIndex < Slots.Num()));
		if (UNLIKELY(TraitType == nullptr))
		{
			return nullptr;
		}
		for (int32 LineIndex = 0; LineIndex < TraitLinesNum(); ++LineIndex)
		{
			if (TraitLineTypeAt(LineIndex) == TraitType)
			{
				return TraitPtrAt(SlotIndex, LineIndex);
			}
		}
		return nullptr;
	}

	/**
	 * Get the trait data pointer from the chunk's subject by its type identifier.
	 * Dynamically-typed mutable data version.
	 *
	 * @param SlotIndex The index of the subject within the chunk.
	 * @param TraitType The trait type identifier.
	 * @return A pointer to the read-only trait data.
	 * @return nullptr If there's no such trait in the chunk.
	 */
	void*
	TraitPtrAt(const int32          SlotIndex,
			   UScriptStruct* const TraitType)
	{
		check((SlotIndex != FSubjectInfo::InvalidSlotIndex) &&
			  (SlotIndex < Slots.Num()));
		if (UNLIKELY(TraitType == nullptr))
		{
			return nullptr;
		}
		for (int32 LineIndex = 0; LineIndex < TraitLinesNum(); ++LineIndex)
		{
			if (TraitLineTypeAt(LineIndex) == TraitType)
			{
				return TraitPtrAt(SlotIndex, LineIndex);
			}
		}
		return nullptr;
	}

	/**
	 * Get the trait data pointer from the chunk's subject by its type identifier.
	 * Templated constant version.
	 *
	 * @tparam T The trait type identifier.
	 * @param SlotIndex The index of the subject within the chunk.
	 * @return A pointer to the trait data.
	 */
	template < typename T >
	FORCEINLINE const T*
	TraitPtrAt(const int32 SlotIndex) const
	{
		return static_cast<const T*>(TraitPtrAt(SlotIndex, T::StaticStruct()));
	}

	/**
	 * Get the trait data pointer from the chunk's subject by its type identifier.
	 * Templated version.
	 *
	 * @tparam T The trait type identifier.
	 * @param SlotIndex The index of the subject within the chunk.
	 * @return A pointer to the trait data.
	 */
	template < typename T >
	FORCEINLINE T*
	TraitPtrAt(const int32 SlotIndex)
	{
		return static_cast<T*>(TraitPtrAt(SlotIndex, T::StaticStruct()));
	}

	/**
	 * Get a read-only trait reference from the chunk's subject by its type identifier.
	 * Statically-typed constant version.
	 *
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex The index of the subject within the chunk.
	 * @return A reference for the read-only trait.
	 */
	template < typename T >
	FORCEINLINE const T&
	TraitRefAt(const int32 SlotIndex) const
	{
		return *TraitPtrAt<T>(SlotIndex);
	}

	/**
	 * Get a trait reference from the chunk's subject by its type identifier.
	 * Statically-typed mutable version.
	 *
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex The index of the subject within the chunk.
	 * @return A reference for the trait.
	 */
	template < typename T >
	FORCEINLINE T&
	TraitRefAt(const int32 SlotIndex)
	{
		return *TraitPtrAt<T>(SlotIndex);
	}

	/**
	 * Get the trait pointer given subject's index and a line index hint.
	 * Constant version.
	 * 
	 * @param SlotIndex The index of the subject.
	 * @param TraitType The type of the trait to get.
	 * @param LineIndexHint The hint for the index of the line.
	 * @return The pointer to the trait data.
	 */
	FORCEINLINE const void*
	TraitPtrAtHinted(const int32          SlotIndex,
					 UScriptStruct* const TraitType,
					 int32                LineIndexHint) const
	{
		if (UNLIKELY(!TraitType))
		{
			return nullptr;
		}
		if (UNLIKELY((LineIndexHint == InvalidTraitLineIndex) || 
					 (LineIndexHint >= TraitLinesNum()) ||
					 (TraitLineTypeAt(LineIndexHint) != TraitType)))
		{
			LineIndexHint = Traitmark.IndexOf(TraitType);
			if (UNLIKELY(LineIndexHint == InvalidTraitLineIndex))
			{
				return nullptr;
			}
		}
		return TraitPtrAt(SlotIndex, LineIndexHint);
	}

	/**
	 * Get the trait pointer given subject's index and a line index hint.
	 * 
	 * @param SlotIndex The index of the subject.
	 * @param TraitType The type of the trait to get.
	 * @param LineIndexHint The hint for the index of the line.
	 * @return The pointer to the trait data.
	 */
	FORCEINLINE void*
	TraitPtrAtHinted(const int32          SlotIndex,
					 UScriptStruct* const TraitType,
					 int32                LineIndexHint)
	{
		if (UNLIKELY(!TraitType))
		{
			return nullptr;
		}
		if (UNLIKELY((LineIndexHint == InvalidTraitLineIndex) || 
					 (LineIndexHint >= TraitLinesNum()) ||
					 (TraitLineTypeAt(LineIndexHint) != TraitType)))
		{
			LineIndexHint = Traitmark.IndexOf(TraitType);
			if (UNLIKELY(LineIndexHint == InvalidTraitLineIndex))
			{
				return nullptr;
			}
		}
		return TraitPtrAt(SlotIndex, LineIndexHint);
	}

	/**
	 * Get the trait pointer given subject's index and a line index hint.
	 * Constant templated version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex The index of the subject.
	 * @param LineIndexHint The hint for the index of the line.
	 * @return The pointer to the trait.
	 */
	template < typename T >
	FORCEINLINE const T*
	TraitPtrAtHinted(const int32 SlotIndex,
					 int32       LineIndexHint) const
	{
		return static_cast<const T*>(TraitPtrAtHinted(SlotIndex, T::StaticStruct(), LineIndexHint));
	}

	/**
	 * Get the trait pointer given subject's index and a line index hint.
	 * Templated version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex The index of the subject.
	 * @param LineIndexHint The hint for the index of the line.
	 * @return The pointer to the trait.
	 */
	template < typename T >
	FORCEINLINE T*
	TraitPtrAtHinted(const int32 SlotIndex,
					 int32       LineIndexHint)
	{
		return static_cast<T*>(TraitPtrAtHinted(SlotIndex, T::StaticStruct(), LineIndexHint));
	}

	/// @}
#pragma endregion Traits Data Access

	/**
	 * Set the trait for the chunk's subject by its type identifier.
	 *
	 * @param SlotIndex The index of the subject within the chunk.
	 * @param TraitType The type of the trait to set.
	 * @param InTraitData The trait data to set.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	SetTraitAt(const int32                SlotIndex,
			   const UScriptStruct* const TraitType,
			   const void* const          InTraitData);

	/**
	 * Fetch the traits for a subject into a traits data array.
	 * 
	 * @param SlotIndex The index of the subject within the belt.
	 * @param Mapping The mapping to use while fetching.
	 * @param OutTraits Initialized traits data to fill.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	FetchTraitsPtrs(const int32          SlotIndex,
					const TArray<int32>& Mapping,
					void**               OutTraits);

	/**
	 * Copy the traits from this chunk to another one, given subject indices.
	 * Constant version.
	 * 
	 * If the trait is missing within the destination chunk it is safely skipped.
	 * @note The constant version doesn't support copying to itself.
	 * 
	 * @param SrcSlotIndex The subject's index within the source chunk.
	 * @param DstChunk The destination chunk to copy to.
	 * @param DstSlotIndex The subject's index within the destination chunk.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	OverwriteTraits(const int32         SrcSlotIndex,
					class UChunk* const DstChunk,
					const int32         DstSlotIndex) const;

	/**
	 * Copy the traits from this chunk to another one, given subject indices.
	 * 
	 * If the trait is missing within the destination chunk it is safely skipped.
	 * 
	 * @param SrcSlotIndex The subject's index within the source chunk.
	 * @param DstChunk The destination chunk to copy to. May the same as the source (this).
	 * @param DstSlotIndex The subject's index within the destination chunk.
	 * @return The status of the operation.
	 */
	EApparatusStatus
	OverwriteTraits(const int32         SrcSlotIndex,
					class UChunk* const DstChunk,
					const int32         DstSlotIndex);

	/**
	 * Swap the traits from this chunk with another one, given subject indices.
	 * 
	 * If the trait is missing within the destination chunk it is safely skipped.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param SlotIndex The subject's index within the source chunk.
	 * @param OtherChunk The destination chunk to copy to. May the same as the source (this).
	 * @param OtherSlotIndex The subject's index within the destination chunk.
	 * @param bClearMissing Clear the original traits missing within the other chunk.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::DefaultInternal >
	TOutcome<Paradigm>
	SwapTraits(const int32   SlotIndex,
			   class UChunk* OtherChunk,
			   const int32   OtherSlotIndex,
			   const bool    bClearMissing = true);

	/**
	 * Copy the traits within the chunk, given subject indices.
	 * 
	 * @param SrcSlotIndex The subject's index to copy from.
	 * @param DstSlotIndex The subject's index to copy to.
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	OverwriteTraits(const int32 SrcSlotIndex,
					const int32 DstSlotIndex)
	{
		return OverwriteTraits(SrcSlotIndex, this, DstSlotIndex);
	}

	/**
	 * Check whether the chunk matches a filter.
	 * 
	 * @param Filter A filter to match.
	 * @return The state of matching.
	 */
	FORCEINLINE bool
	Matches(const FFilter& Filter) const override
	{
		return Traitmark.Matches(Filter);
	}

	static FORCEINLINE bool
	IsValidChunk(const UChunk* const InChunk)
	{
		return InChunk != nullptr;
	}

	/**
	 * Check the validity of the chunk.
	 */
	bool
	Check() const;

#pragma region Conversion

	/**
	 * Get a textural representation of the chunk.
	 * 
	 * @return The textual representation of the chunk.
	 */
	FString
	ToString() const
	{
		return TEXT("Chunk [") + Traitmark.ToString() + TEXT("](") + FString::FromInt(Count) + TEXT(")");
	}

#pragma endregion Conversion

}; //-class UChunk

inline EApparatusStatus
UChunk::TraitAt(const int32                SubjectIndex,
				const UScriptStruct* const TraitType,
				void* const                OutTraitData,
				const bool                 bTraitDataInitialized) const
{
	check((SubjectIndex > FSubjectInfo::InvalidSlotIndex) &&
		  (SubjectIndex < Slots.Num()));
	check(TraitType != nullptr);
	check(OutTraitData != nullptr);

	for (int32 LineIndex = 0; LineIndex < TraitLinesNum(); ++LineIndex)
	{
		if (TraitType == TraitLineTypeAt(LineIndex))
		{
			Lines[LineIndex].ElementAt(SubjectIndex, OutTraitData, bTraitDataInitialized);
			return EApparatusStatus::Success;
		}
	}
	return EApparatusStatus::Missing;
}

inline EApparatusStatus
UChunk::SetTraitAt(const int32                SubjectIndex,
				   const UScriptStruct* const TraitType,
				   const void* const          InTraitData)
{
	check((SubjectIndex > FSubjectInfo::InvalidSlotIndex) &&
		  (SubjectIndex < Slots.Num()));
	check(TraitType != nullptr);
	check(InTraitData != nullptr);

	for (int32 ti = 0; ti < TraitLinesNum(); ++ti)
	{
		if (TraitType == TraitLineTypeAt(ti))
		{
			return Lines[ti].SetElementAt(SubjectIndex, InTraitData);
		}
	}

	return EApparatusStatus::Missing;
}

FORCEINLINE void
UChunk::Reserve(const int32 InCapacity)
{
	Slots.Reserve(InCapacity);
	for (int32 i = 0; i < Lines.Num(); ++i)
	{
		Lines[i].Reserve(InCapacity);
	}
}

inline EApparatusStatus
FMechanicInfo::AddChunk(class UChunk* Chunk)
{
	check(Chunk);

	FMechanicChunkEntry& ChunkEntry =  Chunks.AddDefaulted_GetRef();
	ChunkEntry.Chunk = Chunk;
	Filter.GetFingerprint().FindTraitsMappingTo(Chunk->Traitmark, ChunkEntry.Lines);

	return EApparatusStatus::Success;
}

inline EApparatusStatus
UChunk::AddAdjective(struct FAdjectiveInfo* Adjective)
{
	check(Adjective);

	struct FChunkAdjectiveEntry& AdjectiveEntry = Adjectives.AddDefaulted_GetRef();

	AdjectiveEntry.Id      = Adjective->Id;
	AdjectiveEntry.Handler = Adjective->Handler;

	Adjective->Filter.GetFingerprint().FindTraitsMappingTo(Traitmark, AdjectiveEntry.Lines);

	APPARATUS_REPORT_SUCCESS(TEXT("Successfully added adjective #%d to chunk #%d."),
							 Adjective->Id, Id);
	return EApparatusStatus::Success;
}

#undef LOCTEXT_NAMESPACE

#if CPP && !defined(SKIP_MACHINE_H)
#include "Machine.h"
#endif
