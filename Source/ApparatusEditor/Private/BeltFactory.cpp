/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BeltFactory.cpp
 * Created: Friday, 23rd October 2020 7:00:48 pm
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2021 - 2022, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#include "BeltFactory.h"
#include "Kismet2/KismetEditorUtilities.h"


UBeltFactory::UBeltFactory(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UBelt::StaticClass();
}

UObject* UBeltFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) 
{
	check(Class->IsChildOf(UBelt::StaticClass()));
	return NewObject<UBelt>(InParent, Name, Flags);
}
