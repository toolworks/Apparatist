/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BitMask.h
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

#include "CoreMinimal.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define BIT_MASK_H_SKIPPED_MACHINE_H
#endif

#include "ApparatusStatus.h"
#include "ApparatusCustomVersion.h"

#ifdef BIT_MASK_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "BitMask.generated.h"


/**
 * A memory-efficient bit mask.
 * 
 * Please, note that this is not a usual bit array
 * and serves some different purposes.
 */
USTRUCT(BlueprintType)
struct APPARATUSRUNTIME_API FBitMask
{
	GENERATED_BODY()

  public:

	/**
	 * The type of the bit group elements.
	 */
	typedef uint64 GroupType;

#pragma region Constants

	enum
	{
		/**
		 * The size of the group type in bits.
		 */
		GroupSizeBits = sizeof(GroupType) * 8,

		/**
		 * A number of bits to shift (right) for getting a group index from a bit index.
		 */
		GroupShift = 6,

		/**
		 * A mask for the group to get an index of the bit from.
		 */
		GroupBitIndexMask = GroupSizeBits - 1,

		/**
		 * The number of entries in the bits count LUT.
		 */
		bitsCountLUTSize = 256,

		/**
		 * The number of inline-allocated groups.
		 */
		InlineGroupsCount = 4
	};

	/**
	 * A group of all one bits set.
	 */
	static const GroupType
	ZeroGroup{0};

	/**
	 * A group with the first bit set.
	 */
	static const GroupType
	FirstOneGroup{1};

	/**
	 * A group with the last bit set.
	 */
	static const GroupType
	LastOneGroup{(((GroupType)1) << (GroupSizeBits - 1))};

	/**
	 * A group of all one bits set.
	 */
	static const GroupType
	AllOnesGroup{TNumericLimits<GroupType>::Max()};

#pragma endregion Constants

	/**
	 * A special adapter for
	 * setting individual bits.
	 */
	struct FBitAccessor
	{
	  private:

		friend struct FBitMask;

		FBitMask& Owner;

		int32     BitIndex;

		FORCEINLINE
		FBitAccessor(FBitMask* const InOwner,
				   const int32     InBitIndex)
		  : Owner(*InOwner)
		  , BitIndex(InBitIndex)
		{}

	  public:

		FORCEINLINE
		FBitAccessor(const FBitAccessor& InBitAccessor)
		  : Owner(InBitAccessor.Owner)
		  , BitIndex(InBitAccessor.BitIndex)
		{}

		FORCEINLINE operator bool() const
		{
			return Owner.At(BitIndex);
		}

		FORCEINLINE FBitAccessor&
		operator=(const bool InValue)
		{
			Owner.SetAt(BitIndex, InValue);
			return *this;
		}

	}; // struct FBitAccessor

  private:

	/**
	 * A zero bit mask.
	 */
	static const FBitMask Zero;

	/**
	 * The current bit groups of the mask.
	 */
	TArray<uint64, TInlineAllocator<InlineGroupsCount>> Groups;

	/**
	 * Safely get a bit group by its index, 0 is returned for out of range
	 * indices.
	 */
	FORCEINLINE GroupType
	GroupAt(const int32 Index) const
	{
		checkf(Index >= 0, TEXT("An index can't be negative: %d"), Index);
		if (Index >= Groups.Num())
			return ZeroGroup;
		return Groups[Index];
	}

	friend class StaticConstructor;

	struct StaticConstructor
	{
		FORCEINLINE StaticConstructor()
		{
			for (size_t i = 0; i < bitsCountLUTSize; ++i)
			{
				int32 c = 0;
				for (int32 j = 0; j < 8; ++j)
				{
					if (((i >> j) & 1) != 0)
						++c;
				}
				bitsCountLUT[i] = c;
			}
		}
	};

	static StaticConstructor staticConstructor;

	/**
	 * The bits count LUT.
	 */
	static int32 bitsCountLUT[bitsCountLUTSize];

