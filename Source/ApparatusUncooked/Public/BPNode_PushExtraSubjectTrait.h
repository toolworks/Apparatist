/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_PushExtraSubjectTrait.h
 * Created: 2021-10-20 20:46:16
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

#include "BPNode_GenericTraitFunction.h"
#include "ApparatusFunctionLibrary.h"

#include "BPNode_PushExtraSubjectTrait.generated.h"


/**
 * Sending a subject trait over a network node.
 */
UCLASS(MinimalAPI)
class UBPNode_PushExtraSubjectTrait
  : public UBPNode_GenericTraitFunction
{
	GENERATED_BODY()

  public:

	UBPNode_PushExtraSubjectTrait(const FObjectInitializer& ObjectInitializer)
	  : Super(ObjectInitializer)
	{
		static const auto Name =  GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, PushExtraSubjectTrait);
		FunctionReference.SetExternalMember(
			Name, UApparatusFunctionLibrary::StaticClass());
	}

}; //-UBPNode_PushExtraSubjectTrait
