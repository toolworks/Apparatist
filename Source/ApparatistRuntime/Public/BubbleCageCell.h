// 2022 Vladislav Dmitrievich Turbanov

#pragma once

#include "CoreMinimal.h"

#include "Machine.h"
#include "SubjectHandles8.h"

#include "BubbleCageCell.generated.h"


/**
 * Struct representing a one grid cell.
 */
USTRUCT(BlueprintType, Category = "BubbleCage")
struct APPARATISTRUNTIME_API FBubbleCageCell
{
	GENERATED_BODY()

  public:

	/* Subjects in a grid cell. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BubbleCage")
	FSubjectHandles8 Subjects;
};
