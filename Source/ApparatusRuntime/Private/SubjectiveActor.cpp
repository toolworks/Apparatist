/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectiveActor.cpp
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

#include "SubjectiveActor.h"

#include "Net/UnrealNetwork.h"

#include "BeltSlot.h"
#include "Machine.h"


ASubjectiveActor::ASubjectiveActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASubjectiveActor::BeginPlay()
{
	Super::BeginPlay();

	Belt      = nullptr;
	SlotIndex = InvalidSlotIndex;

	DoRegister();
}

void ASubjectiveActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EnsureOK(DoUnregister<EParadigm::DefaultPortable>());

	check(Belt == nullptr);
	check(SlotIndex == InvalidSlotIndex);

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void
ASubjectiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void
ASubjectiveActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	AActor::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASubjectiveActor, SubjectNetworkId);
}
