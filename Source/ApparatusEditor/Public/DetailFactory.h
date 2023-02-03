/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: MechanicalActorFactory.h
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
#include "Detail.h"

#include "DetailFactory.generated.h"

/**
 * The Detail factory class.
 */
UCLASS(MinimalAPI)
class UDetailFactory : public UFactory
{
	GENERATED_BODY()

public:

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	virtual FText GetDisplayName() const override
	{
		return FText::FromString(TEXT("Detail"));
	}

	UDetailFactory(const class FObjectInitializer& ObjectInitializer);

protected:
	virtual bool IsMacroFactory() const { return false; }
};
