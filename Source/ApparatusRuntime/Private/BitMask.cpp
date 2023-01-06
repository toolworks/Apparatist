/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BitMask.cpp
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

#include "BitMask.h"


const FBitMask
FBitMask::Zero = FBitMask();

const FBitMask::GroupType
FBitMask::ZeroGroup;

const FBitMask::GroupType
FBitMask::FirstOneGroup;

const FBitMask::GroupType
FBitMask::LastOneGroup;

const FBitMask::GroupType
FBitMask::AllOnesGroup;

FBitMask::StaticConstructor FBitMask::staticConstructor;

int32 FBitMask::bitsCountLUT[FBitMask::bitsCountLUTSize];

int32 FBitMask::DifferencesCount(const FBitMask &BitMask) const
{
	int32 Res = 0;
	if (Groups.Num() == BitMask.Groups.Num())
	{
		for (int32 i = 0; i < Groups.Num(); ++i)
		{
			const GroupType bgA = Groups[i];
			const GroupType bgB = BitMask.Groups[i];
			if (bgA != bgB)
			{
				Res += CountOneBits(bgA ^ bgB);
			}
		}
	}
	else
	{
		auto bitGroupsCount =
			FMath::Max(Groups.Num(), BitMask.Groups.Num());
		for (int32 i = 0; i < bitGroupsCount; ++i)
		{
			const GroupType bgA = GroupAt(i);
			const GroupType bgB = BitMask.GroupAt(i);
			if (bgA != bgB)
			{
				Res += CountOneBits(bgA ^ bgB);
			}
		}
	}
	return Res;
}

int32 FBitMask::InclusionsCount(const FBitMask &BitMask) const
{
	int32 Res = 0;
	if (Groups.Num() == BitMask.Groups.Num())
	{
		for (int32 i = 0; i < Groups.Num(); ++i)
		{
			const GroupType bgA = Groups[i];
			const GroupType bgB = BitMask.Groups[i];
			if (bgA != bgB)
			{
				Res += CountOneBits(bgA & bgB);
			}
			else
			{
				Res += GroupSizeBits;
			}
		}
	}
	else
	{
		auto bitGroupsCount =
			FMath::Max(Groups.Num(), BitMask.Groups.Num());
		for (int32 i = 0; i < bitGroupsCount; ++i)
		{
			const GroupType bgA = GroupAt(i);
			const GroupType bgB = BitMask.GroupAt(i);
			if (bgA != bgB)
			{
				Res += CountOneBits(bgA & bgB);
			}
			else
			{
				Res += GroupSizeBits;
			}
		}
	}
	return Res;
}

int32
FBitMask::IndexOf(const bool  Bit,
				  const int32 Offset/*=0*/) const
{
	if (Bit)
	{
		const auto FirstGroupIndex = Offset >> GroupShift;
		if (FirstGroupIndex >= Groups.Num())
		{
			return INDEX_NONE;
		}
		// First group checking...
		const auto FirstGroup = Groups[FirstGroupIndex];
		if (FirstGroup != ZeroGroup)
		{
			for (int32 bi = (Offset & GroupBitIndexMask); bi < GroupSizeBits; ++bi)
			{
				if ((FirstGroup & (((GroupType)1) << bi)) != ZeroGroup)
				{
					return (FirstGroupIndex << GroupShift) + bi;
				}
			}
		}
		// Remaining groups checking...
		for (int32 gi = FirstGroupIndex + 1; gi < Groups.Num(); ++gi)
		{
			if (Groups[gi] != ZeroGroup)
			{
				for (int32 bi = 0; bi < GroupSizeBits; ++bi)
				{
					if ((Groups[gi] & (((GroupType)1) << bi)) != ZeroGroup)
					{
						return (gi << GroupShift) + bi;
					}
				}
				check(false);
			}
		}
		return INDEX_NONE;
	}
	else // if (!Bit)
	{
		const auto FirstGroupIndex = Offset >> GroupShift;
		if (FirstGroupIndex >= Groups.Num())
		{
			return Offset;
		}
		// First group checking...
		const auto FirstGroup = Groups[FirstGroupIndex];
		if (FirstGroup != AllOnesGroup)
		{
			for (int32 bi = (Offset & GroupBitIndexMask); bi < GroupSizeBits; ++bi)
			{
				if ((FirstGroup & (((GroupType)1) << bi)) == ZeroGroup)
				{
					return (FirstGroupIndex << GroupShift) + bi;
				}
			}
		}
		for (int32 gi = FirstGroupIndex + 1; gi < Groups.Num(); ++gi)
		{
			if (Groups[gi] != AllOnesGroup)
			{
				for (int32 bi = 0; bi < GroupSizeBits; ++bi)
				{
					if ((Groups[gi] & (((GroupType)1) << bi)) == ZeroGroup)
					{
						return (gi << GroupShift) + bi;
					}
				}
				check(false);
			}
		}
		return Groups.Num() << GroupShift;
	}
}