	/**
	 * Get the number of one bits in a group.
	 */
	static FORCEINLINE int32
	CountOneBits(const GroupType BitGroup)
	{
		return bitsCountLUT[0xff & (BitGroup)] + 
			   bitsCountLUT[0xff & (BitGroup >> 8)] +
			   bitsCountLUT[0xff & (BitGroup >> 16)] +
			   bitsCountLUT[0xff & (BitGroup >> 24)] +
			   bitsCountLUT[0xff & (BitGroup >> 32)] +
			   bitsCountLUT[0xff & (BitGroup >> 40)] +
			   bitsCountLUT[0xff & (BitGroup >> 48)] +
			   bitsCountLUT[0xff & (BitGroup >> 56)];
	}

	/**
	 * Get a bit at a certain index. Fast unsafe version.
	 */
	FORCEINLINE bool
	AtFast(const int32 Index) const 
	{
		checkf(Index >= 0, TEXT("An index must be non-negative: %d"), Index);
		checkf(Index < BitsNum(),
			   TEXT("An index must be less than the total available bit count: %d >= %d"), Index, BitsNum());

		const auto Group = Groups[Index >> GroupShift];
		return (Group & (FirstOneGroup << (Index & GroupBitIndexMask))) !=
			   ZeroGroup;
	}

	friend uint32
	GetTypeHash(const FBitMask &bm);

  public:

	/**
	 * Get the number of available bit groups.
	 */
	FORCEINLINE int32
	GroupsNum() const
	{
		return Groups.Num();
	}

	/**
	 * Get the number of available (allocated)
	 * bits.
	 */
	FORCEINLINE int32
	BitsNum() const
	{
		return Groups.Num() << GroupShift;
	}

	/**
	 * Get the bit flag at the specified index.
	 * 
	 * @param Index The index of the bit to get. If it's out of the number of elements,
	 * @c false is returned.
	 * @return The state of the bit at the specified index.
	 */
	FORCEINLINE bool
	At(const int32 Index) const
	{
		checkf(Index >= 0, TEXT("A bit index must be non-negative. Got: %d"), Index);

		if (UNLIKELY(Index >= BitsNum()))
			return false;

		return AtFast(Index);
	}

	/**
	 * Get a bit element by its index.
	 * Constant version.
	 */
	FORCEINLINE bool
	operator[](const int32 Index) const
	{
		return At(Index);
	}

	/**
	 * Get a bit element by its index.
	 */
	FORCEINLINE FBitAccessor
	operator[](const int32 Index)
	{
		return FBitAccessor(this, Index);
	}

	/**
	 * Set a bit at a specific index.
	 */
	FORCEINLINE void
	SetAt(const int32 Index, const bool Value)
	{
		checkf(Index >= 0, TEXT("An index must be non-negative: %d"),
				Index);

		auto gi = Index >> GroupShift;

		if (Value)
		{
			if (Index >= BitsNum())
			{
				EnsureGroupsForBitAt(Index);
			}
			Groups[gi] |= (((GroupType)1) << (Index & GroupBitIndexMask));
		}
		else
		{
			if (gi < Groups.Num())
			{
				Groups[gi] &=
					~(((GroupType)1) << (Index & GroupBitIndexMask));
			}
		}
	}

	/**
	 * Check if there are any non-zero bits in the mask.
	 */
	FORCEINLINE bool
	IsNonZero() const
	{
		for	(int32 i = 0; i < Groups.Num(); ++i)
		{
			if (Groups[i])
			{
				return true;
			}
		}
		return false;
	}

#pragma region Comparison

	/**
	 * Compare two bit masks for equality.
	 */
	FORCEINLINE bool
	operator==(const FBitMask& Other) const
	{
		if (UNLIKELY(this == std::addressof(Other)))
		{
			return true;
		}
		const auto MaxCount = FMath::Max(Groups.Num(), Other.Groups.Num());
		for (int32 gi = 0; gi < MaxCount; ++gi)
		{
			if (GroupAt(gi) != Other.GroupAt(gi))
				return false;
		}
		return true;
	}

	/**
	 * Compare two bit masks for inequality.
	 */
	FORCEINLINE bool
	operator!=(const FBitMask& Other) const
	{
		if (UNLIKELY(this == std::addressof(Other)))
		{
			return false;
		}
		const auto MaxCount = FMath::Max(Groups.Num(), Other.Groups.Num());
		for (int32 gi = 0; gi < MaxCount; ++gi)
		{
			if (GroupAt(gi) != Other.GroupAt(gi))
				return true;
		}
		return false;
	}

