/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ApparatusRuntime.h
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
/**
 * @file 
 * @brief The main Apparatus runtime module.
 */

#pragma once

#include "More/type_traits"

#include "Runtime/Launch/Resources/Version.h"
#include "CoreMinimal.h"
#include "Engine/NetConnection.h"
#include "Modules/ModuleManager.h"
#include "HAL/UnrealMemory.h"
#include "Paradigm.h"


// Global forward declarations:
enum class EApparatusStatus : int8;
enum class EFlagmarkBit : uint8;
class UChunk;
class UBelt;
class AMechanism;
class UMachine;

template < typename ChunkItT, typename BeltItT, EParadigm Paradigm = EParadigm::Safe >
struct TChain;

/**
 * Checks if a type can be considered an array.
 */
template < typename T >
struct TArrayTypeChecker
{
	/**
	 * The type is not an array.
	 */
	static constexpr bool Value = false;
}; //-class TArrayTypeChecker

template < typename ElementT, typename AllocatorT >
struct TArrayTypeChecker<TArray<ElementT, AllocatorT>>
{
	/**
	 * The type of the array elements.
	 */
	using ElementType = ElementT;

	/**
	 * The type of the array allocator.
	 */
	using AllocatorType = AllocatorT;

	/**
	 * The type is an array.
	 */
	static constexpr bool Value = true;

}; //-class TArrayTypeChecker

/**
 * Check if the supplied type is actually an array.
 * 
 * @tparam T The type to examine.
 * @return The state of examination.
 */
template < typename T >
constexpr FORCEINLINE bool
IsArrayType()
{
	return TArrayTypeChecker<T>::Value;
}

/**
 * The main Apparatus runtime module.
 */
class APPARATUSRUNTIME_API FApparatusRuntimeModule : public IModuleInterface
{
	friend class UChunk;
	friend class UBelt;
	friend class AMechanism;
	friend class UMachine;

  public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

/**
 * A utility function to reallocate an array.
 */
APPARATUSRUNTIME_API FORCEINLINE void*
Apparatus_ReallocArray(void* Original,
					   int32 ElementsCount,
					   int32 ElementSize,
					   uint32 Alignment = 0U)
{
	return FMemory::Realloc(Original, (SIZE_T)ElementsCount * (SIZE_T)ElementSize, Alignment);
}

/**
 * A utility function to allocate an array of elements.
 */
APPARATUSRUNTIME_API FORCEINLINE void*
Apparatus_MallocArray(int32 ElementsCount,
					  int32 ElementSize,
					  uint32 Alignment = 0U)
{
	return FMemory::Malloc((SIZE_T)ElementsCount * (SIZE_T)ElementSize, Alignment);
}

APPARATUSRUNTIME_API DECLARE_LOG_CATEGORY_EXTERN(LogApparatus, Log, All);
