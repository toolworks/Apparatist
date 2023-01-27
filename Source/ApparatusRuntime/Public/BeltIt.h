/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BeltIt.h
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
#include "UObject/NoExportTypes.h"
#include "UObject/WeakInterfacePtr.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define BELT_IT_H_SKIPPED_MACHINE_H
#endif

#include "Belt.h"

#ifdef BELT_IT_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif


// Forward declarations:
struct FFilter;
struct FBeltSlot;
struct FSubjectHandle;
struct FSolidSubjectHandle;

template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
struct TChain;

class UBelt;

/**
 * The iterator of the belt slots.
 * 
 * This actually iterates inter-combinations when there
 * are multiple details of the same type within the slot and
 * the locking filter.
 * 
 * @warning You should never persist (save) instances of this
 * class within your code. It must be used solely within a loop.
 */
template < typename SubjectHandleT >
struct TBeltIt final
{

  public:

	/**
	 * The type of the subject handle used within
	 * the belt iterator.
	 */
	typedef SubjectHandleT SubjectHandleType;

	/**
	 * The type of the detail line type.
	 */
	using DetailLineIndexType = FBeltSlot::DetailLineIndexType;

	/**
	 * The type of the subjective used.
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
	 * Invalid belt slot index.
	 */
	static constexpr auto InvalidSlotIndex = FBeltSlot::InvalidIndex;

	/**
	 * Invalid detail index.
	 */
	static constexpr auto InvalidDetailLineIndex = UBelt::InvalidDetailLineIndex;

	/**
	 * The solidity state of the iterator.
	 */
	static constexpr bool IsHandleSolid()
	{
		return SubjectHandleType::IsHandleSolid();
	}

  private:

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	friend class UBelt;

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
	 * The belt of the iterator.
	 */
	UBelt* Belt = nullptr;

	/**
	 * The filter used for the iterating.
	 * 
	 * This is set in the constructor
	 * and may actually include
	 * additional flags/exclusions.
	 */
	FFilter Filter;

	/**
	 * The index of the iterated slot.
	 * 
	 * If it is valid, the belt should be locked
	 * by this iterator.
	 */
	int32 SlotIndex = InvalidSlotIndex;

	/**
	 * The indices of the details being
	 * actually used within the iterating process.
	 */
	TArray<DetailLineIndexType, TInlineAllocator<8>> MainlineIndices;

	/**
	 * Unlock the belt of the iterator.
	 * 
	 * If the iterator is not locking the belt currently
	 * nothing is performed and EApparatusStatus::Noop is returned.
	 * 
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	Unlock()
	{
		if (LIKELY(IsLocking()))
		{
			EnsureOK(Belt->Unlock<IsSolid>());
		}
		SlotIndex = InvalidSlotIndex;
		return EApparatusStatus::Success;
	}

	/**
	 * Move the iterator to an end.
	 */
	FORCEINLINE void
	MoveToEnd()
	{
		Unlock();
	}

	/**
	 * Construct an ending iterator with its owner.
	 */
	FORCEINLINE
	TBeltIt(const UBelt* const InBelt)
	  : Belt(const_cast<UBelt*>(InBelt)) // Guaranteed to not access any mutable methods.
	{
		checkf(Belt, TEXT("A valid belt must be present in order to initialize an ending belt iterator."));
	}

