/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: BubbleCageComponentVisualizer.h
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

#include "ComponentVisualizer.h"


class APPARATISTEDITOR_API FBubbleCageComponentVisualizer
  : public FComponentVisualizer
{
  private:
	
	void
	DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
};