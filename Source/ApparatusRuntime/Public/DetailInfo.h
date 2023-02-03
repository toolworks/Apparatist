/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: DetailInfo.h
 * Created: 2021-06-21 22:15:22
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
#define DETAIL_INFO_H_SKIPPED_MACHINE_H
#endif

#include "ApparatusStatus.h"
#include "BitMask.h"
#include "Detail.h"

#ifdef DETAIL_INFO_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "DetailInfo.generated.h"


/**
 * The internal information about a detail.
 */
USTRUCT()
struct APPARATUSRUNTIME_API FDetailInfo
{
	GENERATED_BODY()

  public:

	/**
	 * The type of the detail identifier.
	 */
	using IdType = int32;

	/**
	 * Invalid detail identifier.
	 */
	static constexpr IdType InvalidId = -1;

	/**
	 * First detail identifier.
	 */
	static constexpr IdType FirstId = 0;

  private:

	template < typename ChunkItT, typename BeltItT, EParadigm Paradigm >
	friend struct TChain;

	friend struct FSubjectHandle;
	friend class UChunk;
	friend class UMachine;
	friend class ISubjective;

  public:

	/**
	 * The class of the detail.
	 * 
	 * This is a strong pointer that
	 * should grab and hold the class,
	 * so that non-property detail references in
	 * detailmarks are guaranteed to be
	 * present.
	 */
	UPROPERTY()
	TSubclassOf<UDetail> Class = nullptr;

	/**
	 * The unique identifier of the detail.
	 *
	 * Matches its index within the global machine's array.
	 */
	IdType Id = InvalidId;

	/**
	 * The bitmask of the detail.
	 */
	FBitMask Mask;

	/**
	 * The excluded bitmask of the detail.
	 */
	FBitMask ExcludingMask;

	/**
	 * Construct a stale detail information struct.
	 */
	FORCEINLINE
	FDetailInfo()
	{}

	/**
	 * Construct a new detail information struct.
	 */
	FDetailInfo(const TSubclassOf<UDetail> InClass,
				const IdType               InId);
}; //-struct FDetailInfo
