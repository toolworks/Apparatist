/*
 * ░▒▓ APPARATUS ▓▒░
 *
 * File: AdjectiveInfo.h
 * Created: Friday, 23rd October 2020 7:00:48 pm
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
#include "UObject/Class.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define ADJECTIVE_INFO_H_SKIPPED_MACHINE_H
#endif

#include "Filter.h"

#ifdef ADJECTIVE_INFO_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "AdjectiveInfo.generated.h"


// Forward declarations:
class UChunk;
struct FChunkAdjectiveEntry;
struct FSubjectHandle;
class AMechanism;
class UMachine;

/**
 * The internal adjective information.
 */
USTRUCT(BlueprintType)
struct APPARATUSRUNTIME_API FAdjectiveInfo
{
	GENERATED_BODY()

  public:

	enum
	{
		/**
		 * Invalid adjective identifier.
		 */
		InvalidId = 0,

		/**
		 * First subjective identifier.
		 */
		FirstId = 1
	};

  private:

	friend class UChunk;
	friend struct FChunkAdjectiveEntry;
	friend class AMechanism;
	friend class UMachine;

	/**
	 * The adjective's main callback type.
	 *
	 * @param SubjectHandle A valid subject, that satisfies the adjective's filter.
	 * @param Traits An array of pointers to the traits in the include filter.
	 * These are the copies of the original traits of the subject that 
	 * are stored in the same order as traits in the adjective's filter.
	 * @return The status of the handling. Return #EApparatusStatus::Success,
	 * if the handler completed its operation without errors.
	 */
	typedef EApparatusStatus (*HandlerCallback)(const FSubjectHandle& SubjectHandle,
												void*                 Traits[]);

	/**
	 * The unique identifier of the adjective.
	 *
	 * Matches its index within the global mechanism's array.
	 */
	int32 Id;

	/**
	 * The adjective's main routine (callback).
	 */
	HandlerCallback Handler;

	/**
	 * The filter of the adjective.
	 */
	FFilter Filter;

public:

	FAdjectiveInfo() : Id(InvalidId), Handler(nullptr)
	{}

}; //-struct FAdjectiveInfo
