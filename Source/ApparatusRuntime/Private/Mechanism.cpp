/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Mechanism.cpp
 * Created: 2021-08-30 21:30:44
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

#include "Mechanism.h"

#include "Misc/App.h"

#include "Machine.h"
#include "SubjectRecordCollection.h"


DEFINE_STAT(STAT_MechanismBoot);
DEFINE_STAT(STAT_MechanismBuffer);
DEFINE_STAT(STAT_MechanismFindMatchingIterables);
DEFINE_STAT(STAT_MechanismEvaluate);
DEFINE_STAT(STAT_MechanismEvaluateInput);
DEFINE_STAT(STAT_MechanismEvaluateSteady);
DEFINE_STAT(STAT_MechanismEvaluatePresentation);
DEFINE_STAT(STAT_MechanismFetchDetails);

void
AMechanism::BeginPlay()
{
	Super::BeginPlay();

	OperatingsCompleted->Trigger();

	const auto Machine = UMachine::RetainInstance();
	if (ensure(Machine != nullptr))
	{
		const auto World = GetWorld();
		if (LIKELY(World))
		{
			// Make sure the mechanism is registered as a default one
			// if it's still not set...
			const auto MechanismPtr = Machine->DefaultMechanismByWorldCache.Find(World);
			if (!MechanismPtr || (*MechanismPtr) == nullptr)
			{
				Machine->DefaultMechanismByWorldCache.Add(World, this);
			}
		}
		Machine->DoRegisterMechanism(this);
	}	
}

void
AMechanism::Dispose(const bool bWaitForOperatings/*=true*/)
{
	if (UNLIKELY(bDisposed))
	{
		return;
	}

	if (LIKELY(bWaitForOperatings))
	{
		WaitForOperatingsCompletion();
	}

	if (LocksCount.load(std::memory_order_relaxed) != 0)
	{
		// This can also theoretically happen when the game is closed Alt-F4-way...
		UE_LOG(LogApparatus, Warning,
				TEXT("There are still some iterables locked (iterated) within the '%s' mechanism. "
					 "Is this some kind of preliminary exit?"),
					 *GetName());
	}

	UE_LOG(LogApparatus, Display, TEXT("Disposing the '%s' mechanism..."),
		   *GetName());

	this->Reset();

	if (LIKELY(UMachine::HasInstance()))
	{
		UMachine::Instance->DoUnregisterMechanism(this);
	}

	bDisposed = true;
}

void
AMechanism::Reset()
{
	EApparatusStatus Status = UnregisterAllSubjectives<EParadigm::Polite>();
	StatusAccumulate(Status, DespawnAllSubjects<EParadigm::Polite>());
	StatusAccumulate(Status, UnregisterAllMechanicals());
	StatusAccumulate(Status, ClearCache());

	Chains.Reset();
	SolidChains.Reset();
	Chunks.Reset();
	Belts.Reset();
	HaltedSubjects.Reset();

	Deferreds.Empty();

	Super::Reset();
}
