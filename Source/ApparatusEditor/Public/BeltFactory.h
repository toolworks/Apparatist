/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BeltFactory.h
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

#pragma once

#include "Factories/Factory.h"
#include "Belt.h"

#include "BeltFactory.generated.h"


/**
 * The Belt factory class.
 */
UCLASS(MinimalAPI)
class UBeltFactory : public UFactory 
{
	GENERATED_BODY()
	
protected:

	virtual bool IsMacroFactory() const { return false; }

public:
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
                                      EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	virtual FText GetDisplayName() const override
	{
		return FText::FromString(TEXT("Belt"));
	}

	UBeltFactory(const class FObjectInitializer& ObjectInitializer);
};
