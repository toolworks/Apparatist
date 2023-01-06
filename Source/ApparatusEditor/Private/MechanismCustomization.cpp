/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: MechanismCustomization.cpp
 * Created: 2022-05-12 19:50:46
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2021 - 2022, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */


#include "MechanismCustomization.h"
#include "Mechanism.h"
#include "IContentBrowserSingleton.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "ContentBrowserModule.h"
#include "SubjectRecordCollection.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "FMechanismCustomization"


TSharedRef<IDetailCustomization> FMechanismCustomization::MakeInstance()
{
	return MakeShareable(new FMechanismCustomization);
}

void FMechanismCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);
	if (Objects.Num() != 1)
	{
		return;
	}

	TWeakObjectPtr<AMechanism> Mechanism = Cast<AMechanism>(Objects[0].Get());
	if (Mechanism == nullptr)
	{
		return;
	}

	auto OnDump = [Mechanism]
	{
		if(Mechanism.IsValid())
		{
			FSaveAssetDialogConfig SaveAssetDialog;
			SaveAssetDialog.DefaultAssetName = FString("SubjectsDump");
			SaveAssetDialog.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;
			auto& ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();
			ContentBrowser.CreateSaveAssetDialog(
				SaveAssetDialog,
				FOnObjectPathChosenForSave::CreateLambda([Mechanism](const FString& Path)
				{
					if (!Mechanism.IsValid()) return;
					const FString PackageName = FPackageName::ObjectPathToPackageName(Path);
					const FString CollectionName = FPaths::GetBaseFilename(PackageName);
					const auto Package = CreatePackage(*PackageName);
					const auto Collection = NewObject<USubjectRecordCollection>(Package, *CollectionName, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
					Mechanism->CollectSubjects(Collection, Mechanism->DumpingFilter, FM_All);
					FString PackageFileName = FPackageName::LongPackageNameToFilename(*PackageName, FPackageName::GetAssetPackageExtension());
					UPackage::SavePackage(Package, Collection, RF_Standalone, *PackageFileName);
				}),
				FOnAssetDialogCancelled::CreateLambda([](){}));
		}
		return FReply::Handled();
	};
	auto& Category = DetailBuilder.EditCategory(TEXT("Debugging"));
	Category.AddCustomRow(LOCTEXT("Dumping", "Dumping"))
	.NameContent()
		[
			SNew(STextBlock)
			.Text_Lambda([Mechanism](){
				if (!Mechanism.IsValid())
					return LOCTEXT("NotAvailable", "N/A");
				return FText::Format(LOCTEXT("SubjectsCountFormat", "Subjects ({0})"), Mechanism->SubjectsNum());
			})
			.ToolTipText(LOCTEXT("SubjectsCountTooltip", "The current number of all active subjects."))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
	.ValueContent()
		[
			SNew(SButton)
			.Text(LOCTEXT("DumpSubjectsButtonTitle", "Dump to File..."))
			.TextStyle(FEditorStyle::Get(), "SmallText")
			.ToolTipText(LOCTEXT("DumpSubjectsButtonTooltip", "Dump all of the mechanism's subjects to a record collection asset."))
			.OnClicked_Lambda(OnDump)
		];
}

#undef LOCTEXT_NAMESPACE