	/**
	 * Construct an iterator with its owner and a slot index.
	 */
	inline
	TBeltIt(UBelt* const   InBelt,
			const FFilter& InFilter,
			const int32    InSlotIndex = InvalidSlotIndex)
	  : Belt(InBelt)
	  , Filter(InFilter)
	  , SlotIndex(InSlotIndex) 
	{
		checkf(InBelt, TEXT("A valid belt must be provided in order to initialize the iterator."));

		// Set mandatory flags.
		Filter.Exclude(FM_Stale | FM_DeferredDespawn); // Stale slots should always be excluded.
#if WITH_EDITOR
		if (LIKELY(!Filter.Includes(EFlagmarkBit::Editor))) // Editor entities can be included explicitly.
		{
			const auto World = Belt->GetWorld();
			if (LIKELY(World && World->IsGameWorld()))
			{
				// Exclude the in-editor subjects during the gameplay iterating.
				Filter.Exclude(EFlagmarkBit::Editor);
			}
		}
#endif // WITH_EDITOR


		// Fetch the indices of the utilized details:
		Belt->CollectMainlineIndices(Filter.GetDetailmark().GetDetails(), /*Out=*/MainlineIndices);

		if (LIKELY(SlotIndex != InvalidSlotIndex))
		{
			VerifyOK(Belt->template Lock<IsSolid>());

			// Skip invalid slots...
			while ((SlotIndex < Belt->IterableNum()) &&
				   (!Belt->Slots[SlotIndex].PrepareForIteration(Filter, MainlineIndices)))
			{
				SlotIndex += 1;
			}

			if (UNLIKELY(SlotIndex == Belt->IterableNum()))
			{
				// No valid slot was found...
				Unlock();
				return;
			}
		}
	}

  public:

	/**
	 * Get the belt of the iterator.
	 */
	FORCEINLINE UBelt*
	GetBelt() const
	{
		return Belt;
	}

	/**
	 * Get the filter of the current iterating.
	 * 
	 * @return The filter of the current iterating.
	 */
	FORCEINLINE const FFilter&
	GetFilter() const
	{
		return Filter;
	}

	/**
	 * Get the detailmark of the current iterating.
	 * 
	 * @return The detailmark of the iterating.
	 * 
	 * @see GetFilter()
	 */
	FORCEINLINE const FDetailmark&
	GetDetailmark() const
	{
		check(Belt);
		check(Belt->IsLocked());
		check(SlotIndex != FBeltSlot::InvalidIndex);
		return GetFilter().GetDetailmark();
	}

	/**
	 * Get the current belt's slot index.
	 * 
	 * @return The current iterated slot index.
	 */
	FORCEINLINE int32
	GetSlotIndex() const
	{
		return SlotIndex;
	}

	/**
	 * Construct a new uninitialized iterator.
	 */
	FORCEINLINE
	TBeltIt()
	{}

	/**
	 * Move-construct a new belt iterator.
	 */
	FORCEINLINE
	TBeltIt(TBeltIt&& InIterator)
	  : Belt           (InIterator.Belt)
	  , Filter         (MoveTemp(InIterator.Filter))
	  , SlotIndex      (InIterator.SlotIndex) 
	  , MainlineIndices(MoveTemp(InIterator.MainlineIndices))
	{
		InIterator.Belt       = nullptr;
		InIterator.SlotIndex  = InvalidSlotIndex;
	}

	/**
	 * Copy-construct a new belt iterator.
	 */
	TBeltIt(const TBeltIt& InIterator)
	  : Belt           (InIterator.Belt)
	  , Filter         (InIterator.Filter)
	  , SlotIndex      (InIterator.SlotIndex)
	  , MainlineIndices(InIterator.MainlineIndices)
	{
		if (LIKELY(Belt))
		{
			if (LIKELY(SlotIndex > InvalidSlotIndex))
			{
				// The belt should be locked...
				VerifyOK(Belt->template Lock<IsSolid>());
			}
		}
		else
		{
			check(SlotIndex <= InvalidSlotIndex);
		}
	}

	/**
	 * Check if the iterator should be locking its belt.
	 */
	FORCEINLINE bool
	IsLocking() const
	{
		return Belt && (SlotIndex > InvalidSlotIndex);
	}

