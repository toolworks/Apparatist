/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ChunkIt.h
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

#include "More/type_traits"

#include "CoreMinimal.h"
#include "UObject/Class.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define CHUNK_IT_H_SKIPPED_MACHINE_H
#endif

#include "Fingerprint.h"
#include "SubjectInfo.h"
#include "Chunk.h"

#ifdef CHUNK_IT_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif


// Forward declarations:
class ISubjective;
class UDetail;
class UChunk;

/**
 * A generic chunk's iterator base.
 * 
 * Can be a solid or an ordinary mutable iterator.
 * 
 * @tparam StructHandleT The type of the subject handle.
 */
template < typename SubjectHandleT >
struct TChunkIt final
{

  public:

	/**
	 * The type of the subject handle used.
	 */
	typedef SubjectHandleT SubjectHandleType;

	/**
	 * The compatible type of the subjective used.
	 */
	typedef typename SubjectHandleType::SubjectivePtrType SubjectivePtrType;

	/**
	 * The solidity state of the chunk iterator.
	 */
	static constexpr bool IsSolid = SubjectHandleType::IsHandleSolid();

	/**
	 * Is the subject of a mutable semantics.
	 */
	static constexpr bool AllowsChanges = SubjectHandleType::AllowsChanges;

	/**
	 * Are structural changes (adding/removing components) allowed?
	 */
	static constexpr bool AllowsStructuralChanges = SubjectHandleType::AllowsStructuralChanges;

	/**
	 * The direct trait data access status.
	 */
	static constexpr bool AllowsDirectTraitAccess = SubjectHandleType::AllowsDirectTraitAccess;

	/**
	 * Invalid subject index.
	 */
	static constexpr auto InvalidSlotIndex = FSubjectInfo::InvalidSlotIndex;

	/**
	 * Invalid trait index.
	 */
	static constexpr auto InvalidTraitIndex = UChunk::InvalidTraitLineIndex;

  private:

	friend class UChunk;

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	/**
	 * Protect the type of a trait void pointer returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 */
	template < EParadigm Paradigm >
	using TTraitVoidPtrResultSecurity = typename SubjectHandleT::template TTraitVoidPtrResultSecurity<Paradigm>;

	/**
	 * The type of a trait void pointer returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 */
	template < EParadigm Paradigm >
	using TTraitVoidPtrResult = typename SubjectHandleT::template TTraitVoidPtrResult<Paradigm>;

	/**
	 * Protect the type of a trait pointer returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitPtrResultSecurity = typename SubjectHandleT::template TTraitPtrResultSecurity<Paradigm, T>;

	/**
	 * The type of a trait pointer returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitPtrResult = typename SubjectHandleT::template TTraitPtrResult<Paradigm, T>;

	/**
	 * Protect the type of a trait reference returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitRefResultSecurity = typename SubjectHandleT::template TTraitRefResultSecurity<Paradigm, T>;

	/**
	 * The type of a trait reference returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitRefResult = typename SubjectHandleT::template TTraitRefResult<Paradigm, T>;

	/**
	 * The class of a detail pointer returned by the methods.
	 * 
	 * @tparam D The class of the detail.
	 */
	template < class D >
	using TDetailPtrResultSecurity = typename SubjectHandleT::template TDetailPtrResultSecurity<D>;

	/**
	 * The class of a detail pointer returned by the methods.
	 * 
	 * @tparam D The class of the detail.
	 */
	template < class D >
	using TDetailPtrResult = typename SubjectHandleT::template TDetailPtrResult<D>;

	/**
	 * The chunk this iterator relates to currently.
	 * 
	 * Set during the construction.
	 */
	UChunk* Chunk = nullptr;

	/**
	 * The filter used for the iterating.
	 * 
	 * This is set in the constructor
	 * and may actually include
	 * additional flags/exclusions.
	 */
	FFilter Filter;

	/**
	 * The index of the current subject
	 * this iterator points to within the chunk.
	 */
	int32 SlotIndex = InvalidSlotIndex;

	/**
	 * Do unlock the chunk (if is currently locked).
	 */
	FORCEINLINE void
	Unlock()
	{
		if (LIKELY(Chunk && (SlotIndex > InvalidSlotIndex)))
		{
			Chunk->Unlock<IsSolid>();
			SlotIndex = InvalidSlotIndex;
		}
	}

