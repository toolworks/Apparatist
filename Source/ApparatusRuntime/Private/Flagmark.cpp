/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Flagmark.cpp
 * Created: 2021-12-06 15:41:10
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

#include "Flagmark.h"


FString
ToString(const EFlagmark Flagmark)
{
	const UEnum* const EnumPtr = GetFlagmarkClass();
	if (EnumPtr == nullptr)
	{
		return FString("<Invalid-Flagmark-State>");
	}

	FString Str;
	for (int32 i = 0; i <= (int32)EFlagmarkBit::Max; ++i)
	{
		const auto Bit = (EFlagmarkBit)i;
		if (EnumHasFlag(Flagmark, Bit))
		{
			if (Str.Len())
			{
				Str += ", ";
			}
			Str += ToString(Bit);
		}
	}

	return Str;
}
