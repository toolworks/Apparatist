/*
 * ░▒▓ APPARATUS ▓▒░
 *
 * File: Belt.inl
 * Created: 2022-11-13 14:12:42
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

template < EParadigm Paradigm/*=EParadigm::Default*/ >
TOutcome<Paradigm>
UBelt::Expand(const FDetailmark& InDetailmark)
{
	checkf(bSparse, TEXT("Only sparse belts may be expanded."));

	EApparatusStatus Status = Detailmark.template AddDecomposed<MakePolite(Paradigm)>(InDetailmark);
	if (UNLIKELY(Status != EApparatusStatus::Success))
	{
		return Status;
	}

	// Expand all of the slots for the new detail count...
	for (FBeltSlot& Slot : Slots)
	{
		Slot.Expand();
	}

	const auto Mechanism = GetOwner();
	if (LIKELY(Mechanism))
	{
		// Update the cache...
		for (auto& Pair : Mechanism->BeltsByFilterCache)
		{
			if (Matches(Pair.Key))
			{
				Pair.Value.AddUnique(this);
			}
			else
			{
				// Make sure we're not participating
				// in some no-longer compatible state.
				Pair.Value.Remove(this);
			}
		}
	}

	return EApparatusStatus::Success;
}