	/**
	 * Prepare the iteration, skip the following invalid iterations for the iterator.
	 * 
	 * @return Returns @c true, if the iteration was prepared and is valid.
	 */
	bool
	PrepareIteration()
	{
		check(Chunk);
		check(Chunk->IsLocked());
		check(SlotIndex > InvalidSlotIndex);
		check(SlotIndex < Chunk->IterableCount);

		if (UNLIKELY(Filter.ExcludedDetailsNum()))
		{
			// Excluding details should be matched iteration-time.
			do
			{
				const auto& Slot        = Chunk->Slots[SlotIndex];
				const auto& Fingerprint = Slot.GetFingerprint();
				// Match both the flagmark and the details here, since the excluded
				// details are present...
				if (LIKELY(Fingerprint.FlagmarkMatches(Filter) &&
						   Fingerprint.DetailsMatch(Filter)))
				{
					return true;
				}
				if (UNLIKELY(SlotIndex >= (Chunk->IterableCount - 1))) // Overflow protection
				{
					// No more subjects available:
					SlotIndex = InvalidSlotIndex;
					Chunk->Unlock<IsSolid>();
					return false;
				}
				SlotIndex += 1;
			}
			while (true);
		}
		else
		{
			// No excluded details. Match only the flagmarks.
			do
			{
				const auto& Slot        = Chunk->Slots[SlotIndex];
				const auto& Fingerprint = Slot.GetFingerprint();
				// Only the flagmark part should be matched here,
				// since the chunk itself is matched during the construction...
				if (LIKELY(Fingerprint.FlagmarkMatches(Filter)))
				{
					return true;
				}
				if (UNLIKELY(SlotIndex >= (Chunk->IterableCount - 1))) // Overflow protection
				{
					// No more subjects available:
					SlotIndex = InvalidSlotIndex;
					Chunk->Unlock<IsSolid>();
					return false;
				}
				SlotIndex += 1;
			}
			while (true);
		}

		checkf(false, TEXT("Failed to return a result while preparing a chunk iteration. This should never happen."));
		return false;
	}

	/**
	 * Initialize an ending chunk iterator for a chunk.
	 */
	FORCEINLINE
	TChunkIt(const UChunk* const InChunk)
	  : Chunk(const_cast<UChunk*>(InChunk)) // Guaranteed to not access any mutable methods.
	{
		checkf(InChunk, TEXT("A valid chunk must be provided in order to initialize the end iterator."));
	}

	/**
	 * Initialize a chunk iterator for a chunk with a filter.
	 *
	 * @param InChunk The chunk to iterate.
	 * @param InFilter The filter to iterate under.
	 * @param InSlotIndex The index of the starting slot
	 * to begin iterating with.
	 */
	TChunkIt(UChunk* const  InChunk,
			 const FFilter& InFilter,
			 const int32    InSlotIndex = InvalidSlotIndex)
	  : Chunk(InChunk)
	  , Filter(InFilter)
	  , SlotIndex(InSlotIndex)
	{
		checkf(InChunk, TEXT("A valid chunk must be provided in order to initialize the iterator."));
		checkf(InChunk->GetTraitmark().Matches(InFilter),
			   TEXT("The '%s' chunk's traitmark must match the filter in order to be iterated under it."),
			   *InChunk->GetName());
		checkf(InFilter.GetFingerprint().DetailsNum() == 0,
			   TEXT("Iterating chunks with details filtering is lossy and is strictly prohibited."));

		// Set mandatory flags.
		Filter.Exclude(FM_Stale | FM_DeferredDespawn); // Stale slots should always be excluded.
#if WITH_EDITOR
		if (LIKELY(!Filter.Includes(EFlagmarkBit::Editor))) // Editor entities can be included explicitly.
		{
			const auto World = Chunk->GetWorld();
			if (World && World->IsGameWorld())
			{
				// Exclude the in-editor subjects during the gameplay iterating.
				Filter.Exclude(EFlagmarkBit::Editor);
			}
		}
#endif

		if (LIKELY(SlotIndex > InvalidSlotIndex))
		{
			VerifyOK(Chunk->template Lock<IsSolid>());

			// Make sure the iterator points to a valid, non-skipped subject:
			if (UNLIKELY(!PrepareIteration()))
			{
				APPARATUS_REPORT_SUCCESS(TEXT("No valid places available to begin iterating the '%s' chunk."),
										 *Chunk->GetName());
				return;
			}

			check(SlotIndex > InvalidSlotIndex);
			check(Chunk->Slots[SlotIndex].template GetHandle<SubjectHandleT>());
			APPARATUS_REPORT_SUCCESS(TEXT("Began iterating the '%s' chunk."),
									 *Chunk->GetName());
		}
	}

  public:

	/**
	 * Check if the iterator is currently locking its chunk.
	 */
	FORCEINLINE bool
	IsLocking() const
	{
		return Chunk && (SlotIndex > InvalidSlotIndex);
	}

	/**
	 * Get the chunk of the iterator.
	 * 
	 * When an iterator comes to an end of a chunk
	 * it still persists this state value, unless cleared
	 * with Reset().
	 * 
	 * @return The chunk of the iterator (if any).
	 */
	FORCEINLINE UChunk*
	GetChunk() const 
	{
		return Chunk;
	}

	/**
	 * Get the filter of the iterator.
	 */
	FORCEINLINE const FFilter&
	GetFilter() const
	{
		return Filter;
	}

	/**
	 * Get the current chunk's slot index.
	 * 
	 * @return The current iterated slot index.
	 */
	FORCEINLINE int32
	GetSlotIndex() const
	{
		return SlotIndex;
	}

