/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: Detail.h
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

#include "More/type_traits"

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "UObject/NoExportTypes.h"
#include "UObject/UnrealType.h"

#include "Detail.generated.h"


// Forward declarations:
class ISubjective;
class AMechanism;
class UMachine;

/**
 * The base subjective data block class.
 * 
 * Details are high-level components which
 * are basically Unreal Engine's Objects (@c UObject).
 *
 * These are stored within subjectives and are
 * iterated through the caching belts.
 * 
 * Details (unlike traits) support the OOP-like inheritance
 * model and can also have multiple instances
 * of the same class assigned for each subjective.
 * 
 * New detail instances are enabled by default.
 */
UCLASS(Blueprintable, EditInlineNew, Abstract, CollapseCategories, CustomConstructor)
class APPARATUSRUNTIME_API UDetail : public UObject
{
	GENERATED_BODY()

	/**
	 * Is the detail currently active for the subject?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Detail,
			  BlueprintGetter = IsEnabled, BlueprintSetter = SetEnabled,
			  Meta = (AllowPrivateAccess = "true"))
	uint32 bEnabled : 1;

	friend class ISubjective;
	friend class AMechanism;
	friend class UMachine;

  public:

	UDetail();

	/**
	 * Check if the detail currently active.
	 */
	UFUNCTION(BlueprintGetter, Category = "Apparatus|Detail")
	FORCEINLINE bool IsEnabled() const { return bEnabled; }

	/**
	 * Set a detail to be active, or not.
	 */
	UFUNCTION(BlueprintSetter, Category = "Apparatus|Detail")
	void SetEnabled(const bool bState = true);

	/**
	 * The owning subjective of the detail (if any).
	 */
	ISubjective*
	GetOwner() const;

	/**
	 * The mechanism this detail is part of.
	 */
	AMechanism*
	GetMechanism() const;

	/**
	 * Get the detail class.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Apparatus|Detail",
			  Meta = (Keywords = "type kind"))
	FORCEINLINE TSubclassOf<UDetail> GetClass() const
	{
		return UObject::GetClass();
	}

  protected:

	/**
	 * The owning subjective of the detail (if any).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Apparatus|Detail",
			  Meta = (DisplayName = "Get Owner", Keywords = "subjective"))
	TScriptInterface<ISubjective>
	BP_GetOwner() const;

	/**
	 * The event is fired, when the detail has become active
	 * for a subject.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Apparatus|Detail",
			  Meta = (DisplayName = "Activated"))
	void ReceiveActivated();

	/**
	 * The event is fired, when the detail has become inactive
	 * for a subject.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Apparatus|Detail",
			  Meta = (DisplayName = "Deactivated"))
	void ReceiveDeactivated();

	/**
	 * The event is fired, when the detail has become active
	 * for the subject.
	 *
	 * Calls ReceiveActivated() by default, to trigger the event
	 * in blueprints.
	 */
	FORCEINLINE virtual void
	Activated() { ReceiveActivated(); };

	/**
	 * The event is fired, when the detail has become inactive
	 * for a subject.
	 *
	 * Calls ReceiveDeactivated() by default, to trigger the event
	 * in blueprints.
	 */
	FORCEINLINE virtual void
	Deactivated() { ReceiveDeactivated(); };
}; //-class UDetail

FORCEINLINE
UDetail::UDetail() : bEnabled(true) 
{}

/**
 * Check if the supplied class is actually a detail.
 * 
 * @tparam D The class to examine.
 * @return The state of examination.
 */
template < class D >
constexpr bool IsDetailClass()
{
	return std::is_base_of<UDetail, D>::value;
}

/**
 * A guarantee for a class to be a detail.
 * 
 * @tparam D The class to guarantee.
 */
template < class D >
using TDetailClassSecurity = more::enable_if_t<IsDetailClass<D>(), bool>;

/**
 * A guarantee for a class to be a non-detail.
 * 
 * @tparam D The class to guarantee.
 */
template < class D >
using TNonDetailClassSecurity = more::enable_if_t<!IsDetailClass<D>(), bool>;
