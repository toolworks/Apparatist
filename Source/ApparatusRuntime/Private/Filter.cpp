/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Filter.cpp
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

#include "Filter.h"

#include "Machine.h"


const FFilter FFilter::Zero(FM_None, FM_None);

const FFilter FFilter::Default;

FString
FFilter::ToString() const
{
	FString Result = TEXT("[") + ::ToString(GetFlagmark()) + TEXT("]");

	for (auto t : Fingerprint.GetTraits())
	{
		Result += TEXT(" && ");
		Result += t->GetName();
	}

	for (auto d : Fingerprint.GetDetails())
	{
		Result += TEXT(" && ");
		Result += d->GetName();
	}

	Result += TEXT(" && ![") + ::ToString(GetExcludingFlagmark()) + TEXT("]");

	for (auto t : GetExcludedTraits())
	{
		Result += TEXT(" && ");
		Result += TEXT("!") + t->GetName();
	}

	for (auto d : GetExcludedDetails())
	{
		Result += TEXT(" && ");
		Result += TEXT("!") + d->GetName();
	}

	return Result;
}

void
FFilter::PostSerialize(const FArchive& Archive)
{
	if (Archive.IsLoading())
	{
		// Make sure the flagmarks are valid...
		Exclude(FM_Stale | FM_DeferredDespawn);

		// Clean-up and adapt the masks...
		ExcludedTraitsMask.Reset();
		for (int32 i = 0; i < ExcludedTraits.Num(); ++i)
		{
			auto* const ExcludedTrait = ExcludedTraits[i];
			if (UNLIKELY(!ExcludedTrait))
			{
				// Null excluded traits are fine during the editing.
#if !WITH_EDITOR
				ExcludedTraits.RemoveAtSwap(i--);
#endif
				continue;
			}
			const FBitMask& ExcludedMask = FTraitmark::GetTraitMask(ExcludedTrait);
			// Duplicate excluded traits are fine during the editing...
#if !WITH_EDITOR
			if (UNLIKELY(ExcludedTraitsMask.Includes(ExcludedMask)))
			{
				
				ExcludedTraits.RemoveAtSwap(i--);
				continue;
			}
#endif
			ExcludedTraitsMask.Include(ExcludedMask);
		}
		ExcludedDetailsMask.Reset();
		for (int32 i = 0; i < ExcludedDetails.Num(); ++i)
		{
			const auto& ExcludedDetail = ExcludedDetails[i];
			if (UNLIKELY(!ExcludedDetail))
			{
				// Null details are fine during the editing.
#if !WITH_EDITOR
				ExcludedDetails.RemoveAtSwap(i--);
#endif
				continue;
			}
			const FBitMask& ExcludedMask = FDetailmark::GetExcludedDetailMask(ExcludedDetail);
			// Duplicate excluded details are fine during the editing...
#if !WITH_EDITOR
			if (UNLIKELY(ExcludedDetailsMask.Includes(ExcludedMask)))
			{
				ExcludedDetails.RemoveAtSwap(i--);
				continue;
			}
#endif
			ExcludedDetailsMask.Include(ExcludedMask);
		}

		// Reset the cache in case of loading...
		HashCache = 0;
	}
};
