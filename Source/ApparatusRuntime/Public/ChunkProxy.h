/*
 * ░▒▓ APPARATUS ▓▒░
 *
 * File: ChunkProxy.h
 * Created: 2022-04-03 20:53:32
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

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define CHUNK_PROXY_H_SKIPPED_MACHINE_H
#endif

#include "Chunk.h"

#ifdef CHUNK_PROXY_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif


/**
 * A strongly typed chunk proxy.
 * 
 * @tparam SubjectHandleT The subject handle to utilize.
 * @tparam Paradigm The access security paradigm to use.
 * @tparam Ts The types of traits to proxy.
 * Some of them may actually be missing from the target
 * chunk. May include the ::TParadigm specifications.
 */
template < typename SubjectHandleT, typename... Ts >
struct TChunkProxy
{
	static_assert(!more::has_duplicates<Ts...>::value,
				  "The list of traits must not contain any duplicates.");
  public:

	/**
	 * Should subject changes be allowed.
	 */
	static constexpr auto AllowsChanges = SubjectHandleT::AllowsChanges;

	/**
	 * Should structural (adding/removing traits) subject changes be allowed.
	 */
	static constexpr auto AllowsStructuralChanges = SubjectHandleT::AllowsStructuralChanges;

	/**
	 * Can the trait data be accessed directly?
	 */
	static constexpr auto AllowsDirectTraitAccess = SubjectHandleT::AllowsDirectTraitAccess;

	/**
	 * Is the chunk a solid-locking one?
	 */
	static constexpr auto ShouldLockSolid = SubjectHandleT::IsHandleSolid();

	/**
	 * The security paradigm to use by default.
	 */
	static constexpr EParadigm DefaultParadigm = ParadigmCombine<Ts...>();

	/**
	 * The type of a trait void pointer returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 */
	template < EParadigm Paradigm >
	using TTraitVoidPtrResult = typename SubjectHandleT::template TTraitVoidPtrResult<Paradigm>;

	/**
	 * The type of a trait pointer returned by the methods.
	 * 
	 * @tparam T The type of the trait.
	 * @tparam Paradigm The access safety paradigm.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitPtrResult = typename SubjectHandleT::template TTraitPtrResult<Paradigm, T>;

	/**
	 * The type of a trait reference returned by the methods.
	 * 
	 * @tparam T The type of the trait.
	 * @tparam Paradigm The access safety paradigm.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitRefResult = typename SubjectHandleT::template TTraitRefResult<Paradigm, T>;

	/**
	 * The class of a detail pointer returned by the methods.
	 * 
	 * @tparam D The class of the detail.
	 */
	template < class D >
	using TDetailPtrResult = typename SubjectHandleT::template TDetailPtrResult<D>;

  private:

	/**
	 * The target chunk to represent.
	 */
	UChunk& Target;

	/**
	 * The corresponding fetched lines
	 * from the original target chunk.
	 */
	TArray<FScriptStructArray*> Lines;

	/**
	 * Should the proxy lock its target.
	 */
	bool bLocking = false;

  public:

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
		return Target.At(SlotIndex);
	}

	/**
	 * Get a slot at a certain position.
	 */
	FORCEINLINE std::conditional_t<AllowsChanges, FChunkSlot&, const FChunkSlot&>
	At(const int32 SlotIndex)
	{
		return Target.At(SlotIndex);
	}

	/**
	 * Get a slot at a certain position.
	 * Constant version.
	 */
	FORCEINLINE const FChunkSlot&
	operator[](const int32 SlotIndex) const
	{
		return Target[SlotIndex];
	}

	/**
	 * Get a slot at a certain position.
	 */
	FORCEINLINE std::conditional_t<AllowsChanges, FChunkSlot&, const FChunkSlot&>
	operator[](const int32 SlotIndex)
	{
		return Target[SlotIndex];
	}

	/**
	 * Check if the slot is actually stale at a certain position.
	 * 
	 * Stale slots should generally be skipped during the iterating process.
	 */
	FORCEINLINE bool
	IsStaleAt(const int32 SlotIndex) const
	{
		return Target.IsStaleAt(SlotIndex);
	}

	/// @}
#pragma endregion Slots