	/**
	 * Get the real working chunk
	 * of the current iteration.
	 * 
	 * If the current subject is removed
	 * from the iterated chunk, its
	 * actual present chunk is returned.
	 * 
	 * If the subject is despawned completely
	 * the current chunk is removed for safety.
	 */
	FORCEINLINE UChunk*
	GetIterationChunk() const
	{
		check(Chunk);
		const auto& Slot = Chunk->Slots[SlotIndex];
		if (UNLIKELY(Slot.IsStale()))
		{
			// The subject was removed from the chunk.
			// Get the data from the actual place (if any)...
			const auto Info = Slot.FindInfo();
			if (LIKELY(Info))
			{
				return Info->GetChunk();
			}
		}
		return Chunk;
	}

	/**
	 * Get the subject from the chunk iterator.
	 *
	 * Get the active subject that is currently pointed by the chunk iterator.
	 *
	 * @param OutSubject The subject handle receiver.
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	GetSubject(SubjectHandleT& OutSubject) const
	{
		check(Chunk && Chunk->IsLocked());
		if (UNLIKELY((SlotIndex <= InvalidSlotIndex) ||
					 (SlotIndex >= Chunk->IterableCount)))
		{
			return Report<EApparatusStatus::InvalidState>(
				TEXT("The chunk iterator is not valid to get a subject from. Is it at the end?"));
		}

		OutSubject = (SubjectHandleT)Chunk->Slots[SlotIndex].template GetHandle<SubjectHandleT>();
		return EApparatusStatus::Success;
	}

	/**
	 * Get a subject from the chunk iterator.
	 *
	 * Get a subject that is currently pointed by the chunk iterator.
	 *
	 * @return The subject handle of the current iteration.
	 */
	FORCEINLINE SubjectHandleT
	GetSubject() const
	{
		SubjectHandleT Subject;
		verify(OK(GetSubject(Subject)));
		return MoveTemp(Subject);
	}

	/**
	 * Get the subject fingerprint from the chunk iterator.
	 *
	 * Gets the fingerprint of the subject that is currently pointed
	 * by the chunk iterator.
	 *
	 * @return The fingerprint of the current subject.
	 */
	const FFingerprint&
	GetFingerprint() const
	{
		check(Chunk);
		checkf(Chunk->IsLocked(),
			   TEXT("The iterator's chunk is not locked. "
					"Is the iterator used outside of its iteration loop?"));
		check(SlotIndex > InvalidSlotIndex);
		check(SlotIndex < Chunk->IterableCount);
		
		const auto& Slot = Chunk->Slots[SlotIndex];
		if (UNLIKELY(Slot.IsStale()))
		{
			// Get the actual (moved subject) fingerprint.
			Slot.template GetHandle<SubjectHandleT>().GetFingerprint();
		}
		return Slot.GetFingerprint();
	}

	/**
	 * Get the currently iterated subjective object (if available).
	 */
	FORCEINLINE UObject*
	GetSubjectiveObject() const
	{
		check(IsViable());
		return GetSubject().GetSubjectiveObject();
	}

	/**
	 * Get a subjective from the chunk iterator (if any).
	 *
	 * Get a subjective that is currently pointed by the chunk iterator
	 * if there is any associated with a subject.
	 *
	 * @return The subjective of the current iteration.
	 * Returns @c nullptr, if there is none.
	 */
	FORCEINLINE SubjectivePtrType
	GetSubjective() const
	{
		check(IsViable());
		return GetSubject().GetSubjective();
	}

#pragma region Traits Data Access
	/// @name Traits Data Access
	/// @{

