/*
 * ░▒▓ APPARATUS ▓▒░
 *
 * File: Chain.h
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

#include <array>
#include <atomic>
#include <tuple>
#include <functional>
#include <utility>
#include "More/type_traits"

#include "CoreMinimal.h"
#include "HAL/UnrealMemory.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "UObject/Class.h"
#include "Containers/Union.h"
#include "UObject/WeakInterfacePtr.h"
#include "Math/UnrealMathUtility.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define CHAIN_H_SKIPPED_MACHINE_H
#endif

#include "CommonChain.h"
#include "ChunkIt.h"
#include "BeltIt.h"
#include "SmartCast.h"
#include "RunnableMechanic.h"

#ifdef CHAIN_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif


// Forward declarations:
class UApparatusFunctionLibrary;
class AMechanism;
class UMachine;

/**
 * A group of iterable locked together in a sequence.
 * 
 * Chains internally consist of segments and are iterated by the cursors.
 * 
 * @note The chains are auto-disposed when there are no more
 * active cursors iterating on them.
 * Use explicit TChain::Retain() / TChain::Release() calls to
 * manage the lifetime of the chain in a manual manner.
 * 
 * @tparam ChunkItT The type of the chunk iterator used.
 * @tparam BeltItT The type of the belt iterator used.
 * @tparam InDefaultParadigm The access security paradigm to use by default.
 */
