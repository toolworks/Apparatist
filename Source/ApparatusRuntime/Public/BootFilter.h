/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BootFilter.h
 * Created: Friday, 23rd October 2020 7:00:48 pm
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2022, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"

#ifndef SKIP_MACHINE_H
#define SKIP_MACHINE_H
#define BOOT_FILTER_H_SKIPPED_MACHINE_H
#endif

#include "Flagmark.h"

#ifdef BOOT_FILTER_H_SKIPPED_MACHINE_H
#undef SKIP_MACHINE_H
#endif

#include "BootFilter.generated.h"


/**
 * Hash the class.
 */
FORCEINLINE uint32 GetTypeHash(const UClass* InClass)
{
	return InClass->GetUniqueID();
}

// TODO: Remove this deprecated enumeration.
/**
 * The boot filter specification.
 * 
 * Deprecated as of 1.10. Do not use it, please.
 */
UENUM(BlueprintType, Meta = (Bitflags))
enum class EBootFilter : uint8
{
	/**
	 * A filter matching nothing.
	 */
	None = 0x0 UMETA(DisplayName = "None"),
	/**
	 * A filter matching only the booted subjects.
	 */
	Booted = 1 << 0 UMETA(DisplayName = "Booted"),
	/**
	 * A filter matching only the not-yet-booted subjects.
	 */
	Halted = 1 << 1 UMETA(DisplayName = "Halted"),
	/**
	 * A filter matching both booted and not booted subjects.
	 */
	All = Booted | Halted UMETA(DisplayName = "Booted + Halted")
};

ENUM_CLASS_FLAGS(EBootFilter)

/**
 * The state of booting. A synonym to ::EBootFilter
 */
using EBootState = EBootFilter;

/**
 * Hash the boot filter.
 */
FORCEINLINE uint32 GetTypeHash(const EBootFilter& BootFilter)
{
	return GetTypeHash((uint8)BootFilter);
}

/**
 * Get the boot filter class.
 */
inline APPARATUSRUNTIME_API UEnum*
GetBootFilterClass()
{
	static UEnum* EnumType = FindObject<UEnum>(nullptr, TEXT("/Script/ApparatusRuntime.EBootFilter"), true);
	check(EnumType);
	return EnumType;
}

/**
 * Convert a boolean booted state to a filter (state).
 * 
 * @param bState The state of being booted.
 */
FORCEINLINE EBootFilter BootFilterFromState(const bool bState)
{
	return bState ? EBootFilter::Booted : EBootFilter::Halted;
}

/**
 * Convert a boot filter variable to a string.
 */
FORCEINLINE FString BootFilterToString(const EBootFilter BootFilter)
{
	const UEnum* EnumPtr = GetBootFilterClass();
	if (!EnumPtr)
	{
		return FString("Invalid");
	}

	return EnumPtr->GetNameByValue((int64)BootFilter).ToString();
}

/**
 * Serialize a boot filter to an archive.
 */
FORCEINLINE FArchive&
operator<<(FArchive& Archive, EBootFilter& BootFilter)
{ 
	uint8 Raw = (uint8)BootFilter;
	Archive << Raw;
	
	if (Archive.IsLoading())
	{
		BootFilter = (EBootFilter)Raw;
	}

	return Archive;
}

/**
 * Convert a boot filter to a flagmark state.
 */
FORCEINLINE EFlagmark
ToFlagmark(const EBootFilter& BootFilter)
{
	check(BootFilter != EBootFilter::All);
	if (BootFilter == EBootFilter::Booted)
	{
		return FM_Booted;
	}
	return FM_None;
}

/**
 * Convert a flagmark to a boot state.
 */
FORCEINLINE EBootState
ToBootState(const EFlagmark Flagmark)
{
	return EnumHasFlag(Flagmark, EFlagmarkBit::Booted) ? EBootState::Booted : EBootState::Halted;
}