	/**
	 * Get a trait of a certain type from a currently iterated subject.
	 * 
	 * Can be used during the solid iterating only.
	 * 
	 * @tparam Paradigm The safety paradigm to use.
	 * @param TraitType The type of the trait to get.
	 * @return A pointer to the trait data.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   TTraitVoidPtrResultSecurity<Paradigm> = 0 >
	FORCEINLINE TTraitVoidPtrResult<Paradigm>
	GetTraitPtr(UScriptStruct* const TraitType) const
	{
		if (AvoidFormat(Paradigm, TraitType == nullptr, TEXT("The valid trait type must be provided.")))
		{
			return EApparatusStatus::NullArgument;
		}
		if (AvoidFormat(Paradigm, !IsViable(), TEXT("The chunk iterator is not valid to get a trait pointer from. Is it at the end?")))
		{
			return MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::InvalidState, nullptr);
		}
		if (AvoidFormat(Paradigm, !Chunk->IsLocked(), TEXT("The iterator's chunk is not locked. Is the iterator used outside of its iteration loop?")))
		{
			return MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::InvalidState, nullptr);
		}

		const auto& Slot = Chunk->Slots[SlotIndex];
		if (UNLIKELY(Slot.IsStale()))
		{
			// The subject was moved from the chunk.
			// Get the data from its actual present place...
			return Slot.template GetHandle<SubjectHandleT>().template GetTraitPtr<Paradigm>(TraitType);
		}

		return Chunk->TraitPtrAt(SlotIndex, TraitType);
	}

	/**
	 * Get a pointer to a trait of a certain type from a currently iterated subject.
	 * Templated version.
	 * 
	 * @tparam Paradigm The safety paradigm to use.
	 * @tparam T The type of the trait to get.
	 * @return A pointer to the trait.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitPtrResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtr() const
	{
		if (AvoidFormat(Paradigm, !IsViable(), TEXT("The chunk iterator is not valid to get a trait pointer from. Is it at the end?")))
		{
			return MakeOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>(EApparatusStatus::InvalidState, nullptr);
		}
		if (AvoidFormat(Paradigm, !Chunk->IsLocked(), TEXT("The iterator's chunk is not locked. Is the iterator used outside of its iteration loop?")))
		{
			return MakeOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>(EApparatusStatus::InvalidState, nullptr);
		}

		const auto& Slot = Chunk->Slots[SlotIndex];
		if (UNLIKELY(Slot.IsStale()))
		{
			// The subject was moved from the chunk.
			// Get the data from its actual present place...
			return Slot.template GetHandle<SubjectHandleT>().template GetTraitPtr<Paradigm, T>();
		}

		return Chunk->template TraitPtrAt<T>(SlotIndex);
	}

	/**
	 * Get a pointer to a trait of a certain type from a currently iterated subject.
	 * Templated version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The safety paradigm to use.
	 * @return A pointer to the trait.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default,
			   TTraitPtrResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtr() const
	{
		return GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Get a trait reference of a certain type
	 * from a currently iterated subject.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait to get.
	 * @return A reference for the trait.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitRefResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRef() const
	{
		return *GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Get a trait reference of a certain type
	 * from a currently iterated subject.
	 * 
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The access safety paradigm.
	 * @return A reference for the trait.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default,
			   TTraitRefResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRef() const
	{
		return GetTraitRef<Paradigm, T>();
	}

	/**
	 * Get a trait pointer at a line index from a currently iterated subject.
	 * 
	 * Gets a trait from a currently iterated subject
	 * by its index, relative to the current chunk's traitmark.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T the typ of the trait to get.
	 * @param TraitLineIndex The index of the trait line.
	 * @return A pointer to the trait data.
	 */
	template < EParadigm Paradigm = EParadigm::Default, typename T >
	FORCEINLINE TOutcome<Paradigm, T>
	TraitAtLine(const int32 TraitLineIndex) const
	{
		check(TraitLineIndex >= 0);
		if (AvoidFormat(Paradigm, !IsViable(), TEXT("The chunk iterator is not valid to get a trait pointer with hinting from. Is it at the end?")))
		{
			return MakeOutcome<Paradigm, T>(EApparatusStatus::InvalidState, T());
		}
		if (AvoidFormat(Paradigm, !Chunk->IsLocked(), TEXT("The iterator's chunk is not locked. Is the iterator used outside of its iteration loop?")))
		{
			return MakeOutcome<Paradigm, T>(EApparatusStatus::InvalidState, T());
		}

		const auto& Slot = Chunk->Slots[SlotIndex];
		if (UNLIKELY(Slot.IsStale()))
		{
			// The subject was moved from the chunk or the hint is wrong.
			// Get the data from its actual place (no reason for index-hinting here)...
			return Slot.template GetHandle<SubjectHandleT>().template GetTrait<Paradigm, T>();
		}

		return MakeOutcome<Paradigm, T>(EApparatusStatus::Success, *((T*)(Chunk->TraitPtrAt(T::StaticStruct(), SlotIndex, TraitLineIndex))));
	}