	/**
	 * Move-assign the iterator.
	 * 
	 * @param InIterator An iterator to move.
	 */
	inline TBeltIt&
	operator=(TBeltIt&& InIterator)
	{
		Unlock();

		Belt            = InIterator.Belt;
		Filter          = MoveTemp(InIterator.Filter);
		SlotIndex       = InIterator.SlotIndex;
		MainlineIndices = MoveTemp(InIterator.MainlineIndices);

		return *this;
	}

	/**
	 * Assign the iterator.
	 * 
	 * @param InIterator An iterator to assign from.
	 */
	inline TBeltIt&
	operator=(const TBeltIt& InIterator)
	{
		Unlock();

		Belt            = InIterator.Belt;
		Filter          = InIterator.Filter;
		SlotIndex       = InIterator.SlotIndex;
		MainlineIndices = InIterator.MainlineIndices;

		if (Belt && (SlotIndex != InvalidSlotIndex))
		{
			Belt->Lock<IsSolid>();
		}

		return *this;
	}

	/**
	 * Check if the iterator is viable and is not at the end
	 * of its belt.
	 * 
	 * During the iteration process the current state
	 * of the iterator may point to some invalid slot (missing subjective)
	 * after its processing, but that doesn't mean
	 * that the iterator is not viable itself, since
	 * it can still advance to the next valid slot (if there is any).
	 */
	FORCEINLINE bool
	IsViable() const
	{
		// These should be exactly enough:
		return Belt && (SlotIndex != InvalidSlotIndex);
	}

	/**
	 * Check if the iterator is valid and not at the end.
	 */
	FORCEINLINE operator bool() const
	{
		return IsViable();
	}

	/**
	 * Get the current slot of the iterator.
	 * 
	 * @return The slot of the iterator.
	 */
	FORCEINLINE const FBeltSlot&
	GetSlot() const
	{
		check(Belt);
		check(Belt->IsLocked());
		check(SlotIndex != FBeltSlot::InvalidIndex);
		return Belt->Slots[SlotIndex];
	}

	/**
	 * Get the currently iterated subjective.
	 */
	FORCEINLINE SubjectivePtrType
	GetSubjective() const
	{
		return (SubjectivePtrType)(GetSlot().GetSubjective());
	}

	/**
	 * Get the currently iterated subject.
	 */
	FORCEINLINE SubjectHandleT
	GetSubject() const
	{
		return (SubjectHandleT)GetSubjective()->GetHandle();
	}

#pragma region Details Getting
	/// @name Details Getting
	/// @{

	/**
	 * Get a detail at a certain index.
	 * 
	 * @param DetailLineIndex The detail index, relative
	 * to the belt's detailmark.
	 * @return The detail at the specified index.
	 */
	FORCEINLINE UDetail*
	DetailAtLine(const int32 DetailLineIndex) const
	{
		return GetSlot().DetailAtLine(DetailLineIndex);
	}

	/**
	 * Get details at a certain line index.
	 * 
	 * @param DetailLineIndex The detail index, relative
	 * to the belt's detailmark.
	 * @return The detail at the specified index.
	 */
	template < typename AllocatorT = FDefaultAllocator >
	FORCEINLINE void
	DetailsAtLine(const int32                   DetailLineIndex,
				  TArray<UDetail*, AllocatorT>& OutDetails) const
	{
		GetSlot().DetailsAtLine(DetailLineIndex, OutDetails);
	}

	/**
	 * Get details at a certain line index.
	 * 
	 * @tparam LinesAllocatorT The allocator used within the lines array.
	 * @tparam AllocatorT The type of the allocator used within the details array.
	 * @param[in] DetailLinesIndices The line indices of the details
	 * relative to the belt's detailmark.
	 * @param[out] OutDetails The details receiver.
	 * @return The detail at the specified index.
	 */
	template < typename LinesAllocatorT = FDefaultAllocator,
			   typename AllocatorT      = FDefaultAllocator >
	FORCEINLINE void
	DetailsAtLines(const TArray<int32, LinesAllocatorT>& DetailLinesIndices,
				   TArray<UDetail*, AllocatorT>&         OutDetails) const
	{
		GetSlot().DetailsAtLines(DetailLinesIndices, OutDetails);
	}

