/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectHandles4.h
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

#include "SubjectHandle.h"

#include "SubjectHandles4.generated.h"


/**
 * Four subject handles as a cache-friendly struct.
 * 
 * @note Only valid subject handles can be added,
 * but if the subject becomes invalid during the runtime
 * it will still be present in the array.
 */
USTRUCT(BlueprintType)
struct APPARATUSRUNTIME_API FSubjectHandles4
{
	GENERATED_BODY()

  private:

	/**
	 * The internal inlined handles array.
	 */
	TArray<FSubjectHandle, TInlineAllocator<4>> Handles;

  public:

	/**
	 * Add a new subject handle.
	 * 
	 * Only valid subjects are added.
	 * 
	 * @param SubjectHandle A subject handle to add.
	 * @return The index of the added element.
	 */
	FORCEINLINE int32
	Add(const FCommonSubjectHandle& SubjectHandle)
	{
		if (UNLIKELY(!SubjectHandle.IsValid())) return INDEX_NONE;
		return Handles.Add((FSubjectHandle)SubjectHandle);
	}

	/**
	 * Add a unique subject handle.
	 * 
	 * Only valid subjects are added.
	 * 
	 * @param SubjectHandle A subject handle to add.
	 * @return The index of the added (or existing equal) element.
	 */
	FORCEINLINE int32
	AddUnique(const FCommonSubjectHandle& SubjectHandle)
	{
		if (UNLIKELY(!SubjectHandle.IsValid())) return INDEX_NONE;
		return Handles.AddUnique((FSubjectHandle)SubjectHandle);
	}

	/**
	 * Remove a subject handle.
	 */
	FORCEINLINE void
	Remove(const FSubjectHandle& SubjectHandle)
	{
		if (UNLIKELY(!SubjectHandle.IsValid())) return;
		Handles.Remove(SubjectHandle);
	}

	/**
	 * Remove all invalid subject handles (if any).
	 */
	FORCEINLINE void
	CleanUp()
	{
		Handles.Remove(FSubjectHandle::Invalid);
	}

	/**
	 * Get the number of elements in the list.
	 */
	FORCEINLINE int32
	Num() const
	{
		return Handles.Num();
	}

	/**
	 * Get a subject handle by its index.
	 */
	FORCEINLINE FSubjectHandle&
	At(const int32 Index)
	{
		return Handles[Index];
	}

	/**
	 * Get a subject handle by its index. Constant version.
	 */
	FORCEINLINE const FSubjectHandle&
	At(const int32 Index) const
	{
		return Handles[Index];
	}

	/**
	 * Get a subject handle by its index.
	 */
	FORCEINLINE FSubjectHandle&
	operator[](const int32 Index)
	{
		return At(Index);
	}

	/**
	 * Get a subject handle by its index. Constant version.
	 */
	FORCEINLINE const FSubjectHandle&
	operator[](const int32 Index) const
	{
		return At(Index);
	}

	/**
	 * Empty the list.
	 */
	FORCEINLINE void
	Empty()
	{
		Handles.Empty();
	}

}; //-struct FSubjectHandles4
