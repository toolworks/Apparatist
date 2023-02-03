/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectiveUserWidget.cpp
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

#include "SubjectiveUserWidget.h"

#include "BeltSlot.h"
#include "Machine.h"

USubjectiveUserWidget::USubjectiveUserWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{

}

void USubjectiveUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Belt = nullptr;
	SlotIndex = InvalidSlotIndex;

	DoRegister();
}

void USubjectiveUserWidget::NativeDestruct()
{
	EnsureOK(DoUnregister<EParadigm::DefaultPortable>());

	check(Belt == nullptr);
	check(SlotIndex == InvalidSlotIndex);

	Super::NativeDestruct();
}