template < typename ChunkItT, typename BeltItT, EParadigm InDefaultParadigm >
struct TChain final
  : public FCommonChain
{

  public:

	using Super = FCommonChain;

	typedef ChunkItT ChunkItType;
	typedef BeltItT  BeltItType;

	/**
	 * Is the chain a solid-locking one?
	 */
	static constexpr bool IsSolid = ChunkItType::IsSolid;

	/**
	 * Are subjects of the chain mutable?
	 */
	static constexpr bool AllowsChanges = ChunkItType::AllowsChanges;

	/**
	 * Are structural changes (adding/removing components) allowed?
	 */
	static constexpr bool AllowsStructuralChanges = ChunkItType::AllowsStructuralChanges;

	/**
	 * Is direct access to the traits data allowed?
	 */
	static constexpr bool AllowsDirectTraitAccess = ChunkItType::AllowsDirectTraitAccess;

	/**
	 * The default access security paradigm.
	 */
	static constexpr EParadigm DefaultParadigm = InDefaultParadigm;

	/**
	 * The subject handle argument type.
	 */
	typedef typename ChunkItT::SubjectHandleType SubjectHandleType;

	/**
	 * A subject handle type, specifically incompatible
	 * with the current one used.
	 */
	typedef typename std::conditional<std::template is_same<SubjectHandleType, FSolidSubjectHandle>::value,
									  FSubjectHandle,
									  void>::type
		IncompatibleSubjectHandleType;

	/**
	 * The subjective argument type, including the possible 'const' specification
	 *
	 * Doesn't include the pointer part.
	 */
	typedef typename ChunkItT::SubjectivePtrType SubjectivePtrType;


	static_assert(ChunkItType::IsSolid == BeltItType::IsSolid,
				  "Iterator solidity must match.");
	static_assert(std::is_same<typename ChunkItT::SubjectHandleType,
							   typename BeltItT::SubjectHandleType>(),
				  "Iterator subject handle types must match.");
	static_assert(std::is_same<typename ChunkItT::SubjectivePtrType,
							   typename BeltItT::SubjectivePtrType>(),
				  "Iterator subjective types must match.");

	/**
	 * Protect the type of a trait void pointer returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 */
	template < EParadigm Paradigm >
	using TTraitVoidPtrResultSecurity = typename ChunkItT::template TTraitVoidPtrResultSecurity<Paradigm>;

	/**
	 * The type of a trait void pointer returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 */
	template < EParadigm Paradigm >
	using TTraitVoidPtrResult = typename ChunkItT::template TTraitVoidPtrResult<Paradigm>;

	/**
	 * Protect the type of a trait pointer returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitPtrResultSecurity = typename ChunkItT::template TTraitPtrResultSecurity<Paradigm, T>;

	/**
	 * The type of a trait pointer returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitPtrResult = typename ChunkItT::template TTraitPtrResult<Paradigm, T>;

	/**
	 * Protect the type of a trait reference returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitRefResultSecurity = typename ChunkItT::template TTraitRefResultSecurity<Paradigm, T>;

	/**
	 * The type of a trait reference returned by the methods.
	 * 
	 * @tparam Paradigm The access safety paradigm.
	 * @tparam T The type of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	using TTraitRefResult = typename ChunkItT::template TTraitRefResult<Paradigm, T>;

	/**
	 * The class of a detail pointer returned by the methods.
	 * 
	 * @tparam D The class of the detail.
	 */
	template < class D >
	using TDetailPtrResultSecurity = typename ChunkItT::template TDetailPtrResultSecurity<D>;

	/**
	 * The class of a detail pointer returned by the methods.
	 * 
	 * @tparam D The class of the detail.
	 */
	template < class D >
	using TDetailPtrResult = typename ChunkItT::template TDetailPtrResult<D>;

  private:

	friend class UApparatusFunctionLibrary;
	friend class AMechanism;
	friend class UMachine;

	using FSegment = Super::TSegment<IsSolid>;

  public:

	/**
	 * A unique identifier of an embedded
	 * chain.
	 */
	FORCEINLINE int32
	GetId() const
	{
		return Id;
	}

	/**
	 * A state of the chain iterating.
	 *
	 * Cursors also unify the component hinting indices
	 * across different chunks/belts.
	 *
	 * @tparam Ps The predefined types of parts to obtain efficiently while iterating.
	 */
	template < typename... Ps >
	struct TCursor final
	  : public FCommonCursor
	{
	  public:

		/**
		 * The base cursor type.
		 */
		using CursorSuper = FCommonCursor;

		/**
		 * The common iterator type.
		 */
		typedef TVariant<FEmptyVariantState, ChunkItType, BeltItType> IteratorType;

		/**
		 * The index of the chunk iterator within the variant type.
		 */
		static constexpr auto ChunkItIndex = IteratorType::template IndexOfType<ChunkItType>();

		/**
		 * The index of the belt iterator within the variant type.
		 */
		static constexpr auto BeltItIndex = IteratorType::template IndexOfType<BeltItType>();

	  private:

		template < typename... OtherPs >
		friend struct TCursor;

		/**
		 * The current iterator (either for a chunk or a belt).
		 */
		IteratorType Iterator;

		/**
		 * The type of the array allocator used within the mapping array.
		 */
		typedef typename std::conditional<(sizeof...(Ps) > 0), TInlineAllocator<sizeof...(Ps)>, FDefaultAllocator>::type MappingAllocatorType;

		/**
		 * The type of the mapping array.
		 */
		typedef TArray<int32, MappingAllocatorType> MappingType;

		/**
		 * The mapping array used to hint the parts.
		 */
		MappingType Mapping;

		/**
		 * Begin the iteration of the target chunk or belt using a certain filter.
		 * 
		 * This procedure also performs the necessary checking against the limit.
		 * 
		 * @param Iterable The iterable to iterate on.
		 * @param InFilter The filter to begin iterating under.
		 * @param LocalOffset The starting segment-local slot offset to apply.
		 * @return EApparatusStatus::NoMore
		 * If there are no slots available to iterate within the iterable.
		 */
		EApparatusStatus
		BeginIterator(TScriptInterface<IIterable> Iterable,
					  const FFilter&              InFilter,
					  const int32                 LocalOffset = 0)
		{
			check(Owner);
			if (UNLIKELY(!Iterable))
			{
				return EApparatusStatus::NullArgument;
			}
			if (UNLIKELY(ShouldStop()))
			{
				return EApparatusStatus::NoMore;
			}
			EApparatusStatus Status = EApparatusStatus::Noop;
			if (UChunk* const Chunk = Cast<UChunk>(Iterable.GetObject()))
			{
				auto ChunkIterator = Chunk->Begin<ChunkItType>(InFilter, LocalOffset);
				const int32 StartingSlotIndex = ChunkIterator.GetSlotIndex();
				if (LIKELY(ChunkIterator))
				{
					// The start of the chunk iterating was successful,
					// but we still have to check if the limit was reached...
					check(StartingSlotIndex >= LocalOffset);
					SlotIndex += StartingSlotIndex - LocalOffset + 1;
					if (UNLIKELY(IsLimitReached()))
					{
						ChunkIterator.MoveToEnd();
						Status = EApparatusStatus::NoMore;
					}
					else
					{
						// Fetch the actual mapping to use.
						if (sizeof...(Ps) > 0) // Compile-time branch.
						{
							Mapping = MappingType({Chunk->template TraitLineIndexOf<more::flatten_t<Ps>>()...});
						}
						else
						{
							Chunk->GetTraitmark().FindMappingFrom(InFilter.GetTraitmark(), Mapping);
						}
						Status = EApparatusStatus::Success;
					}
				}
				else
				{
					Status = EApparatusStatus::NoMore;
				}
				Iterator.template Set<ChunkItType>(ChunkIterator);
			}
			else if (UBelt* const Belt = Cast<UBelt>(Iterable.GetObject()))
			{
				auto BeltIterator = Belt->Begin<BeltItType>(InFilter, LocalOffset);
				const int32 StartingSlotIndex = BeltIterator.GetSlotIndex();
				if (LIKELY(BeltIterator))
				{
					// The start of the belt iterating was successful,
					// but we still have to check if the limit was reached...
					check(StartingSlotIndex >= LocalOffset);
					SlotIndex += StartingSlotIndex - LocalOffset + 1;
					if (UNLIKELY(IsLimitReached()))
					{
						BeltIterator.MoveToEnd();
						Status = EApparatusStatus::NoMore;
					}
					else
					{
						// Fetch the actual mapping to use.
						if (sizeof...(Ps) > 0) // Compile-time branch.
						{
							Mapping = MappingType({Belt->template DetailLineIndexOf<Ps>()...});
						}
						else
						{
							Belt->GetDetailmark().FindMappingFrom(InFilter.GetDetailmark(), Mapping);
						}
						Status = EApparatusStatus::Success;
					}
				}
				else
				{
					Status = EApparatusStatus::NoMore;
				}
				Iterator.template Set<BeltItType>(BeltIterator);
			}
			else 
			{
				Status = Report<EApparatusStatus::InvalidArgument>(
					TEXT("Unknown iterable type to begin the chain cursor with."));
			}
			return Status;
		}

		/**
		 * Begin the iteration of the target chunk or belt using the currently applied filter.
		 * 
		 * This procedure also performs the necessary checking against the limit.
		 * 
		 * @param Iterable The iterable to iterate on.
		 * @param LocalOffset The starting segment-local slot offset to apply.
		 * @return EApparatusStatus::NoMore
		 * If there are no slots available to iterate within the iterable.
		 */
		FORCEINLINE EApparatusStatus
		BeginIterator(TScriptInterface<IIterable> Iterable,
					  const int32                 LocalOffset = 0)
		{
			return BeginIterator(Iterable, Owner->GetFilter(), LocalOffset);
		}

		/**
		 * Advance the internal iterator.
		 */
		FORCEINLINE EApparatusStatus
		AdvanceIterator()
		{
			check(Owner);
			check(SlotIndex >= 0);
			check(Limit > 0);
			check(!IsLimitReached());

			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: 
				{
					auto& ChunkIterator = Iterator.template Get<ChunkItType>();
					if (UNLIKELY(Owner->ShouldStopIterating()))
					{
						ChunkIterator.MoveToEnd();
						return EApparatusStatus::NoMore;
					}
					const int32 IndexSave = ChunkIterator.GetSlotIndex();
					const auto Status = ChunkIterator.Advance();
					if (LIKELY(Status == EApparatusStatus::Success))
					{
						const int32 IndexDelta = ChunkIterator.GetSlotIndex() - IndexSave;
						check(IndexDelta > 0);
						SlotIndex += IndexDelta;
						if (UNLIKELY(IsLimitReached()))
						{
							ChunkIterator.MoveToEnd();
							return EApparatusStatus::NoMore;
						}
					}
					return Status;
				}
				case BeltItIndex: 
				{
					auto& BeltIterator = Iterator.template Get<BeltItType>();
					if (UNLIKELY(Owner->ShouldStopIterating()))
					{
						BeltIterator.MoveToEnd();
						return EApparatusStatus::NoMore;
					}
					const int32 IndexSave = BeltIterator.GetSlotIndex();
					const auto Status = BeltIterator.Advance();
					if (LIKELY(Status == EApparatusStatus::Success))
					{
						const int32 IndexDelta = BeltIterator.GetSlotIndex() - IndexSave;
						// Combo sub-iterations are possible for belts,
						// so a zero delta is possible here:
						check(IndexDelta >= 0); 
						if (LIKELY(IndexDelta > 0))
						{
							SlotIndex += IndexDelta;
							if (UNLIKELY(IsLimitReached()))
							{
								BeltIterator.MoveToEnd();
								return EApparatusStatus::NoMore;
							}
						}
					}
					return Status;
				}
				default:
				{
					checkNoEntry();
					return EApparatusStatus::InvalidState;
				}
			}
		}

	  public:

		/**
		 * Get the current size of the hinting mapping.
		 * 
		 * This size is dependent on what is actually
		 * used as a mapping basis, i.e. a template pack of parts
		 * or a current filter's traitmark/detailmark.
		 */
		FORCEINLINE constexpr int32
		GetPartMappingSize() const
		{
			if (sizeof...(Ps) > 0) // Compile-time branch.
			{
				return sizeof...(Ps);
			}
			else
			{
				return Mapping.Num();
			}
		}

		/**
		 * Remap a part index hint using the current mapping.
		 */
		template < EParadigm RemappingParadigm = EParadigm::Safe >
		FORCEINLINE int32
		RemapPartIndex(const int32 PartIndex) const
		{
			if (IsSafe(RemappingParadigm)) // Compile-time branch.
			{
				if (UNLIKELY(PartIndex < 0 || PartIndex >= GetPartMappingSize()))
				{
					return INDEX_NONE;
				}
			}
			return Mapping[PartIndex];
		}

		/**
		 * Move a cursor.
		 */
		FORCEINLINE TCursor&
		operator=(TCursor&& InCursor)
		{
			const auto OwnerSave = GetOwner();

			Owner        = InCursor.Owner;
			Offset       = InCursor.Offset;
			Limit        = InCursor.Limit;
			SlotIndex    = InCursor.SlotIndex;
			Iterator     = InCursor.Iterator;
			SegmentIndex = InCursor.SegmentIndex;
			Mapping      = MoveTemp(InCursor.Mapping);

			if (LIKELY(Owner))
			{
				InCursor.Owner = nullptr;
			}

			if (LIKELY(OwnerSave))
			{
				OwnerSave->Release();
			}

			return *this;
		}

		/**
		 * Copy a cursor.
		 */
		FORCEINLINE TCursor&
		operator=(const TCursor& InCursor)
		{
			if (UNLIKELY(this == std::addressof(InCursor)))
			{
				return *this;
			}
			const auto OwnerSave = Owner;

			Owner        = InCursor.Owner;
			Offset       = InCursor.Offset;
			Limit        = InCursor.Limit;
			SlotIndex    = InCursor.SlotIndex;
			Iterator     = InCursor.Iterator;
			SegmentIndex = InCursor.SegmentIndex;
			Mapping      = InCursor.Mapping;

			if (LIKELY(Owner))
			{
				Owner->Retain();
			}
			if (LIKELY(OwnerSave))
			{
				OwnerSave->Release();
			}

			return *this;
		}

		/**
		 * Get the owning chain of the cursor.
		 */
		FORCEINLINE const TChain*
		GetOwner() const
		{
			return static_cast<const TChain*>(CursorSuper::GetOwner());
		}

		/**
		 * Get the target sequence class.
		 */
		FORCEINLINE UClass*
		GetIterableClass() const
		{
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return UChunk::StaticClass();
				case BeltItIndex: return UBelt::StaticClass();
				default: return nullptr;
			}
		}

		/**
		 * Check if the cursor is viable and has not finished
		 * the iterating.
		 * 
		 * @return The state of examination.
		 */
		bool
		IsViable() const
		{
			if (LIKELY((Owner) && (SegmentIndex > InvalidSegmentIndex)))
			{
				// These must be guaranteed in case of being valid...
				check(!IsLimitReached());
				check(Iterator.GetIndex() != INDEX_NONE);
				check(SegmentIndex < GetOwner()->Segments.Num());
				checkCode
				(
					// Iterators must also be valid...
					switch (Iterator.GetIndex())
					{
						case ChunkItIndex: check(Iterator.template Get<ChunkItType>().IsViable());
							break;
						case BeltItIndex: check(Iterator.template Get<BeltItType>().IsViable());
							break;
					}
				)
				return true;
			}
			
			return false;
		}

		/**
		 * Check if the cursor is viable.
		 */
		FORCEINLINE
		operator bool() const
		{
			return IsViable();
		}

		/**
		 * Detach the cursor from the chain making
		 * it an invalid one.
		 * 
		 * The cursor will also release its owning chain as/if needed.
		 */
		void
		Reset()
		{
			Iterator     = IteratorType();
			SegmentIndex = InvalidSegmentIndex;
			SlotIndex    = InvalidSlotIndex;

			Offset = 0;
			Limit  = TNumericLimits<int32>::Max();

			// Resetting the owner must be at last,
			// cause of possible self-destruction due to
			// a release.
			if (LIKELY(Owner))
			{
				// Using a temporary because of possible destruction
				// due to owner's release.
				const auto OwnerSave = Owner;
				Owner = nullptr;
				OwnerSave->Release(); // This may actually trigger a disposal of the chain.
			}
		}

		/**
		 * Begin iterating with the cursor using the current range.
		 * 
		 * @return Does the first iteration makes sense?
		 */
		bool
		Begin()
		{
			checkf(Owner, TEXT("A cursor must have an owner to begin its iterating. "
							   "Has it already done its iterating and released the chain?"));
			checkf(!Owner->IsDisposed(), TEXT("The cursor's owner should not be disposed in order to begin iterating."));
			checkf(Offset >= 0, TEXT("The iterating offset must not be negative."));
			checkf(!IsViable(), TEXT("The cursor must not already be iterating to start anew."));

			if (UNLIKELY(Limit == 0)) return false;
			int32 ChainLimit = Offset + Limit;
			if (UNLIKELY(ChainLimit <= Offset)) ChainLimit = TNumericLimits<int32>::Max(); // Overflow.
			int32 ChainSegmentOffset = 0;
			for (SegmentIndex = 0; (SegmentIndex < GetOwner()->Segments.Num()) && (ChainSegmentOffset < ChainLimit); ++SegmentIndex)
			{
				const FSegment& Segment = GetOwner()->Segments[SegmentIndex];
				check(Segment.IsLocked());
				const int32 Count = Segment.IterableNum();
				if (UNLIKELY(Count == 0)) continue;
				// Points to the slot past the last one:
				const int32 ChainSegmentEnd = ChainSegmentOffset + Count;
				checkf(ChainSegmentEnd > ChainSegmentOffset,
					   TEXT("The segment ending has overflown. Too many subjects in the mechanism?"));
				if (ChainSegmentEnd > Offset)
				{
					int32 LocalOffset = 0; // Start from the beginning of the segment by default.
					if (ChainSegmentOffset < Offset) 
					{
						LocalOffset = Offset - ChainSegmentOffset;
					}

					const auto Status = BeginIterator(GetOwner()->Segments[SegmentIndex].GetIterable(), LocalOffset);
					if (LIKELY(OK(Status)))
					{
						// Everything should be fine here:
						check(IsViable());
						return true;
					}
					// The iterator could actually fail to start,
					// cause the subjects' flagmark matching failure,
					// which is tested iterating-time, but we still
					// continue to search for the next viable segment then.
					// (though within the limit of course).
				}
				ChainSegmentOffset = ChainSegmentEnd;
			}

			// Failed to find any compatible iterations...
			Reset();
			return false;
		}

		/**
		 * Begin iterating with the cursor using the supplied range.
		 * 
		 * @param InOffset The slot offset to begin iterating with.
		 * @param InLimit The maximum number of slots to iterate by.
		 * @return Does the first iteration makes sense?
		 */
		FORCEINLINE bool
		Begin(const int32 InOffset,
			  const int32 InLimit = TNumericLimits<int32>::Max())
		{
			checkf(Owner, TEXT("A cursor must have an owner to begin its iterating. "
							   "Has it already done its iterating and released the chain?"));
			checkf(!Owner->IsDisposed(), TEXT("The cursor's owner should not be disposed in order to begin iterating."));
			checkf(InOffset >= 0, TEXT("The iterating offset must not be negative."));
			checkf(InLimit > 0, TEXT("The iterating limit must be greater than zero."));
			checkf(!IsViable(), TEXT("The cursor must not already be iterating to start anew."));

			Offset = InOffset;
			Limit  = InLimit;

			return Begin();
		}

		/**
		 * Advance the iterator to the next slot.
		 * 
		 * @return Does the next iteration makes sense?
		 */
		FORCEINLINE bool
		Advance()
		{
			checkf(Owner, TEXT("An owner must be present in order to advance the cursor. "
							   "Has it already done its iterating and released the chain?"));
			checkf(!Owner->IsDisposed(), TEXT("The owner should not be disposed in order to advance the iteration."));

			auto Status = AdvanceIterator();
			if (LIKELY(Status == EApparatusStatus::Success))
			{
				check(IsViable());
				return true;
			}

			// No more subjects in the current iterable.
			// Try the next one (if there is)...
			while (++SegmentIndex < GetOwner()->Segments.Num())
			{
				const auto& Segment = GetOwner()->Segments[SegmentIndex];
				Status = BeginIterator(Segment.GetIterable());
				if (LIKELY(Status == EApparatusStatus::Success))
				{
					// Successfully started the next segment iterating:
					return true;
				}
			}

			// The next suitable segment was not found.
			Reset();
			return false;
		}

		/**
		 * Begin or advance iterating the chain
		 * using this cursor.
		 * 
		 * @return Is the iteration possible? Returns @c false 
		 * if the iterating should be ended.
		 */
		FORCEINLINE bool
		Provide()
		{
			checkf(Owner,
				   TEXT("The owner must be set for the cursor to provide the next iteration. "
						"Has it already done its iterating and released the chain?"));
			if (LIKELY(IsViable()))
			{
				return Advance();
			}
			return Begin();
		}

		/**
		 * Get the subject of the current iteration.
		 */
		FORCEINLINE SubjectHandleType
		GetSubject() const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a subject. "
							   "Has it already done its iterating and released the chain?"));
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.GetSubject();
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.GetSubject();
				default:
				{
					checkNoEntry();
					return SubjectHandleType::Invalid;
				}
			}
		}

		/**
		 * Get the subjective of the current iteration of the cursor.
		 * 
		 * @return The subjective at the current cursor position.
		 */
		FORCEINLINE SubjectivePtrType
		GetSubjective() const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a subjective. "
							   "Has it already done its iterating and released the chain?"));
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.GetSubjective();
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.GetSubjective();
				default: 
				{
					checkNoEntry();
					return nullptr;
				}
			}
		}

		/**
		 * Get a trait pointer of a certain type at the current cursor position.
		 * 
		 * @tparam Paradigm The security paradigm to use.
		 * @return A pointer to the trait of the designated type.
		 */
		template < EParadigm Paradigm = DefaultParadigm,
				   more::enable_if_t<IsUnsafe(Paradigm) || AllowsDirectTraitAccess, int> = 0 >
		FORCEINLINE TOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>
		GetTraitPtr(UScriptStruct* const TraitType) const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a pointer of a trait. "
							   "Has it already done its iterating and released the chain?"));

			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetTraitPtr<Paradigm>(TraitType);
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTraitPtr<Paradigm>(TraitType);
				default: 
				{
					checkNoEntry();
					return nullptr;
				}
			}
		}

		/**
		 * Get a trait pointer of a certain type.
		 * Templated version.
		 * 
		 * @tparam Paradigm The security paradigm to use.
		 * @tparam T The type of the trait to get.
		 * @return A pointer to the trait of the designated type.
		 */
		template < EParadigm Paradigm, typename T,
				   more::enable_if_t<IsUnsafe(Paradigm) || AllowsDirectTraitAccess, int> = 0 >
		FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
		GetTraitPtr() const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait pointer. "
							   "Has it already done its iterating and released the chain?"));
			// Check if the trait is guaranteed to be within the subject:
			static constexpr bool bTraitGuaranteed = more::is_contained_flatly<T, Ps...>::value;
			if (bTraitGuaranteed) // Compile-time branch.
			{
				return TraitPtrAtLine<Paradigm, T, more::index_within_flatly<T, Ps...>::value>();
			}
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetTraitPtr<Paradigm, T>();
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTraitPtr<Paradigm, T>();
				default: 
				{
					checkNoEntry();
					return nullptr;
				}
			}
		}

		/**
		 * Get a trait pointer of a certain type.
		 * Templated version.
		 * 
		 * @tparam T The type of the trait to get.
		 * @tparam Paradigm The security paradigm to use.
		 * @return A pointer to the trait of the designated type.
		 */
		template < typename T, EParadigm Paradigm = DefaultParadigm,
				   more::enable_if_t<IsUnsafe(Paradigm) || AllowsDirectTraitAccess, int> = 0 >
		FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
		GetTraitPtr() const
		{
			return GetTraitPtr<Paradigm, T>();
		}

		/**
		 * Get a trait reference of a certain type.
		 */
		template < EParadigm Paradigm, typename T,
				   more::enable_if_t<IsUnsafe(Paradigm) || AllowsDirectTraitAccess, int> = 0 >
		FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
		GetTraitRef() const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait reference hinted. "
							   "Has it already done its iterating and released the chain?"));
			// Check if the trait is guaranteed to be within the subject:
			static constexpr bool bTraitGuaranteed = more::is_contained_flatly<T, Ps...>::value;
			if (bTraitGuaranteed) // Compile-time branch.
			{
				return TraitRefAtLine<Paradigm, T, more::index_within_flatly<T, Ps...>::value>();
			}
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetTraitRef<Paradigm, T>();
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTraitRef<Paradigm, T>();
				default: 
				{
					checkNoEntry();
					return *(new T());
				}
			}
		}

		/**
		 * Get a trait reference of a certain type.
		 */
		template < typename T, EParadigm Paradigm = DefaultParadigm,
				   more::enable_if_t<IsUnsafe(Paradigm) || AllowsDirectTraitAccess, int> = 0 >
		FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
		GetTraitRef() const
		{
			return GetTraitRef<Paradigm, T>();
		}

		/**
		 * Get a trait copy of a certain type.
		 *
		 * @tparam Paradigm The paradigm to work under.
		 * @tparam T The type of the trait to get.
		 * @return A copy of the trait of the designated type.
		 */
		template < EParadigm Paradigm, typename T >
		FORCEINLINE TOutcome<Paradigm, T>
		GetTrait() const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait. "
							   "Has it already done its iterating and released the chain?"));
			// Check if the trait is guaranteed to be within the subject:
			static constexpr bool bTraitGuaranteed = more::is_contained_flatly<T, Ps...>::value;
			if (bTraitGuaranteed) // Compile-time branch.
			{
				return TraitAtLine<Paradigm, T, more::index_within_flatly<T, Ps...>::value>();
			}
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetTrait<Paradigm, T>();
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTrait<Paradigm, T>();
				default:
				{
					checkNoEntry();
					return T();
				}
			}
		}

		/**
		 * Get a trait copy of a certain type.
		 *
		 * @tparam T The type of the trait to get.
		 * @tparam Paradigm The paradigm to work under.
		 * @return A copy of the trait of the designated type.
		 */
		template < typename T, EParadigm Paradigm = DefaultParadigm >
		FORCEINLINE auto
		GetTrait() const
		{
			return GetTrait<Paradigm, T>();
		}

		/**
		 * Get a trait of a certain type.
		 */
		template < EParadigm Paradigm = DefaultParadigm>
		FORCEINLINE TOutcome<Paradigm>
		GetTrait(UScriptStruct* const TraitType,
				 void* const          OutTraitData,
				 const bool           bTraitDataInitialized = true) const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait. "
							   "Has it already done its iterating and released the chain?"));

			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetTrait<Paradigm>(TraitType, OutTraitData, bTraitDataInitialized);
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTrait<Paradigm>(TraitType, OutTraitData, bTraitDataInitialized);
				default:
				{
					checkNoEntry();
					return EApparatusStatus::InvalidState;
				}
			}
		}

		/**
		 * Get a trait of a certain type. Templated version.
		 */
		template < EParadigm Paradigm, typename T >
		FORCEINLINE TOutcome<Paradigm>
		GetTrait(T&         OutTraitData,
				 const bool bTraitDataInitialized = true) const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait. "
							   "Has it already done its iterating and released the chain?"));
			// Check if the trait is guaranteed to be within the subject:
			static constexpr bool bTraitGuaranteed = more::is_contained_flatly<T, Ps...>::value;
			if (bTraitGuaranteed) // Compile-time branch.
			{
				return GetTraitHinted<Paradigm, T>(more::index_within_flatly<T, Ps...>::value, OutTraitData, bTraitDataInitialized);
			}
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetTrait<Paradigm>(OutTraitData, bTraitDataInitialized);
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTrait<Paradigm>(OutTraitData, bTraitDataInitialized);
				default:
				{
					checkNoEntry();
					return EApparatusStatus::InvalidState;
				}
			}
		}


		/**
		 * Get a trait of a certain type. Templated version.
		 */
		template < typename T, EParadigm Paradigm = DefaultParadigm >
		FORCEINLINE auto
		GetTrait(T&         OutTraitData,
				 const bool bTraitDataInitialized = true) const
		{
			return GetTrait<Paradigm>(OutTraitData, bTraitDataInitialized);
		}

		/**
		 * Get a copy of a trait of a certain type with an index hint.
		 * 
		 * The trait index hint is relative to the parts pack or the traitmark of the filter.
		 */
		template < EParadigm Paradigm, typename T, int32 TraitLineIndex >
		FORCEINLINE TOutcome<Paradigm, T>
		TraitAtLine() const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait hinted. "
							   "Has it already done its iterating and released the chain?"));
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex:
					if (TraitLineIndex >= 0)
						return Iterator.template Get<ChunkItType>()
							.template TraitAtLine<Paradigm, T>(RemapPartIndex<EParadigm::Unsafe>(TraitLineIndex));
					else
						return Iterator.template Get<ChunkItType>()
							.template GetTrait<Paradigm, T>();
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTrait<Paradigm, T>();
				default: 
				{
					checkNoEntry();
					return T();
				}
			}
		}

		/**
		 * Get a pointer to a trait of a certain type with an index hint.
		 * Templated version.
		 * 
		 * The trait index hint is relative to the parts pack or the traitmark of the filter.
		 */
		template < EParadigm Paradigm, typename T, int32 TraitLineIndex,
				   more::enable_if_t<IsUnsafe(Paradigm) || AllowsDirectTraitAccess, int> = 0 >
		FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
		TraitPtrAtLine() const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait pointer hinted. "
							   "Has it already done its iterating and released the chain?"));
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: 
					if (TraitLineIndex >= 0)
					{
						const auto RealLineIndex = RemapPartIndex<EParadigm::Unsafe>(TraitLineIndex);
						if (RealLineIndex >= 0)
							return Iterator.template Get<ChunkItType>()
								.template TraitPtrAtLine<Paradigm, T>(RealLineIndex);
					}
					return Iterator.template Get<ChunkItType>()
						.template GetTraitPtr<Paradigm, T>();
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTraitPtr<Paradigm, T>();
				default: 
				{
					checkNoEntry();
					return nullptr;
				}
			}
		}

		/**
		 * Get a trait of a certain type with an index hint.
		 * 
		 * The trait index hint is relative to the parts pack or the traitmark of the filter.
		 */
		template < EParadigm Paradigm, typename T, int32 TraitLineIndex,
				   more::enable_if_t<IsUnsafe(Paradigm) || AllowsDirectTraitAccess, int> = 0 >
		FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
		TraitRefAtLine() const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait reference hinted. "
							   "Has it already done its iterating and released the chain?"));
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: 
					if (TraitLineIndex >= 0)
						return Iterator.template Get<ChunkItType>()
							.template TraitRefAtLine<Paradigm, T>(RemapPartIndex<EParadigm::Unsafe>(TraitLineIndex));
					else
						return Iterator.template Get<ChunkItType>()
							.template GetTraitRef<Paradigm, T>();
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTraitRef<Paradigm, T>();
				default: 
				{
					checkNoEntry();
					return *(new T());
				}
			}
		}

		/**
		 * Get a pointer to a trait of a certain type with an index hint.
		 * 
		 * The trait index hint is relative to the parts pack or the traitmark of the filter.
		 */
		template < EParadigm Paradigm = DefaultParadigm,
				   more::enable_if_t<IsUnsafe(Paradigm) || AllowsDirectTraitAccess, int> = 0 >
		FORCEINLINE TOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>
		GetTraitPtrHinted(UScriptStruct* const TraitType,
						  const int32          TraitIndexHint) const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait pointer hinted. "
							   "Has it already done its iterating and released the chain?"));
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetTraitPtrHinted<Paradigm>(TraitType, RemapPartIndex<Paradigm>(TraitIndexHint));
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTraitPtr<Paradigm>(TraitType);
				default: 
				{
					checkNoEntry();
					return nullptr;
				}
			}
		}

		/**
		 * Get a pointer to a trait of a certain type with an index hint.
		 * Templated version.
		 * 
		 * The trait index hint is relative to the parts pack or the traitmark of the filter.
		 */
		template < EParadigm Paradigm, typename T,
				   more::enable_if_t<IsUnsafe(Paradigm) || AllowsDirectTraitAccess, int> = 0 >
		FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
		GetTraitPtrHinted(const int32 TraitIndexHint) const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait pointer hinted. "
							   "Has it already done its iterating and released the chain?"));
			// Check if the trait is guaranteed to be within the subject:
			static constexpr bool bTraitGuaranteed = more::is_contained_flatly<T, Ps...>::value;
			if (bTraitGuaranteed) // Compile-time branch.
			{
				return TraitPtrAtLine<Paradigm, T, more::index_within_flatly<T, Ps...>::value>();
			}
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetTraitPtrHinted<Paradigm, T>(RemapPartIndex<Paradigm>(TraitIndexHint));
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTraitPtr<Paradigm, T>();
				default: 
				{
					checkNoEntry();
					return nullptr;
				}
			}
		}

		/**
		 * Get a pointer to a trait of a certain type with an index hint.
		 * Templated version.
		 * 
		 * The trait index hint is relative to the parts pack or the traitmark of the filter.
		 */
		template < typename T, EParadigm Paradigm = DefaultParadigm,
				   more::enable_if_t<IsUnsafe(Paradigm) || AllowsDirectTraitAccess, int> = 0 >
		FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
		GetTraitPtrHinted(const int32 TraitIndexHint) const
		{
			return GetTraitPtrHinted<Paradigm, T>(TraitIndexHint);
		}

		/**
		 * Get a trait of a certain type with an index hint.
		 * 
		 * The trait index hint is relative to the parts pack or the traitmark of the filter.
		 */
		template < EParadigm Paradigm, typename T,
				   more::enable_if_t<IsUnsafe(Paradigm) || AllowsDirectTraitAccess, int> = 0 >
		FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
		GetTraitRefHinted(const int32 TraitIndexHint) const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait reference hinted. "
							   "Has it already done its iterating and released the chain?"));
			// Check if the trait is guaranteed to be within the subject:
			static constexpr bool bTraitGuaranteed = more::is_contained_flatly<T, Ps...>::value;
			if (bTraitGuaranteed) // Compile-time branch.
			{
				return TraitRefAtLine<Paradigm, T, more::index_within_flatly<T, Ps...>::value>();
			}
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetTraitRefHinted<Paradigm, T>(RemapPartIndex<Paradigm>(TraitIndexHint));
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTraitRef<Paradigm, T>();
				default: 
				{
					checkNoEntry();
					return *(new T());
				}
			}
		}

		/**
		 * Get a trait of a certain type with an index hint.
		 * 
		 * The trait index hint is relative to the parts pack or the traitmark of the filter.
		 */
		template < typename T, EParadigm Paradigm = DefaultParadigm,
				   more::enable_if_t<IsUnsafe(Paradigm) || AllowsDirectTraitAccess, int> = 0 >
		FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
		GetTraitRefHinted(const int32 TraitIndexHint) const
		{
			return GetTraitRefHinted<Paradigm, T>(TraitIndexHint);
		}

		/**
		 * Get a copy of a trait of a certain type with an index hint.
		 * 
		 * The trait index hint is relative to the parts pack or the traitmark of the filter.
		 */
		template < EParadigm Paradigm, typename T >
		FORCEINLINE TOutcome<Paradigm, T>
		GetTraitHinted(const int32 TraitIndexHint) const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait hinted. "
							   "Has it already done its iterating and released the chain?"));
			// Check if the trait is guaranteed to be within the subject:
			static constexpr bool bTraitGuaranteed = more::is_contained_flatly<T, Ps...>::value;
			if (bTraitGuaranteed) // Compile-time branch.
			{
				return TraitAtLine<Paradigm, T, more::index_within_flatly<T, Ps...>::value>();
			}
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetTraitHinted<Paradigm, T>(RemapPartIndex(TraitIndexHint));
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTrait<Paradigm, T>();
				default: 
				{
					checkNoEntry();
					return T();
				}
			}
		}

		/**
		 * Get a copy of a trait of a certain type with an index hint.
		 * 
		 * The trait index hint is relative to the parts pack or the traitmark of the filter.
		 */
		template < typename T, EParadigm Paradigm = DefaultParadigm >
		FORCEINLINE TOutcome<Paradigm, T>
		GetTraitHinted(const int32 TraitIndexHint) const
		{
			return GetTraitHinted<Paradigm, T>(TraitIndexHint);
		}

		/**
		 * Get a trait of a certain type with an index hint.
		 * 
		 * The trait index hint is relative to the parts pack or the traitmark of the filter.
		 */
		template < EParadigm Paradigm = DefaultParadigm >
		FORCEINLINE TOutcome<Paradigm>
		GetTraitHinted(UScriptStruct* const TraitType,
					   const int32          TraitIndexHint,
					   void* const          OutTraitData,
					   const bool           bTraitDataInitialized = true) const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait hinted. "
							   "Has it already done its iterating and released the chain?"));
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetTraitHinted<Paradigm>(TraitType, RemapPartIndex(TraitIndexHint), OutTraitData, bTraitDataInitialized);
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTrait<Paradigm>(TraitType, OutTraitData, bTraitDataInitialized);
				default: 
				{
					checkNoEntry();
					return EApparatusStatus::InvalidState;
				}
			}
		}

		/**
		 * Get a trait of a certain type with an index hint.
		 * Templated version.
		 * 
		 * The trait index hint is relative to the parts pack or the traitmark of the filter.
		 */
		template < EParadigm Paradigm = DefaultParadigm, typename T = void >
		FORCEINLINE TOutcome<Paradigm>
		GetTraitHinted(const int32 TraitIndexHint,
					   T&          OutTraitData,
					   const bool  bTraitDataInitialized = true) const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a trait hinted. "
							   "Has it already done its iterating and released the chain?"));
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetTraitHinted<Paradigm>(RemapPartIndex(TraitIndexHint), OutTraitData, bTraitDataInitialized);
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetTrait<Paradigm>(OutTraitData, bTraitDataInitialized);
				default:
				{
					checkNoEntry();
					return EApparatusStatus::InvalidState;
				}
			}
		}

