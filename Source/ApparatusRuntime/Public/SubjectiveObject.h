/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectiveAcObject
 * Created: 2021-12-05 15:04:28
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

#include "UObject/Object.h"
#include "Misc/App.h"

#include "Machine.h"

#include "SubjectiveObject.generated.h"


/**
 * An Unreal Engine object as a subject with details.
 */
UCLASS(BlueprintType, ClassGroup = "Apparatus", CustomConstructor)
class APPARATUSRUNTIME_API USubjectiveObject
  : public UObject
  , public ISubjective
{
	GENERATED_BODY()

  private:

#pragma region Standard Subjective Block

	/**
	 * The list of traits.
	 */
	UPROPERTY(EditAnywhere, Category = Data,
			  Meta = (DisplayAfter = "Flagmark",
					  AllowPrivateAccess = "true"))
	TArray<FTraitRecord> Traits;

	/**
	 * The list of details.
	 */
	UPROPERTY(EditAnywhere, Instanced, Category = Data,
			  Meta = (DisplayAfter = "Traits",
			  		  AllowPrivateAccess = "true"))
	TArray<UDetail*> Details;

	/**
	 * An optional preferred belt for the subject to be placed in.
	 */
	UPROPERTY(EditAnywhere, Category = Optimization,
			  Meta = (AllowPrivateAccess = "true"))
	UBelt* PreferredBelt = nullptr;

	/**
	 * The flagmark of the subjective.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data,
			  Meta = (Bitmask, BitmaskEnum = "EFlagmark",
					  DisplayAfter = "PreferredBelt",
					  AllowPrivateAccess = "true"))
	int32 Flagmark = FM_None;

	/**
	 * The mechanism to use as a default one,
	 * when registering the subjective.
	 * 
	 * If not set, the default mechanism
	 * of the world will be used.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Context,
			  Meta = (AllowPrivateAccess = "true"))
	AMechanism* MechanismOverride = nullptr;

	END_STANDARD_SUBJECTIVE_PROPERTY_BLOCK(USubjectiveObject)

#pragma endregion Standard Subjective Block

  public:
	
	/**
	 * Initialize and register a subjective object.
	 */
	USubjectiveObject()
	{
		DoRegister();
	}

  protected:

	FORCEINLINE void
	Serialize(FArchive& Archive) override
	{
		ISubjective::DoStartSerialization(Archive);
		Super::Serialize(Archive);
		ISubjective::DoFinishSerialization(Archive);
	}

	void
	BeginDestroy() override
	{
		EnsureOK(DoUnregister<EParadigm::DefaultPortable>());

		check(Belt == nullptr);
		check(SlotIndex == InvalidSlotIndex);
		Super::BeginDestroy();
	}

}; //-class USubjectiveObject