	/**
	 * Compare two bit masks for equality. Editor-friendly method.
	 * 
	 * @param Other The other bit mask to compare to.
	 * @param PortFlags The contextual port flags.
	 * @return The state of examination.
	 */
	bool
	Identical(const FBitMask* Other, uint32 PortFlags) const
	{
#if WITH_EDITOR
		if (!FApp::IsGame())
		{
			// Correct support for property editing
			// requires direct groups arrays comparison.
			return Groups == Other->Groups;
		}
#endif
		return (*this) == (*Other);
	}

#pragma endregion Comparison

	/**
	 * Trim the meaningless high-order groups.
	 * 
	 * @param bAllowShrinking Is physical array size allowed to shrunk?
	 */
	FORCEINLINE EApparatusStatus
	Trim(const bool bAllowShrinking = true) const
	{
		if (Groups.Num() == 0) return EApparatusStatus::Noop;
		const auto MutableThis = const_cast<FBitMask*>(this);
		int32 RemovedCount = 0;
		for	(int32 i = Groups.Num() - 1; i >= 0; --i)
		{
			if (Groups[i] == ZeroGroup)
			{
				RemovedCount += 1;
			}
		}
		if (RemovedCount > 0)
		{
			MutableThis->Groups.RemoveAt(Groups.Num() - RemovedCount, RemovedCount, bAllowShrinking);
			return EApparatusStatus::Success;
		}
		return EApparatusStatus::Noop;
	}

	/**
	 * Check if all of the bits in the mask are zeroes or none.
	 */
	FORCEINLINE bool
	IsAllZeroes() const
	{
		for	(int32 i = 0; i < Groups.Num(); ++i)
		{
			if (Groups[i])
			{
				return false;
			}
		}
		return true;
	}

	/**
	 * Get the number of meaningful (non-zero) bit groups.
	 */
	FORCEINLINE int32
	CountNonZeroGroups() const
	{
		for	(int32 i = Groups.Num() - 1; i >= 0; --i)
		{
			if (Groups[i])
			{
				return i + 1;
			}
		}
		return 0;
	}

	/**
	 * Move the bit mask to this bit mask.
	 */
	FORCEINLINE EApparatusStatus
	Set(FBitMask&& InBitMask)
	{
		if (UNLIKELY(Groups == InBitMask.Groups))
		{
			return EApparatusStatus::Noop;
		}
		Groups = MoveTemp(InBitMask.Groups);
		return EApparatusStatus::Success;
	}

	/**
	 * Set this bit mask equal to another bit mask.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InBitMask The bit mask to set to.
	 * @return The outcome of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	FORCEINLINE TOutcome<Paradigm>
	Set(const FBitMask& InBitMask)
	{
		if (UNLIKELY(std::addressof(InBitMask) == this))
		{
			return EApparatusStatus::Noop;
		}
		if (IsPolite(Paradigm) && UNLIKELY(Groups == InBitMask.Groups))
		{
			return EApparatusStatus::Noop;
		}
		Groups = InBitMask.Groups;
		return EApparatusStatus::Success;
	}

	/**
	 * Move the bit mask.
	 */
	FORCEINLINE FBitMask& operator=(FBitMask&& InBitMask)
	{
		Groups = MoveTemp(InBitMask.Groups);
		return *this;
	}

	/**
	 * Set this bit mask equal to another bit mask.
	 */
	FORCEINLINE FBitMask& operator=(const FBitMask& InBitMask)
	{
		if (LIKELY(std::addressof(InBitMask) != this))
		{
			Groups = InBitMask.Groups;
		}
		return *this;
	}

	/**
	 * Get the number of different bits in two masks.
	 */
	int32
	DifferencesCount(const FBitMask& BitMask) const;

	/**
	 * Get the number of included bits in two masks.
	 */
	int32
	InclusionsCount(const FBitMask& BitMask) const;

	/**
	 * Does the mask has all of the bits set in the supplied mask.
	 * 
	 * The same as a bitwise A & B == B.
	 */
	bool
	Includes(const FBitMask& BitMask) const
	{
		for (int32 gi = 0; gi < BitMask.Groups.Num(); ++gi)
		{
			const GroupType BitGroup      = GroupAt(gi);
			const GroupType OtherBitGroup = BitMask.Groups[gi];

			const GroupType Masked = BitGroup & OtherBitGroup;
			if (Masked != OtherBitGroup)
			{
				return false;
			}
		}
		return true;
	}

