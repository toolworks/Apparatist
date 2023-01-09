/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Detailmark.cpp
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

#include "Detailmark.h"

#include "Machine.h"


const FDetailmark FDetailmark::Zero;

FString
FDetailmark::ToString() const
{
	FString str;

	for (auto& d : Details)
	{
		str += TEXT(", ") + d->GetName();
	}

	return MoveTemp(str);
}

void
FDetailmark::PostSerialize(const FArchive& Archive)
{
	if (Archive.IsLoading())
	{
		// Clean-up and adapt the mask...
		DetailsMask.Reset();
		for (int32 i = 0; i < Details.Num(); ++i)
		{
			const auto& Detail = Details[i];
			if (UNLIKELY(!Detail))
			{
				// Null details are fine during the editing.
#if !WITH_EDITOR
				Details.RemoveAtSwap(i--);
#endif
				continue;
			}
			const FBitMask& Mask = GetDetailMask(Detail);
			if (UNLIKELY(DetailsMask.Includes(Mask)))
			{
				// Duplicate details are fine during the editing.
#if !WITH_EDITOR
				Details.RemoveAtSwap(i--);
#endif
				continue;
			}
			DetailsMask.Include(Mask);
		}
	}
};
