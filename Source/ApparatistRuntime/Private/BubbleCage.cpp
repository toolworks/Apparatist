// 2022 Vladislav Dmitrievich Turbanov

#include "BubbleCage.h"


ABubbleCage* ABubbleCage::Instance = nullptr;

/* Sets default values. */
ABubbleCage::ABubbleCage()
{
	PrimaryActorTick.bCanEverTick = false;
}

/* Called when the game starts or when spawned. */
void ABubbleCage::InitializeInternalState()
{
	Cells.Reset();
	if (ensure((int64)Size.X * (int64)Size.Y * (int64)Size.Z < (int64)TNumericLimits<int32>::Max()))
	{
		Cells.AddDefaulted(Size.X * Size.Y * Size.Z);
	}
	FlushPersistentDebugLines(GetWorld());
}