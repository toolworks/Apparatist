/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectRecordCollection.h
 * Created: 2022-05-12 17:24:52
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define SUBJECT_RECORD_COLLECTION_H_SKIPPED_MACHINE_H
#endif

#include "SubjectRecord.h"

#ifdef SUBJECT_RECORD_COLLECTION_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "SubjectRecordCollection.generated.h"


// Forward declarations:
struct FSubjectHandle;

/**
 * A serializable collection of subject records.
 */
UCLASS(BlueprintType)
class APPARATUSRUNTIME_API USubjectRecordCollection
  : public UDataAsset
{
	GENERATED_BODY()

  public:

	/**
	 * The actual list of subject records.
	 * Can be accessed and changed directly.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	TArray<FSubjectRecord> Subjects;

	/**
	 * Add a subject to the collection.
	 * 
	 * @param Subject The subject to add. Must be a valid one.
	 * @param FlagmarkMask The mask used to capture the flags from the subject.
	 * Only user-level flags are captured by default.
	 */
	void
	Add(FSubjectHandle Subject, const EFlagmark FlagmarkMask = FM_AllUserLevel);

	/**
	 * Remove all of the collected subjects, while leaving the slack.
	 * 
	 * @param NewSize The expected usage size after calling this function.
	 */
	void
	Reset(int32 NewSize = 0);

	/**
	 * Remove all of the collected subjects, while leaving the slack.
	 * 
	 * @param Slack The slack capacity to to remained.
	 */
	void
	Empty(int32 Slack = 0);

}; //-class USubjectRecordCollection
