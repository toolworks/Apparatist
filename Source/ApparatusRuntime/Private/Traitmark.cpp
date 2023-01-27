/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Traitmark.cpp
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

#include "Traitmark.h"

#include "Machine.h"


const FTraitmark FTraitmark::Zero = FTraitmark();

FString
FTraitmark::ToString() const
{
	FString Str;

	for (auto t : Traits)
	{
		Str += TEXT(", ") + t->GetName();
	}

	return MoveTemp(Str);
}

void
FTraitmark::PostSerialize(const FArchive& Archive)
{
	if (Archive.IsLoading())
	{
		// Clean-up and adapt the mask...
		TraitsMask.Reset();
		for (int32 i = 0; i < Traits.Num(); ++i)
		{
			auto* const Trait = Traits[i];
			if (UNLIKELY(Trait == nullptr))
			{
#if !WITH_EDITOR
				// Null traits are removed at run-time:
				Traits.RemoveAtSwap(i--);
#endif
				continue;
			}
			const FBitMask& Mask = GetTraitMask(Trait);
			if (UNLIKELY(TraitsMask.Includes(Mask) && Traits.Contains(Trait)))
			{
#if !WITH_EDITOR
				// Duplicate traits are removed at run-time:
				Traits.RemoveAtSwap(i--);
#endif
				continue;
			}
			TraitsMask.Include(Mask);
		}
	}
};
