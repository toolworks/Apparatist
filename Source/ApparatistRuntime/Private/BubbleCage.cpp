/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: BubbleCage.cpp
 * Created: 2022-01-21 12:53:42
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#include "BubbleCage.h"


ABubbleCage* ABubbleCage::Instance = nullptr;

ABubbleCage::ABubbleCage()
{
	PrimaryActorTick.bCanEverTick = false;
	const auto SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	BubbleCageComponent = CreateDefaultSubobject<UBubbleCageComponent>("BubbleCage");
	SceneComponent->Mobility = EComponentMobility::Static;
	RootComponent = SceneComponent;
}
