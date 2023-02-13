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

	/**
	 * The list of subjects with their centers within this cage cell.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "BubbleCage")
	FSubjectHandles8 Subjects;

	/**
	 * The accumulated fingerprint of all subjects within this cell.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "BubbleCage")
	FFingerprint Fingerprint;
};
