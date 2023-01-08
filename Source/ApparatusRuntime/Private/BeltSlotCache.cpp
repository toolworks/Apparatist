/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BeltSlotCache.cpp
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

#include "BeltSlotCache.h"
#include "SubjectiveActorComponent.h"
#include "Detail.h"



FBeltSlotCache& FBeltSlotCache::operator=(const FBeltSlotCache& Cache)
{
	if (!IsLocked())
	{
		Details.Reset();
	}
	for (auto Detail : Cache.Details)
	{
		if (UNLIKELY(!Detail.IsValid())) continue;
		if (!Detail->IsEnabled()) continue;
		if (UNLIKELY(IsLocked() &&
					 (Details.Find(Detail) != INDEX_NONE)))
		{
			continue;
		}
		Details.Add(Detail);
	}
	return *this;
}

FBeltSlotCache& FBeltSlotCache::operator+=(const FBeltSlotCache& Cache)
{
	for (auto Detail : Cache.Details)
	{
		if (UNLIKELY(!Detail.IsValid())) continue;
		if (!Detail->IsEnabled()) continue;
		if (UNLIKELY(Details.Find(Detail) != INDEX_NONE))
		{
			continue;
		}
		Details.Add(Detail);
	}
	return *this;
}