	/**
	 * Check if the mask has any of the bits set in the supplied mask.
	 * 
	 * The same as a bitwise A & B != 0.
	 */
	bool
	IncludesPartially(const FBitMask& BitMask) const
	{
		const int32 GroupsCount = FMath::Min(Groups.Num(), BitMask.Groups.Num());
		if (UNLIKELY(GroupsCount == 0)) return false;
		for (int32 gi = 0; gi < GroupsCount; ++gi)
		{
			const GroupType BitGroup      = Groups[gi];
			const GroupType OtherBitGroup = BitMask.Groups[gi];

			const GroupType Masked = BitGroup & OtherBitGroup;
			if (Masked != ZeroGroup)
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * The bit mask bit iterator.
	 * Constant version.
	 */
	struct ConstIterator
	{
		/**
		 * The owner of the iterator.
		 */
		const FBitMask& Owner;

		enum
		{
			/**
			 * Invalid bit index.
			 */
			InvalidIndex = -1
		};

		/**
		 * The index of the bit.
		 */
		int32 Index = InvalidIndex;

		FORCEINLINE
		ConstIterator(const FBitMask* const InOwner,
					  const int32           InIndex = 0)
		  : Owner(*InOwner)
		  , Index(InIndex)
		{}

	  public:

		FORCEINLINE bool
		operator*() const { return Owner[Index]; }

		FORCEINLINE bool
		next()
		{ 
			if (Index <= InvalidIndex) return false;
			Index += 1;
			if (Index >= Owner.BitsNum()) Index = InvalidIndex;
			return Index > InvalidIndex;
		}

		FORCEINLINE ConstIterator&
		operator++()
		{
			next();
			return *this;
		}

		FORCEINLINE ConstIterator
		operator++(int)
		{
			ConstIterator Temp(*this);
			next();
			return MoveTemp(Temp);
		}

		FORCEINLINE bool
		operator==(const ConstIterator& OtherIterator) const
		{
			checkf(std::addressof(Owner) == std::addressof(OtherIterator.Owner),
				   TEXT("Comparing iterators from two different bit masks is not supported."));
			return Index == OtherIterator.Index;
		}

		FORCEINLINE bool
		operator!=(const ConstIterator& OtherIterator) const
		{
			checkf(std::addressof(Owner) == std::addressof(OtherIterator.Owner),
				   TEXT("Comparing iterators from two different bit masks is not supported."));
			return Index != OtherIterator.Index;
		}
	};

	/**
	 * The bit mask bit iterator.
	 */
	struct Iterator
	{
		/**
		 * The owner of the iterator.
		 */
		FBitMask& Owner;

		enum
		{
			/**
			 * Invalid bit index.
			 */
			InvalidIndex = -1
		};

		/**
		 * The index of the bit.
		 */
		int32 Index = InvalidIndex;

		FORCEINLINE
		Iterator(FBitMask* const InOwner,
				 const int32     InIndex = 0)
		  : Owner(*InOwner)
		  , Index(InIndex)
		{}

	  public:

		FORCEINLINE FBitAccessor
		operator*() const { return Owner[Index]; }

		FORCEINLINE bool
		next()
		{ 
			if (Index <= InvalidIndex) return false;
			Index += 1;
			if (Index >= Owner.BitsNum()) Index = InvalidIndex;
			return Index > InvalidIndex;
		}

		FORCEINLINE Iterator&
		operator++()
		{
			next();
			return *this;
		}

		FORCEINLINE Iterator
		operator++(int)
		{
			Iterator Temp(*this);
			next();
			return MoveTemp(Temp);
		}

		FORCEINLINE bool
		operator==(const Iterator& OtherIterator) const
		{
			checkf(std::addressof(Owner) == std::addressof(OtherIterator.Owner),
				   TEXT("Comparing iterators from two different bit masks is not supported."));
			return Index == OtherIterator.Index;
		}

		FORCEINLINE bool
		operator!=(const Iterator& OtherIterator) const
		{
			checkf(std::addressof(Owner) == std::addressof(OtherIterator.Owner),
				   TEXT("Comparing iterators from two different bit masks is not supported."));
			return Index != OtherIterator.Index;
		}
	};

	FORCEINLINE Iterator
	begin()
	{
		return Iterator(this); 
	}

	FORCEINLINE ConstIterator
	begin() const
	{
		return ConstIterator(this); 
	}

	FORCEINLINE Iterator end()
	{
		return Iterator(this, Iterator::InvalidIndex);
	}

	FORCEINLINE ConstIterator end() const
	{
		return ConstIterator(this, Iterator::InvalidIndex);
	}

	/**
	 * Get the index of the first matching bit in the mask.
	 * 
	 * The mask is considered to be zero-limitless,
	 * so a falsy bit is always found in the end.
	 * 
	 * @param Bit The bit state to find.
	 * @param Offset The offset index to start with.
	 * @return The index of the bit in question or @c INDEX_NONE,
	 * if the bit couldn't be found.
	 */
	int32
	IndexOf(const bool Bit, const int32 Offset = 0) const;

	/**
	 * Ensure a bit group count.
	 */
	FORCEINLINE void
	EnsureGroupsNum(const int32 InGroupsCount)
	{
		Groups.Reserve(InGroupsCount);

		while (Groups.Num() < InGroupsCount)
		{
			Groups.Add(ZeroGroup);
		}
	}

	/**
	 * Ensure that there is enough bit groups for an index.
	 */
	FORCEINLINE void
	EnsureGroupsForBitAt(const int32 BitIndex)
	{
		const int32 GroupIndex = BitIndex >> GroupShift;
		EnsureGroupsNum(GroupIndex + 1);
	}

	/**
	 * Get the current maximum number of bit groups that can be stored
	 * without any additional allocations.
	 */
	FORCEINLINE int32
	GroupsMax() const
	{
		return Groups.Max();
	}


	/**
	 * Get the current maximum number of bits that can be stored
	 * without any additional allocations.
	 */
	FORCEINLINE int32
	BitsMax() const
	{
		return GroupsMax() << GroupShift;
	}

	/**
	 * Reserve space for a given number of bits.
	 */
	FORCEINLINE void
	Reserve(const int32 BitCapacity)
	{
		Groups.Reserve((BitCapacity >> GroupShift) + 1);
	}

	/**
	 * Empties the array.
	 * It calls the destructors on held flags if needed.
	 *
	 * @param Slack The expected usage size in bits after empty operation.
	 */
	FORCEINLINE void
	Empty(const int32 Slack = 0)
	{
		checkf(Slack >= 0, TEXT("A Slack must be non-negative: %d"), Slack);
		Groups.Empty(FMath::DivideAndRoundUp(Slack, (int32)GroupSizeBits));
	}

	/**
	 * Same as empty, but doesn't change memory allocations, unless the new size
	 * is larger than the current array.
	 *
	 * @param NewSize The expected usage size in bits after calling this function.
	 */
	FORCEINLINE void
	Reset(const int32 NewSize = 0)
	{
		checkf(NewSize >= 0, TEXT("A NewSize must be non-negative: %d"), NewSize);
		Groups.Reset(FMath::DivideAndRoundUp(NewSize, (int32)GroupSizeBits));
	}

	/**
	 * Set the bits based on a mask.
	 * 
	 * @param Mask The mask to use while setting the bits.
	 * @param State The state to set.
	 */
	void
	SetMasked(const FBitMask& Mask, const bool State)
	{
		if (Mask.BitsNum() == 0) return;
		const int32 NonZeroGroupsCount = Mask.CountNonZeroGroups();
		if (NonZeroGroupsCount == 0) return;
		const GroupType FullSet = State ? TNumericLimits<GroupType>::Max() : 0;
		EnsureGroupsNum(NonZeroGroupsCount);
		for	(int32 i = 0; i < NonZeroGroupsCount; ++i)
		{
			const GroupType MaskGroup = Mask.Groups[i];
			if (!MaskGroup) continue;
			
			Groups[i] = (Groups[i] & (~MaskGroup)) | //: Save all bits but masked.
						   (MaskGroup & FullSet); //: Add masked bits.
		}
	}

	/**
	 * Insert a bit at the specified index.
	 */
	void Insert(const int32 Index, const bool Bit);

	/**
	 * Remove the first occurrence of the specified bit from the mask.
	 */
	void Remove(const bool Bit);

	/**
	 * Remove the bit at the specified index.
	 * 
	 * @param Index The index of the bit to remove.
	 */
	void RemoveAt(const int32 Index);

	/**
	 * Remove an element at the specified index.
	 *
	 * @param Index An index of the element to erase.
	 */
	FORCEINLINE void
	Erase(const int32 Index)
	{
		RemoveAt(Index);
	}

	/**
	 * Logically-and the bitmask with a given mask.
	 * 
	 * @tparam Paradigm The paradigm to work under.
	 * @param InMask The mask to intersect with.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	inline TOutcome<Paradigm>
	Intersect(const FBitMask& InMask)
	{
		if (UNLIKELY(std::addressof(InMask) == this))
		{
			return EApparatusStatus::Noop;
		}
		auto Status = EApparatusStatus::Noop;

		if (Groups.Num() > InMask.Groups.Num())
		{
			// Search for higher non-zero groups and zero them.
			if (IsPolite(Paradigm)) // Compile-time branch.
			{
				for (int32 i = InMask.Groups.Num(); i < Groups.Num(); ++i)
				{
					if (Groups[i] != ZeroGroup)
					{
						Status = EApparatusStatus::Success;
						break;
					}
				}
			}
			Groups.RemoveAt(InMask.Groups.Num(), Groups.Num() - InMask.Groups.Num(), /*bAllowShrinking=*/false);
		}

