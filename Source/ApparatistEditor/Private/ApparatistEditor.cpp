/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: ApparatistEditor.cpp
 * Created: 2023-02-02 14:38:01
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#include "ApparatistEditor.h"

#include "UnrealEd.h"

#include "BubbleCageComponent.h"
#include "BubbleCageComponentVisualizer.h"


void FApparatistEditorModule::StartupModule()
{
	if (GUnrealEd != nullptr)
	{
		TSharedPtr<FComponentVisualizer> Visualizer = MakeShareable(new FBubbleCageComponentVisualizer());

		if (Visualizer.IsValid())
		{
			GUnrealEd->RegisterComponentVisualizer(UBubbleCageComponent::StaticClass()->GetFName(), Visualizer);
			Visualizer->OnRegister();
		}
	}
}

void FApparatistEditorModule::ShutdownModule()
{
	if (GUnrealEd != nullptr)
	{
		GUnrealEd->UnregisterComponentVisualizer(UBubbleCageComponent::StaticClass()->GetFName());
	}
}

IMPLEMENT_MODULE(FApparatistEditorModule, ApparatistEditor);
