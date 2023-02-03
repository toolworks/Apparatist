/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: ApparatistEditor.h
 * Created: 2023-02-02 14:36:16
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "HAL/UnrealMemory.h"


// Forward declarations:
class ABubbleCage;

/**
 * The main Apparatist editor module.
 */
class APPARATISTEDITOR_API FApparatistEditorModule
  : public IModuleInterface
{
	friend class ABubbleCage;

  public:

#pragma region IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
#pragma endregion IModuleInterface
};
