/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ApparatusEditor.Build.cs
 * Created: Friday, 23rd October 2020 7:00:48 pm
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2021, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ❤
 */

namespace UnrealBuildTool.Rules
{
    public class ApparatusEditor : ModuleRules
    {
        public ApparatusEditor(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
            PublicIncludePaths.AddRange(
                new string[] {
					// ... add public include paths required here ...
                }
                );

            PrivateIncludePaths.AddRange(
                new string[] {
					// ... add other private include paths required here ...
				}
                );

            PrivateIncludePathModuleNames.AddRange(
                new string[] {
                "Settings",
                "AssetTools",
                "KismetCompiler",
                "ToolMenus"
            }
            );

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "EditorStyle",
                    "UnrealEd",
                    "AdvancedPreviewScene",
                    "Kismet",
                    "PlacementMode",
                    "EditorWidgets",
                    "BlueprintGraph",
                    "Json",
                    "JsonUtilities",
                }
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "InputCore",
                    "Slate",
                    "SlateCore",
                    "RenderCore",
                    "PropertyEditor",
                    "WorkspaceMenuStructure",
                    "LevelEditor",
                    "EditorStyle",
                    "ContentBrowser",
                    "Projects",
                    "ApparatusRuntime",
                    "KismetCompiler",
					"ToolMenus",
                    "AssetTools"
				}
                );

            DynamicallyLoadedModuleNames.AddRange(
                new string[]
                {
					// ... add any modules that your module loads dynamically here ...
                }
                );
        }
    }
}
