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
}

void FApparatistRuntimeModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FApparatistRuntimeModule, ApparatistRuntime)

#undef LOCTEXT_NAMESPACE
