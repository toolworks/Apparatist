/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: ApparatistRuntime.cpp
 * Created: 2022-01-21 12:53:42
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2022, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#include "ApparatistRuntime.h"

#include "Modules/ModuleManager.h"
#include "BubbleCage.h"


#define LOCTEXT_NAMESPACE "FApparatistRuntimeModule"

DEFINE_LOG_CATEGORY(LogApparatist);

void FApparatistRuntimeModule::StartupModule()
{
	ABubbleCage::NeighbourOffsets.Reset(); // Just in case...
	for (int32 i = 0; i < 3; ++i)
	{
		for (int32 j = 0; j < 3; ++j)
		{
			for (int32 k = 0; k < 3; ++k)
			{
				ABubbleCage::NeighbourOffsets.Add(FIntVector(i - 1, j - 1, k - 1));
			}
		}	
	}
}

void FApparatistRuntimeModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FApparatistRuntimeModule, ApparatistRuntime)

#undef LOCTEXT_NAMESPACE