#pragma region Cursor Details

		/**
		 * Get a detail of a certain class at an index hinted.
		 *
		 * The detail index hint is relative to the parts pack or the detailmark of the filter.
		 *
		 * @tparam Paradigm The paradigm to work under.
		 * @param DetailClass The class of the detail to get.
		 * @param DetailIndexHint The hinting detail index.
		 * @return The detail of the specified class.
		 */
		template < EParadigm Paradigm = DefaultParadigm >
		FORCEINLINE TOutcome<Paradigm, UDetail*>
		GetDetailHinted(const TSubclassOf<UDetail> DetailClass,
						const int32                DetailIndexHint) const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a detail hinted. "
							   "Has it already done its iterating and released the chain?"));
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetDetail<Paradigm>(DetailClass);
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetDetailHinted<Paradigm>(DetailClass, RemapPartIndex(DetailIndexHint));
				default:
				{
					checkf(false, TEXT("Invalid cursor state to get a hinted detail from."));
					return nullptr;
				}
			}
		}

		/**
		 * Get a detail of a certain class at an index hinted. Templated version.
		 *
		 * The detail index hint is relative to the parts pack or the detailmark of the filter.
		 *
		 * @tparam Paradigm The paradigm to work under.
		 * @tparam D The class of the detail to get.
		 * @param DetailIndexHint The hinting detail index.
		 */
		template < EParadigm Paradigm, class D,
				   TDetailPtrResultSecurity<D> = 0 >
		FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<D>>
		GetDetailHinted(const int32 DetailIndexHint) const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a detail hinted. "
							   "Has it already done its iterating and released the chain?"));
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetDetail<Paradigm, D>();
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetDetailHinted<Paradigm, D>(RemapPartIndex(DetailIndexHint));
				default:
				{
					checkf(false, TEXT("Invalid cursor state to get a hinted detail from."));
					return nullptr;
				}
			}
		}

		/**
		 * Get a detail of a certain class at an index hinted. Templated version.
		 *
		 * The detail index hint is relative to the parts pack or the detailmark of the filter.
		 *
		 * @tparam D The class of the detail to get.
		 * @tparam Paradigm The paradigm to work under.
		 * @param DetailIndexHint The hinting detail index.
		 */
		template < class D, EParadigm Paradigm = DefaultParadigm >
		FORCEINLINE auto
		GetDetailHinted(const int32 DetailIndexHint) const
		{
			return GetDetailHinted<Paradigm, D>(DetailIndexHint);
		}

		/**
		 * Get a detail of a certain class.
		 * 
		 * @tparam Paradigm The paradigm to work under.
		 * @param DetailClass The class of detail to get. May be a base class.
		 * @return A pointer to the detail of the specified type.
		 * @return nullptr If there is no such detail within the current iteration.
		 */
		template < EParadigm Paradigm = DefaultParadigm,
				   TDetailPtrResultSecurity<UDetail> = 0 >
		FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<UDetail>>
		GetDetail(const TSubclassOf<UDetail> DetailClass) const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a detail. "
							   "Has it already done its iterating and released the chain?"));
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetDetail<Paradigm>(DetailClass);
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetDetail<Paradigm>(DetailClass);
				default:
				{
					checkf(false, TEXT("Invalid cursor state to get a detail from."));
					return MakeOutcome<Paradigm, TDetailPtrResult<UDetail>>(EApparatusStatus::InvalidState, nullptr);
				}
			}
		}

		/**
		 * Get a detail of a certain class.
		 * Statically typed version.
		 * 
		 * @tparam Paradigm The paradigm to work under.
		 * @tparam D The class of detail to get. May be a base class.
		 * @return A pointer to the detail of the specified type.
		 * @return nullptr If there is no such detail within the current iteration.
		 */
		template < EParadigm Paradigm, class D,
				   TDetailPtrResultSecurity<D> = 0 >
		FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<D>>
		GetDetail() const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a detail. "
							   "Has it already done its iterating and released the chain?"));
			if (more::is_contained_flatly<D, Ps...>::value) // Compile-time branch.
			{
				return GetDetailHinted<Paradigm, D>(more::index_within_flatly<D, Ps...>::value);
			}
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>()
					.template GetDetail<Paradigm, D>();
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template GetDetail<Paradigm, D>();
				default:
				{
					checkf(false, TEXT("Invalid cursor state to get a detail."));
					return MakeOutcome<Paradigm, TDetailPtrResult<D>>(EApparatusStatus::InvalidState, nullptr);
				}
			}
		}

		/**
		 * Get a detail of a certain class.
		 * Statically typed default paradigm version.
		 * 
		 * @tparam D The class of detail to get. May be a base class.
		 * @tparam Paradigm The paradigm to work under.
		 * @return A pointer to the detail of the specified type.
		 * @return nullptr If there is no such detail within the current iteration.
		 */
		template < class D, EParadigm Paradigm = DefaultParadigm,
				   TDetailPtrResultSecurity<D> = 0 >
		FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<D>>
		GetDetail() const
		{
			return GetDetail<Paradigm, D>();
		}

		/**
		 * Get a detail at a certain line index.
		 * 
		 * @tparam Paradigm The paradigm to work under.
		 * @param LineIndex The detail line index.
		 * @return The detail at the specified index.
		 */
		template < EParadigm Paradigm = DefaultParadigm,
				   TDetailPtrResultSecurity<UDetail> = 0 >
		FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<UDetail>>
		DetailAtLine(const int32 LineIndex) const
		{
			checkf(Owner, TEXT("The cursor must have an owner in order to get a detail at a specific index. "
							   "Has it already done its iterating and released the chain?"));
			switch (Iterator.GetIndex())
			{
				case BeltItIndex: return Iterator.template Get<BeltItType>()
					.template DetailAtLine<Paradigm>(RemapPartIndex(LineIndex));
				default: 
				{
					checkf(false, TEXT("Invalid cursor state to get a detail at line."));
					return MakeOutcome<Paradigm, TDetailPtrResult<UDetail>>(EApparatusStatus::InvalidState, nullptr);
				}
			}
		}

#pragma endregion Cursor Details

	  private:

#pragma region Part Switching

		enum EPartType
		{
			PT_Other = 0,
			PT_Flagmark,
			PT_Trait,
			PT_Detail,
			PT_SubjectHandle,
			PT_Subjective,
			PT_Cursor
		};

		enum EPartDeliveryType
		{
			PDT_None = 0,
			PDT_Value,
			PDT_Reference,
			PDT_Pointer
		};

		template< class P,
				  bool IsValue     = std::is_same< typename std::remove_cv<P>::type,
												   typename more::flatten<P>::type >::value,
				  bool IsReference = std::is_reference<P>::value,
				  bool IsPointer   = std::is_pointer<P>::value >
		struct TPartDeliverySwitcher
		{
			static constexpr enum EPartType Value = PDT_None;
		};

		template< class P >
		struct TPartDeliverySwitcher<P, true, false, false>
		{
			static constexpr enum EPartDeliveryType Value = PDT_Value;
		};

		template< class P >
		struct TPartDeliverySwitcher<P, false, true, false>
		{
			static constexpr enum EPartDeliveryType Value = PDT_Reference;
		};

		template< class P >
		struct TPartDeliverySwitcher<P, false, false, true>
		{
			static constexpr enum EPartDeliveryType Value = PDT_Pointer;
		};

		template< class P,
				  bool IsFlagmark      = ::IsFlagmarkType     <typename more::flatten<P>::type>(),
				  bool IsTrait         = ::IsTraitType        <typename more::flatten<P>::type>(),
				  bool IsDetail        = ::IsDetailClass      <typename more::flatten<P>::type>(),
				  bool IsSubjectHandle = ::IsSubjectHandleType<typename more::flatten<P>::type>(),
				  bool IsSubjective    = ::IsSubjectiveClass  <typename more::flatten<P>::type>(),
				  bool IsCursor        = ::IsChainCursorType  <typename more::flatten<P>::type>() >
		struct TPartSwitcher
		{ static constexpr enum EPartType Value = PT_Other; };

		template< class P >
		struct TPartSwitcher<P, true, false, false, false, false, false>
		{ static constexpr enum EPartType Value = PT_Flagmark; };

		template< class P >
		struct TPartSwitcher<P, false, true, false, false, false, false>
		{ static constexpr enum EPartType Value = PT_Trait; };

		template< class P >
		struct TPartSwitcher<P, false, false, true, false, false, false>
		{ static constexpr enum EPartType Value = PT_Detail; };

		template< class P >
		struct TPartSwitcher<P, false, false, false, true, false, false>
		{ static constexpr enum EPartType Value = PT_SubjectHandle; };

		template< class P >
		struct TPartSwitcher<P, false, false, false, false, true, false>
		{ static constexpr enum EPartType Value = PT_Subjective; };

		template< class P >
		struct TPartSwitcher<P, false, false, false, false, false, true>
		{ static constexpr enum EPartType Value = PT_Cursor; };

		template< EPartType Part, bool Defer = true >
		struct TStaticSignaler
		{
			static constexpr bool Do()
			{
				static_assert(!Defer, "Unknown/other part type detected.");
				return true;
			}
		};

		template< bool Defer >
		struct TStaticSignaler<EPartType::PT_Cursor, Defer>
		{
			static constexpr bool Do()
			{
				static_assert(!Defer, "Cursor part was detected.");
				return true;
			}
		};

		template< bool Defer >
		struct TStaticSignaler<EPartType::PT_Flagmark, Defer>
		{
			static constexpr bool Do()
			{
				static_assert(!Defer, "Flagmark part was detected.");
				return true;
			}
		};

		template< bool Defer >
		struct TStaticSignaler<EPartType::PT_Trait, Defer>
		{
			static constexpr bool Do()
			{
				static_assert(!Defer, "Trait part was detected.");
				return true;
			}
		};

		template< bool Defer >
		struct TStaticSignaler<EPartType::PT_Detail, Defer>
		{
			static constexpr bool Do()
			{
				static_assert(!Defer, "Detail part was detected.");
				return true;
			}
		};

		template< bool Defer >
		struct TStaticSignaler<EPartType::PT_SubjectHandle, Defer>
		{
			static constexpr bool Do()
			{
				static_assert(!Defer, "Subject handle part was detected.");
				return true;
			}
		};

		template< bool Defer >
		struct TStaticSignaler<EPartType::PT_Subjective, Defer>
		{
			static constexpr bool Do()
			{
				static_assert(!Defer, "Subjective part was detected.");
				return true;
			}
		};

#pragma endregion Part Switching

		/**
		 * Generic subject part getter.
		 *
		 * This also supports getting the current
		 * cursor and the current subject.
		 *
		 * @tparam P The type of the part to get. May contain all of the reference/pointer specifiers.
		 * @tparam Part The type of part to supply.
		 * @tparam Delivery The type of delivery to use.
		 * @tparam ArgumentTypeCheck This is to delay an actual type
		 * derivation for static asserts to work correctly. If you
		 * don't want static asserts, pass false here.
		 */
		template < class                  P,
				   enum EPartType         InPart            = TPartSwitcher<P>::Value,
				   enum EPartDeliveryType InDelivery        = TPartDeliverySwitcher<P>::Value,
				   bool                   ArgumentTypeCheck = true >
		struct TPartGetter
		{
			static_assert(!ArgumentTypeCheck,
						  "Invalid part type to get. "
						  "Make sure you're specifying either of these: chain, cursor, subject handle, subjective, flagmark, trait, detail.");

			static constexpr bool Check = TStaticSignaler<InPart, ArgumentTypeCheck>::Do();
		};

		/**
		 * The common part getting functionality.
		 */
		template < class                  P,
				   enum EPartType         InPart     = TPartSwitcher<P>::Value,
				   enum EPartDeliveryType InDelivery = TPartDeliverySwitcher<P>::Value >
		struct TCommonPartGetter
		{
			/**
			 * Raw value type of the component with pointers and references removed.
			 */
			typedef typename more::flatten<P>::type FlatType;

			/**
			 * The type of part to get.
			 */
			static constexpr EPartType Part = InPart;

			/**
			 * The type of delivery to supply.
			 */
			static constexpr EPartDeliveryType Delivery = InDelivery;

			template < bool Defer = true >
			static FORCEINLINE P
			Get(const TCursor&)
			{
				static_assert(!Defer, "Using a placeholder is forbidden.");
				return *((P*)(0xBAADF00D));
			}

			template < bool Defer = true >
			static FORCEINLINE P
			GetHinted(const TCursor&, const int32)
			{
				static_assert(!Defer, "Using a placeholder is forbidden.");
				return *((P*)(0xBAADF00D));
			}
		};

