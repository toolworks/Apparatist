/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: UnrealMemory.h
 * Created: 2021-10-27 16:14:36
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
/**
 * @file 
 * @brief Additional utilities for Unreal Engine's memory handling.
 */

#pragma once

#include "HAL/UnrealMemory.h"


/**
 * Additional utilities for Unreal Engine's memory handling.
 */
struct FMoreMemory
{
  private:

	FORCEINLINE
	FMoreMemory() {}

  public:

	/**
	 * Type-safe memory swapping.
	 */
	template < typename T >
	static FORCEINLINE void
	Memswap(T& RefA, T& RefB)
	{
		FMemory::Memswap(static_cast<void*>(&RefA),
						 static_cast<void*>(&RefB),
						 sizeof(T));
	}


	/**
	 * Type-safe memory comparison.
	 */
	template < typename T >
	static FORCEINLINE int32
	Memcmp(const T& RefA, const T& RefB)
	{
		return FMemory::Memcmp(static_cast<const void*>(&RefA),
							   static_cast<const void*>(&RefB),
							   sizeof(T));
	}
}; //-struct FMoreMemory
