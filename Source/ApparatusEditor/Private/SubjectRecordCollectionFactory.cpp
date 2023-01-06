/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectRecordCollectionFactory.cpp
 * Created: 2022-05-12 18:50:30
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

#include "SubjectRecordCollectionFactory.h"
#include "Kismet2/KismetEditorUtilities.h"


USubjectRecordCollectionFactory::USubjectRecordCollectionFactory(const class FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = USubjectRecordCollection::StaticClass();
}

UObject* USubjectRecordCollectionFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) 
{
	check(Class->IsChildOf(USubjectRecordCollection::StaticClass()));
	return NewObject<USubjectRecordCollection>(InParent, Name, Flags);
}