#pragma region Flagmark Getting

		template < typename FlagmarkT, bool ArgumentTypeCheck >
		struct TPartGetter<FlagmarkT, PT_Flagmark, PDT_Value, ArgumentTypeCheck>
		  : public TCommonPartGetter<FlagmarkT, PT_Flagmark, PDT_Value>
		{
			static FORCEINLINE FlagmarkT
			Get(const TCursor& InCursor)
			{
				return InCursor.GetSubject().GetFlagmark();
			}

			static FORCEINLINE FlagmarkT
			GetHinted(const TCursor& InCursor, const int32)
			{
				return InCursor.GetSubject().GetFlagmark();
			}
		};

		template < typename FlagmarkT, bool ArgumentTypeCheck >
		struct TPartGetter<FlagmarkT, PT_Flagmark, PDT_Reference, ArgumentTypeCheck>
		  : public TCommonPartGetter<FlagmarkT, PT_Flagmark, PDT_Reference>
		{
			static_assert(!ArgumentTypeCheck, "Flagmarks can't be delivered by reference. Use a value type, please.");
		};

		template < typename FlagmarkT, bool ArgumentTypeCheck >
		struct TPartGetter<FlagmarkT, PT_Flagmark, PDT_Pointer, ArgumentTypeCheck>
		  : public TCommonPartGetter<FlagmarkT, PT_Flagmark, PDT_Pointer>
		{
			static_assert(!ArgumentTypeCheck, "Flagmarks can't be delivered through a pointer. Use a value type, please.");
		};

#pragma endregion Flagmark Getting

#pragma region Trait Getting

		template < typename TraitT, bool ArgumentTypeCheck >
		struct TPartGetter<TraitT, PT_Trait, PDT_Value, ArgumentTypeCheck>
		  : public TCommonPartGetter<TraitT, PT_Trait, PDT_Value>
		{
			using Super = TCommonPartGetter<TraitT, PT_Trait, PDT_Value>;

			static FORCEINLINE TraitT
			Get(const TCursor& InCursor)
			{
				if (more::is_contained<TraitT, Ps...>::value) // Compile-time branch.
				{
					return GetHinted(InCursor, more::index_within<TraitT, Ps...>::value);
				}
				return InCursor.template GetTrait<typename Super::FlatType>();
			}

			static FORCEINLINE TraitT
			GetHinted(const TCursor& InCursor, const int32 TraitIndexHint)
			{
				return InCursor.template GetTraitHinted<typename Super::FlatType>(TraitIndexHint);
			}
		};

		template < typename TraitT, bool ArgumentTypeCheck >
		struct TPartGetter<TraitT, PT_Trait, PDT_Pointer, ArgumentTypeCheck>
		  : public TCommonPartGetter<TraitT, PT_Trait, PDT_Pointer>
		{
			using Super = TCommonPartGetter<TraitT, PT_Trait, PDT_Pointer>;

			static FORCEINLINE TraitT
			Get(const TCursor& InCursor)
			{
				if (more::is_contained<TraitT, Ps...>::value) // Compile-time branch.
				{
					return GetHinted(InCursor, more::index_within<TraitT, Ps...>::value);
				}
			}

			static FORCEINLINE TraitT
			GetHinted(const TCursor& InCursor, const int32 TraitIndexHint)
			{
				return InCursor.template GetTraitPtrHinted<typename Super::FlatType>(TraitIndexHint);
			}
		};

		template < typename TraitT, bool ArgumentTypeCheck >
		struct TPartGetter<TraitT, PT_Trait, PDT_Reference, ArgumentTypeCheck>
		  : public TCommonPartGetter<TraitT, PT_Trait, PDT_Reference>
		{
			using Super = TCommonPartGetter<TraitT, PT_Trait, PDT_Reference>;

			static FORCEINLINE TraitT
			Get(const TCursor& InCursor)
			{
				if (more::is_contained<TraitT, Ps...>::value) // Compile-time branch.
				{
					return GetHinted(InCursor, more::index_within<TraitT, Ps...>::value);
				}
				return InCursor.template GetTraitRef<typename Super::FlatType>();
			}

			static FORCEINLINE TraitT
			GetHinted(const TCursor& InCursor, const int32 TraitIndexHint)
			{
				return InCursor.template GetTraitRefHinted<typename Super::FlatType>(TraitIndexHint);
			}
		};

#pragma endregion Trait Getting

#pragma region Detail Getting

		template < class DetailT, bool ArgumentTypeCheck >
		struct TPartGetter<DetailT, PT_Detail, PDT_Value, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Getting a detail by value is prohibited. "
											  "Use a reference or a pointer, please.");
		};

		template < class DetailT, bool ArgumentTypeCheck >
		struct TPartGetter<DetailT, PT_Detail, PDT_Reference, ArgumentTypeCheck>
		  : public TCommonPartGetter<DetailT, PT_Detail, PDT_Reference>
		{
			using Super = TCommonPartGetter<DetailT, PT_Detail, PDT_Reference>;

			static FORCEINLINE DetailT
			Get(const TCursor& InCursor)
			{
				if (more::is_contained<DetailT, Ps...>::value) // Compile-time branch.
				{
					return GetHinted(InCursor, more::index_within<DetailT, Ps...>::value);
				}
				return *(InCursor.template GetDetail<typename Super::FlatType>());
			}

			static FORCEINLINE DetailT
			GetHinted(const TCursor& InCursor, const int32 DetailIndexHint)
			{
				return *(InCursor.template GetDetailHinted<typename Super::FlatType>(DetailIndexHint));
			}
		};

		template < class DetailT, bool ArgumentTypeCheck >
		struct TPartGetter<DetailT, PT_Detail, PDT_Pointer, ArgumentTypeCheck>
		  : public TCommonPartGetter<DetailT, PT_Detail, PDT_Pointer>
		{
			using Super = TCommonPartGetter<DetailT, PT_Detail, PDT_Pointer>;

			static FORCEINLINE DetailT
			Get(const TCursor& InCursor)
			{
				if (more::is_contained<DetailT, Ps...>::value) // Compile-time branch.
				{
					return GetHinted(InCursor, more::index_within<DetailT, Ps...>::value);
				}
				return InCursor.template GetDetail<typename Super::FlatType>();
			}

			static FORCEINLINE DetailT
			GetHinted(const TCursor& InCursor, const int32 DetailIndexHint)
			{
				return InCursor.template GetDetailHinted<typename Super::FlatType>(DetailIndexHint);
			}
		};

#pragma endregion Detail Getting

#pragma region Cursor Getting

		template < bool ArgumentTypeCheck >
		struct TPartGetter<const TCursor&, PT_Cursor, PDT_Reference, ArgumentTypeCheck>
		  : public TCommonPartGetter<const TCursor&, PT_Cursor, PDT_Reference>
		{
			using Super = TCommonPartGetter<const TCursor&, PT_Cursor, PDT_Reference>;

			static FORCEINLINE const TCursor&
			Get(const TCursor& InCursor)
			{
				return InCursor;
			}

			static FORCEINLINE const TCursor&
			GetHinted(const TCursor& InCursor,
					  const int32)
			{
				return InCursor;
			}
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<const TCursor*, PT_Cursor, PDT_Pointer, ArgumentTypeCheck>
		  : public TCommonPartGetter<const TCursor*, PT_Cursor, PDT_Pointer>
		{
			using Super = TCommonPartGetter<const TCursor*, PT_Cursor, PDT_Pointer>;

			static FORCEINLINE const TCursor*
			Get(const TCursor& InCursor)
			{
				return &InCursor;
			}

			static FORCEINLINE const TCursor*
			GetHinted(const TCursor& InCursor,
					  const int32)
			{
				return Get(InCursor);
			}
		};

		template < bool ArgumentTypeCheck, typename ...OtherPs >
		struct TPartGetter<const TCursor<OtherPs...>, PT_Cursor, PDT_Value, ArgumentTypeCheck>
		  : public TCommonPartGetter<const TCursor<OtherPs...>, PT_Cursor, PDT_Value>
		{
			using Super = TCommonPartGetter<const TCursor<OtherPs...>, PT_Cursor, PDT_Value>;

			static FORCEINLINE TCursor<OtherPs...>
			Get(const TCursor& InCursor)
			{
				return InCursor;
			}

			static FORCEINLINE TCursor<OtherPs...>
			GetHinted(const TCursor& InCursor,
					  const int32)
			{
				return Get(InCursor);
			}
		};

		template < bool ArgumentTypeCheck, typename ...OtherPs >
		struct TPartGetter<TCursor<OtherPs...>, PT_Cursor, PDT_Value, ArgumentTypeCheck>
		  : public TPartGetter<const TCursor<OtherPs...>, PT_Cursor, PDT_Value, ArgumentTypeCheck>
		{};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<const CursorSuper&, PT_Cursor, PDT_Reference, ArgumentTypeCheck>
		  : public TCommonPartGetter<const CursorSuper&, PT_Cursor, PDT_Reference>
		{
			using Super = TCommonPartGetter<const CursorSuper&, PT_Cursor, PDT_Reference>;

			static FORCEINLINE const CursorSuper&
			Get(const TCursor& InCursor)
			{
				return InCursor;
			}

			static FORCEINLINE const CursorSuper&
			GetHinted(const TCursor& InCursor,
					  const int32)
			{
				return Get(InCursor);
			}
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<CursorSuper&, PT_Cursor, PDT_Reference, ArgumentTypeCheck>
		  : public TCommonPartGetter<CursorSuper&, PT_Cursor, PDT_Reference>
		{
			static_assert(
				!ArgumentTypeCheck,
				"Only an immutable common cursor can be delivered. "
				"Add the 'const' specifier, please."
			);
		};

		template < bool ArgumentTypeCheck, typename ...OtherPs >
		struct TPartGetter<const TCursor<OtherPs...>&, PT_Cursor, PDT_Reference, ArgumentTypeCheck>
		  : public TCommonPartGetter<const TCursor<OtherPs...>&, PT_Cursor, PDT_Reference>
		{
			using Super = TCommonPartGetter<const TCursor<OtherPs...>&, PT_Cursor, PDT_Reference>;

			static_assert(
				std::is_same<const TCursor&, const TCursor<OtherPs...>&>::value,
				"A reference to a constant cursor can be delivered only if the cursor type is the same as the iterating one. "
				"Are you using this inside of an operating lambda? Please, try specifying the common chain cursor type instead."
			);

			static FORCEINLINE const TCursor&
			Get(const TCursor& InCursor)
			{
				return InCursor;
			}

			static FORCEINLINE const TCursor&
			GetHinted(const TCursor& InCursor,
					  const int32)
			{
				return Get(InCursor);
			}
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<TCursor&, PT_Cursor, PDT_Reference, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't supply an immutable cursor. Use a 'const' specifier for the cursor reference argument, please.");
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<TCursor*, PT_Cursor, PDT_Pointer, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't supply an immutable cursor. Use a 'const' specifier for the cursor pointer argument, please.");
		};

#pragma endregion Cursor Getting

#pragma region Subject Getting

		template < typename PartSubjectHandleT, bool ArgumentTypeCheck >
		struct TPartGetter<PartSubjectHandleT, PT_SubjectHandle, PDT_Value, ArgumentTypeCheck>
		  : public TCommonPartGetter<PartSubjectHandleT>
		{
			using Super = TCommonPartGetter<PartSubjectHandleT>;

			static FORCEINLINE PartSubjectHandleT
			Get(const TCursor& InCursor)
			{
				return ::template SmartCast<PartSubjectHandleT, DefaultParadigm>(InCursor.GetSubject());
			}

			static FORCEINLINE PartSubjectHandleT
			GetHinted(const TCursor& InCursor,
					  const int32    NotUsed)
			{
				(void)NotUsed;
				return Get(InCursor);
			}
		};

		template < typename PartSubjectHandleT, bool ArgumentTypeCheck >
		struct TPartGetter<const PartSubjectHandleT, PT_SubjectHandle, PDT_Value, ArgumentTypeCheck>
		  : public TCommonPartGetter<const PartSubjectHandleT>
		{
			using Super = TCommonPartGetter<const PartSubjectHandleT>;

			static FORCEINLINE const PartSubjectHandleT
			Get(const TCursor& InCursor)
			{
				return ::template SmartCast<PartSubjectHandleT, DefaultParadigm>(InCursor.GetSubject());
			}

			static FORCEINLINE const PartSubjectHandleT
			GetHinted(const TCursor& InCursor,
					  const int32    NotUsed)
			{
				(void)NotUsed;
				return Get(InCursor);
			}
		};

		template < typename PartSubjectHandleT, bool ArgumentTypeCheck >
		struct TPartGetter<PartSubjectHandleT&, PT_SubjectHandle, PDT_Reference, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't provide a subject handle reference. Use a value type, please.");
		};

		template < typename PartSubjectHandleT, bool ArgumentTypeCheck >
		struct TPartGetter<const PartSubjectHandleT&, PT_SubjectHandle, PDT_Reference, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't provide a subject handle reference. Use a value type, please.");
		};

		template < typename PartSubjectHandleT, bool ArgumentTypeCheck >
		struct TPartGetter<PartSubjectHandleT*, PT_SubjectHandle, PDT_Pointer, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't provide a subject handle pointer. Use a value type, please.");
		};

		template < typename PartSubjectHandleT, bool ArgumentTypeCheck >
		struct TPartGetter<const PartSubjectHandleT*, PT_SubjectHandle, PDT_Pointer, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't provide a subject handle pointer. Use a value type, please.");
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<IncompatibleSubjectHandleType, PT_SubjectHandle, PDT_Value, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Incompatible subject handle type used. The solidity must match.");
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<const IncompatibleSubjectHandleType, PT_SubjectHandle, PDT_Value, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Incompatible subject handle type used. The solidity must match.");
		};

#pragma endregion Subject Getting

#pragma region Subjective Getting

		template < class SubjectiveC, bool ArgumentTypeCheck >
		struct TPartGetter<SubjectiveC, PT_Subjective, PDT_Pointer, ArgumentTypeCheck>
		  : public TCommonPartGetter<SubjectiveC>
		{
			using Super = TCommonPartGetter<SubjectiveC>;

			static FORCEINLINE SubjectiveC
			Get(const TCursor& InCursor)
			{
				return SmartCast<SubjectiveC>(InCursor.GetSubjective());
			}

			static FORCEINLINE SubjectiveC
			GetHinted(const TCursor& InCursor,
					  const int32    NotUsed)
			{
				(void)NotUsed;
				return Get(InCursor);
			}
		};

		template < class SubjectiveC, bool ArgumentTypeCheck >
		struct TPartGetter<SubjectiveC, PT_Subjective, PDT_Reference, ArgumentTypeCheck>
		  : public TCommonPartGetter<SubjectiveC>
		{
			using Super = TCommonPartGetter<SubjectiveC>;

			static FORCEINLINE SubjectiveC
			Get(const TCursor& InCursor)
			{
				return *SmartCast<typename Super::FlatType>(InCursor.GetSubjective());
			}

			static FORCEINLINE SubjectiveC
			GetHinted(const TCursor& InCursor,
					  const int32    NotUsed)
			{
				(void)NotUsed;
				return Get(InCursor);
			}
		};

		template < class SubjectiveC, bool ArgumentTypeCheck >
		struct TPartGetter<SubjectiveC, PT_Subjective, PDT_Value, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't provide a subjective by value. Use a pointer (*) or a reference (&) type, please.");
		};

#pragma endregion Subjective Getting

