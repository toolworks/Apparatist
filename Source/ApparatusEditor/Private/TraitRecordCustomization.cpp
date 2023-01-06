// Source\MyGameEditor\Private\Customization\MyStructCustomization.cpp

#include "TraitRecordCustomization.h"

#include "Templates/UnrealTemplate.h"
#include "PropertyEditor/Public/DetailLayoutBuilder.h"
#include "PropertyEditor/Public/DetailWidgetRow.h"
#include "PropertyEditor/Public/PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IPropertyUtilities.h"
#include "SubjectHandle.h"
#include "JsonObjectConverter.h"


#define LOCTEXT_NAMESPACE "FTraitRecordCustomization"

#pragma region Data Customization

uint64 FTraitRecordDataCustomization::LastTickPullFrame = 0;

TSet<FTraitRecordDataCustomization*> FTraitRecordDataCustomization::TransferringCustomizations;

FTraitRecordDataCustomization::~FTraitRecordDataCustomization()
{
	TransferringCustomizations.Remove(this);
	if (UNLIKELY(TransferTarget != nullptr))
	{
		// This won't usually happen,
		// as the target gets cleared after the transfer...
		TransferTarget.Pin()->bTransferTarget = false;
		TransferTarget = nullptr;
	}
}

UScriptStruct*
FTraitRecordDataCustomization::CalcEditedType() const
{
	if (!Property->IsValidHandle()) return nullptr;
	UScriptStruct* TraitType = nullptr;
	Property->EnumerateConstRawData(
	[&](const void* RawData, const int32 /*DataIndex*/, const int32 /*NumDatas*/)
	{
		if (UNLIKELY(RawData == nullptr))
		{
			TraitType = nullptr;
			return false;
		}
		const auto TraitRecord = static_cast<const FTraitRecord*>(RawData);
		const auto CurrentTraitType = TraitRecord->GetType();
		if (CurrentTraitType == nullptr)
		{
			TraitType = nullptr;
			return false;
		}
		if ((TraitType != nullptr) && (TraitType != TraitRecord->GetType()))
		{
			TraitType = nullptr;
			return false;
		}
		TraitType = CurrentTraitType;
		return true;
	});
	return TraitType;
}

void
FTraitRecordDataCustomization::DoRegenerate()
{
	if (bRegenerating) return;
	
	TGuardValue<bool> Guard(bRegenerating, true);
	OnRegenerateChildren.ExecuteIfBound();
}

bool
FTraitRecordDataCustomization::PullEditedData()
{
	if (bPulling) return true;

	TGuardValue<bool> Guard(bPulling, true);
	// This is the current type being edited de facto:
	const UScriptStruct* EditedType = (EditedData != nullptr) ? Cast<UScriptStruct>(EditedData->GetStruct()) : nullptr;
	if (UNLIKELY(!Property->IsValidHandle()))
	{
		if (EditedType != nullptr)
		{
			// There should be no edited type whatsoever,
			// so regenerate the widget:
			DoRegenerate();
			return false;
		}
		return true;
	}
	if (UNLIKELY(EditedType == nullptr))
	{
		if (CalcEditedType() == nullptr)
		{
			// There's no possible type to edit at this moment,
			// so just return:
			return true;
		}
	}

	bool bNeedsRegeneration = false;
	const auto bHadMixedData = bMixedData;
	bMixedData = false;

	// An etalon trait record to compare with:
	const FTraitRecord* EtalonRecord = nullptr;
	Property->EnumerateConstRawData(
	[&](const void* RawData, const int32 /*DataIndex*/, const int32 NumDatas)
	{
		if (UNLIKELY(RawData == nullptr))
		{
			// The data comes unavailable, so refresh:
			EtalonRecord = nullptr;
			bNeedsRegeneration = true;
			return false;
		}
		const auto TraitRecord = static_cast<const FTraitRecord*>(RawData);
		const auto CurrentTraitType = TraitRecord->GetType();
		if (UNLIKELY((CurrentTraitType != EditedType) && !bHadMixedData))
		{
			// The type doesn't match anymore, so have to refresh:
			bNeedsRegeneration = true;
			EtalonRecord = nullptr;
			return false;
		}
		if (NumDatas > 1)
		{
			// If there are more instances being edited,
			// we can only pull them if they are the same...
			if ((EtalonRecord != nullptr) && (EtalonRecord != TraitRecord))
			{
				if (EtalonRecord->GetType() != TraitRecord->GetType())
				{
					bMixedData = true;
					EtalonRecord = nullptr;
					return false;
				}
				else
				{
					if (!EtalonRecord->GetType()->CompareScriptStruct(EtalonRecord->GetData(), TraitRecord->GetData(), STRUCT_IdenticalNative))
					{
						// We can no longer consistently display the traits,
						// as they have different data state.
						// TODO: Maybe try to implement mixed-value display per property someday.
						bMixedData = true;
						EtalonRecord = nullptr;
						return false;
					}
				}
			}
		}
		EtalonRecord = TraitRecord;
		return true;
	});
	
	if (bNeedsRegeneration)
	{
		DoRegenerate();
		return false;
	}

	if (LIKELY(!bMixedData))
	{
		// The data can be showed for all instances.
		if (bHadMixedData)
		{
			// This was not the case before.
			bNeedsRegeneration = true;
		}
		else
		{
			// The data was already consistent,
			// so just update it:
			EtalonRecord->GetData(EditedData->GetStructMemory());
		}
	}
	else
	{
		if (!bHadMixedData)
		{
			EditedData = nullptr;
			bNeedsRegeneration = true;
		}
	}

	return true;
}