		for (int32 i = 0; i < Groups.Num(); ++i)
		{
			if (IsHarsh(Paradigm)) // Compile-time branch.
			{
				Groups[i] &= InMask.Groups[i];
			}
			else
			{
				const auto NewGroup = Groups[i] & InMask.Groups[i];
				if (LIKELY(Groups[i] != NewGroup))
				{
					Groups[i] = NewGroup;
					Status = EApparatusStatus::Success;
				}
			}
		}

		return Status;
	}

	/**
	 * Conjunct the bitmask with a given mask.
	 */
	FORCEINLINE FBitMask&
	operator&=(const FBitMask& InMask)
	{
		Intersect(InMask);
		return *this;
	}

	/**
	 * Logical-or the bitmask with a an another mask.
	 * 
	 * This can actually be slower than FBitMask::operator|=
	 * since it provides a status.
	 * 
	 * @tparam Paradigm The paradigm to work on.
	 * @param InMask The mask to include.
	 * @return The status of the operation.
	 */
	template < EParadigm Paradigm = EParadigm::Default >
	inline TOutcome<Paradigm>
	Include(const FBitMask& InMask)
	{
		if (UNLIKELY(std::addressof(InMask) == this))
		{
			return EApparatusStatus::Noop;
		}
		const int32 NewBitGroupsCount = FMath::Max(Groups.Num(),
												   InMask.Groups.Num());
		EnsureGroupsNum(NewBitGroupsCount);

		auto Status = EApparatusStatus::Noop;
		for (int32 i = 0; i < InMask.Groups.Num(); ++i)
		{
			if (IsHarsh(Paradigm))
			{
				Groups[i] |= InMask.Groups[i];
			}
			else
			{
				const auto NewGroup = Groups[i] | InMask.Groups[i];
				if (LIKELY(Groups[i] != NewGroup))
				{
					Groups[i] = NewGroup;
					Status = EApparatusStatus::Success;
				}
			}
		}

		return Status;
	}

	/**
	 * Disjunct the bitmask with a given mask.
	 */
	FORCEINLINE FBitMask&
	operator|=(const FBitMask& InMask)
	{
		Include(InMask);
		return *this;
	}

	/**
	 * Convert the bit mask to a string representation.
	 */
	FORCEINLINE FString
	ToString() const
	{
		FString str;
		for (int32 i = 0; i < BitsNum(); ++i)
		{
			auto b = AtFast(i);
			str += b ? TEXT("1") : TEXT("0");
		}
		return str;
	}

	friend FBitMask
	operator&(const FBitMask& MaskA, const FBitMask& MaskB);

	friend FBitMask
	operator|(const FBitMask& MaskA, const FBitMask& MaskB);

	/**
	 * Calculate the bit mask hash.
	 */
	uint32
	CalcHash() const
	{
		uint32 HashCode = 0;
		int32 gi;
		// Skip meaningless high-order groups...
		for (gi = Groups.Num() - 1; gi >= 0; --gi)
		{
			if (Groups[gi] != ZeroGroup)
			{
				break;	
			}
		}
		for (; gi >= 0; --gi)
		{
			const auto Group = Groups[gi];
			HashCode = HashCombine(HashCode, GetTypeHash(Group));
		}
		return HashCode;
	}

	/**
	 * Serialize the bit mask.
	 */
	FORCEINLINE bool
	Serialize(FArchive& Archive)
	{
		Archive.UsingCustomVersion(FApparatusCustomVersion::GUID);
		const int32 Version = Archive.CustomVer(FApparatusCustomVersion::GUID);
		if (Version < FApparatusCustomVersion::Online)
		{
			return false;
		}

		int32 GroupsNum = Groups.Num();
		Archive << GroupsNum;
		if (Archive.IsLoading())
		{
			Reset();
			EnsureGroupsNum(GroupsNum);
		}
		for (int32 i = 0; i < GroupsNum; ++i)
		{
			Archive << Groups[i];
		}
		return true;
	}