#pragma region Subjects
	/// @name Subjects
	/// @{

	/**
	 * Get a subject at a certain index within the chunk.
	 *
	 * @param SlotIndex The index of the slot to get the subject of.
	 * @return A handle to the subject.
	 */
	FORCEINLINE SubjectHandleT
	SubjectAt(const int32 SlotIndex) const
	{
		return Target.template SubjectAt<SubjectHandleT>(SlotIndex);
	}

	/**
	 * Get a subject's fingerprint at a specific slot index.
	 *
	 * The operation is thread-safe and atomic.
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
		return Target.SubjectFingerprintAt(SlotIndex);
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
		return Target.SubjectFlagmarkAt(SlotIndex);
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
		return Target.HasSubjectFlagAt(SlotIndex, Flag);
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
		return Target.template SetSubjectFlagmarkAt<Paradigm>(SlotIndex, Flagmark);
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
		return Target.template SetSubjectFlagmarkMaskedAt<Paradigm>(SlotIndex, Flagmark, Mask);
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
		return Target.template SetSubjectFlagAt<Paradigm>(SlotIndex, Flag, bState);
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
		return Target.template ToggleSubjectFlagAt<Paradigm>(SlotIndex, Flag);
	}

	/// @}
#pragma endregion Subjects

#pragma region Traits Access
	/// @name Traits Access
	/// @{

	/**
	 * Get a trait from the chunk at a certain subject index.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex The index of the subject to examine.
	 * @return The trait reference.
	 */
	template < typename T >
	FORCEINLINE T
	TraitAt(const int32 SlotIndex) const
	{
		constexpr auto LineIndex = more::index_within<T, Ts...>::value;
		if (LineIndex <= -1) // Compile-time branch.
		{
			return Target.template TraitAt<T>(SlotIndex);
		}
		const auto Line = Lines[LineIndex];
		checkf(Line, TEXT("The trait is not available within the chunk: %s"), *T::StaticStruct()->GetName());
		return Line->template ElementAt<T>(SlotIndex);
	}

	/**
	 * Get a trait from the chunk at a certain subject index.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex The index of the subject to examine.
	 * @param OutStruct The struct to receive the trait.
	 * @param bStructInitialized Was the output struct actually initialized?
	 * @return The status of the operation.
	 * @return EApparatusStatus::Missing If there's no such trait within the chunk.
	 */
	template < typename T >
	FORCEINLINE EApparatusStatus
	TraitAt(const int32 SlotIndex,
			T&          OutStruct,
			const bool  bStructInitialized = true) const
	{
		constexpr auto LineIndex = more::index_within<T, Ts...>::value;
		if (LineIndex <= -1) // Compile-time branch.
		{
			return Target.template TraitAt<T>(SlotIndex, OutStruct, bStructInitialized);
		}
		const auto Line = Lines[LineIndex];
		if (LIKELY(Line))
		{
			Line->template ElementAt<T>(SlotIndex, OutStruct, bStructInitialized);
		}
		return EApparatusStatus::Missing;
	}

	/// @}
#pragma endregion Traits Access

