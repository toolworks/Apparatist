/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: ApparatistEditor.Build.cs
 * Created: 2023-02-02 14:21:52
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

using UnrealBuildTool;

public class ApparatistEditor : ModuleRules
{
    public ApparatistEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "Engine", "CoreUObject", "ApparatistRuntime" });

        PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd" });
    }
}
