/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectiveUserWidget.h
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

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "Machine.h"

#include "SubjectiveUserWidget.generated.h"


/**
 * A UI widget subject functionality.
 */
UCLASS(ClassGroup = "Apparatus")
class APPARATUSRUNTIME_API USubjectiveUserWidget 
  : public UUserWidget
  , public ISubjective
{
	GENERATED_BODY()

  public:

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

	END_STANDARD_SUBJECTIVE_PROPERTY_BLOCK(USubjectiveUserWidget)

#pragma endregion Standard Subjective Block

  public:

	USubjectiveUserWidget(const FObjectInitializer& ObjectInitializer);

  protected:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	FORCEINLINE void
	Serialize(FArchive& Archive) override
	{
		ISubjective::DoStartSerialization(Archive);
		Super::Serialize(Archive);
		ISubjective::DoFinishSerialization(Archive);
	}

	FORCEINLINE void
	NotifyHandleDespawned() override
	{
		ISubjective::NotifyHandleDespawned();
		SetIsEnabled(false);
		SetVisibility(ESlateVisibility::Collapsed);
	}

}; //-class USubjectiveUserWidget