#pragma region Solid Subjective Getting

		template < bool ArgumentTypeCheck >
		struct TPartGetter<ISolidSubjective&, PT_Other, PDT_Reference, ArgumentTypeCheck>
		  : public TCommonPartGetter<ISolidSubjective&>
		{
			using Super = TCommonPartGetter<ISolidSubjective&>;

			static FORCEINLINE ISolidSubjective&
			Get(const TCursor& InCursor)
			{
				return *SmartCast<ISolidSubjective>(InCursor.GetSubjective());
			}

			static FORCEINLINE ISolidSubjective&
			GetHinted(const TCursor& InCursor,
					  const int32    NotUsed)
			{
				(void)NotUsed;
				return Get(InCursor);
			}
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<ISolidSubjective*, PT_Other, PDT_Pointer, ArgumentTypeCheck>
		  : public TCommonPartGetter<ISolidSubjective*>
		{
			using Super = TCommonPartGetter<ISolidSubjective*>;

			static FORCEINLINE ISolidSubjective*
			Get(const TCursor& InCursor)
			{
				return SmartCast<ISolidSubjective>(InCursor.GetSubjective());
			}

			static FORCEINLINE ISolidSubjective*
			GetHinted(const TCursor& InCursor,
					  const int32    NotUsed)
			{
				(void)NotUsed;
				return Get(InCursor);
			}
		};

#pragma endregion Solid Subjective Getting

#pragma region Chain Getting

		template < bool ArgumentTypeCheck >
		struct TPartGetter<const TChain*, PT_Other, PDT_Pointer, ArgumentTypeCheck>
		  : public TCommonPartGetter<const TChain*>
		{
			using Super = TCommonPartGetter<const TChain*>;

			static FORCEINLINE const TChain*
			Get(const TCursor& InCursor)
			{
				return InCursor.GetOwner();
			}

			static FORCEINLINE const TChain*
			GetHinted(const TCursor& InCursor,
					  const int32)
			{
				return Get(InCursor);
			}
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<const TChain&, PT_Other, PDT_Reference, ArgumentTypeCheck>
		  : public TCommonPartGetter<const TChain&>
		{
			using Super = TCommonPartGetter<const TChain&>;

			static FORCEINLINE const TChain&
			Get(const TCursor& InCursor)
			{
				return *(InCursor.GetOwner());
			}

			static FORCEINLINE const TChain&
			GetHinted(const TCursor& InCursor,
					  const int32)
			{
				return Get(InCursor);
			}
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<TChain*, PT_Other, PDT_Pointer, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't provide a mutable chain pointer. Use a 'const' specifier, please.");
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<TChain&, PT_Other, PDT_Reference, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't provide a mutable chain reference. Use a 'const' specifier, please.");
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<TChain, PT_Other, PDT_Value, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't provide a chain value copy. Use a pointer (*) or a reference (&) type, please.");
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<const TChain, PT_Other, PDT_Value, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't provide a chain value copy. Use a pointer (*) or a reference (&) type, please.");
		};

#pragma endregion Chain Getting

#pragma region Mechanism Getting

		template < bool ArgumentTypeCheck >
		struct TPartGetter<const AMechanism*, PT_Other, PDT_Pointer, ArgumentTypeCheck>
		  : public TCommonPartGetter<const AMechanism*, PT_Other, PDT_Pointer>
		{
			using Super = TCommonPartGetter<const AMechanism*, PT_Other, PDT_Pointer>;

			static FORCEINLINE const AMechanism*
			Get(const TCursor& InCursor)
			{
				return InCursor.GetOwner()->GetOwner();
			}

			static FORCEINLINE const AMechanism*
			GetHinted(const TCursor& InCursor,
					  const int32)
			{
				return Get(InCursor);
			}
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<const AMechanism&, PT_Other, PDT_Reference, ArgumentTypeCheck>
		  : public TCommonPartGetter<const AMechanism&, PT_Other, PDT_Reference>
		{
			using Super = TCommonPartGetter<const AMechanism&, PT_Other, PDT_Reference>;

			static FORCEINLINE const AMechanism&
			Get(const TCursor& InCursor)
			{
				return *(InCursor.GetOwner()->GetOwner());
			}

			static FORCEINLINE const AMechanism&
			GetHinted(const TCursor& InCursor,
					  const int32)
			{
				return Get(InCursor);
			}
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<AMechanism*, PT_Other, PDT_Pointer, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't provide a mutable mechanism pointer. Use a 'const' specifier, please.");
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<AMechanism&, PT_Other, PDT_Reference, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't provide a mutable mechanism reference. Use a 'const' specifier, please.");
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<AMechanism, PT_Other, PDT_Value, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't provide a mechanism value copy. Use a pointer (*) or a reference (&) type, please.");
		};

		template < bool ArgumentTypeCheck >
		struct TPartGetter<const AMechanism, PT_Other, PDT_Value, ArgumentTypeCheck>
		{
			static_assert(!ArgumentTypeCheck, "Can't provide a mechanism value copy. Use a pointer (*) or a reference (&) type, please.");
		};

#pragma endregion Mechanism Getting

	  public:

		/**
		 * Find a hinting index for a part.
		 * 
		 * If there's a template part pack it will be used as a basis for the index.
		 * Otherwise, the current filter's traitmark/detailmark will be used instead.
		 * 
		 * @tparam P The class of the part to find an index of.
		 * @return The index of the part or @c INDEX_NONE, if
		 * it was not found for hinting.
		 */
		template < class P >
		FORCEINLINE constexpr int32
		IndexOfPart() const
		{
			if (sizeof...(Ps) > 0) // Compile-time branch.
			{
				return more::index_within_flatly<P, Ps...>::value;
			}
			switch (Iterator.GetIndex())
			{
				case ChunkItIndex: return Iterator.template Get<ChunkItType>().GetChunk()
					->template TraitLineIndexOf<more::flatten_t<P>>();
				case BeltItIndex: return Iterator.template Get<BeltItType>().GetBelt()
					->template DetailLineIndexOf<more::flatten_t<P>>();
				default: 
				{
					return INDEX_NONE;
				}
			}
		}

		/**
		 * Get a part (trait, detail, subject, other) of the current slot by its type.
		 * Templated version.
		 * 
		 * If the specified part type is also within the cursor's specified parts list,
		 * the method is performed using a fast hinting.
		 * 
		 * @tparam PartT The type of the part to get. Can be a value. a reference or a pointer.
		 * @return The part of the current slot in the designated type.
		 */
		template < typename PartT >
		FORCEINLINE PartT
		GetPart() const
		{
			return TPartGetter<PartT>::Get(*this);
		}

		/**
		 * Get a part (trait, detail, subject, other) of the current slot by its type
		 * with an index hint.
		 * Templated version.
		 * 
		 * @tparam PartT The type of the part to get. Can be a value, a reference or a pointer.
		 * @param PartIndexHint A component index hint, corresponding to an index within the locking filter.
		 * @return The part of the current slot in the designated type.
		 * 
		 * @see GetPart()
		 */
		template < typename PartT >
		FORCEINLINE PartT
		GetPartHinted(const int32 PartIndexHint) const
		{
			return TPartGetter<PartT>::GetHinted(*this, PartIndexHint);
		}

#pragma region Initialization

		/**
		 * Initialize a new cursor instance for a chain.
		 * 
		 * @param InOwner The owner of the cursor to iterate.
		 * @param InOffset The slot offset of the cursor to begin with.
		 * @param InLimit The maximum number of slots to iterate by.
		 */
		FORCEINLINE
		TCursor(const TChain* const InOwner,
				const int32         InOffset = 0,
				const int32         InLimit = TNumericLimits<int32>::Max())
		  : CursorSuper(InOwner, InOffset, InLimit)
		{}

		/**
		 * Move-initialize a cursor.
		 */
		FORCEINLINE
		TCursor(TCursor&& InCursor)
		  : CursorSuper(MoveTemp(InCursor))
		  , Mapping(MoveTemp(InCursor.Mapping))
		{}

		/**
		 * Copy-initialize a cursor.
		 * 
		 * @param InCursor The original cursor to copy.
		 */
		FORCEINLINE
		TCursor(const TCursor& InCursor)
		  : CursorSuper(InCursor)
		  , Mapping(InCursor.Mapping)
		{}

		/**
		 * Copy-initialize a cursor from an another cursor type.
		 * 
		 * @param InCursor The original cursor to copy.
		 */
		template < typename ...OtherPs >
		FORCEINLINE
		TCursor(const TCursor<OtherPs...>& InCursor)
		  : CursorSuper(InCursor)
		{
			if (LIKELY(Owner))
			{
				switch (Iterator.GetIndex())
				{
					case ChunkItIndex:
					{
						const auto Chunk = Iterator.template Get<ChunkItType>().GetChunk();
						if (sizeof...(Ps) > 0) // Compile-time branch.
						{
							Mapping = MappingType({Chunk->template TraitLineIndexOf<more::flatten_t<Ps>>()...});
						}
						else
						{
							Chunk->GetTraitmark().FindMappingFrom(Owner->GetFilter().GetTraitmark(), Mapping);
						}
						break;
					}
					case BeltItIndex:
					{
						const auto Belt = Iterator.template Get<BeltItType>().GetBelt();
						if (sizeof...(Ps) > 0) // Compile-time branch.
						{
							Mapping = MappingType({Belt->template DetailLineIndexOf<Ps>()...});
						}
						else
						{
							Belt->GetDetailmark().FindMappingFrom(Owner->GetFilter().GetDetailmark(), Mapping);
						}
						break;
					}
				}
			}
		}

		/**
		 * Initialize a new invalid cursor.
		 */
		FORCEINLINE
		TCursor()
		{
			static_assert(::IsChainCursorType<TCursor>(), "The cursor type must be identified as such.");
		}

		/**
		 * Finalize the cursor.
		 */
		FORCEINLINE
		~TCursor()
		{
			Reset();
		}

#pragma endregion Initialization

	}; //-struct TCursor

	/**
	 * Non-templated cursor version.
	 */
	using FCursor = TCursor<>;

  private:

	/**
	 * The runnable mechanical wrapping.
	 * 
	 * Can be run both in the main and parallel threads.
	 * 
	 * The wrapping is also used in the context of
	 * usual single-threaded operating as a wrapper.
	 *
	 * @tparam MechanicT The type of mechanic used. May be a function, functor (lambda).
	 * @tparam Ps The types of subject parts requested.
	 */
	template < typename MechanicT, typename... Ps >
	struct TRunnableMechanic
	  : public TCommonRunnableMechanic<TChain>
	{
	  public:

		/**
		 * The base class.
		 */
		using Super = TCommonRunnableMechanic<TChain>;

		/**
		 * The type of the mechanic used.
		 */
		typedef MechanicT MechanicType;

	  private:

		/**
		 * The type of the mechanic argument received by the
		 * mechanic operator.
		 * 
		 * This can either be a static function
		 * or a functor (lambda) reference type.
		 */
		typedef typename std::conditional<std::is_class<MechanicT>::value,
										  MechanicType&,
										  const MechanicType&>::type MechanicArgumentType;

		/**
		 * The mechanic instance used that is currently
		 * operating the chain interval.
		 *
		 * This can either be a static function pointer
		 * or a functor (lambda) state copy.
		 */
		typename std::conditional<std::is_class<MechanicT>::value,
								  MechanicType,
								  const MechanicType&>::type OperatingMechanic;

		/**
		 * A cursor used during the evaluation.
		 */
		TCursor<Ps...> IteratingCursor;

		FORCEINLINE void
		DoIteration()
		{
			OperatingMechanic(IteratingCursor.template GetPart<Ps>()...);
		}

		FORCEINLINE void
		DoRun()
		{
			if (LIKELY(Owner != nullptr))
			{
				while (IteratingCursor.Provide() && !Super::bStop.load(std::memory_order_relaxed))
				{
					DoIteration();
				}
			}
		}

		FORCEINLINE uint32
		Run() override
		{
			Super::DoBeginRun();
			DoRun();
			Super::DoEndRun();
			return 0;
		}

	  public:

		/**
		 * Launch the synchronous execution.
		 * 
		 * @note The workflow can still be concurrent.
		 */
		void
		LaunchSync();

		/**
		 * Launch the execution on a thread.
		 * 
		 * The thread is created automatically.
		 * 
		 * @param Id The unique identifier of the thread.
		 * @return The status of the operation.
		 */
		FORCEINLINE EApparatusStatus
		LaunchAsync(int32 Id)
		{
			return Super::DoLaunchAsync(Id);
		}

		/**
		 * Initialize a new runnable mechanic instance.
		 * 
		 * @param InMechanic The mechanic to operate with.
		 * @param InCursor The cursor to operate with.
		 * @param bInConcurrent Should the concurrent workflow be applied.
		 */
		FORCEINLINE
		TRunnableMechanic(const MechanicType& InMechanic,
						  TCursor<Ps...>&&    InCursor,
						  const bool          bInConcurrent = false)
		  : Super(InCursor.GetOwner(), bInConcurrent)
		  , OperatingMechanic(InMechanic)
		  , IteratingCursor(InCursor)
		{}
		
		/**
		 * Finalize the mechanic runnable.
		 */
		FORCEINLINE
		~TRunnableMechanic() override
		{
			// We don't wait here, since
			// the wait is performed in the chain disposal.
			Super::Halt(/*bShouldWait=*/false);
		}

	}; //-struct TRunnableMechanic

	/**
	 * An asynchronous mechanical task.
	 *
	 * Can be launched on a parallel or on the main thread.
	 * 
	 * @tparam MechanicT The type of mechanic used. May be a function, functor (lambda).
	 * @tparam Ps The types of subject parts that are requested.
	 */
	template < typename MechanicT, typename... Ps >
	struct TMechanicTask final : public TCommonMechanicTask<TChain>
	{
	  public:

		using Super = TCommonMechanicTask<TChain>;

		/**
		 * The type of the mechanic used.
		 */
		typedef MechanicT MechanicType;

		FORCEINLINE TStatId
		GetStatId() const
		{
			RETURN_QUICK_DECLARE_CYCLE_STAT(TMechanicAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
		}

	  private:

		friend class FAsyncTask<TMechanicTask>;
		friend class FAutoDeleteAsyncTask<TMechanicTask>;

		/**
		 * The type of argument received by the operator.
		 */
		typedef typename std::conditional<std::is_class<MechanicT>::value,
										  MechanicType&,
										  const MechanicType&>::type MechanicArgumentType;

		/**
		 * The mechanic instance used that is currently
		 * operating the chain interval.
		 */
		typename std::conditional<std::is_class<MechanicT>::value,
								  MechanicType,
								  const MechanicType&>::type OperatingMechanic;

		/**
		 * A cursor used during the evaluation.
		 */
		TCursor<Ps...> IteratingCursor;

		FORCEINLINE void
		DoIteration()
		{
			OperatingMechanic(IteratingCursor.template GetPart<Ps>()...);
		}

	  public:

		FORCEINLINE void
		DoWork()
		{
			Super::DoBeginWork();
			if (LIKELY(Owner != nullptr))
			{
				while (IteratingCursor.Provide())
				{
					DoIteration();
				}
			}
			Super::DoEndWork();
		}

		FORCEINLINE
		TMechanicTask()
		{}

		FORCEINLINE
		TMechanicTask(const MechanicType& InMechanic,
					  TCursor<Ps...>&&    InCursor,
					  const bool          bInConcurrent = true)
		  : Super(InCursor.GetOwner(), bInConcurrent)
		  , OperatingMechanic(InMechanic)
		  , IteratingCursor(InCursor)
		{}

	}; //-struct TMechanicTask

	/**
	 * The currently chained segments comprising the chain.
	 */
	TArray<FSegment> Segments;

	/**
	 * The embedded (internally-managed) cursors of the chain.
	 */
	mutable TArray<FCursor> Cursors;

	/**
	 * Obtain an embedded cursor for a specific identifier.
	 */
	FORCEINLINE FCursor&
	ObtainCursor(const int32 CursorId) const
	{
		check(CursorId > InvalidCursorId);
		if (UNLIKELY(CursorId >= Cursors.Num()))
		{
			const int32 Count = (CursorId + 1) - Cursors.Num();
			for (int32 i = 0; i < Count; ++i)
			{
				Cursors.Add(this);
			}
		}
		return Cursors[CursorId];
	}

	/**
	 * Check if a cursor is valid
	 * and is currently iterating.
	 */
	FORCEINLINE bool
	IsCursorViable(const int32 CursorId) const
	{
		return (CursorId > InvalidCursorId) &&
			   (CursorId < Cursors.Num()) && 
			   (Cursors[CursorId].IsViable());
	}

	/**
	 * Get the cursor data by its identifier.
	 */
	FORCEINLINE FCursor&
	GetCursor(const int32 CursorId = DefaultCursorId) const
	{
		check(CursorId > InvalidCursorId);
		return Cursors[CursorId];
	}

  public:

	/**
	 * Reset the chain completely.
	 * 
	 * @param bWaitForOperatings Should the method wait
	 * till all the operatings are complete.
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	Reset(bool bWaitForOperatings = true) override
	{
		// The disposed chain is always reset.
		if (UNLIKELY(bDisposed)) return EApparatusStatus::Noop;

		if (bWaitForOperatings)
		{
			// Wait for all async operations to complete:
			WaitForOperatingsCompletion();
		}

		auto Status = EApparatusStatus::Noop;
		// Reset the embedded cursors:
		if (Cursors.Num())
		{
			// Reset the embedded cursors first, so they would release
			// the chain appropriately.
			// This can trigger the (recursive) disposal of the chain itself,
			// but it would actually perform safely.
			Cursors.Reset();
			Status = EApparatusStatus::Success;
		}

		if (Segments.Num())
		{
			Segments.Reset();
			Status = EApparatusStatus::Success;
		}

		return StatusCombine(Status, Super::Reset(/*bWaitForOperatings=*/false));
	}

	/**
	 * Set to chain a set of iterables with a filter.
	 * 
	 * @tparam FilterT The type of filter used. Forwarded.
	 * @tparam IterableT The type of iterables to enchain.
	 * @tparam AllocatorT The type of iterables array allocator.
	 * @param[in] InFilter The target filter to enchain with.
	 * @param[in] InIterables The iterables to enchain.
	 * @return The status of the operation. 
	 */
	template< typename FilterT, typename IterableT, typename AllocatorT,
			  EParadigm Paradigm = DefaultParadigm,
			  TFilterTypeSecurity<std::decay_t<FilterT>> = true >
	TOutcome<Paradigm>
	Enchain(FilterT&&                             InFilter,
			const TArray<IterableT*, AllocatorT>& InIterables)
	{
		AssessConditionFormat(Paradigm, bDisposed, EApparatusStatus::InvalidState,
							  TEXT("The chain should be disposed in order to enchain (new) iterables. "
								   "Have you previously finished iterating the chain up to the end?"));
		Filter = std::forward<FilterT>(InFilter);
		for (IterableT* const InIterable : InIterables)
		{
			AssessCondition(Paradigm, InIterable != nullptr, EApparatusStatus::InvalidArgument);
			AssessCondition(Paradigm, InIterable->Matches(Filter), EApparatusStatus::InvalidArgument);
			if (UNLIKELY(InIterable->Num() == 0)) continue;
			auto& Segment = Segments.Add_GetRef(FSegment(InIterable));
			verify(OK(Segment.Lock()));
		}

		bDisposed = false;
		return EApparatusStatus::Success;
	}

	/**
	 * Find a certain chain segment by a chain's slot index.
	 *
	 * No iterating is done here, so no iterating-time checks
	 * (flagmark matching) is performed here.
	 * 
	 * @param InSlotIndex The index of the slot to find.
	 * @param OutLocalIndex The local index of the slot within the chain.
	 * @param SegmentOffset The starting segment offset.
	 * @return The index of the segment.
	 */
	FORCEINLINE int32
	FindSegment(const int32 InSlotIndex,
				int32&      OutLocalIndex,
				const int32 SegmentOffset = 0) const
	{
		OutLocalIndex = -1;
		if (UNLIKELY(InSlotIndex <= -1)) return InvalidSegmentIndex;
		int32 Offset = 0;
		for (int32 i = SegmentOffset; i < Segments.Num(); ++i)
		{
			const FSegment& Segment = Segments[i];
			check(Segment.IsLocked());
			const int32 Count = Segment.IterableNum();
			if (UNLIKELY(Count == 0)) continue;
			const int32 NewOffset = Offset + Count;
			if (InSlotIndex >= Offset && InSlotIndex < NewOffset)
			{
				OutLocalIndex = InSlotIndex - Offset;
				return i;
			}
			Offset = NewOffset;
		}
		// Segment was not found:
		return InvalidSegmentIndex;
	}

	/**
	 * Prepare the chain for iterating returning the cursor.
	 *
	 * @tparam Ps The types of components to provide efficiently
	 * using the hinting mechanism.
	 * @param Offset The slot offset to begin iterating with.
	 * @param Limit The maximum number of slots to iterate by.
	 * @return The cursor used for iterating.
	 */
	template < typename... Ps >
	FORCEINLINE TCursor<Ps...>
	Iterate(const int32 Offset = 0,
			const int32 Limit  = TNumericLimits<int32>::Max()) const
	{
		return TCursor<Ps...>(this, Offset, Limit);
	}

  private:

	template < EParadigm Paradigm, typename MechanicT, typename... Ts >
	FORCEINLINE TOutcome<Paradigm>
	DoOperate(const MechanicT& InMechanic) const
	{
		AssessConditionFormat(Paradigm, !bDisposed, EApparatusStatus::InvalidState,
							  TEXT("A chain should not be disposed in order to be processed."));

		typedef TRunnableMechanic<MechanicT, Ts...> RunnableType;
		typedef typename RunnableType::MechanicType MechanicType;
		static_assert(std::is_same<MechanicT, MechanicType>::value, "Mechanic types must be the same.");

		const auto IterableCount = IterableNum();
		if (UNLIKELY(IterableCount == 0)) 
		{
			// Nothing to iterate upon...
			if (UsersNum() == 0)
			{
				const auto MutableChain = const_cast<TChain*>(this);
				AssessOK(Paradigm, MutableChain->Dispose());
			}
			return EApparatusStatus::Noop;
		}

		RunnableType Runnable(InMechanic, Iterate<Ts...>());
		Runnable.LaunchSync();
		return EApparatusStatus::Success;
	}

	template < EParadigm Paradigm, typename MechanicT, typename... Ts >
	TOutcome<Paradigm>
	DoOperateViaTasks(const MechanicT& InMechanic,
					  const int32      ThreadsCountMax,
					  const int32      SlotsPerThreadMin,
					  const bool       bSync) const;

	template < EParadigm Paradigm, typename MechanicT, typename... Ts >
	TOutcome<Paradigm>
	DoOperateViaThreads(const MechanicT& InMechanic,
						const int32      ThreadsCountMax,
						const int32      SlotsPerThreadMin,
						const bool       bSync) const;

	template < EParadigm Paradigm, typename MechanicT, typename... Ts >
	FORCEINLINE TOutcome<Paradigm>
	DoOperate(const MechanicT& InMechanic,
			  const int32      ThreadsCountMax,
			  const int32      SlotsPerThreadMin,
			  const bool       bSync) const
	{
		if (LIKELY(FPlatformProcess::SupportsMultithreading()
				&& (ThreadsCountMax != 1)))
		{
			return DoOperateViaTasks<Paradigm, MechanicT, Ts...>(InMechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
		}
		else
		{
			return DoOperate<Paradigm, MechanicT, Ts...>(InMechanic);
		}
	}

	template < EParadigm Paradigm, typename RunnableMechanicT, typename... Ts >
	FORCEINLINE TOutcome<Paradigm>
	DoOperate(const RunnableMechanicT& Mechanic,
			  TComponentTypesPack<Ts...>&&) const
	{
		return DoOperate<Paradigm, RunnableMechanicT, Ts...>(Mechanic);
	}

	template < EParadigm Paradigm, typename RunnableMechanicT, typename... Ts >
	FORCEINLINE TOutcome<Paradigm>
	DoOperate(const RunnableMechanicT& Mechanic,
			  TComponentTypesPack<Ts...>&&,
			  const int32              ThreadsCountMax,
			  const int32              SlotsPerThreadMin = 1,
			  const bool               bSync = true) const
	{
		return DoOperate<Paradigm, RunnableMechanicT, Ts...>(Mechanic, ThreadsCountMax, SlotsPerThreadMin, bSync);
	}

	template<typename F>
	struct TFunctorTraits;

	/**
	 * Generic mechanic traits extractor.
	 */
	template < typename M >
	struct TMechanicTraits
	{
		using ComponentTypes = typename M::ComponentTypes;
	};

	// Function version:
	template < typename... Args >
	struct TMechanicTraits<void(*)(Args...)>
	{
		using ComponentTypes = TComponentTypesPack<Args...>;
	};

	// Functor/lambda version:
	template < typename... Args >
	struct TMechanicTraits<std::function<void(Args...)>>
	{
		using ComponentTypes = TComponentTypesPack<Args...>;
	};

	template < EParadigm Paradigm, typename M, typename... Ts >
	FORCEINLINE auto
	DoOperate(M& Obj, void (M::*)(Ts...)) const
	{
		return DoOperate<Paradigm, M, Ts...>(Obj);
	}

	template < EParadigm Paradigm, typename M, typename... Ts >
	FORCEINLINE auto
	DoOperate(const M& Obj, void (M::*)(Ts...)) const
	{
		return DoOperate<Paradigm, M, Ts...>(Obj);
	}

	template < EParadigm Paradigm, typename M, typename... Ts >
	FORCEINLINE auto
	DoOperate(const M& Obj, void (M::*)(Ts...) const) const
	{
		return DoOperate<Paradigm, M, Ts...>(Obj);
	}

	template < EParadigm Paradigm, typename M, typename... Ts >
	FORCEINLINE auto
	DoOperate(M& Obj, void (M::*)(Ts...),
			  const int32 ThreadsCountMax,
			  const int32 SlotsPerThreadMin = 1,
			  const bool  bSync = true) const
	{
		return DoOperate<Paradigm, M, Ts...>(Obj, ThreadsCountMax, SlotsPerThreadMin, bSync);
	}

	template < EParadigm Paradigm, typename M, typename... Ts >
	FORCEINLINE auto
	DoOperate(const M& Obj, void (M::*)(Ts...),
			  const int32 ThreadsCountMax,
			  const int32 SlotsPerThreadMin = 1,
			  const bool  bSync = true) const
	{
		return DoOperate<Paradigm, M, Ts...>(Obj, ThreadsCountMax, SlotsPerThreadMin, bSync);
	}

	template < EParadigm Paradigm, typename M, typename... Ts >
	FORCEINLINE auto
	DoOperate(const M& Obj, void (M::*)(Ts...) const,
			  const int32 ThreadsCountMax,
			  const int32 SlotsPerThreadMin = 1,
			  const bool  bSync = true) const
	{
		return DoOperate<Paradigm, M, Ts...>(Obj, ThreadsCountMax, SlotsPerThreadMin, bSync);
	}

	/**
	 * The type of entity used for operating.
	 */
	enum OperatorType
	{
		/**
		 * A normal function.
		 */
		Function,

		/**
		 * A functor/lambda.
		 */
		Functor
	};

	/**
	 * The unified generic function/functor operator.
	 *
	 * This launches the internal operating procedure.
	 */
	template < enum OperatorType Type >
	struct TOperator;
	
	// Functor/lambda version:
	template<>
	struct TOperator< Functor >
	{
		template < EParadigm Paradigm = DefaultParadigm,
				   typename  FunctorT = void >
		static FORCEINLINE auto
		Do(const TChain* InChain, const FunctorT& InMechanic)
		{
			return InChain->template DoOperate<Paradigm>(
						InMechanic,
						&FunctorT::operator());
		}

		template < EParadigm Paradigm = DefaultParadigm,
				   typename  FunctorT = void >
		static FORCEINLINE auto
		Do(const TChain* InChain, FunctorT& InMechanic)
		{
			return InChain->template DoOperate<Paradigm>(
						InMechanic,
						&FunctorT::operator());
		}

		template < EParadigm Paradigm = DefaultParadigm,
				   typename  FunctorT = void >
		static FORCEINLINE auto
		Do(const TChain*    InChain,
		   const FunctorT&  InMechanic,
		   const int32      ThreadsCountMax,
		   const int32      SlotsPerThreadMin = 1,
		   const bool       bSync = true)
		{
			return InChain->template DoOperate<Paradigm>(
						InMechanic,
						&FunctorT::operator(),
						ThreadsCountMax,
						SlotsPerThreadMin,
						bSync);
		}

		template < EParadigm Paradigm = DefaultParadigm,
				   typename  FunctorT = void >
		static FORCEINLINE EApparatusStatus
		Do(const TChain* InChain,
		   FunctorT&     InMechanic,
		   const int32   ThreadsCountMax,
		   const int32   SlotsPerThreadMin = 1,
		   const bool    bSync = true)
		{
			return InChain->template DoOperate<Paradigm>(
						InMechanic,
						&FunctorT::operator(),
						ThreadsCountMax,
						SlotsPerThreadMin,
						bSync);
		}
	};

	// Function version:
	template<>
	struct TOperator< Function >
	{
		template < EParadigm Paradigm  = DefaultParadigm,
				   typename  FunctionT = void >
		static FORCEINLINE auto
		Do(const TChain* InChain, const FunctionT& InMechanic)
		{
			return InChain->template DoOperate<Paradigm, FunctionT>(
						InMechanic,
						typename TMechanicTraits<FunctionT>::ComponentTypes());
		}

		template < EParadigm Paradigm  = DefaultParadigm,
				   typename  FunctionT = void >
		static FORCEINLINE auto
		Do(const TChain* InChain, const FunctionT& InMechanic,
		   const int32   ThreadsCountMax,
		   const int32   SlotsPerThreadMin = 1,
		   const bool    bSync = true)
		{
			return InChain->template DoOperate<Paradigm, FunctionT>(
						InMechanic,
						typename TMechanicTraits<FunctionT>::ComponentTypes(),
						ThreadsCountMax, SlotsPerThreadMin,
						bSync);
		}
	};

  public:

#pragma region Operating
	/// @name Operating
	/// @{

	/**
	 * Process the chain using a functor mechanic.
	 * Supports lambdas. Constant mechanic version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots.
	 * Can be a lambda expression.
	 * @param InMechanic The functor mechanic to operate with.
	 * Can be a lambda expression.
	 * @return The status of the operation. 
	 */
	template < EParadigm Paradigm = DefaultParadigm, typename MechanicT = void,
			   typename std::enable_if<!std::is_function<MechanicT>::value, bool>::type = true >
	FORCEINLINE auto
	Operate(const MechanicT& InMechanic) const
	{
		return TOperator<Functor>::template Do<Paradigm>(this, InMechanic);
	}

	/**
	 * Process the chain using a functor mechanic.
	 * Supports lambdas. Mutable mechanic version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam MechanicT The type of the functor
	 * to operate on the enchained slots. Can be a lambda expression.
	 * @return The status of the operation. 
	 */
	template < EParadigm Paradigm = DefaultParadigm, typename MechanicT = void,
			   typename std::enable_if<!std::is_function<MechanicT>::value, bool>::type = true >
	FORCEINLINE auto
	Operate(MechanicT& InMechanic) const
	{
		return TOperator<Functor>::template Do<Paradigm>(this, InMechanic);
	}

	/**
	 * Process the chain using a free function.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam FunctionT The type of the function to
	 * operate on the enchained slots.
	 * @return The status of the operation. 
	 */
	template < EParadigm Paradigm = DefaultParadigm, typename FunctionT = void,
			   typename std::enable_if<std::is_function<FunctionT>::value, bool>::type = true >
	FORCEINLINE auto
	Operate(FunctionT* const InMechanic) const
	{
		return TOperator<Function>::template Do<Paradigm>(this, InMechanic);
	}

	/**
	 * Process the chain using a functor mechanic in a
	 * parallel manner. Supports lambdas. Constant mechanic version.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 * 
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam FunctorT The type of the mechanic functor
	 * to operate on the slots. May be a lambda.
	 * @param InMechanic The mechanic functor to operate with. Can be a lambda.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The status of the operation. 
	 */
	template < EParadigm Paradigm = DefaultParadigm, typename FunctorT = void,
			   typename std::enable_if<(IsSolid || IsUnsafe(Paradigm))
			   						&& !std::is_void<FunctorT>::value
									&& !std::is_function<std::decay_t<FunctorT>>::value, bool>::type = true >
	FORCEINLINE auto
	OperateConcurrently(const FunctorT& InMechanic,
						const int32     ThreadsCountMax,
						const int32     SlotsPerThreadMin = 1,
						const bool      bSync = true) const
	{
		return TOperator<Functor>::template Do<Paradigm>(
			this, InMechanic,
			ThreadsCountMax, SlotsPerThreadMin,
			bSync);
	}

	/**
	 * Process the chain using a functor mechanic in a
	 * parallel manner. Supports lambdas.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 * 
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam FunctorT The type of the mechanic functor
	 * to operate on the slots. May be a lambda.
	 * @param InMechanic The mechanic functor to operate with. Can be a lambda.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The status of the operation. 
	 */
	template < EParadigm Paradigm = DefaultParadigm, typename FunctorT = void,
			   typename std::enable_if<(IsSolid || IsUnsafe(Paradigm))
			   						&& !std::is_void<FunctorT>::value
									&& !std::is_function<std::decay_t<FunctorT>>::value, bool>::type = true >
	FORCEINLINE auto
	OperateConcurrently(FunctorT&   InMechanic,
						const int32 ThreadsCountMax,
						const int32 SlotsPerThreadMin = 1,
						const bool  bSync = true) const
	{
		return TOperator<Functor>::template Do<Paradigm>(
			this, InMechanic,
			ThreadsCountMax, SlotsPerThreadMin,
			bSync);
	}

	/**
	 * Process the chain using a function mechanic in a
	 * parallel manner.
	 * 
	 * @note Only solid chains can be safely operated concurrently.
	 *
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam FunctionT The type of the mechanic free function
	 * to operate on the slots.
	 * @param InMechanic The mechanical function to operate with.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The status of the operation. 
	 */
	template < EParadigm Paradigm = DefaultParadigm, typename FunctionT = void,
			   more::enable_if_t<(IsSolid || IsUnsafe(Paradigm)) 
							  && std::is_function<FunctionT>::value, bool> = true >
	FORCEINLINE auto
	OperateConcurrently(FunctionT* const InMechanic,
						const int32      ThreadsCountMax,
						const int32      SlotsPerThreadMin = 1,
						const bool       bSync = true) const
	{
		return TOperator<Function>::template Do<Paradigm>(
			this, InMechanic,
			ThreadsCountMax, SlotsPerThreadMin,
			bSync);
	}

	/**
	 * Process the chain using a functor mechanic.
	 *
	 * @tparam FunctorT The type of the mechanical functor
	 * to operate on the chain slots.
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Ts The type of components to provide to theTComponentTypesPack mechanic.
	 * @return The status of the operation. 
	 */
	template < typename FunctorT, EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE auto
	Operate() const
	{
		return TOperator<Functor>::template Do<Paradigm>(this, FunctorT());
	}

	/**
	 * Process the chain using a functor mechanic.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam FunctorT The type of the mechanical functor
	 * to operate on the chain slots.
	 * @tparam Ts The type of components to provide to theTComponentTypesPack mechanic.
	 * @return The status of the operation. 
	 */
	template < EParadigm Paradigm, typename FunctorT >
	FORCEINLINE auto
	Operate() const
	{
		return TOperator<Functor>::template Do<Paradigm>(this, FunctorT());
	}

	/**
	 * Process the chain using a functor mechanic in a
	 * parallel manner.
	 *
	 * @tparam FunctorT The type of the mechanical functor
	 * to operate on the slots.
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam Ts The type of components to supply to the mechanic.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The outcome of the operation. 
	 */
	template < typename FunctorT, EParadigm Paradigm = DefaultParadigm,
			   more::enable_if_t<(IsSolid || IsUnsafe(Paradigm))
			   				  && !std::is_void<FunctorT>::value, bool> = true >
	FORCEINLINE auto
	OperateConcurrently(const int32 ThreadsCountMax,
						const int32 SlotsPerThreadMin = 1,
						const bool  bSync = true) const
	{
		return TOperator<Functor>::template Do<Paradigm>(this, FunctorT(), ThreadsCountMax, SlotsPerThreadMin, bSync);
	}

	/**
	 * Process the chain using a functor mechanic in a
	 * parallel manner.
	 *
	 * @tparam Paradigm The security paradigm to utilize.
	 * @tparam FunctorT The type of the mechanical functor
	 * to operate on the slots.
	 * @tparam Ts The type of components to supply to the mechanic.
	 * @param ThreadsCountMax The maximum number of threads to process with.
	 * @param SlotsPerThreadMin The minimum number of slots per thread to process.
	 * @param bSync Should the operation be synced within the current branch.
	 * @return The outcome of the operation. 
	 */
	template < EParadigm Paradigm, typename FunctorT,
			   more::enable_if_t<(IsSolid || IsUnsafe(Paradigm))
			   				  && !std::is_void<FunctorT>::value, bool> = true >
	FORCEINLINE auto
	OperateConcurrently(const int32 ThreadsCountMax,
						const int32 SlotsPerThreadMin = 1,
						const bool  bSync = true) const
	{
		return TOperator<Functor>::template Do<Paradigm>(this, FunctorT(), ThreadsCountMax, SlotsPerThreadMin, bSync);
	}

	/// @}
#pragma endregion Operating

	/**
	 * Begin iterating the chain using an embedded cursor.
	 * 
	 * @param CursorId The identifier of the cursor to use.
	 * Will be obtained anew if doesn't exist.
	 * @param Offset The slot offset to begin iterating with.
	 * @param Limit The maximum number of slots to iterate by.
	 * @return Are there any actual slots to be iterated?
	 */
	inline bool
	Begin(const int32 CursorId = DefaultCursorId,
		  const int32 Offset   = 0,
		  const int32 Limit    = TNumericLimits<int32>::Max()) const
	{
		checkf(!bDisposed, TEXT("The chain should not be disposed in order to begin iterating."));
		checkf(!IsCursorViable(CursorId), TEXT("The chain must not be already iterated by."));

		return ObtainCursor(CursorId).Begin(Offset, Limit);
	}

	/**
	 * Advance iterating the chain to the next iteration
	 * using the embedded cursor.
	 * 
	 * @param CursorId The identifier of an already iterating cursor.
	 * @return Was the advancement successful?
	 */
	FORCEINLINE bool
	Advance(const int32 CursorId = DefaultCursorId) const
	{
		checkf(!bDisposed, TEXT("The chain should not be disposed in order to advance the iteration."));
		checkf(IsCursorViable(CursorId), TEXT("Must already be iterating in order to advance."));

		return Cursors[CursorId].Advance();
	}

	/**
	 * Begin or advance iterating the chain.
	 * 
	 * @param CursorId The identifier of the cursor. Created anew if needed.
	 * @param Offset The slot offset to begin iterating with.
	 * @param Limit The maximum number of slots to iterate by.
	 * @return Is the iteration possible? Returns @c false 
	 * if the iteration should be ended.
	 */
	FORCEINLINE bool
	BeginOrAdvance(const int32 CursorId = DefaultCursorId,
				   const int32 Offset   = 0,
				   const int32 Limit    = TNumericLimits<int32>::Max()) const
	{
		auto& Cursor = ObtainCursor(CursorId);
		if (Cursor.IsViable())
		{
			return Advance(CursorId);
		}
		return Begin(CursorId, Offset, Limit);
	}

#pragma region Iteration State
	/// @name Iteration State
	/// @{

	/**
	 * Get the subject of the current iteration.
	 * 
	 * @return The subject of the current iteration.
	 */
	FORCEINLINE SubjectHandleType
	GetSubject(const int32 CursorId = DefaultCursorId) const
	{
		checkf(IsCursorViable(CursorId),
			TEXT("Must be inside of a chain iteration to get a subject handle."));
		return Cursors[CursorId].GetSubject();
	}

	/**
	 * Get the subjective of the current iteration (if any).
	 * 
	 * @return The subjective of the current iteration
	 * or @c nullptr, if there is none.
	 */
	FORCEINLINE SubjectivePtrType
	GetSubjective(const int32 CursorId = DefaultCursorId) const
	{
		checkf(IsCursorViable(CursorId),
			TEXT("Must be inside of a chain iteration to get a subjective."));
		return Cursors[CursorId].GetSubjective();
	}

	/// @}
#pragma endregion Iteration State

#pragma region Traits Getting
	/// @name Traits Getting
	/// @{

	/**
	 * Get a trait of a certain type on the current iteration.
	 * 
	 * @param CursorId The identifier of the cursor to get from.
	 * @param TraitType The type of the trait.
	 * @param OutTraitData The output trait data.
	 * @param bTraitDataInitialized Was trait data initialized prior to calling
	 * this function?
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	GetTrait(const int32          CursorId,
			 UScriptStruct* const TraitType,
			 void* const          OutTraitData,
			 const bool           bTraitDataInitialized = true) const
	{
		if (LIKELY(ensure(IsCursorViable(CursorId))))
		{
			auto& Cursor = GetCursor(CursorId);
			return Cursor.GetTrait(TraitType,
								   OutTraitData,
								   bTraitDataInitialized);
		}
		return EApparatusStatus::InvalidArgument;
	}

	/**
	 * Get the trait on the current iteration.
	 * 
	 * @param TraitType The type of the trait to get.
	 * @param OutTraitData The output trait data buffer.
	 * The size of the buffer must be enough to store the whole trait.
	 * @param bTraitDataInitialized Was trait data initialized prior to calling
	 * this function?
	 * @return The status of the operation.
	 */
	FORCEINLINE EApparatusStatus
	GetTrait(UScriptStruct* const TraitType,
			 void* const          OutTraitData,
			 const bool           bTraitDataInitialized = true) const
	{
		return GetTrait(DefaultCursorId, TraitType,
						OutTraitData,
						bTraitDataInitialized);
	}

	/**
	 * Get a trait on the current iteration. Templated version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param CursorId The identifier of the cursor to get from.
	 * @param OutTraitData The output trait data buffer.
	 * The size of the buffer must be enough to store the whole trait.
	 * @param bTraitDataInitialized Was trait data initialized prior to calling
	 * this function?
	 * @return The status of the operation.
	 */
	template < typename T >
	FORCEINLINE EApparatusStatus
	GetTrait(const int32 CursorId,
			 T&          OutTraitData,
			 const bool  bTraitDataInitialized = true) const
	{
		if (LIKELY(ensure(IsCursorViable(CursorId))))
		{
			auto& Cursor = GetCursor(CursorId);
			return Cursor.GetTrait(OutTraitData,
								   bTraitDataInitialized);
		}
		return EApparatusStatus::InvalidArgument;
	}

	/**
	 * Get the trait on the current iteration. Templated version.
	 * 
	 * @tparam T The type of the trait.
	 * @param OutTraitData The output trait data.
	 * @param bTraitDataInitialized Was trait data initialized prior to calling
	 * this function?
	 * @return The status of the operation.
	 */
	template < typename T >
	FORCEINLINE EApparatusStatus
	GetTrait(T&         OutTraitData,
			 const bool bTraitDataInitialized = true) const
	{
		return GetTrait(DefaultCursorId, T::StaticStruct(),
						static_cast<void*>(&OutTraitData),
						bTraitDataInitialized);
	}

	/**
	 * Get a trait copy on the current iteration. Templated version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @param CursorId The identifier of the cursor to examine.
	 * @return The copy of the trait.
	 */
	template < typename T >
	FORCEINLINE T
	GetTrait(const int32 CursorId = DefaultCursorId) const
	{
		if (LIKELY(ensure(IsCursorViable(CursorId))))
		{
			auto& Cursor = GetCursor(CursorId);
			return Cursor.template GetTrait<T>();
		}
		return T();
	}

	/**
	 * Get the trait on the current iteration
	 * at a specified index hinted.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param CursorId The identifier of the cursor to get from.
	 * @param TraitIndexHint The index of the trait.
	 * @param TraitType The type of the trait.
	 * @param OutTraitData The output trait data.
	 * @param bTraitDataInitialized Was trait data initialized prior to calling
	 * this function?
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TOutcome<Paradigm>
	GetTraitHinted(const int32    CursorId,
				   UScriptStruct* TraitType,
				   const int32    TraitIndexHint,
				   void* const    OutTraitData,
				   const bool     bTraitDataInitialized = true) const
	{
		AssessCondition(Paradigm, IsCursorViable(CursorId), EApparatusStatus::InvalidArgument);
		auto& Cursor = GetCursor(CursorId);
		return Cursor.template GetTraitHinted<Paradigm>(
									TraitType, TraitIndexHint,
									OutTraitData,
									bTraitDataInitialized);
	}

	/**
	 * Get the trait on the current iteration
	 * at a specified index hinted.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitType The type of the trait to get.
	 * @param TraitIndexHint The index of the trait hinting.
	 * @param OutTraitData The output trait data.
	 * @param bTraitDataInitialized Was trait data initialized prior to calling
	 * this function?
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TOutcome<Paradigm>
	GetTraitHinted(UScriptStruct* TraitType,
				   const int32    TraitIndexHint,
				   void* const    OutTraitData,
				   const bool     bTraitDataInitialized = true) const
	{
		return GetTraitHinted<Paradigm>(
					DefaultCursorId, TraitType, TraitIndexHint,
					OutTraitData,
					bTraitDataInitialized);
	}

	/**
	 * Get the trait on the current iteration
	 * at a specified index hinted. Templated version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @param[in] CursorId The identifier of the cursor to get from.
	 * @param[in] TraitIndexHint The index of the trait hinting.
	 * @param[out] OutTraitData The output trait data.
	 * @param[in] bTraitDataInitialized Was trait data initialized prior to calling
	 * this function?
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm>
	GetTraitHinted(const int32 CursorId,
				   const int32 TraitIndexHint,
				   T&          OutTraitData,
				   const bool  bTraitDataInitialized = true) const
	{
		AssessCondition(Paradigm, IsCursorViable(CursorId), EApparatusStatus::InvalidArgument);
		auto& Cursor = GetCursor(CursorId);
		return Cursor.template GetTraitHinted<Paradigm>(
								TraitIndexHint, OutTraitData,
								bTraitDataInitialized);
	}

	/**
	 * Get the trait on the current iteration
	 * at a specified index hinted. Templated version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @param[in] CursorId The identifier of the cursor to get from.
	 * @param[in] TraitIndexHint The index of the trait hinting.
	 * @param[out] OutTraitData The output trait data.
	 * @param[in] bTraitDataInitialized Was trait data initialized prior to calling
	 * this function?
	 * @return The status of the operation.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE auto
	GetTraitHinted(const int32 CursorId,
				   const int32 TraitIndexHint,
				   T&          OutTraitData,
				   const bool  bTraitDataInitialized = true) const
	{
		return GetTraitHinted<Paradigm>(CursorId, TraitIndexHint, OutTraitData, bTraitDataInitialized);
	}

	/**
	 * Get the trait on the current iteration
	 * at a specified index hinted. Templated version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait.
	 * @param TraitIndexHint The index of the trait hint.
	 * @param OutTraitData The output trait data.
	 * @param bTraitDataInitialized Was trait data initialized prior to calling
	 * this function?
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE auto
	GetTraitHinted(const int32 TraitIndexHint,
				   T&          OutTraitData,
				   const bool  bTraitDataInitialized = true) const
	{
		return GetTraitHinted<Paradigm>(DefaultCursorId, TraitIndexHint, OutTraitData,
										bTraitDataInitialized);
	}

	/**
	 * Get the trait on the current iteration
	 * at a specified index hinted. Templated version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait.
	 * @param TraitIndexHint The index of the trait hint.
	 * @param OutTraitData The output trait data.
	 * @param bTraitDataInitialized Was trait data initialized prior to calling
	 * this function?
	 * @return The status of the operation.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE auto
	GetTraitHinted(const int32 TraitIndexHint,
				   T&          OutTraitData,
				   const bool  bTraitDataInitialized = true) const
	{
		return GetTraitHinted<Paradigm>(TraitIndexHint, OutTraitData, bTraitDataInitialized);
	}

	/**
	 * Get a copy of a trait on the current iteration
	 * at a specified index hinted. Templated version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait.
	 * @param CursorId The identifier of the embedded cursor.
	 * @param TraitIndexHint The hinting index of the trait.
	 * @return A copy of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm, T>
	GetTraitHinted(const int32 CursorId,
				   const int32 TraitIndexHint) const
	{
		AssessCondition(Paradigm, IsCursorViable(CursorId),
					(MakeOutcome<Paradigm, T>(EApparatusStatus::InvalidArgument, T())));
		auto& Cursor = GetCursor(CursorId);
		return Cursor.template GetTraitHinted<Paradigm, T>(TraitIndexHint);
	}

	/**
	 * Get a copy of a trait on the current iteration
	 * at a specified index hinted. Templated version.
	 * 
	 * @tparam T The type of the trait.
	 * @tparam Paradigm The paradigm to work under.
	 * @param CursorId The identifier of the embedded cursor.
	 * @param TraitIndexHint The hinting index of the trait.
	 * @return A copy of the trait.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE auto
	GetTraitHinted(const int32 CursorId,
				   const int32 TraitIndexHint) const
	{
		return GetTraitHinted<Paradigm, T>(CursorId, TraitIndexHint);
	}

	/**
	 * Get a copy of a trait on the current iteration
	 * at a specified index hinted. Templated version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait.
	 * @param TraitIndexHint The index of the trait hint.
	 * @return A copy of the trait.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm, T>
	GetTraitHinted(const int32 TraitIndexHint) const
	{
		return GetTraitHinted<Paradigm, T>(DefaultCursorId, TraitIndexHint);
	}

	/**
	 * Get a copy of a trait on the current iteration
	 * at a specified index hinted. Templated version.
	 * 
	 * @tparam T The type of the trait.
	 * @tparam Paradigm The paradigm to work under.
	 * @param TraitIndexHint The index of the trait hint.
	 * @return A copy of the trait.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TOutcome<Paradigm, T>
	GetTraitHinted(const int32 TraitIndexHint) const
	{
		return GetTraitHinted<Paradigm, T>(TraitIndexHint);
	}

	/// @}
#pragma endregion Traits Getting

#pragma region Traits Data Access
	/// @name Traits Data Access
	/// @{

	/**
	 * Get a pointer to a trait of a certain type on the current iteration.
	 * 
	 * @tparam Paradigm The security paradigm override to use.
	 * @param CursorId The identifier of the cursor to get from.
	 * @param TraitType The type of the trait to get.
	 * @return The resulting trait data.
	 */
	template < EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>
	GetTraitPtr(const int32          CursorId,
				UScriptStruct* const TraitType) const
	{
		AssessCondition(Paradigm, IsCursorViable(CursorId),
					(MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::InvalidArgument, nullptr)));
		auto& Cursor = GetCursor(CursorId);
		return Cursor.template GetTraitPtr<Paradigm>(TraitType);
	}

	/**
	 * Get a pointer to a trait of a certain type on the current iteration.
	 * Templated version.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The security paradigm override to use.
	 * @param CursorId The identifier of the cursor to get from.
	 * @return The resulting trait data.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitPtrResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtr(const int32 CursorId = DefaultCursorId) const
	{
		AssessCondition(Paradigm, IsCursorViable(CursorId),
						(MakeOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>(EApparatusStatus::InvalidArgument, nullptr)));
		const auto& Cursor = GetCursor(CursorId);
		return Cursor.template GetTraitPtr<Paradigm, T>();
	}

	/**
	 * Get a pointer to a trait of a certain type on the current iteration.
	 * Templated version.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The security paradigm override to use.
	 * @param CursorId The identifier of the cursor to get from.
	 * @return The resulting trait data.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm,
			   TTraitPtrResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtr(const int32 CursorId = DefaultCursorId) const
	{
		return GetTraitPtr<Paradigm, T>(CursorId);
	}

	/**
	 * Get a trait reference on the current iteration.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The security paradigm override to use.
	 * @param CursorId The identifier of the cursor to get from.
	 * @return The resulting trait data.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitRefResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRef(const int32 CursorId = DefaultCursorId) const
	{
		if (AvoidCondition(Paradigm, !IsCursorViable(CursorId)))
		{
			static thread_local T TraitTemp;
			return MakeOutcome<Paradigm, TTraitRefResult<Paradigm, T>>(EApparatusStatus::InvalidArgument, TraitTemp);
		}
		const auto& Cursor = GetCursor(CursorId);
		return Cursor.template GetTraitRef<Paradigm, T>();
	}

	/**
	 * Get a trait reference on the current iteration.
	 * 
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam Paradigm The security paradigm override to use.
	 * @param CursorId The identifier of the cursor to get from.
	 * @return The resulting trait data.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm,
			   TTraitRefResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRef(const int32 CursorId = DefaultCursorId) const
	{
		return GetTraitRef<Paradigm, T>(CursorId);
	}

	/**
	 * Get a trait pointer of a specific type on the current iteration.
	 * 
	 * @tparam Paradigm The security paradigm override to use.
	 * @param TraitType The type of the trait to get.
	 * @return The resulting trait data.
	 */
	template < EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TTraitVoidPtrResult<Paradigm>
	GetTraitPtr(UScriptStruct* const TraitType) const
	{
		return GetTraitPtr<Paradigm>(DefaultCursorId, TraitType);
	}

	/**
	 * Get a pointer to the trait on the current iteration
	 * at a specified index hinted.
	 * 
	 * @tparam Paradigm The security paradigm override.
	 * @param CursorId The identifier of the cursor to get from.
	 * @param TraitType The type of the trait to get.
	 * @param TraitIndexHint The index of the trait hinting.
	 * @return A pointer to the trait of the designated type.
	 */
	template < EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>
	GetTraitPtrHinted(const int32          CursorId,
					  UScriptStruct* const TraitType,
					  const int32          TraitIndexHint) const
	{
		AssessCondition(Paradigm, IsCursorViable(CursorId),
						(MakeOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>(EApparatusStatus::InvalidArgument, nullptr)));
		auto& Cursor = GetCursor(CursorId);
		return Cursor.template GetTraitPtrHinted<Paradigm>(TraitType, TraitIndexHint);
	}

	/**
	 * Get a pointer to the trait on the current iteration
	 * at a specified index hinted.
	 *
	 * @tparam Paradigm The security paradigm override.
	 * @param TraitType The type of the trait to get.
	 * @param TraitIndexHint The index of the trait hinting.
	 * @return A pointer to the trait of the designated type.
	 */
	template < EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TOutcome<Paradigm, TTraitVoidPtrResult<Paradigm>>
	GetTraitPtrHinted(UScriptStruct* const TraitType,
					  const int32          TraitIndexHint) const
	{
		return GetTraitPtrHinted<Paradigm>(DefaultCursorId, TraitType, TraitIndexHint);
	}

	/**
	 * Get a pointer to the trait on the current iteration
	 * at a specified index hinted. Templated version.
	 *
	 * @tparam Paradigm The security paradigm override.
	 * @tparam T The type of the trait to get.
	 * @param CursorId The identifier of the cursor to get from.
	 * @param TraitIndexHint The index of the trait hinting.
	 * @return A pointer to the trait of the designated type.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtrHinted(const int32 CursorId,
					  const int32 TraitIndexHint) const
	{
		AssessCondition(Paradigm, IsCursorViable(CursorId),
					(MakeOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>(EApparatusStatus::InvalidArgument, nullptr)));
		auto& Cursor = GetCursor(CursorId);
		return Cursor.template GetTraitPtrHinted<Paradigm, T>(TraitIndexHint);
	}

	/**
	 * Get a pointer to the trait on the current iteration
	 * at a specified index hinted. Templated version.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The security paradigm override.
	 * @param CursorId The identifier of the cursor to get from.
	 * @param TraitIndexHint The index of the trait hinting.
	 * @return A pointer to the trait of the designated type.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtrHinted(const int32 CursorId,
					  const int32 TraitIndexHint) const
	{
		return GetTraitPtrHinted<Paradigm, T>(CursorId, TraitIndexHint);
	}

	/**
	 * Get a pointer to the trait on the current iteration
	 * at a specified index hinted. Templated version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The security paradigm override.
	 * @param TraitIndexHint The index of the trait hinting.
	 * @return A pointer to the trait of the designated type.
	 */
	template < EParadigm Paradigm, typename T >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtrHinted(const int32 TraitIndexHint) const
	{
		return GetTraitPtrHinted<Paradigm, T>(DefaultCursorId, TraitIndexHint);
	}

	/**
	 * Get a pointer to the trait on the current iteration
	 * at a specified index hinted. Templated version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The security paradigm override.
	 * @param TraitIndexHint The index of the trait hinting.
	 * @return A pointer to the trait of the designated type.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TOutcome<Paradigm, TTraitPtrResult<Paradigm, T>>
	GetTraitPtrHinted(const int32 TraitIndexHint) const
	{
		return GetTraitPtrHinted<Paradigm, T>(TraitIndexHint);
	}

	/**
	 * Get a reference for the trait on the current iteration
	 * at a specified index hinted. Templated version.
	 *
	 * @tparam Paradigm The security paradigm override.
	 * @tparam T The type of the trait to get.
	 * @param CursorId The identifier of the cursor to get from.
	 * @param TraitIndexHint The index of the trait hinting.
	 * @return The trait reference.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitPtrResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRefHinted(const int32 CursorId,
					  const int32 TraitIndexHint) const
	{
		if (AvoidCondition(Paradigm, !IsCursorViable(CursorId)))
		{
			static thread_local T TraitTemp;
			return MakeOutcome<Paradigm, TTraitRefResult<Paradigm, T>>(EApparatusStatus::InvalidArgument, TraitTemp);
		}
		auto& Cursor = GetCursor(CursorId);
		return Cursor.template GetTraitRefHinted<Paradigm, T>(TraitIndexHint);
	}

	/**
	 * Get a reference for the trait on the current iteration
	 * at a specified index hinted. Templated version.
	 *
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The security paradigm override.
	 * @param CursorId The identifier of the cursor to get from.
	 * @param TraitIndexHint The index of the trait hinting.
	 * @return The trait reference.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm,
			   TTraitPtrResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRefHinted(const int32 CursorId,
					  const int32 TraitIndexHint) const
	{
		return GetTraitRefHinted<Paradigm, T>(CursorId, TraitIndexHint);
	}

	/**
	 * Get a reference for the trait on the current iteration
	 * at a specified index hinted. Templated version.
	 * 
	 * @tparam Paradigm The security paradigm override.
	 * @tparam T The type of the trait to get.
	 * @param TraitIndexHint The index of the trait hinting.
	 * @return The trait reference.
	 */
	template < EParadigm Paradigm, typename T,
			   TTraitPtrResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRefHinted(const int32 TraitIndexHint) const
	{
		return GetTraitRefHinted<Paradigm, T>(DefaultCursorId, TraitIndexHint);
	}

	/**
	 * Get a reference for the trait on the current iteration
	 * at a specified index hinted. Templated version.
	 * 
	 * @tparam T The type of the trait to get.
	 * @tparam Paradigm The security paradigm override.
	 * @param TraitIndexHint The index of the trait hinting.
	 * @return The trait reference.
	 */
	template < typename T, EParadigm Paradigm = DefaultParadigm,
			   TTraitPtrResultSecurity<Paradigm, T> = 0 >
	FORCEINLINE TOutcome<Paradigm, TTraitRefResult<Paradigm, T>>
	GetTraitRefHinted(const int32 TraitIndexHint) const
	{
		return GetTraitRefHinted<Paradigm, T>(TraitIndexHint);
	}

	/// @}
