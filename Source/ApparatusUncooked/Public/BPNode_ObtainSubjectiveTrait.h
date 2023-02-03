/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_ObtainSubjectiveTrait.h
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

#include "K2Node.h"

#include "BPNode_GenericTraitFunction.h"
#include "ApparatusFunctionLibrary.h"

#include "BPNode_ObtainSubjectiveTrait.generated.h"

/**
 * A user-friendly filter construction node.
 */
UCLASS(MinimalAPI)
class UBPNode_ObtainSubjectiveTrait
	: public UBPNode_GenericTraitFunction
{
	GENERATED_BODY()

  public:

	UBPNode_ObtainSubjectiveTrait(const FObjectInitializer& ObjectInitializer)
	  : Super(ObjectInitializer)
	{
		static const auto Name =  GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, ObtainSubjectiveTrait);
		FunctionReference.SetExternalMember(
			Name, UApparatusFunctionLibrary::StaticClass());
	}
}; //-UBPNode_ObtainSubjectiveTrait
