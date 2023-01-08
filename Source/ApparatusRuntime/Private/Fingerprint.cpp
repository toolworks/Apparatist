/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Fingerprint.cpp
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

#include "Fingerprint.h"

#include "Machine.h"


const FFingerprint FFingerprint::Zero;

FString FFingerprint::ToString() const
{
	FString str = Traitmark.ToString();
	if (!str.IsEmpty())
	{
		str += ", ";
	}
	str += Detailmark.ToString();
	return MoveTemp(str);
}
