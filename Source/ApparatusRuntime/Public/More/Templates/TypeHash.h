/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: TypeHash.h
 * Created: 2021-10-23 17:02:23
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
/**
 * @file 
 * @brief Additional utilities for Unreal Engine's type hashing.
 */

#pragma once

#include "Templates/TypeHash.h"


/**
 * Combine several hashes.
 * 
 * This function is present because the standard one doesn't support
 * more than two operands.
 */
template < size_t N >
FORCEINLINE uint32
MoreHashCombine(const uint32 (&Hashes)[N])
{
	static_assert(N > 0, "The number of hashes must be larger than zero.");
	auto Hash = Hashes[0];
	for (int32 i = 1; i < N; ++i) // Compile-time unwinding.
	{
		Hash = HashCombine(Hash, Hashes[i]);
	}
	return Hash;
}

/**
 * Combine several hashes.
 * 
 * This function is present because the standard one doesn't support
 * more than two operands.
 */
template < typename ...Hs >
FORCEINLINE uint32
MoreHashCombine(const Hs... Hashes)
{
	const uint32 All[] = { Hashes... };
	return MoreHashCombine(All);
}