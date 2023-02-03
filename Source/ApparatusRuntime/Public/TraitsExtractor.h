
/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: TraitsExtractor.h
 * Created: 2021-10-18 16:02:14
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


/**
 * Generic access to traits and their types.
 */
template < typename ...Ts >
struct TTraitsExtractor;

template < typename A >
struct TTraitsExtractor<TArray<UScriptStruct*, A>>
{
  private:

	const TArray<UScriptStruct*, A>& Types;

  public:

	FORCEINLINE
	TTraitsExtractor(const TArray<UScriptStruct*, A>& InTypes)
	  : Types(InTypes)
	{}

	FORCEINLINE int32
	Num() const
	{
		return Types.Num();
	}

	FORCEINLINE UScriptStruct*
	TypeAt(const int32 Index) const
	{
		return Types[Index];
	}

	FORCEINLINE bool
	Contains(UScriptStruct* const Type) const
	{
		return Types.Contains(Type);
	}
};
