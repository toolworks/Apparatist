/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Chunk.cpp
 * Created: 2021-10-04 21:39:55
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

#include "Chunk.h"

#include "Machine.h"


void
UChunk::BeginDestroy()
{
	checkf(!IsLocked(),
			TEXT("The chunk must be unlocked before its destruction."));

	if (TraitsTemp)
	{
		for (int32 i = 0; i < TraitLinesNum(); ++i)
		{
			if (TraitsTemp[i])
			{
				const auto TraitType = TraitLineTypeAt(i);
				TraitType->DestroyStruct(TraitsTemp[i]);
				FMemory::Free(TraitsTemp[i]);
			}
		}
		FMemory::Free(TraitsTemp);
	}

	auto* const Mechanism = GetOwner();
	if (UNLIKELY(Mechanism))
	{
		Mechanism->Chunks.Remove(this);
	}

	Super::BeginDestroy();
}

EApparatusStatus
UChunk::ReleaseSlot(const int32 SlotIndex,
					const bool  bHard)
{
	checkf(!IsSolid(),
		   TEXT("The chunk must not be in a solid state for a slot to be released."));
	check(Count == Slots.Num());
	checkf((SlotIndex > FSubjectInfo::InvalidSlotIndex) && (SlotIndex < Count),
		   TEXT("The index of the slot to release is invalid: %d"), SlotIndex);

	// Note, that the subject can actually be already stale here,
	// since it can be marked by the hard deferred operation.
	// Nevertheless we have to release it now.

	// Always clear the elements so they can be reused later.
	// If the user wants to access the former traits, the
	// subject identifier still in the slot will redirect
	// accordingly (if not despawning completely).
	ClearSubjectAt(SlotIndex, bHard);

	return DoReleaseSlot(SlotIndex);
}
