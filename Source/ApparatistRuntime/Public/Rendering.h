/*
 * ░▒▓ APPARATIST ▓▒░
 * 
 * File: BubbleCageCell.h
 * Created: 2022-01-21
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#pragma once

#include "CoreMinimal.h"

#include "Rendering.generated.h"


// Forward declarations:
class UTraitRendererComponent;

/**
 * Struct representing a render buffer state of the subject,
 * when the it is currently rendering.
 */
USTRUCT(Category = "TraitRenderer")
struct APPARATISTRUNTIME_API FRendering
{
	GENERATED_BODY()

  public:

	/**
	 * The owning trait renderer component.
	 */
	UTraitRendererComponent* Owner = nullptr;

	/**
	 * The unique identifier of the object. 
	 */
	int32 InstanceId = -1;

	/* Default constructor. */
	FRendering() {}

	/**
	 * Initialize a new rendering instance.
	 */
	FRendering(UTraitRendererComponent* const InOwner,
			   const int32                    InInstanceId)
	  : Owner(InOwner)
	  , InstanceId(InInstanceId)
	{}
};
