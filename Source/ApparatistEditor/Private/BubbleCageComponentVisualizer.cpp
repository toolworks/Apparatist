/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: BubbleCageComponentVisualizer.cpp
 * Created: 2023-02-02 14:46:13
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#include "BubbleCageComponentVisualizer.h"

#include "BubbleCageComponent.h"


void
FBubbleCageComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	const auto BubbleCageComponent = Cast<const UBubbleCageComponent>(Component);

	const auto Color = FLinearColor::Yellow;

	const auto Bounds = BubbleCageComponent->GetBounds();
	DrawWireBox(PDI, Bounds, Color, 0, /*DepthPriority=*/0, true);

	const auto CellSize = BubbleCageComponent->GetCellSize();
	const auto Size = BubbleCageComponent->GetSize();
	if (BubbleCageComponent->bDebugDrawCageCells)
	{
		for (int32 i = 0; i <= Size.X; ++i)
		{
			for (int32 j = 0; j <= Size.Y; ++j)
			{
				for (int32 k = 0; k <= Size.Z; ++k)
				{
					const auto Point = Bounds.Min + FVector(i, j, k) * CellSize;
					PDI->DrawPoint(Point, Color, 3, /*DepthPriority=*/0);
				}
			}
		}
	}
}
