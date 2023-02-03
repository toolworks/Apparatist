/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_Mechanic.cpp
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
#include "MechanicalActor.h"

#include "MechanicalActorFactory.generated.h"

/**
 * The mechanism factory class.
 */
UCLASS(MinimalAPI)
class UMechanicalActorFactory : public UFactory
{
	GENERATED_BODY()

public:

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	virtual FText GetDisplayName() const override
	{
		return FText::FromString(TEXT("Mechanical Actor"));
	}

	UMechanicalActorFactory(const class FObjectInitializer& ObjectInitializer);

protected:

	virtual bool IsMacroFactory() const { return false; }
};