	/**
	 * Get a detail of a certain class.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	GetDetail(const TSubclassOf<UDetail> DetailClass) const
	{
		return GetSlot().template GetDetail<Paradigm>(DetailClass);
	}

	/**
	 * Get a detail of a certain class.
	 * Statically typed version.
	 */
	template < EParadigm Paradigm, class D >
	FORCEINLINE auto
	GetDetail() const
	{
		return GetSlot().template GetDetail<Paradigm, D>();
	}

	/**
	 * Get a detail of a certain class.
	 * Statically typed version.
	 */
	template < class D, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	GetDetail() const
	{
		return GetSlot().template GetDetail<Paradigm, D>();
	}

	/**
	 * Get a list of details of the subjective on the current iteration.
	 */
	template < EParadigm Paradigm   = EParadigm::Default,
			   typename  AllocatorT = FDefaultAllocator >
	FORCEINLINE auto
	GetDetails(const TSubclassOf<UDetail>    DetailClass,
			   TArray<UDetail*, AllocatorT>& OutDetails) const
	{
		return GetSlot().template GetDetails<Paradigm>(DetailClass, OutDetails);
	}

	/**
	 * Get a list of details of the subjective on the current iteration.
	 */
	template < EParadigm Paradigm   = EParadigm::Default,
			   class     D          = UDetail,
			   typename  AllocatorT = FDefaultAllocator,
			   TDetailClassSecurity<D> = true >
	FORCEINLINE auto
	GetDetails(TArray<D*, AllocatorT>& OutDetails) const
	{
		return GetSlot().template GetDetails<Paradigm>(OutDetails);
	}

	/// @}
#pragma endregion Details Getting

#pragma region Hinted Details Getting
	/// @name Hinted Details Getting
	/// @{

	/**
	 * Get the hinting index of a detail.
	 * 
	 * @param DetailClass The detail class to query for.
	 * @return The hinting index of the detail.
	 */
	FORCEINLINE int32
	HintDetail(const TSubclassOf<UDetail> DetailClass) const
	{
		return Belt->DetailLineIndexOf(DetailClass);
	}

	/**
	 * Get the hinting index of a detail. Templated version.
	 *
	 * @note May actually be safely used on non-detail types and
	 * will return @c INDEX_NONE in that case.
	 *
	 * @tparam D The detail class to query for.
	 * @return The hinting index of the detail.
	 */
	template < typename D >
	FORCEINLINE constexpr int32
	HintDetail() const
	{
		return Belt->template DetailLineIndexOf<D>();
	}

	/**
	 * Get a detail by a class at a certain index hinted.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass The class of the detail to get.
	 * @param DetailIndexHint The hinting index of the detail.
	 * @return The requested detail.
	 * @return nullptr If the detail is not available.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   TDetailPtrResultSecurity<UDetail> = true >
	FORCEINLINE TOutcome<Paradigm, UDetail*>
	GetDetailHinted(const TSubclassOf<UDetail> DetailClass,
					const int32                DetailIndexHint) const
	{
		return GetSlot().template GetDetailHinted<Paradigm>(DetailClass, DetailIndexHint);
	}

	/**
	 * Get a detail by a class at a certain index hinted.
	 * Templated version.
	 * 
	 * @tparam D The class of the detail to get.
	 * @param DetailIndexHint The hinting index of the detail.
	 * @return The requested detail.
	 * @return nullptr If the detail is not available.
	 */
	template < EParadigm Paradigm, class D,
			   TDetailPtrResultSecurity<D> = true >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<D>>
	GetDetailHinted(const int32 DetailIndexHint) const
	{
		return GetSlot().template GetDetailHinted<D>(DetailIndexHint);
	}

