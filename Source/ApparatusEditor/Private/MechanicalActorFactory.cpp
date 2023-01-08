/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: MechanicalActorFactory.cpp
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

#include "MechanicalActorFactory.h"
#include "MechanicalActor.h"
#include "KismetCompilerModule.h"
#include "Kismet2/KismetEditorUtilities.h"

UMechanicalActorFactory::UMechanicalActorFactory(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = AMechanicalActor::StaticClass();
}

UObject* UMechanicalActorFactory::FactoryCreateNew(
	UClass*  Class,   UObject*          InParent,
	FName    Name,    EObjectFlags      Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	UClass* BlueprintClass          = nullptr;
	UClass* BlueprintGeneratedClass = nullptr;

	IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
	KismetCompilerModule.GetBlueprintTypesForClass(AMechanicalActor::StaticClass(), BlueprintClass, BlueprintGeneratedClass);

	return FKismetEditorUtilities::CreateBlueprint(AMechanicalActor::StaticClass(), InParent, Name, BPTYPE_Normal, BlueprintClass, BlueprintGeneratedClass, NAME_None);
}