void
FTraitRecordDataCustomization::PushEditedData()
{
	// This is the current type being edited de facto:
	const UScriptStruct* EditedType = (EditedData != nullptr) ? Cast<UScriptStruct>(EditedData->GetStruct()) : nullptr;
	if (UNLIKELY(!Property->IsValidHandle()))
	{
		if (EditedType != nullptr)
		{
			// There should be no edited type whatsoever,
			// so regenerate the widget:
			DoRegenerate();
		}
		return;
	}

	// Push to as much compatible data as possible,
	// just not to lose any changes, 
	// but also track the need for a (later) regeneration...
	bool bNeedsRegeneration = false;
	Property->EnumerateRawData(
	[&](void* RawData, const int32 /*DataIndex*/, const int32 /*NumDatas*/)
	{
		if (UNLIKELY(RawData == nullptr))
		{
			// If the data is missing just ignore it:
			return true;
		}
		const auto TraitRecord = static_cast<FTraitRecord*>(RawData);
		const auto CurrentTraitType = TraitRecord->GetType();
		if (UNLIKELY(CurrentTraitType != EditedType))
		{
			// The type doesn't match anymore, so have to refresh:
			bNeedsRegeneration = true;
			return true;
		}
		TraitRecord->SetData(EditedData->GetStructMemory());
		return true;
	});
	
	if (UNLIKELY(bNeedsRegeneration))
	{
		DoRegenerate();
	}
}

void
FTraitRecordDataCustomization::OnRecordChanging()
{
	if (bPulling || bRegenerating) return;
	// The record type is changing, so make sure
	// the current data is invalidated...
	Property->EnumerateRawData(
	[&](void* RawData, const int32 /*DataIndex*/, const int32 /*NumDatas*/)
	{
		if (UNLIKELY(RawData == nullptr))
		{
			// If the data is missing just ignore it:
			return true;
		}
		const auto TraitRecord = static_cast<FTraitRecord*>(RawData);
		TraitRecord->DoDestroyData();
		return true;
	});
	bForcePullOnTick = true;
}

void
FTraitRecordDataCustomization::OnRecordChanged()
{	

}

void
FTraitRecordDataCustomization::OnStructChanging()
{
	if (UNLIKELY(!Property->IsValidHandle())) return;
	TransferringCustomizations.Add(this);
	if (PropertyPath.IsEmpty())
	{
		PropertyPath = Property->GeneratePathToProperty();
	}
}

void
FTraitRecordDataCustomization::OnStructChanged()
{	
	if (TransferTarget != nullptr)
	{
		// The data has changed,
		// but it can't be pushed directly as it was
		// already replaced by a new customization.
		// Push the target of transfer explicitly now,
		// so the changes are applied to the underlying instance...
		TransferTarget.Pin()->PushEditedData();
		TransferTarget.Pin()->bTransferTarget = false;
		TransferTarget = nullptr;
	}
	TransferringCustomizations.Remove(this);
	PushEditedData();
}

void
FTraitRecordDataCustomization::Tick(float DeltaSeconds)
{
	if (!Property->IsValidHandle()) return;
	if (TransferTarget != nullptr) return;
	if (bTransferTarget) return;

	// Updating only a single trait record at a time seems like a reasonable
	// tradeoff...
	if (bForcePullOnTick)
	{
		PullEditedData();
		bForcePullOnTick = false;
		return;
	}
	if (LastTickPullFrame == GFrameCounter) return;
	PullEditedData();
	LastTickPullFrame = GFrameCounter;
}

void
FTraitRecordDataCustomization::SetOnRebuildChildren(FSimpleDelegate InOnRegenerateChildren)
{
	OnRegenerateChildren = InOnRegenerateChildren;
}

void
FTraitRecordDataCustomization::GenerateHeaderRowContent(FDetailWidgetRow& NodeRow)
{
	const auto TypePropertyHandle =
		Property->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTraitRecord, Type)).ToSharedRef();
	TypePropertyHandle->SetOnPropertyValuePreChange(FSimpleDelegate::CreateSP(this, &FTraitRecordDataCustomization::OnRecordChanging));
	TypePropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FTraitRecordDataCustomization::OnRecordChanged));
}

