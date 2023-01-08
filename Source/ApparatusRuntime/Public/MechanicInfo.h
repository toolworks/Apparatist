/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: MechanicInfo.h
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
#include "Kismet/KismetSystemLibrary.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define MECHANIC_INFO_H_SKIPPED_MACHINE_H
#endif

#include "Filter.h"
#include "MechanicChunkEntry.h"
#include "SubjectHandle.h"

#ifdef MECHANIC_INFO_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "MechanicInfo.generated.h"


// Forward declarations:
class UChunk;
class AMechanism;
class UMachine;

/**
 * The internal mechanic information.
 */
USTRUCT(BlueprintType)
struct APPARATUSRUNTIME_API FMechanicInfo
{
	GENERATED_BODY()

  public:

	enum
	{
		/**
		 * Invalid mechanic identifier.
		 */
		InvalidId = 0
	};

  private:

	friend class UChunk;
	friend class AMechanism;
	friend class UMachine;

	/**
	 * The mechanic's main callback type.
	 * 
	 * @param subject A valid subject, that satisfies the mechanic's filter.
	 * @param Traits An array of pointers to the traits in the include filter.
	 * @return The status of the handling. Return #EApparatusStatus::Success,
	 * if the handler completed its operation without errors.
	 */
	typedef EApparatusStatus (*HandlerCallback)(FSubjectHandle Subject, void* Traits[]);

	/**
	 * The unique identifier of the mechanic.
	 *
	 * Matches its index within the global mechanism's array.
	 */
	int32 Id;

	/**
	 * The mechanic's main routine (callback).
	 */
	HandlerCallback Handler;

	/**
	 * The filter of the mechanic.
	 */
	FFilter Filter;

	/**
	 * The chunk compatible with the mechanic.
	 */
	TArray<FMechanicChunkEntry> Chunks;

	/**
	 * Register a chunk within mechanic.
	 * 
	 * @param Chunk The chunk to register.
	 * @return The status of the operation.
	 */
	EApparatusStatus AddChunk(class UChunk* Chunk);

}; // struct FMechanicInfo
