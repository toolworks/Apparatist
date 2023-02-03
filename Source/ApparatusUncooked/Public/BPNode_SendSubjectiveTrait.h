/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_SendSubjectiveTrait.h
 * Created: 2021-09-20 21:36:03
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

#include "BPNode_SendSubjectiveTrait.generated.h"

/**
 * Sending a trait over a network node.
 */
UCLASS(MinimalAPI)
class UBPNode_SendSubjectiveTrait
	: public UBPNode_GenericTraitFunction
{
	GENERATED_BODY()

  public:

	UBPNode_SendSubjectiveTrait(const FObjectInitializer& ObjectInitializer)
	  : Super(ObjectInitializer)
	{
		static const auto Name =  GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, PushExtraSubjectiveTrait);
		FunctionReference.SetExternalMember(
			Name, UApparatusFunctionLibrary::StaticClass());
	}

}; //-UBPNode_SendSubjectiveTrait