	/**
	 * Get a detail by a class at a certain index hinted.
	 * Templated version.
	 * 
	 * @tparam D The class of the detail to get.
	 * @param DetailIndexHint The hinting index of the detail.
	 * @return The requested detail.
	 * @return nullptr If the detail is not available.
	 */
	template < class D, EParadigm Paradigm = EParadigm::Default,
			   TDetailPtrResultSecurity<D> = true >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<D>>
	GetDetailHinted(const int32 DetailIndexHint) const
	{
		return GetDetailHinted<Paradigm, D>(DetailIndexHint);
	}

	/// @}
#pragma endregion Hinted Details Getting

#pragma region Traits Data Access
	/// @name Traits Data Access
	/// @{

	/**
	 * Get a trait of a certain type from a currently iterated subject.
	 * Dynamically-typed version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to get.
	 * @return A pointer to the trait data.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   TTraitVoidPtrResultSecurity<Paradigm> = true  >
	FORCEINLINE TOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>
	GetTraitPtr(UScriptStruct* const TraitType) const
	{
		return GetSubject().template GetTraitPtr<Paradigm>(TraitType);
	}

	/**
	 * Get a pointer to a trait of a certain type from a currently iterated subject.
	 * Statically typed paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return The pointer to the trait data.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitPtrResultSecurity<Paradigm, T> = true >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtr() const
	{
		return GetSubject().template GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Get a pointer to a trait of a certain type from a currently iterated subject.
	 * Statically typed version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get a pointer of.
	 * @return The pointer to the trait data.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default,
			   TTraitPtrResultSecurity<Paradigm, T> = true >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtr() const
	{
		return GetSubject().template GetTraitPtr<T, Paradigm>();
	}

	/**
	 * Get a trait of a certain type from a currently iterated subject.
	 * Templated version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The access safety paradigm to use.
	 * @return A reference to the trait.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitRefResultSecurity<Paradigm, T> = true >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRef() const
	{
		return GetSubject().template GetTraitRef<Paradigm, T>();
	}

	/**
	 * Get a trait of a certain type from a currently iterated subject.
	 * Templated version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The access safety paradigm to use.
	 * @return A reference to the trait.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default,
			    TTraitRefResultSecurity<Paradigm, T> = true >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRef() const
	{
		return GetSubject().template GetTraitRef<T, Paradigm>();
	}

#pragma region Multi-Trait Data Access
	/// @name Multi-Trait Data Access
	/// @{

	/**
	 * Get a list of trait pointers into an array of immutable data.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @param TraitType The type of the trait to get.
	 * @param OutTraits The results receiver.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm   = EParadigm::Default,
			   typename  AllocatorT = FDefaultAllocator,
			   TTraitVoidPtrResultSecurity<Paradigm> = true >
	TOutcome<Paradigm>
	GetTraitsPtrs(UScriptStruct* const             TraitType,
				 TArray<const void*, AllocatorT>& OutTraits) const
	{
		return GetSubject()->template GetTraitsPtrs<Paradigm>(TraitType, OutTraits);
	}

	/**
	 * Get a list of trait pointers.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @param TraitType The type of the trait to get.
	 * @param OutTraits The results receiver.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm   = EParadigm::Default,
			   typename  AllocatorT = FDefaultAllocator,
			   TTraitVoidPtrResultSecurity<Paradigm> = true,
			   more::enable_if_t<AllowsChanges || IsUnsafe(Paradigm), bool> = true >
	TOutcome<Paradigm>
	GetTraitsPtrs(UScriptStruct* const       TraitType,
				  TArray<void*, AllocatorT>& OutTraits) const
	{
		return GetSubject()->template GetTraitsPtrs<Paradigm>(TraitType, OutTraits);
	}

	/**
	 * Get a list of pointers to immutable traits data.
	 * Statically-typed version.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @param OutTraits The results receiver.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm   = EParadigm::Default,
			   typename  T          = void,
			   typename  AllocatorT = FDefaultAllocator,
			   TTraitPtrResultSecurity<Paradigm, T> = true >
	FORCEINLINE auto
	GetTraitsPtrs(TArray<const T*, AllocatorT>& OutTraits) const
	{
		return GetSubject()->template GetTraitsPtrs<Paradigm>(OutTraits);
	}

	/**
	 * Get a list of pointers to mutable traits data.
	 * Statically-typed version.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @param OutTraits The results receiver.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm   = EParadigm::Default,
			   typename  T          = void,
			   typename  AllocatorT = FDefaultAllocator,
			   TTraitPtrResultSecurity<Paradigm, T> = true >
	FORCEINLINE auto
	GetTraitsPtrs(TArray<T*, AllocatorT>& OutTraits) const
	{
		return GetSubject()->template GetTraitsPtrs<Paradigm>(OutTraits);
	}

	/**
	 * Get a list of trait pointers by their common type.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @param TraitType The type of the trait to get.
	 * @return The resulting list of traits.
	 */
	template < EParadigm Paradigm   = EParadigm::Default,
			   typename  AllocatorT = FDefaultAllocator,
			   TTraitVoidPtrResultSecurity<Paradigm> = true >
	FORCEINLINE auto
	GetTraitsPtrs(UScriptStruct* const TraitType) const
	{
		return GetSubject()->template GetTraitsPtrs<Paradigm>(TraitType);
	}