void
FTraitRecordDataCustomization::GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder)
{
	EditedData.Reset();
	const auto TraitType = CalcEditedType();
	if (LIKELY(TraitType != nullptr && Property->IsValidHandle()))
	{
		bTransferTarget = false;
		if (TransferringCustomizations.Num() > 0)
		{
			// A transferring is occurring, so we are a new candidate for it.
			for (auto TransferringCustomization : TransferringCustomizations)
			{
				// Try to save as much performance as possible:
				if ((TransferringCustomization->EditedData                  != nullptr)
				 && (TransferringCustomization->EditedData->GetStruct()     == TraitType)
				 && (TransferringCustomization->Property->GetProperty()     == Property->GetProperty())
				 && (TransferringCustomization->Property->GetIndexInArray() == Property->GetIndexInArray()))
				{
					PropertyPath = Property->GeneratePathToProperty();
					if (TransferringCustomization->PropertyPath == PropertyPath)
					{
						// We have found a source of transfer.
						// Move the already allocated data, instead of creating
						// a new struct instance later:
						EditedData = TransferringCustomization->EditedData;
						TransferringCustomization->TransferTarget = this->AsShared();
						bTransferTarget = true;
						TransferringCustomizations.Remove(TransferringCustomization);
						break;
					}
				}
			}
		}
		if (!bTransferTarget)
		{
			EditedData = MakeShared<FStructOnScope>(TraitType);
			// Pull the trait's data:
			if (!PullEditedData())
			{
				return;
			}
		}
		if (bMixedData)
		{
			static const FText MultipleTraitDataValues = LOCTEXT("MultipleTraitDataValues", "Trait Data Values Multiple");
			static const FText MultipleTraitDataValuesTooltip = LOCTEXT("MultipleTraitDataValuesTooltip", "There're currently multiple trait data values.");
			ChildrenBuilder.AddCustomRow(MultipleTraitDataValues)
			[
				SNew(STextBlock)
			   .Text(LOCTEXT("MultipleDataValues", "Multiple Values..."))
			//    .Font(FEditorStyle::GetFontStyle("ItalicFont"))
			   .ToolTipText(MultipleTraitDataValuesTooltip)
			];
		}
		else
		{
			// Construct a trait's DOM:
			const auto DataProperties = Property->AddChildStructure(EditedData.ToSharedRef());
			// Generate the UI for the DOM...
			for (auto DataProperty : DataProperties)
			{
				ChildrenBuilder.AddProperty(DataProperty.ToSharedRef());
				DataProperty->SetOnPropertyValuePreChange(
					FSimpleDelegate::CreateSP(this, &FTraitRecordDataCustomization::OnStructChanging));
				DataProperty->SetOnPropertyValueChanged(
					FSimpleDelegate::CreateSP(this, &FTraitRecordDataCustomization::OnStructChanged));

				// Also for the children of the struct...
				DataProperty->SetOnChildPropertyValuePreChange(
					FSimpleDelegate::CreateSP(this, &FTraitRecordDataCustomization::OnStructChanging));
				DataProperty->SetOnChildPropertyValueChanged(
					FSimpleDelegate::CreateSP(this, &FTraitRecordDataCustomization::OnStructChanged));
			}
		}
	}
}

FTraitRecordDataCustomization::FTraitRecordDataCustomization(TSharedRef<IPropertyHandle> InTraitRecordProp)
  : Name(*InTraitRecordProp->GeneratePathToProperty())
  , Property(InTraitRecordProp)
{}

#pragma endregion Data Customization

TSharedRef<IPropertyTypeCustomization>
FTraitRecordCustomization::MakeInstance()
{
	// Create the instance and returned a SharedRef
	return MakeShareable(new FTraitRecordCustomization());
}

FTraitRecordCustomization::~FTraitRecordCustomization()
{}

void
FTraitRecordCustomization::OnTypeChanging(TSharedRef<IPropertyHandle> RecordProperty,
										  TSharedRef<IPropertyHandle> TypeProperty)
{}

void
FTraitRecordCustomization::OnTypeChanged(TSharedRef<IPropertyHandle> RecordProperty,
										 TSharedRef<IPropertyHandle> TypeProperty)
{}

void FTraitRecordCustomization::CustomizeHeader(
	TSharedRef<IPropertyHandle>      RecordPropertyHandle,
	class FDetailWidgetRow&          HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	const auto TypePropertyHandle =
		RecordPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTraitRecord, Type)).ToSharedRef();
	HeaderRow
		.NameContent()[RecordPropertyHandle->CreatePropertyNameWidget()]
		.ValueContent().MinDesiredWidth(318.0f)[TypePropertyHandle->CreatePropertyValueWidget()];
	TypePropertyHandle->SetOnPropertyValuePreChange(FSimpleDelegate::CreateSP(this, &FTraitRecordCustomization::OnTypeChanging,
													RecordPropertyHandle, TypePropertyHandle));
	TypePropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FTraitRecordCustomization::OnTypeChanged,
												  RecordPropertyHandle, TypePropertyHandle));
}

void FTraitRecordCustomization::CustomizeChildren(
	TSharedRef<IPropertyHandle>      RecordPropertyHandle,
	IDetailChildrenBuilder&          StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	const auto DataCustomization = MakeShared<FTraitRecordDataCustomization>(RecordPropertyHandle);
	StructBuilder.AddCustomBuilder(DataCustomization);
}

#undef LOCTEXT_NAMESPACE
