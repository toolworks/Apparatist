/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: MechanicChunkEntry.h
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
#define MECHANIC_CHUNK_ENTRY_H_SKIPPED_MACHINE_H
#endif

#include "BitMask.h"

#ifdef MECHANIC_CHUNK_ENTRY_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "MechanicChunkEntry.generated.h"

/**
 * A single chunk entry within a mechanic.
 */
USTRUCT()
struct APPARATUSRUNTIME_API FMechanicChunkEntry
{
	GENERATED_BODY()

	friend class UChunk;
	friend struct FMechanicInfo;

  private:

	/**
	 * The linked chunk, this mechanic is
	 * compatible with.
	 */
	TWeakObjectPtr<class UChunk> Chunk;

	/**
	 * The indices of the lines, this mechanic
	 * subject's reside in.
	 * 
	 * The order is the same as of inclusion details
	 * in the mechanic's filter.
	 */
	TArray<int32> Lines;
}; //-struct FMechanicChunkEntry
