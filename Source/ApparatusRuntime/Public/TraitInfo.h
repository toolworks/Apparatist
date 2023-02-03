/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: TraitInfo.h
 * Created: 2021-06-21 21:41:56
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
#define TRAIT_INFO_H_SKIPPED_MACHINE_H
#endif

#include "ApparatusStatus.h"
#include "BitMask.h"

#ifdef TRAIT_INFO_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "TraitInfo.generated.h"

/**
 * The internal information about a trait.
 */
USTRUCT(BlueprintType)
struct APPARATUSRUNTIME_API FTraitInfo
{
	GENERATED_BODY()

  public:

	/**
	 * The type of the trait identifier.
	 */
	using IdType = int32;

	/**
	 * Invalid trait identifier.
	 */
	static constexpr IdType InvalidId = -1;

	/**
	 * First trait identifier.
	 */
	static constexpr IdType FirstId = 0;

  private:

	friend struct FSubjectHandle;
	friend class UChunk;
	friend class UMachine;
	friend class ISubjective;

	template < typename ChunkItT, typename BeltItT >
	struct TChain;

  public:

	/**
	 * The type of the trait type.
	 * 
	 * This is a strong pointer that
	 * should grab and hold the type,
	 * so that non-property trait references in
	 * traitmarks are guaranteed to be
	 * present.
	 */
	UPROPERTY()
	const UScriptStruct* Type = nullptr;

	/**
	 * The unique identifier of the trait.
	 *
	 * Matches its index within the global machine's array.
	 */
	IdType Id = InvalidId;

	/**
	 * The bitmask of the trait.
	 */
	FBitMask Mask;

	/**
	 * The excluded bitmask of the trait.
	 */
	FBitMask ExcludingMask;

	/**
	 * Construct a stale trait information struct.
	 */
	FORCEINLINE
	FTraitInfo()
	{}

	/**
	 * Construct a new trait information struct.
	 * 
	 * @param InType The type of the trait.
	 * @param InId The identifier of the trait.
	 */
	FTraitInfo(const UScriptStruct* const InType,
			   const IdType               InId);
}; //-struct FTraitInfo
