/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: ApparatusEditor.cpp
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

#include "ApparatusEditor.h"

#include "Editor.h"
#include "PropertyEditorModule.h"
#include "PropertyEditorDelegates.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions_Base.h"
#include "Belt.h"
#include "Detail.h"
#include "MechanicalActor.h"
#include "MechanicalGameModeBase.h"
#include "MechanicalGameMode.h"
#include "Mechanism.h"
#include "SubjectRecordCollection.h"

#include "TraitRecordCustomization.h"
#include "MechanismCustomization.h"


#define LOCTEXT_NAMESPACE "FApparatusEditorModule"

/**
 * Generic database actions for the supported Apparatus assets.
 */
class FApparatusDatabaseActions : public FAssetTypeActions_Base
{
	uint32 Category;
	UClass* SupportedClass;
	bool bAsFilter;

  public:

	FApparatusDatabaseActions(uint32 InCategory, UClass* InSupportedClass, bool bInAsFilter = false)
	  : Category(InCategory)
	  , SupportedClass(InSupportedClass)
	  , bAsFilter(bInAsFilter)
	{}

	virtual uint32 GetCategories() override 
	{ 
		return Category; 
	}

	virtual FText GetName() const override
	{
		return FText::FromString(SupportedClass->GetName());
	}

	virtual bool CanFilter() override
	{
		return bAsFilter;
	}

	virtual void BuildBackendFilter(FARFilter& InFilter) override
	{
		// Add the supported class for this type to a filter
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 1
		InFilter.ClassPaths.Add(GetSupportedClass()->GetClassPathName());
#else
		InFilter.ClassNames.Add(GetSupportedClass()->GetFName());
#endif
		InFilter.bRecursiveClasses = true;
	}

	virtual FColor GetTypeColor() const override
	{
		return FColor(0x37abc8ff);
	}

	virtual UClass* GetSupportedClass() const override
	{
		return SupportedClass;
	}
};

void FApparatusEditorModule::OnBeginPIE(const bool bInIsSimulating)
{
	check(ActivePIEsCount >= 0);
	ActivePIEsCount += 1;
	if (!FApparatusDelegates::ReportError.IsBound())
	{
		FApparatusDelegates::ReportError.BindStatic(&FApparatusDelegates::StandardReportError);
	}
}

void FApparatusEditorModule::OnEndPIE(const bool bInIsSimulating)
{
	if (ensure(ActivePIEsCount > 0))
	{
		ActivePIEsCount -= 1;
		if (ActivePIEsCount == 0)
		{
			if (UMachine::HasInstance())
			{
				// Despawn runtime subjects only here:
				EnsureOK(UMachine::DespawnSubjects<EParadigm::DefaultPortable>(FM_None, FM_Editor));
			}
		}
	}
	// Restore standard error reporting...
	if (!FApparatusDelegates::ReportError.IsBound())
	{
		FApparatusDelegates::ReportError.BindStatic(&FApparatusDelegates::StandardReportError);
	}
}

void FApparatusEditorModule::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	GameAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Apparatus")),
			LOCTEXT("ApparatusAssetCategory", "Apparatus"));

	FEditorDelegates::BeginPIE.AddRaw(this, &FApparatusEditorModule::OnBeginPIE);
	FEditorDelegates::EndPIE.AddRaw(this, &FApparatusEditorModule::OnEndPIE);

	AssetTools.RegisterAssetTypeActions(MakeShareable(new FApparatusDatabaseActions(GameAssetCategory, UBelt::StaticClass(), true)));
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FApparatusDatabaseActions(GameAssetCategory, AMechanicalActor::StaticClass())));
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FApparatusDatabaseActions(GameAssetCategory, AMechanicalGameMode::StaticClass())));
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FApparatusDatabaseActions(GameAssetCategory, AMechanicalGameModeBase::StaticClass())));
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FApparatusDatabaseActions(GameAssetCategory, UDetail::StaticClass())));
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FApparatusDatabaseActions(GameAssetCategory, USubjectRecordCollection::StaticClass())));

#pragma region Custom Editors

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomPropertyTypeLayout(
		FTraitRecord::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTraitRecordCustomization::MakeInstance));

	PropertyModule.RegisterCustomClassLayout(
		AMechanism::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FMechanismCustomization::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();

#pragma endregion Custom Editors

}

void FApparatusEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

		PropertyModule.UnregisterCustomPropertyTypeLayout(FTraitRecord::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(AMechanism::StaticClass()->GetFName());

		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

IMPLEMENT_MODULE(FApparatusEditorModule, ApparatusEditor)

#undef LOCTEXT_NAMESPACE