	/**
	 * Get a trait pointer at a line index from a currently iterated subject.
	 * 
	 * Gets a trait from a currently iterated subject
	 * by its index, relative to the current chunk's traitmark.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @param TraitType The type of the trait to get.
	 * @param TraitLineIndex The index of the trait line.
	 * @return A pointer to the trait data.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   TTraitVoidPtrResultSecurity<Paradigm> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>
	TraitPtrAtLine(UScriptStruct* TraitType, const int32 TraitLineIndex) const
	{
		check(TraitLineIndex >= 0);
		if (AvoidFormat(Paradigm, TraitType == nullptr, TEXT("A valid trait type must be provided.")))
		{
			return MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::InvalidArgument, nullptr);
		}
		if (AvoidFormat(Paradigm, !IsViable(), TEXT("The chunk iterator is not valid to get a trait pointer with hinting from. Is it at the end?")))
		{
			return MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::InvalidState, nullptr);
		}
		if (AvoidFormat(Paradigm, !Chunk->IsLocked(), TEXT("The iterator's chunk is not locked. Is the iterator used outside of its iteration loop?")))
		{
			return MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::InvalidState, nullptr);
		}

		const auto& Slot = Chunk->Slots[SlotIndex];
		if (UNLIKELY(Slot.IsStale()))
		{
			// The subject was moved from the chunk or the hint is wrong.
			// Get the data from its actual place (no reason for index-hinting here)...
			return Slot.template GetHandle<SubjectHandleT>().template GetTraitPtr<Paradigm>(TraitType);
		}

		return Chunk->TraitPtrAt(TraitType, SlotIndex, TraitLineIndex);
	}

	/**
	 * Get a trait pointer at a line index from a currently iterated subject.
	 * Templated version.
	 * 
	 * Gets a trait from a currently iterated subject
	 * by its index, relative to the current chunk's traitmark.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait to get.
	 * @param TraitLineIndex The index of the trait line to get from.
	 * @return A pointer to the trait data.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitPtrResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	TraitPtrAtLine(const int32 TraitLineIndex) const
	{
		return OutcomeStaticCast<TTraitPtrResult<Paradigm, T>>(TraitPtrAtLine<Paradigm>(T::StaticStruct(), TraitLineIndex));
	}

	/**
	 * Get a trait reference at a line index from a currently iterated subject.
	 * Templated version.
	 * 
	 * Gets a trait from a currently iterated subject
	 * by its index, relative to the current chunk's traitmark.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait to get.
	 * @param TraitLineIndex The index of the trait line to get from.
	 * @return A pointer to the trait data.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitRefResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	TraitRefAtLine(const int32 TraitLineIndex) const
	{
		return OutcomeDereference(TraitPtrAtLine<Paradigm, T>(TraitLineIndex));
	}

	/// @}
#pragma endregion Traits Data Access

#pragma region Traits Getting
	/// @name Traits Getting
	/// @{

	/**
	 * Get a trait of a certain type from a currently iterated subject.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] TraitType The type of the trait to get.
	 * @param[out] OutTraitData The trait data receiver.
	 * @param[in] bTraitDataInitialized Is the @p OutTraitData buffer actually initialized?
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	GetTrait(UScriptStruct* const TraitType,
			 void* const          OutTraitData,
			 const bool           bTraitDataInitialized = true) const
	{
		if (AvoidFormat(Paradigm, TraitType == nullptr, TEXT("The valid trait type must be provided.")))
		{
			return EApparatusStatus::NullArgument;
		}
		if (AvoidFormat(Paradigm, !IsViable(), TEXT("The chunk iterator is not valid to get a trait copy from. Is it at the end?")))
		{
			return MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::InvalidState, nullptr);
		}
		if (AvoidFormat(Paradigm, !Chunk->IsLocked(), TEXT("The iterator's chunk is not locked. Is the iterator used outside of its iteration loop?")))
		{
			return MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::InvalidState, nullptr);
		}

		auto& Slot = Chunk->Slots[SlotIndex];
		if (UNLIKELY(Slot.IsStale()))
		{
			// The subject was moved from the chunk.
			// Get the data from its actual place...
			return Slot.template GetHandle<SubjectHandleT>().template GetTrait<Paradigm>(TraitType, OutTraitData, bTraitDataInitialized);
		}
		return Chunk->TraitAt(SlotIndex, TraitType, OutTraitData, bTraitDataInitialized);
	}

	/**
	 * Get a copy of a trait of a certain type from a currently iterated subject.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A copy of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm, T>
	GetTrait() const
	{
		if (AvoidFormat(Paradigm, !IsViable(), TEXT("The chunk iterator is not valid to get a trait from. Is it at the end?")))
		{
			return MakeOutcome<Paradigm, T>(EApparatusStatus::InvalidState, T());
		}
		if (AvoidFormat(Paradigm, !Chunk->IsLocked(), TEXT("The iterator's chunk is not locked. Is the iterator used outside of its iteration loop?")))
		{
			return MakeOutcome<Paradigm, T>(EApparatusStatus::InvalidState, T());
		}

		const auto& Slot = Chunk->Slots[SlotIndex];
		if (UNLIKELY(Slot.IsStale()))
		{
			// The subject was moved from the chunk.
			// Get the data from its actual place...
			return Slot.template GetHandle<SubjectHandleT>().template GetTrait<Paradigm, T>();
		}

		return Chunk->template TraitAt<T>(SlotIndex);
	}

	/**
	 * Get a copy of a trait of a certain type from a currently iterated subject.
	 * 
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @return A copy of the trait.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	GetTrait() const
	{
		return GetTrait<Paradigm, T>();
	}

	/// @}
#pragma endregion Traits Getting

#pragma region Details Getting
	/// @name Details Getting
	/// @{

	/**
	 * Get a detail of a certain class from a currently iterated subject.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass The class of the detail to get.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   TDetailPtrResultSecurity<UDetail> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<UDetail>>
	GetDetail(const TSubclassOf<UDetail> DetailClass) const
	{
		return GetSubjective()->template GetDetail<Paradigm>(DetailClass);
	}

	/**
	 * Get a detail of a certain class from a currently iterated subject.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The class of the detail to get.
	 */
	template < EParadigm Paradigm, class D,
			   TDetailPtrResultSecurity<D> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<D>>
	GetDetail() const
	{
		return GetSubjective()->template GetDetail<Paradigm, D>();
	}

	/**
	 * Get a detail of a certain class from a currently iterated subject.
	 * 
	 * @tparam D The class of the detail to get.
	 * @tparam Paradigm The paradigm to work under.
	 */
	template < class D, EParadigm Paradigm = EParadigm::Default,
			   TDetailPtrResultSecurity<D> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<D>>
	GetDetail() const
	{
		return GetDetail<Paradigm, D>();
	}

