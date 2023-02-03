/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Machine.cpp
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

#include "Machine.h"


DEFINE_STAT(STAT_MachineBoot);
DEFINE_STAT(STAT_MachineBuffer);
DEFINE_STAT(STAT_MachineFindMatchingIterables);
DEFINE_STAT(STAT_MachineEvaluate);
DEFINE_STAT(STAT_MachineEvaluateInput);
DEFINE_STAT(STAT_MachineEvaluateSteady);
DEFINE_STAT(STAT_MachineEvaluatePresentation);
DEFINE_STAT(STAT_MachineFetchDetails);

UMachine* UMachine::Instance = nullptr;

void
UMachine::BeginDestroy()
{
	if (UNLIKELY(ShouldBeRetained()))
	{
		// This can actually happen when the game is closed Alt-F4-way...
		UE_LOG(LogApparatus, Warning, TEXT("The '%s' machine is being destroyed while it's still needed."), *GetName());
	}
	UE_LOG(LogApparatus, Display, TEXT("Shutting down the '%s' machine..."), *GetName());
	if (LIKELY(Instance == this))
	{
		Instance = nullptr;
	}
	Super::BeginDestroy();
}
