/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BeltSlot.cpp
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

#include "BeltSlot.h"

#include "Machine.h"


bool
FBeltSlot::FetchDetails()
{
	check(Owner);

	if (UNLIKELY(!Subjective))
	{
		ResetDetails();
		return false;
	}

	const FDetailmark& BeltDetailmark = Owner->GetDetailmark();
	if (LIKELY(BeltDetailmark.DetailsNum() > 0))
	{
		// Self-expand just in case:
		Expand();

		check(Details.Num() == BeltDetailmark.DetailsNum());
		check(Details.Num() >= Subjective->GetFingerprint().DetailsNum());

		for (int32 i = 0; i < BeltDetailmark.DetailsNum(); ++i)
		{
			Details[i].Fetch(BeltDetailmark.DetailAt(i), Subjective);
		}
	}

	return true;
} //- FetchDetails

bool
FBeltSlot::IsComboValid(const TArray<int32>& DetailsIndices,
						const int32          ComboIndex) const
{
	check(Owner);
	check(IsLocked());
	if (!IsViable()) return false; // This also checks for matching against the current locking filter.

	UDetail* rDetail = nullptr;

	int32 AccumCount = 1; // Accumulated detail count.
	for (int32 i = 0; i < DetailsIndices.Num(); ++i)
	{
		// Calculate the detail sub-index.
		int32 SubIndex = 0;
		for (int32 j = 0; j <= i; ++j)
		{
			const int32 SubDetailIndex = DetailsIndices[j];
			check(SubDetailIndex > -1);
			const auto& Cache = Details[SubDetailIndex];
			const int32 SubCount = Cache.IterableNum();
			if (UNLIKELY(SubCount == 0))
			{
				return false;
			}
			if (LIKELY(SubCount == 1))
			{
				if ((!Cache[0].IsValid()) ||
					(!Cache[0]->IsEnabled()))
				{
					return false;
				}
				continue;
			}

			SubIndex = (ComboIndex / AccumCount) % SubCount;
			AccumCount *= SubCount;
		}

		const auto& Detail = Details[DetailsIndices[i]][SubIndex];
		if (UNLIKELY(!Detail.IsValid() || !Detail->IsEnabled()))
		{
			return false;
		}
	}

	// The combo is valid only if it's within the maximum count:
	return ComboIndex < AccumCount;
} // IsComboValid(const int32 ComboIndex)