	/// @}
#pragma endregion Details Getting

#pragma region Hinted Traits Getting
	/// @name Hinted Traits Getting
	/// @{

	/**
	 * Get the hinting index of a trait.
	 * 
	 * @param TraitType The trait type to query for.
	 * @return The hinting index of the trait.
	 */
	FORCEINLINE int32
	HintTrait(UScriptStruct* const TraitType) const
	{
		return Chunk->TraitLineIndexOf(TraitType);
	}

	/**
	 * Get the hinting index of a trait. Templated version.
	 *
	 * @note May actually be safely used on non-trait types and
	 * will return @c INDEX_NONE in that case.
	 *
	 * @tparam T The trait type to query for.
	 * @return The hinting index of the trait.
	 */
	template < typename T >
	FORCEINLINE constexpr int32
	HintTrait() const
	{
		return Chunk->template TraitLineIndexOf<T>();
	}

	/**
	 * Get a trait pointer at a hinted index from a currently iterated subject.
	 * 
	 * Gets a trait from a currently iterated subject
	 * by its index, relative to the current chunk's traitmark.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @param TraitType The type of the trait to get.
	 * @param TraitIndexHint The index hinting of the trait to get.
	 * @return A pointer to the trait data.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   TTraitVoidPtrResultSecurity<Paradigm> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>
	GetTraitPtrHinted(UScriptStruct* const TraitType,
					  const int32          TraitIndexHint) const
	{
		if (AvoidFormat(Paradigm, TraitType == nullptr, TEXT("The valid trait type must be provided.")))
		{
			return MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::NullArgument, nullptr);
		}
		if (AvoidFormat(Paradigm, !IsViable(), TEXT("The chunk iterator is not valid to get a trait pointer with hinting from. Is it at the end?")))
		{
			return MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::InvalidState, nullptr);
		}
		if (AvoidFormat(Paradigm, !Chunk->IsLocked(), TEXT("The iterator's chunk is not locked. Is the iterator used outside of its iteration loop?")))
		{
			return MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::InvalidState, nullptr);
		}

		const auto& Slot = Chunk->Slots[SlotIndex];
		if (UNLIKELY(Slot.IsStale()))
		{
			// The subject was moved from the chunk or the hint is wrong.
			// Get the data from its actual place (no reason for index-hinting here)...
			return Slot.template GetHandle<SubjectHandleT>().template GetTraitPtr<Paradigm>(TraitType);
		}

		return Chunk->TraitPtrAtHinted(SlotIndex, TraitType, TraitIndexHint);
	}

	/**
	 * Get a trait pointer at a hinted index from a currently iterated subject.
	 * Templated version.
	 * 
	 * Gets a trait from a currently iterated subject
	 * by its index, relative to the current chunk's traitmark.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait to get.
	 * @param TraitIndexHint The index hinting of the trait to get.
	 * @return A pointer to the trait data.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitPtrResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtrHinted(const int32 TraitIndexHint) const
	{
		return OutcomeStaticCast<TTraitPtrResult<Paradigm, T>>(GetTraitPtrHinted<Paradigm>(T::StaticStruct(), TraitIndexHint));
	}

	/**
	 * Get a trait pointer at a hinted index from a currently iterated subject.
	 * Templated version.
	 * 
	 * Gets a trait from a currently iterated subject
	 * by its index, relative to the current chunk's traitmark.
	 * 
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The access safety paradigm.
	 * @param TraitIndexHint The index hinting of the trait to get.
	 * @return A pointer to the trait data.
	 */
	template < typename T,
			   EParadigm Paradigm = EParadigm::Default,
			   TTraitPtrResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtrHinted(const int32 TraitIndexHint) const
	{
		return GetTraitPtrHinted<Paradigm, T>(TraitIndexHint);
	}

	/**
	 * Get a trait reference at a hinted index from a currently iterated subject.
	 * 
	 * Gets a trait from a currently iterated subject
	 * by its index, relative to the current chunk's traitmark.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of trait to get.
	 * Must be a USTRUCT-type.
	 * @param TraitIndexHint The index hint of the trait to get.
	 * @return A reference to the trait data.
	 */
	template < EParadigm Paradigm, typename T, TTraitRefResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRefHinted(const int32 TraitIndexHint) const
	{
		static thread_local T TraitTemp = T();
		if (AvoidFormat(Paradigm, !IsViable(), TEXT("The chunk iterator is not valid to get a trait with hinting from. Is it at the end?")))
		{
			return MakeOutcome<Paradigm, TTraitRefResult<Paradigm, T>>(EApparatusStatus::InvalidState, TraitTemp);
		}
		if (AvoidFormat(Paradigm, !Chunk->IsLocked(), TEXT("The iterator's chunk is not locked. Is the iterator used outside of its iteration loop?")))
		{
			return MakeOutcome<Paradigm, TTraitRefResult<Paradigm, T>>(EApparatusStatus::InvalidState, TraitTemp);
		}

		const auto& Slot = Chunk->Slots[SlotIndex];
		if (UNLIKELY(Slot.IsStale()))
		{
			// The subject was moved from the chunk or the hint is wrong.
			// Get the data from its actual place (no reason for index-hinting here)...
			return Slot.template GetHandle<SubjectHandleT>().template GetTraitRef<Paradigm, T>();
		}

		return Chunk->template TraitRefAtHinted<T>(SlotIndex, TraitIndexHint);
	}