#pragma region Traits Data Access
	/// @name Traits Data Access
	/// @{

	/**
	 * Get the trait reference from the chunk.
	 * Constant version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex The index of the subject to examine.
	 * @return The trait reference.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TTraitRefResult<Paradigm, const T>
	TraitRefAt(const int32 SlotIndex) const
	{
		constexpr auto LineIndex = more::index_within<T, Ts...>::value;
		if (LineIndex <= -1) // Compile-time branch.
		{
			return const_cast<TTraitRefResult<Paradigm, const T>>(Target.template TraitRefAt<T>(SlotIndex));
		}
		const auto Line = Lines[LineIndex];
		checkf(Line, TEXT("The trait is not available within the chunk: %s"), *T::StaticStruct()->GetName());
		return const_cast<TTraitRefResult<Paradigm, const T>>(Line->template RefAt<T>(SlotIndex));
	}

	/**
	 * Get the trait reference from the chunk.
	 * Constant version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex The index of the subject to examine.
	 * @return The trait reference.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TTraitRefResult<Paradigm, const T>
	TraitRefAt(const int32 SlotIndex) const
	{
		return TraitRefAt<Paradigm, T>();
	}

	/**
	 * Get the trait reference from the chunk.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex The index of the subject to examine.
	 * @return The trait reference.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TTraitRefResult<Paradigm, T>
	TraitRefAt(const int32 SlotIndex)
	{
		constexpr auto LineIndex = more::index_within<T, Ts...>::value;
		if (LineIndex <= -1) // Compile-time branch.
		{
			return const_cast<TTraitRefResult<Paradigm, T>>(Target.template TraitRefAt<T>(SlotIndex));
		}
		const auto Line = Lines[LineIndex];
		checkf(Line, TEXT("The trait is not available within the chunk: %s"), *T::StaticStruct()->GetName());
		return const_cast<TTraitRefResult<Paradigm, T>>(Line->template RefAt<T>(SlotIndex));
	}

	/**
	 * Get the trait reference from the chunk.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param SlotIndex The index of the subject to examine.
	 * @return The trait reference.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TTraitRefResult<Paradigm, T>
	TraitRefAt(const int32 SlotIndex)
	{
		return TraitRefAt<Paradigm, T>(SlotIndex);
	}

	/**
	 * Get the trait pointer from the chunk.
	 * Constant version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param Index The index of the subject to examine.
	 * @return A pointer to the trait.
	 * @return nullptr If there is no such trait within the chunk.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TTraitPtrResult<Paradigm, const T>
	TraitPtrAt(const int32 Index) const
	{
		constexpr auto LineIndex = more::index_within<T, Ts...>::value;
		if (LineIndex <= -1) // Compile-time branch.
		{
			return const_cast<TTraitPtrResult<Paradigm, const T>>(Target.template TraitPtrAt<T>(Index));
		}
		const auto Line = Lines[LineIndex];
		if (LIKELY(Line))
		{
			return const_cast<TTraitPtrResult<Paradigm, const T>>(Line->template PtrAt<T>(Index));
		}
		return nullptr;
	}

	/**
	 * Get the trait pointer from the chunk.
	 * Constant version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param Index The index of the subject to examine.
	 * @return A pointer to the trait.
	 * @return nullptr If there is no such trait within the chunk.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TTraitPtrResult<Paradigm, const T>
	TraitPtrAt(const int32 Index) const
	{
		return TraitPtrAt<Paradigm, T>(Index);
	}

	/**
	 * Get the trait pointer from the chunk.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param Index The index of the subject to examine.
	 * @return A pointer to the trait.
	 * @return nullptr If there is no such trait within the chunk.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TTraitPtrResult<Paradigm, T>
	TraitPtrAt(const int32 Index)
	{
		constexpr auto LineIndex = more::index_within<T, Ts...>::value;
		if (LineIndex <= -1) // Compile-time branch.
		{
			return const_cast<TTraitPtrResult<Paradigm, T>>(Target.template TraitPtrAt<T>(Index));
		}
		const auto Line = Lines[LineIndex];
		if (LIKELY(Line))
		{
			return const_cast<TTraitPtrResult<Paradigm, T>>(Line->template PtrAt<T>(Index));
		}
		return nullptr;
	}

	/**
	 * Get the trait pointer from the chunk.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param Index The index of the subject to examine.
	 * @return A pointer to the trait.
	 * @return nullptr If there is no such trait within the chunk.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TTraitPtrResult<Paradigm, T>
	TraitPtrAt(const int32 Index)
	{
		return TraitPtrAt<Paradigm, T>(Index);
	}

	/// @}
#pragma endregion Traits Data Access

#pragma region Iterating
	/// @name Iterating
	/// @{

	/**
	 * Get the total number of slots currently in the iterable.
	 *
	 * @note Returns the active iterable slots number
	 * when the iterable is locked, which also includes 
	 * the stale (to be skipped) slots.
	 * 
	 * @return The current number of slots.
	 */
	FORCEINLINE int32
	Num() const
	{
		return Target.Num();
	}

	/**
	 * Get the current number of elements
	 * valid for iterating.
	 * 
	 * @note This number includes the stale (to be skipped)
	 * slots also.
	 */
	FORCEINLINE int32
	IterableNum() const
	{
		return Target.IterableNum();
	}

	/// @}
#pragma endregion Iterating

#pragma region Locking & Unlocking
	/// @name Locking & Unlocking
	/// @{

	/**
	 * Lock the chunk for iterating.
	 * 
	 * @see Unlock()
	 */
	FORCEINLINE EApparatusStatus
	Lock()
	{
		return Target.Lock<ShouldLockSolid>();
	}

	/**
	 * Unlock the chunk from iterating.
	 * 
	 * @see Lock()
	 */
	FORCEINLINE EApparatusStatus
	Unlock()
	{
		return Target.Unlock<ShouldLockSolid>();
	}

	/// @}
#pragma endregion Locking & Unlocking

#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Initialize a new proxy for a chunk.
	 * 
	 * @param InTarget The chunk to proxy.
	 * @param bLock Should the target chunk be locked.
	 */
	FORCEINLINE
	TChunkProxy(UChunk* const InTarget, const bool bLock = false)
	  : Target(*InTarget)
	  , Lines({const_cast<FScriptStructArray*>(InTarget->template FindTraitLine<Ts>())...})
	  , bLocking(bLock)
	{
		check(InTarget);
		if (bLocking)
		{
			verify(OK(Lock()));
		}
	}

	/**
	 * Copy a proxy chunk.
	 * 
	 * @param InProxyChunk The proxy chunk to copy.
	 */
	FORCEINLINE
	TChunkProxy(const TChunkProxy& InProxyChunk)
	  : Target(InProxyChunk.Target)
	  , Lines(InProxyChunk.Lines)
	  , bLocking(InProxyChunk.bLocking)
	{}

	/**
	 * Move a proxy chunk.
	 * 
	 * @param InProxyChunk The proxy chunk to move.
	 */
	FORCEINLINE
	TChunkProxy(TChunkProxy&& InProxyChunk)
	  : Target(InProxyChunk.Target)
	  , Lines(MoveTemp(InProxyChunk.Lines))
	  , bLocking(InProxyChunk.bLocking)
	{
		InProxyChunk.bLocking = false;
	}

	/// @}
#pragma endregion Initialization

	virtual FORCEINLINE
	~TChunkProxy()
	{
		if (bLocking)
		{
			ensure(OK(Unlock()));
		}
	}

}; //-struct TChunkProxy
