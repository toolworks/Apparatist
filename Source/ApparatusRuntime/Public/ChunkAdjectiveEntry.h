/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ChunkAdjectiveEntry.h
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
#define CHUNK_ADJECTIVE_ENTRY_H_SKIPPED_MACHINE_H
#endif

#include "Fingerprint.h"
#include "AdjectiveInfo.h"

#ifdef CHUNK_ADJECTIVE_ENTRY_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "ChunkAdjectiveEntry.generated.h"

/**
 * An entry for a mechanic in the chunk.
 *
 * The fields in this structure should be performance-oriented
 * and cache the most needed data for applying the mechanics.
 */
USTRUCT(BlueprintType)
struct APPARATUSRUNTIME_API FChunkAdjectiveEntry
{
	GENERATED_BODY()

public:

	enum
	{
		/**
		 * Invalid adjective identifier.
		 */
		InvalidId = FAdjectiveInfo::InvalidId
	};

private:

	friend class UChunk;

	/**
	 * The adjective's unique identifier.
	 */
	int32 Id = FAdjectiveInfo::InvalidId;

	/**
	 * The main adjective's handler (callback).
	 */
	FAdjectiveInfo::HandlerCallback Handler = nullptr;

	/**
	 * A mapping to the chunk lines indices
	 * from the adjective's traits.
	 *
	 * The order matches the order of traits
	 * in the adjective's traitmark.
	 */
	TArray<int32> Lines;

	/**
	 * The trait data holders for the handler evaluation.
	 */
	void** Traits = nullptr;

  public:

	FChunkAdjectiveEntry()
	{}
}; // struct FChunkAdjectiveEntry
