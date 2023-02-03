/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: BubbleCageComponent.h
 * Created: 2023-02-02 16:26:26
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#include "BubbleCageComponent.h"


UBubbleCageComponent::UBubbleCageComponent()
{
	bWantsInitializeComponent = true;
}

void
UBubbleCageComponent::InitializeComponent()
{
	DoInitializeCells();
	GetBounds();
	bInitialized = true;
}
