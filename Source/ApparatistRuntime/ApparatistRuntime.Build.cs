/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: ApparatistRuntime.Build.cs
 * Created: 2022-01-21 12:49:43
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

using UnrealBuildTool;

public class ApparatistRuntime : ModuleRules
{
    public ApparatistRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "ApparatusRuntime" });
    }
}
