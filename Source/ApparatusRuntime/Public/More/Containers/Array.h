/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Array.h
 * Created: 2023-01-23 15:38:38
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
 * @brief Additional utilities for Unreal Engine's Array type.
 */

#pragma once

#include "Containers/Array.h"


/**
 * Insert an element into an array at a certain index while swapping the existing element to the back.
 * Copying version.
 * 
 * @tparam AllocatorT The allocator of the array.
 * @tparam ElementT The type of the elements in the array.
 * @param Array The array to alter.
 * @param Element The element to insert.
 * @param Index The index to insert at.
 */
template < typename ElementT, typename AllocatorT >
APPARATUSRUNTIME_API FORCEINLINE void
InsertSwap(TArray<ElementT, AllocatorT>& Array, const ElementT& Element, const typename TArray<ElementT, AllocatorT>::SizeType Index)
{
	Array.Add(Element);
	Array.Swap(Index, Array.Num() - 1);
}

/**
 * Insert an element into an array at a certain index while swapping the existing element to the back.
 * Moving version.
 * 
 * @tparam AllocatorT The allocator of the array.
 * @tparam ElementT The type of the elements in the array.
 * @param Array The array to alter.
 * @param Element The element to insert.
 * @param Index The index to insert at.
 */
template < typename ElementT, typename AllocatorT >
APPARATUSRUNTIME_API FORCEINLINE void
InsertSwap(TArray<ElementT, AllocatorT>& Array, ElementT&& Element, const typename TArray<ElementT, AllocatorT>::SizeType Index)
{
	Array.Add(Element);
	Array.Swap(Index, Array.Num() - 1);
}