#pragma region Initialization
	/// @name Initialization
	/// @{

	/**
	 * Construct a new empty bit mask.
	 */
	FORCEINLINE
	FBitMask() {}

	/**
	 * Copy-construct the bit mask.
	 */
	FORCEINLINE FBitMask(const FBitMask& InBitMask)
	  : Groups(InBitMask.Groups)
	{}

	/**
	 * Move-construct the bit mask.
	 */
	FORCEINLINE FBitMask(FBitMask&& InBitMask)
	  : Groups(MoveTemp(InBitMask.Groups))
	{}

	/**
	 * Initialize a new instance of a mask with a certain bit capacity.
	 */
	FORCEINLINE
	FBitMask(const int32 Capacity)
	{
		Reserve(Capacity);
	}

	/**
	 * Construct a new bit mask
	 * with an initializer list.
	 */
	FORCEINLINE
	FBitMask(std::initializer_list<bool> list)
	{
		Reserve(list.size());
		int32 i = 0;
		for (const auto b : list)
		{
			SetAt(i++, b);
		}
	}

	/// @}
#pragma endregion Initialization

}; //-struct FBitMask

/**
 * Logically-conjunct the bits in masks.
 * 
 * @param MaskA The first mask to conjunct.
 * @param MaskB The second mask to conjunct.
 * @return The resulting conjunct mask.
 */
