/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: ApparatistRuntime.h
 * Created: 2022-01-21 12:54:05
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2022, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#pragma once

#include "CoreMinimal.h"
#include "Engine/NetConnection.h"
#include "Modules/ModuleManager.h"
#include "HAL/UnrealMemory.h"


// Forward declarations:
class ABubbleCage;

/**
 * The main Apparatist runtime module.
 */
class FApparatistRuntimeModule : public IModuleInterface
{
	friend class ABubbleCage;

  public:

#pragma region IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
#pragma endregion IModuleInterface
};


APPARATISTRUNTIME_API DECLARE_LOG_CATEGORY_EXTERN(LogApparatist, Log, All);