	/**
	 * Get a trait reference at a hinted index from a currently iterated subject.
	 * 
	 * Gets a trait from a currently iterated subject
	 * by its index, relative to the current chunk's traitmark.
	 * 
	 * @tparam T The type of trait to get.
	 * Must be a USTRUCT-type.
	 * @tparam Paradigm The access safety paradigm.
	 * @param TraitIndexHint The index hint of the trait to get.
	 * @return A reference to the trait data.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default,
			   TTraitRefResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRefHinted(const int32 TraitIndexHint) const
	{
		return GetTraitRefHinted<Paradigm, T>(TraitIndexHint);
	}

	/**
	 * Get a trait at index from a currently iterated subject.
	 * 
	 * Gets a trait from a currently iterated subject
	 * by its index, relative to the current chunk-locking
	 * filter's traitmark.
	 * 
	 * @param TraitType The type of the trait to get.
	 * @param TraitIndexHint The hinting index for the trait to get.
	 * Relative to the current's filter traitmark.
	 * @param OutTraitData The trait data receiver.
	 * @param bTraitDataInitialized Is the @p OutTraitData buffer actually initialized?
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	GetTraitHinted(UScriptStruct* const TraitType,
				   const int32          TraitIndexHint,
				   void* const          OutTraitData,
				   const bool           bTraitDataInitialized = true) const
	{
		if (Avoid(Paradigm, TraitType == nullptr))
		{
			return EApparatusStatus::NullArgument;
		}
		if (AvoidFormat(Paradigm, !IsViable(),
						TEXT("The chunk iterator is not valid to get a trait with hinting from. Is it at the end?")))
		{
			return EApparatusStatus::InvalidState;
		}
		if (AvoidFormat(Paradigm, !Chunk->IsLocked(),
						TEXT("The iterator's chunk is not locked. Is the iterator used outside of its iteration loop?")))
		{
			return EApparatusStatus::InvalidState;
		}

		const auto& Slot = Chunk->Slots[SlotIndex];
		if (UNLIKELY(Slot.IsStale()))
		{
			// The subject was moved from the chunk or the hint is wrong.
			// Get the data from its actual place...
			return Slot.template GetHandle<SubjectHandleT>().template GetTrait<Paradigm>(
				TraitType, OutTraitData, bTraitDataInitialized);
		}

		return Chunk->TraitAtHinted(
							SlotIndex, TraitType, TraitIndexHint,
							OutTraitData, bTraitDataInitialized);
	}

	/**
	 * Get a copy of a trait at a hinted index from a currently iterated subject.
	 * 
	 * Gets a trait from a currently iterated subject
	 * with its hinted index, relative to the current chunk-locking
	 * filter's traitmark.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of trait to get.
	 * @param TraitIndexHint The index of the trait to hint with.
	 * Relative to the current's filter traitmark.
	 * @return A copy of the requested trait.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm, T>
	GetTraitHinted(const int32 TraitIndexHint) const
	{
		if (AvoidFormat(Paradigm, !IsViable(), TEXT("The chunk iterator is not valid to get a trait by index from. Is it at the end?")))
		{
			return MakeOutcome<Paradigm, T>(EApparatusStatus::InvalidState, T());
		}
		if (AvoidFormat(Paradigm, !Chunk->IsLocked(), TEXT("The iterator's chunk is not locked. Is the iterator used outside of its iteration loop?")))
		{
			return MakeOutcome<Paradigm, T>(EApparatusStatus::InvalidState, T());
		}

		const auto& Slot = Chunk->Slots[SlotIndex];
		if (UNLIKELY(Slot.IsStale()))
		{
			// The subject was moved from the chunk or the hint is wrong.
			// Get the data from its actual place (no reason for index-hinting here)...
			return Slot.template GetHandle<SubjectHandleT>().template GetTrait<Paradigm, T>();
		}

		return Chunk->template TraitAtHinted<T>(SlotIndex, TraitIndexHint);
	}

	/**
	 * Get a copy of a trait at a hinted index from a currently iterated subject.
	 * 
	 * Gets a trait from a currently iterated subject
	 * with its hinted index, relative to the current chunk-locking
	 * filter's traitmark.
	 * 
	 * @tparam T The type of trait to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitIndexHint The index of the trait to hint with.
	 * Relative to the current's filter traitmark.
	 * @return A copy of the requested trait.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm, T>
	GetTraitHinted(const int32 TraitIndexHint) const
	{
		return GetTraitHinted<Paradigm, T>(TraitIndexHint);
	}

	/// @}
#pragma endregion Hinted Traits Getting

	/**
	 * Move the iterator to the end of the chunk.
	 * 
	 * This basically unlocks the locked chunk.
	 */
	FORCEINLINE void
	MoveToEnd()
	{
		check(Chunk);
		if (LIKELY(SlotIndex > InvalidSlotIndex))
		{
			Chunk->Unlock<IsSolid>();
			SlotIndex = InvalidSlotIndex;
		}
	}