inline FBitMask
operator&(const FBitMask& MaskA, const FBitMask& MaskB)
{
	const auto GroupsCount = FMath::Min(MaskA.Groups.Num(), MaskB.Groups.Num());
	auto Result = FBitMask();
	Result.EnsureGroupsNum(GroupsCount);
	for (int32 i = 0; i < GroupsCount; ++i)
	{
		Result.Groups[i] = MaskA.Groups[i] & MaskB.Groups[i];
	}
	return MoveTemp(Result);
}

/**
 * Logically-disjunct the bits in masks.
 * 
 * @param MaskA The first mask to disjunct.
 * @param MaskB The second mask to disjunct.
 * @return The resulting disjunct mask.
 */
inline FBitMask
operator|(const FBitMask& MaskA, const FBitMask &MaskB)
{
	const auto GroupsCount = FMath::Max(MaskA.Groups.Num(), MaskB.Groups.Num());
	auto Result = FBitMask();
	Result.EnsureGroupsNum(GroupsCount);
	for (int32 i = 0; i < GroupsCount; ++i)
	{
		Result.Groups[i] = MaskA.GroupAt(i) | MaskB.GroupAt(i);
	}
	return MoveTemp(Result);
}

FORCEINLINE uint32
GetTypeHash(const FBitMask& BitMask)
{
	return BitMask.CalcHash();
}

template<>
struct TStructOpsTypeTraits<FBitMask> : public TStructOpsTypeTraitsBase2<FBitMask>
{
	enum 
	{
		WithCopy = true,
		WithIdentical = true,
		WithSerializer = true
	}; 
};
