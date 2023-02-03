/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ApparatusEditor.h
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

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"


/**
 * The public interface to this module
 */
class APPARATUSEDITOR_API FApparatusEditorModule
  : public IModuleInterface
{
	uint32 GameAssetCategory;

	/**
	 * The number of active PIE instances.
	 */
	int32 ActivePIEsCount = 0;

  public:
	
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	 * Called when Editor began playing the game.
	 */
	void OnBeginPIE(const bool bInIsSimulating);

	/**
	 * Called when Editor ended playing the game.
	 */
	void OnEndPIE(const bool bInIsSimulating);
};