	/**
	 * Get a list of trait pointers.
	 * Mutable data version.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of traits to get.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @return The resulting list of traits.
	 */
	template < EParadigm Paradigm,
			   typename  T,
			   typename  AllocatorT = FDefaultAllocator,
			   TTraitPtrResultSecurity<Paradigm, T> = true >
	FORCEINLINE auto
	GetTraitsPtrs() const
	{
		return GetSubject()->template GetTraitsPtrs<Paradigm, T, AllocatorT>();
	}

	/**
	 * Get a list of trait pointers.
	 * Mutable data default paradigm version.
	 *
	 * Respects the inheritance.
	 *
	 * @tparam T The type of traits to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam AllocatorT The type of the allocator used in the results receiver.
	 * @return The resulting list of traits.
	 */
	template < typename  T,
			   EParadigm Paradigm   = EParadigm::Default,
			   typename  AllocatorT = FDefaultAllocator,
			   TTraitPtrResultSecurity<Paradigm, T> = true >
	FORCEINLINE auto
	GetTraitsPtrs() const
	{
		return GetSubject()->template GetTraitsPtrs<Paradigm, T, AllocatorT>();
	}

	/// @}
#pragma endregion Multi-Trait Data Access

	/// @}
#pragma endregion Traits Data Access

#pragma region Traits Getting
	/// @name Traits Getting
	/// @{

	/**
	 * Get a trait of a certain type from a currently iterated subject.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to get.
	 * @param OutTraitData The trait data receiver.
	 * @param bTraitDataInitialized Is the @p OutTraitData buffer actually initialized?
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	GetTrait(UScriptStruct* const TraitType,
			 void* const          OutTraitData,
			 const bool           bTraitDataInitialized = true) const
	{
		return GetSubject().template GetTrait<Paradigm>(TraitType, OutTraitData, bTraitDataInitialized);
	}

	/**
	 * Get a trait of a certain type from a currently iterated subject.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @param OutTrait The trait receiver.
	 * @param bTraitInitialized Is the @p OutTrait actually initialized?
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm>
	GetTrait(T&         OutTrait,
			 const bool bTraitInitialized = true) const
	{
		return GetSubject().template GetTrait<Paradigm>(OutTrait, bTraitInitialized);
	}

	/**
	 * Get a trait of a certain type from a currently iterated subject.
	 * 
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @param OutTrait The trait receiver.
	 * @param bTraitInitialized Is the @p OutTrait actually initialized?
	 * @return The outcome of the operation.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	GetTrait(T&         OutTrait,
			 const bool bTraitInitialized = true) const
	{
		return GetSubject().template GetTrait<T, Paradigm>(OutTrait, bTraitInitialized);
	}

	/**
	 * Get a copy of a trait of a certain type from a currently iterated subject.
	 * Copy-returning version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A copy of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE auto
	GetTrait() const
	{
		return GetSubject().template GetTrait<Paradigm, T>();
	}

	/**
	 * Get a copy of a trait of a certain type from a currently iterated subject.
	 * Copy-returning default paradigm version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @return A copy of the trait.
	 */
	template < typename T, EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE auto
	GetTrait() const
	{
		return GetSubject().template GetTrait<T, Paradigm>();
	}