#pragma endregion Traits Data Access

#pragma region Details Getting
	/// @name Details Getting
	/// @{

	/**
	 * Get a detail of a specific type on the current iteration.
	 *
	 * @tparam Paradigm The paradigm to work under.
	 * @param CursorId The identifier of the embedded cursor.
	 * @param DetailClass The class of the detail to get.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = DefaultParadigm >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<UDetail>>
	GetDetail(const int32                CursorId,
			  const TSubclassOf<UDetail> DetailClass) const
	{
		AssessCondition(Paradigm, IsCursorViable(CursorId),
					(MakeOutcome<Paradigm, TDetailPtrResult<UDetail>>(EApparatusStatus::InvalidArgument, nullptr)));
		return GetCursor(CursorId).template GetDetail<Paradigm>(DetailClass);
	}

	/**
	 * Get the detail on the current iteration
	 * of a certain class.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass The class of the detail to get.
	 * @return The detail of the current iteration at a certain class.
	 */
	template < EParadigm Paradigm = DefaultParadigm,
			   TDetailPtrResultSecurity<UDetail> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<UDetail>>
	GetDetail(const TSubclassOf<UDetail> DetailClass) const
	{
		return GetDetail<Paradigm>(DefaultCursorId, DetailClass);
	}

	/**
	 * Get the detail on the current iteration
	 * of a certain class. Templated version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The class of the detail to get.
	 * @return The detail of the current iteration at a certain class.
	 */
	template < EParadigm Paradigm, class D, TDetailPtrResultSecurity<D> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<D>>
	GetDetail(const int32 CursorId = DefaultCursorId) const
	{
		AssessCondition(Paradigm, IsCursorViable(CursorId),
					(MakeOutcome<Paradigm, TDetailPtrResult<D>>(EApparatusStatus::InvalidArgument, nullptr)));
		return GetCursor(CursorId).template GetDetail<Paradigm, D>();
	}

	/**
	 * Get the detail on the current iteration
	 * of a certain class. Templated version.
	 * 
	 * @tparam D The class of the detail to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @param CursorId The identifier of the embedded cursor.
	 * @return The detail of the current iteration of a certain class.
	 */
	template < class D, EParadigm Paradigm = DefaultParadigm,
			   TDetailPtrResultSecurity<D> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<D>>
	GetDetail(const int32 CursorId = DefaultCursorId) const
	{
		return GetDetail<Paradigm, D>(CursorId);
	}

	/**
	 * Get the detail on the current iteration
	 * at a certain index.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param CursorId The identifier of the cursor to examine.
	 * @param DetailClass The class of the detail to get.
	 * @param DetailIndexHint The hinting index of the detail.
	 * @return The detail of the current iteration at a certain index.
	 */
	template < EParadigm Paradigm = DefaultParadigm,
			   TDetailPtrResultSecurity<UDetail> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<UDetail>>
	GetDetailHinted(const int32                CursorId,
					const TSubclassOf<UDetail> DetailClass,
					const int32                DetailIndexHint) const
	{
		AssessCondition(Paradigm, IsCursorViable(CursorId),
					(MakeOutcome<Paradigm, TDetailPtrResult<UDetail>>(EApparatusStatus::InvalidArgument, nullptr)));
		return GetCursor(CursorId).template GetDetailHinted<Paradigm>(DetailClass, DetailIndexHint);
	}

	/**
	 * Get the detail on the current iteration
	 * at a certain index.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailClass The class of the detail to get.
	 * @param DetailIndexHint The index of the detail hinting.
	 * @return The detail of the current iteration at a certain index.
	 */
	template < EParadigm Paradigm = DefaultParadigm,
			   TDetailPtrResultSecurity<UDetail> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<UDetail>>
	GetDetailHinted(const TSubclassOf<UDetail> DetailClass,
					const int32                DetailIndexHint) const
	{
		return GetDetailHinted<Paradigm>(DefaultCursorId, DetailClass, DetailIndexHint);
	}

	/**
	 * Get the detail on the current iteration
	 * at a certain index hinted. Templated version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam D The class of the detail to get.
	 * @param CursorId The identifier of the cursor.
	 * @param DetailIndexHint The index of the detail hinting.
	 * @return The detail of the current iteration at a certain index.
	 */
	template < EParadigm Paradigm, class D,
			   TDetailPtrResultSecurity<D> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<D>>
	GetDetailHinted(const int32 CursorId,
					const int32 DetailIndexHint) const
	{
		AssessCondition(Paradigm, IsCursorViable(CursorId),
					(MakeOutcome<Paradigm, TDetailPtrResult<UDetail>>(EApparatusStatus::InvalidArgument, nullptr)));
		return GetCursor(CursorId).template GetDetailHinted<Paradigm, D>(DetailIndexHint);
	}

	/**
	 * Get the detail on the current iteration
	 * at a certain index hinted. Templated version.
	 * 
	 * @tparam D The class of the detail to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @param CursorId The identifier of the cursor.
	 * @param DetailIndexHint The index of the detail hinting.
	 * @return The detail of the current iteration at a certain index.
	 */
	template < class D, EParadigm Paradigm = DefaultParadigm,
			   TDetailPtrResultSecurity<D> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<D>>
	GetDetailHinted(const int32 CursorId,
					const int32 DetailIndexHint) const
	{
		return GetDetailHinted<Paradigm, D>(CursorId, DetailIndexHint);
	}

	/**
	 * Get the detail on the current iteration
	 * at a certain index hinted. Templated version.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @tparam T The class of the detail to get.
	 * @param DetailIndexHint The index of the detail hinting.
	 * @return The detail of the current iteration at a certain index.
	 */
	template < EParadigm Paradigm, class D,
			   TDetailPtrResultSecurity<D> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<D>>
	GetDetailHinted(const int32 DetailIndexHint) const
	{
		return GetDetailHinted<Paradigm, D>(DefaultCursorId, DetailIndexHint);
	}

	/**
	 * Get the detail on the current iteration
	 * at a certain index hinted. Templated version.
	 * 
	 * @tparam T The class of the detail to get.
	 * @tparam Paradigm The paradigm to work under.
	 * @param DetailIndexHint The index of the detail hinting.
	 * @return The detail of the current iteration at a certain index.
	 */
	template < class D, EParadigm Paradigm = DefaultParadigm,
			   TDetailPtrResultSecurity<D> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<D>>
	GetDetailHinted(const int32 DetailIndexHint) const
	{
		return GetDetailHinted<Paradigm, D>(DetailIndexHint);
	}

	/**
	 * Get the detail on the current iteration
	 * at a certain line index.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param CursorId The embedded cursor identifier.
	 * @param LineIndex The index of the detail line.
	 * @return The detail of the current iteration at a certain index.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   TDetailPtrResultSecurity<UDetail> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<UDetail>>
	DetailAtLine(const int32 CursorId,
				 const int32 LineIndex) const
	{
		AssessConditionFormat(Paradigm, IsCursorViable(CursorId),
					 EApparatusStatus::InvalidArgument,
					 TEXT("Must be inside of a chain iteration to get a detail at a certain index."));
		return GetCursor(CursorId).template DetailAtLine<Paradigm>(LineIndex);
	}

	/**
	 * Get the detail on the current iteration
	 * at a certain line index.
	 *
	 * @tparam Paradigm The paradigm to work under..
	 * @param LineIndex The index of the detail line.
	 * @return The detail of the current iteration at a certain index.
	 */
	template < EParadigm Paradigm = EParadigm::Default,
			   TDetailPtrResultSecurity<UDetail> = 0 >
	FORCEINLINE TOutcome<Paradigm, TDetailPtrResult<UDetail>>
	DetailAtLine(const int32 LineIndex) const
	{
		return DetailAtLine<Paradigm>(DefaultCursorId, LineIndex);
	}

	/// @}
