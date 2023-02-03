/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectRecordCollectionFactory.h
 * Created: 2022-05-12 18:49:12
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

#pragma once

#include "Factories/Factory.h"
#include "SubjectRecordCollection.h"

#include "SubjectRecordCollectionFactory.generated.h"


/**
 * The SubjectRecordCollection factory class.
 */
UCLASS(MinimalAPI)
class USubjectRecordCollectionFactory : public UFactory 
{
	GENERATED_BODY()
	
protected:

	virtual bool IsMacroFactory() const { return false; }

public:
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
                                      EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	virtual FText GetDisplayName() const override
	{
		return FText::FromString(TEXT("Subject Record Collection"));
	}

	USubjectRecordCollectionFactory(const class FObjectInitializer& ObjectInitializer);
};
