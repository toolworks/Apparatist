/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_PushExtraSubjectTraitThroughBearer.h
 * Created: 2021-11-26 16:05:11
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

#include "BPNode_PushExtraSubjectTraitThroughBearer.generated.h"

/**
 * A node to push an additional trait
 * to a subject through a network bearer.
 */
UCLASS(MinimalAPI)
class UBPNode_PushExtraSubjectTraitThroughBearer
	: public UBPNode_GenericTraitFunction
{
	GENERATED_BODY()

  public:

	UBPNode_PushExtraSubjectTraitThroughBearer(const FObjectInitializer& ObjectInitializer)
	  : Super(ObjectInitializer)
	{
		static const auto Name =  GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, PushExtraSubjectTraitThroughBearer);
		FunctionReference.SetExternalMember(
			Name, UApparatusFunctionLibrary::StaticClass());
	}
}; //-UBPNode_PushExtraSubjectTraitThroughBearer