#pragma endregion Details Getting

#pragma region State Examination
	/// @name State Examination
	/// @{

	/**
	 * Check if there any any viable (active) embedded
	 * cursors within the chain.
	 *
	 * @return The state of examination.
	 */
	FORCEINLINE bool
	HasViableEmbeddedCursors() const
	{
		for (const auto& Cursor : Cursors)
		{
			if (Cursor.IsViable()) return true;
		}
		return false;
	}

	/**
	 * Get the total number of iterable entities available.
	 * within the chain.
	 * 
	 * This would actually include the subjects that could
	 * be filtered out due to iterating-time checks (i.e. flagmarks).
	 */
	FORCEINLINE int32
	IterableNum() const
	{
		int32 Count = 0;
		for (const auto& Segment : Segments)
		{
			Count += Segment.IterableNum();
		}
		return Count;
	}

	/// @}
#pragma endregion State Examination

#pragma region Assignment
	/// @name Assignment
	/// @{

	/**
	 * Move a chain.
	 */
	FORCEINLINE TChain&
	operator=(TChain&& InChain)
	{
		Reset();

		Segments = MoveTemp(InChain.Segments);
		Cursors  = MoveTemp(InChain.Cursors);

		return *this;
	}

	/**
	 * Set a chain equal to another chain.
	 * 
	 * @note The embedded cursors won't get copied.
	 * 
	 * @return Returns a reference for itself.
	 */
	FORCEINLINE TChain&
	operator=(const TChain& InChain)
	{
		Reset();

		// The segments assignment will also make locks appropriately:
		Segments = InChain.Segments;
		return *this;
	}

	/// @}
