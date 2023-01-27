/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Belt.cpp
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

#include "Belt.h"

#include "Machine.h"


DECLARE_CYCLE_STAT(TEXT("Belts ~ Fetch Details"), STAT_Belts_FetchDetails,
				   STATGROUP_Machine);

DECLARE_CYCLE_STAT(TEXT("Belt ~ Fetch Details"), STAT_Belt_FetchDetails,
				   STATGROUP_Machine);

UBelt::ChildLinesCacheEntryType UBelt::EmptyChildLinesEntry;

EApparatusStatus
UBelt::ReleaseSlotAt(const int32 SlotIndex)
{
	checkf((SlotIndex > FBeltSlot::InvalidIndex) && (SlotIndex < Count),
		   TEXT("The slot index to release is invalid (out of range): %d"), SlotIndex);
	checkf(Slots.Num() >= Count,
		   TEXT("Allocated slots size must be larger than or equal to their logical count."));
	checkf(!IsSolid(),
		   TEXT("Releasing the #%d slot inside of a solid belt is not possible."),
		   SlotIndex);

	FBeltSlot& Slot = Slots[SlotIndex];
	check(Slot.Index == SlotIndex);

	// The method does everything needed:
	return Slot.SetRemoved();
}

void
UBelt::BeginDestroy()
{
	checkf(!IsLocked(),
			TEXT("The belt must be unlocked before its destruction."));

	for (int32 i = 0; i < Count; ++i)
	{
		checkf(!Slots[i].HasLockedDetails(),
			   TEXT("No slots should be locked during the belt destruction."));
		// Make sure the subjectives are unregistered correctly
		// from this belt...
		if (UNLIKELY(Slots[i].Subjective && (Slots[i].Subjective->GetBelt() == this)))
		{
			Slots[i].Subjective->TakeBeltSlot(nullptr, FBeltSlot::InvalidIndex);
		}
	}

	const auto Mechanism = GetOwner();
	if (UNLIKELY(Mechanism))
	{
		Mechanism->Belts.Remove(this);
		// Not checking for tag validity is fine here:
		Mechanism->BeltByTag.Remove(Tag);
	}

	Super::BeginDestroy();
}