void FBitMask::Insert(const int32 Index, bool Bit)
{
	checkf(Index >= 0 && Index <= BitsNum(),
			TEXT("An index is out of range: %d"), Index);

	if (Index == BitsNum())
	{
		// The bit past the last is to be inserted.
		// Set it in a usual way...
		SetAt(Index, Bit);
		return;
	}

	const int32 GroupIndex = Index >> GroupShift;
	if (GroupIndex >= Groups.Num())
	{
		// The group index is above
		// the maximum allocated.
		if (Bit)
		{
			EnsureGroupsNum(GroupIndex + 1);
		}
		else
		{
			// No change would actually
			// be made if we're setting a zero flag
			// above the limit, so return:
			return;
		}
	}
	else
	{
		// There are enough groups
		// in the mask, but we have to see
		// if the shifting should produce a new group.
		if (Groups.Last() & LastOneGroup)
		{
			// The last bit is meaningful within the last group
			// so we have to add a new group:
			Groups.Add(ZeroGroup);
		}
	}

	const int32 BitIndex = Index & GroupBitIndexMask;

	// Shift all of the bits past the index.
	const auto NewBitInGroup = ((GroupType)Bit) << BitIndex;
	auto TargetGroup     = Groups[GroupIndex];
	auto TargetGroupMask = (((GroupType)1 << BitIndex) - 1);
	// Save the top-most bit in the group since it gets erased by the shift:
	GroupType SavedBit = Groups[GroupIndex] >> (GroupSizeBits - 1);
	// Shift all but pre-bit bits in the active group:
	Groups[GroupIndex] = ((TargetGroup & ~TargetGroupMask) << 1) |
						  (TargetGroup & TargetGroupMask) |
						 NewBitInGroup;
	// Shift the remaining groups (if any) alltogether:
	for (int32 sgi = GroupIndex + 1; sgi < Groups.Num(); sgi ++)
	{
		GroupType NewGroup = (Groups[sgi] << 1) | SavedBit;
		SavedBit = Groups[sgi] >> (GroupSizeBits - 1);
		Groups[sgi] = NewGroup;
	}
}

void FBitMask::Remove(const bool Bit)
{
	if (Groups.Num() == 0)
	{
		return;
	}
	for (int32 i = 0; i < BitsNum(); ++i)
	{
		if (AtFast(i) == Bit)
		{
			RemoveAt(i);
			return;
		}
	}
	return;
}

void FBitMask::RemoveAt(const int32 Index)
{
	checkf(Index >= 0, TEXT("Invalid index to remove at: %d"), Index);
	if (Index >= BitsNum()) return; // Nothing to remove.
	const int32 GroupIndex = Index >> GroupShift;
	const int32 BitIndex   = Index & GroupBitIndexMask;
	if (Index == BitsNum() - 1)
	{
		Groups[GroupIndex] &= ~(((GroupType)1) << BitIndex);
		return;
	}
	GroupType SavedBit = 0;
	// Shift all but post-bit bits in the active group...
	const GroupType ShiftMask = (1 << BitIndex) - 1;
	Groups[GroupIndex] = ((Groups[GroupIndex] >> 1) & ~ShiftMask) |
						 (Groups[GroupIndex] & ShiftMask);
	for (int32 i = GroupIndex; i < Groups.Num() - 1; i++)
	{
		SavedBit = Groups[i + 1] & 1;
		Groups[i] |= SavedBit;
		Groups[i + 1] >>= 1;
	}
}