#pragma endregion Assignment

#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Initialize a chain with its owner.
	 */
	FORCEINLINE
	TChain(AMechanism* const InOwner)
	  : Super(InOwner)
	{}

	/**
	 * Construct a new chain as a copy of another one.
	 * 
	 * @note The embedded cursors won't get copied.
	 * 
	 * @param InChain A chain to copy.
	 */
	FORCEINLINE
	TChain(const TChain& InChain)
	  : Super(InChain.Owner)
	  , Segments(InChain.Segments)
	{}

	/**
	 * Finalize the chain.
	 */
	FORCEINLINE virtual
	~TChain()
	{
		if (bInDestructor) return;
		TGuardValue<bool> Guard(bInDestructor, true);
		// This is needed to be called here explicitly since otherwise the
		// members will be destroyed prior to proper waiting:
		Dispose();
	}

	/// @}
#pragma endregion Initialization

#pragma region Bulk-Copying
	/// @name Bulk-Copying
	/// @{

	/**
	 * The type of gathering to
	 * use during the collection.
	 */
	enum EGatheringType
	{
		/**
		 * Clear the output buffer before collecting.
		 */
		Clear,

		/**
		 * Append to the existing elements of the outputting buffer.
		 */
		Collect
	};

	/**
	 * Copy the traits of the enchained subjects to an array.
	 * 
	 * @tparam T The type of traits to copy.
	 * Must match the current filtering used.
	 * @tparam AllocatorT The allocator used within the array.
	 * @param[out] OutArray The array to output the traits to.
	 * @param[in] GatheringType The type of gathering to use.
	 * @return The status of the operation. 
	 */
	template < typename T, typename AllocatorT >
	EApparatusStatus
	GetTraits(TArray<T, AllocatorT>& OutArray,
			  const EGatheringType   GatheringType = Clear) const;

	/// @}
#pragma endregion Bulk-Copying

}; //-struct TChain

/**
 * Mutable sequence holder.
 */
typedef TChain<FChunkIt, FBeltIt> FChain;

/**
 * Unsafe mutable sequence holder.
 */
typedef TChain<FChunkIt, FBeltIt, EParadigm::Unsafe> FUnsafeChain;

/**
 * Solid sequence holder.
 */
typedef TChain<FSolidChunkIt, FSolidBeltIt> FSolidChain;

/**
 * Unsafe solid sequence holder.
 */
typedef TChain<FSolidChunkIt, FSolidBeltIt, EParadigm::Unsafe> FUnsafeSolidChain;

/**
 * A chain-iterating, subject-pointing cursor.
 */
typedef typename FChain::FCursor FChainCursor;

/**
 * A chain-iterating, subject-pointing cursor.
 * Template pack version.
 * 
 * @tparam Ps The types of parts to deliver efficiently.
 */
template < typename ...Ps >
using TChainCursor = FChain::TCursor<Ps...>;

/**
 * An unsafe chain-iterating, subject-pointing cursor.
 */
typedef typename FUnsafeChain::FCursor FUnsafeChainCursor;

/**
 * An unsafe chain-iterating, subject-pointing cursor.
 * Template pack version.
 * 
 * @tparam Ps The types of parts to deliver efficiently.
 */
template < typename ...Ps >
using TUnsafeChainCursor = FUnsafeChain::TCursor<Ps...>;

/**
 * A homogenous chain-iterating, subject-pointing cursor.
 */
typedef typename FSolidChain::FCursor FSolidChainCursor;

/**
 * A homogenous chain-iterating, subject-pointing cursor.
 * Template pack version.
 * 
 * @tparam Ps The types of parts to deliver efficiently.
 */
template < typename ...Ps >
using TSolidChainCursor = FSolidChain::TCursor<Ps...>;

/**
 * An unsafe homogenous chain-iterating, subject-pointing cursor.
 */
typedef typename FUnsafeSolidChain::FCursor FUnsafeSolidChainCursor;
