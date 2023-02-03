/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: BPNode_MakeTraitRecord.h
 * Created: 2023-01-05 19:52:32
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

#include "BPNode_MakeTraitRecord.generated.h"

/**
 * A user-friendly filter construction node.
 */
UCLASS(MinimalAPI)
class UBPNode_MakeTraitRecord
  : public UBPNode_GenericTraitFunction
{
	GENERATED_BODY()

  public:

	UBPNode_MakeTraitRecord(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	{
		static const auto Name = GET_FUNCTION_NAME_CHECKED(UApparatusFunctionLibrary, MakeTraitRecord);
		FunctionReference.SetExternalMember(
			Name, UApparatusFunctionLibrary::StaticClass());
	}
}; //-UBPNode_MakeTraitRecord