	/// @}
#pragma endregion Traits Getting

	/**
	 * Advance to the next iteration.
	 * 
	 * @return The status of the operation.
	 */
	inline EApparatusStatus
	Advance()
	{
		check(IsViable());
		check(Belt->IsLocked());

		const FBeltSlot& Slot = GetSlot();

		// We have iterated all of the available combintations within the slot
		// and now have to advance to the next slot (if any)...

		// End testing with overflow protection...
		if (UNLIKELY(SlotIndex >= (Belt->IterableCount - 1))) // Overflow protection
		{
			// The iterator was already at the last slot available...
			MoveToEnd();
			return EApparatusStatus::NoMore;
		}

		// Safely advance to the next slot now:
		SlotIndex += 1;

		// Skip invalid slots...
		while (!Belt->Slots[SlotIndex].PrepareForIteration(Filter, MainlineIndices))
		{
			if (UNLIKELY(SlotIndex >= (Belt->IterableCount - 1))) // Overflow protection
			{
				// The end was reached while skipping...
				MoveToEnd();
				return EApparatusStatus::NoMore;
			}
			SlotIndex += 1;
		}
		
		return EApparatusStatus::Success;
	} //-Advance()

	/**
	 * Get the next iterator (prefix version).
	 */
	FORCEINLINE TBeltIt&
	operator++()
	{
		VerifyOK(Advance());
		return *this;
	}

	/**
	 * Get the next iterator (postfix version).
	 */
	FORCEINLINE TBeltIt
	operator++(int)
	{
		TBeltIt Save = *this;
		VerifyOK(Advance());
		return MoveTemp(Save);
	}

	FORCEINLINE bool
	operator==(const TBeltIt& Other) const
	{
		if (UNLIKELY(std::addressof(Other) == this))
			return true;

		checkf(Belt != nullptr,
			   TEXT("Comparing an iterator without an owner is not supported."));
		checkf(Belt == Other.Belt,
			   TEXT("Comparing iterators with different owning belts is not supported."));

		if (!IsViable() && !Other.IsViable())
			return true;
		if (SlotIndex != Other.SlotIndex)
			return false;
		return true;
	}

	FORCEINLINE bool
	operator!=(const TBeltIt& Other) const
	{
		if (UNLIKELY(std::addressof(Other) == this))
			return false;

		checkf(Belt != nullptr,
			   TEXT("Comparing an iterator without an owner is not supported."));
		checkf(Belt == Other.Belt,
			   TEXT("Comparing iterators with different owning belts is not supported."));

		if (IsViable() != Other.IsViable())
			return true;
		if (SlotIndex != Other.SlotIndex)
			return true;
		return false;
	}

	/**
	 * Destroy the iterator unlocking the belt
	 * if necessary.
	 */
	FORCEINLINE
	~TBeltIt()
	{
		Unlock();
	}

}; //-struct TBeltIt

/**
 * Mutable subject belt iterator.
 */
typedef TBeltIt<FSubjectHandle> FBeltIt;

/**
 * Homogenous subject belt iterator.
 */
typedef TBeltIt<FSolidSubjectHandle> FSolidBeltIt;