	/**
	 * Clear the iterator to a null-state.
	 */
	FORCEINLINE void
	Reset()
	{
		MoveToEnd();
		Chunk = nullptr;
		Filter.Reset();
	}

	/**
	 * Check if the iterator is valid
	 * and is not at the end of the chunk.
	 * 
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	IsViable() const
	{
		return (Chunk != nullptr) &&
			   (SlotIndex > InvalidSlotIndex) &&
			   (SlotIndex < Chunk->IterableCount);
	}

	/**
	 * Check if the iterator is valid
	 * and is not at the end of the chunk.
	 * 
	 * @return The state of examination.
	 */
	FORCEINLINE
	operator bool() const
	{
		return IsViable();
	}

	/**
	 * Move an iterator.
	 */
	TChunkIt&
	operator=(TChunkIt&& Iterator)
	{
		Unlock(); // Previous state.
		Chunk     = Iterator.Chunk;
		Filter    = MoveTemp(Iterator.Filter);
		SlotIndex = Iterator.SlotIndex;
		Iterator.SlotIndex = InvalidSlotIndex; // Invalidate.
		return *this;
	}

	/**
	 * Set the iterator equal to an another one.
	 */
	TChunkIt&
	operator=(const TChunkIt& Iterator)
	{
		if (UNLIKELY(std::addressof(Iterator) == this))
		{
			return *this;
		}
		if (UNLIKELY((Chunk     == Iterator.Chunk) &&
					 (Filter    == Iterator.Filter) &&
					 (SlotIndex == Iterator.SlotIndex)))
		{
			// Nothing is actually changed:
			return *this;
		}
		Unlock(); // Previous state.
		Chunk     = Iterator.Chunk;
		Filter    = Iterator.Filter;
		SlotIndex = Iterator.SlotIndex;
		if (LIKELY(Chunk && (SlotIndex > InvalidSlotIndex)))
		{
			Chunk->Lock<IsSolid>();
			APPARATUS_REPORT_SUCCESS(TEXT("Copied the iterator for the #%d chunk."), Chunk->Id);
		}
		return *this;
	}

	/**
	 * Move iterator to the next place (if available).
	 * 
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	Advance()
	{
		check(Chunk);
		checkf(SlotIndex > InvalidSlotIndex,
			   TEXT("The iterator is already at the end of the chunk and was finalized."));
		checkf(Chunk->IsLocked(),
			   TEXT("The chunk has to be locked in order for iterator to advance."));

		if (UNLIKELY(SlotIndex >= (Chunk->IterableCount - 1))) // Overflow protection.
		{
			// We already came to the last subject...
			Unlock();
			return EApparatusStatus::NoMore;
		}

		// Go to the next slot:
		SlotIndex += 1;

		// Skip invalid/flagged subject slots:
		if (UNLIKELY(!PrepareIteration()))
		{
			return EApparatusStatus::NoMore;
		}

		return EApparatusStatus::Success;
	}

#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Initialize an empty chunk iterator.
	 */
	FORCEINLINE
	TChunkIt() {};

	/**
	 * Move-initialize a chunk iterator.
	 */
	FORCEINLINE
	TChunkIt(TChunkIt&& Iterator)
	  : Chunk(Iterator.Chunk)
	  , Filter(MoveTemp(Iterator.Filter))
	  , SlotIndex(Iterator.SlotIndex)
	{
		Iterator.SlotIndex = InvalidSlotIndex; // Mark so the destructor won't do the unlocking.
	}

	/**
	 * Initialize a chunk iterator as a copy of another iterator.
	 * 
	 * @note Locks the chunk appropriately, if it's available in the original.
	 */
	FORCEINLINE
	TChunkIt(const TChunkIt& Iterator)
	  : Chunk(Iterator.Chunk)
	  , Filter(Iterator.Filter)
	  , SlotIndex(Iterator.SlotIndex)
	{
		if (LIKELY(Chunk && (SlotIndex > InvalidSlotIndex)))
		{
			VerifyOK(Chunk->template Lock<IsSolid>());
		}
	}

	/// @}
#pragma endregion Initialization

	/**
	 * Destroy the chunk iterator.
	 * 
	 * @note Unlocks the chunk, if it has been locked.
	 */
	FORCEINLINE
	~TChunkIt()
	{
		Reset();
	}

}; //-struct TChunkIt

/**
 * Mutable chunk iterator.
 */
typedef TChunkIt<FSubjectHandle> FChunkIt;

/**
 * Homogenous chunk iterator.
 */
typedef TChunkIt<FSolidSubjectHandle> FSolidChunkIt; 
