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
inline TOutcome<Paradigm>
UBelt::Expand(const FDetailmark& InDetailmark)
{
	checkf(bSparse, TEXT("Only sparse belts may be expanded."));

	const auto DetailsNumSave = Detailmark.DetailsNum();
	EApparatusStatus Status = Detailmark.template Add<MakePolite(Paradigm)>(InDetailmark);
	if (UNLIKELY(Status != EApparatusStatus::Success))
	{
		// Nothing was done or an error has happened:
		return Status;
	}

	// Expand all of the slots for the new detail count...
	for (FBeltSlot& Slot : Slots)
	{
		Slot.Expand();
	}

	// Calculate the cached lines indices for new detail lines...
	for (auto li = DetailsNumSave; li < Detailmark.DetailsNum(); ++li)
	{
		const auto DetailClass = Detailmark[li];
		for (auto Parent = DetailClass; Parent != nullptr; Parent = Parent->GetSuperClass())
		{
			auto& LinesCache = ChildLinesCache.FindOrAdd(Parent);
			for (int32 di = 0; di < Detailmark.DetailsNum(); ++di)
			{
				if (Detailmark[di]->IsChildOf(Parent))
				{
					if ((Detailmark[di] == Parent)
					 && ((LinesCache.Num() == 0) ||
						 (LinesCache[0] != di)))
					{
						// Exact matches must always be the first ones:
						InsertSwap(LinesCache, di, 0);
					}
					else
					{
						LinesCache.AddUnique(di);
					}
				}
			}
		}
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
