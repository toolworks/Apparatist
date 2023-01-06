#pragma once

#include "IDetailCustomNodeBuilder.h"
#include "PropertyEditor/Public/IPropertyTypeCustomization.h"
#include "EditorUndoClient.h"

#include "TraitRecord.h"


/**
 * The editor of the trait's data struct.
 */
class APPARATUSEDITOR_API FTraitRecordDataCustomization
  : public IDetailCustomNodeBuilder
  , public TSharedFromThis<FTraitRecordDataCustomization>
{
  private:

	/**
	 * The last frame when the data
	 * was pulled on a tick.
	 */
	static uint64 LastTickPullFrame;

	/**
	 * A unique name used to save the expansion state.
	 */
	FName Name;

	/**
	 * The trait record property in question.
	 */
	TSharedRef<IPropertyHandle> Property;

	/**
	 * The cached full path to the property.
	 * 
	 * Used within the transferring workaround.
	 */
	FString PropertyPath;

	/**
	 * The editable data representation.
	 */
	TSharedPtr<FStructOnScope> EditedData;

	/**
	 * The trait data is actually different
	 * within multiple edited instances.
	 */
	bool bMixedData = false;

	/**
	 * This delegate must be called when the children get regenerated.
	 */
	FSimpleDelegate OnRegenerateChildren;

	/**
	 * Full paths to the outers before the actual change.
	 * 
	 * The outer context is sometimes lost within
	 * the property 'change' event and has to be collected beforehand.
	 */
	TArray<FString> SavedOuterPaths;

	/**
	 * Properties transitioning to a new state.
	 */
	static TSet<FTraitRecordDataCustomization*> TransferringCustomizations;

	/**
	 * The target for the transfer of the data.
	 */
	TWeakPtr<FTraitRecordDataCustomization> TransferTarget = nullptr;

	/**
	 * Is this customization a target for a data transfer?
	 */
	bool bTransferTarget = false;

	/**
	 * Should the deferred trait data push take place.
	 */
	bool bDeferredDataPush = false;

	/**
	 * Is currently regenerating?
	 */
	bool bRegenerating = false;

	/**
	 * Is currently pulling data?
	 */
	bool bPulling = false;

	/**
	 * Is the data pull forced on the next tick?
	 */
	bool bForcePullOnTick = false;

	/**
	 * Trigger the widget's regeneration.
	 * 
	 * Must keep track of not multi-regenerating
	 * itself recursively.
	 */
	void
	DoRegenerate();

  public:

	/**
	 * The unique name of this customization to remember
	 * the expansion state.
	 */
	FName
	GetName() const override
	{
		return Name;
	}

	/**
	 * Get the unified currently edited type.
	 * 
	 * There can be a case where multiple different trait records are edited
	 * under the same customization. Will return @c nullptr in that case.
	 * 
	 * @return An unambiguous type of the trait.
	 */
	UScriptStruct*
	CalcEditedType() const;

	/**
	 * Called when a trait's property is changing.
	 */
	void
	OnStructChanging();

	/**
	 * Called when a trait's property is changed.
	 */
	void
	OnStructChanged();

	/**
	 * Called when the record is changing somehow.
	 */
	void
	OnRecordChanging();

	/**
	 * Called when the trait record has changed somehow.
	 */
	void
	OnRecordChanged();

	/**
	 * Pull the data being edited from its target(s).
	 * 
	 * @return Returns @c true, if the data is actual
	 * and needs no regeneration, @c false otherwise.
	 */
	bool
	PullEditedData();

	/**
	 * Push the data being edited back to the target(s).
	 */
	void
	PushEditedData();

	TSharedPtr<IPropertyHandle>
  	GetPropertyHandle() const override
	{
		return Property;
	}

	/**
	 * Set the system callback to launch when the structure of the
	 * customization is changed.
	 * 
	 * @param InOnRegenerateChildren The passed-in callback.
	 */
	void
	SetOnRebuildChildren(FSimpleDelegate InOnRegenerateChildren);

	/**
	 * The record should be collapsed by default
	 * for an overview-like picture.
	 */
	bool
	InitiallyCollapsed() const override
	{
		return true;
	}

	/**
	 * Should refresh the edited data representation constantly.
	 */
	bool
	RequiresTick() const override
	{
		return true;
	}

	/**
	 * Refresh to fetch the actual trait data state.
	 * 
	 * @param DeltaSeconds The time interval.
	 */
	void
	Tick(float DeltaSeconds) override;

	/**
	 * Produce the heading row.
	 */
	void
	GenerateHeaderRowContent(FDetailWidgetRow& NodeRow) override;

	/**
	 * Produce the struct properties.
	 */
	void
	GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder) override;

	FTraitRecordDataCustomization(TSharedRef<IPropertyHandle> InTraitRecordProp);

	~FTraitRecordDataCustomization() override;

}; //-struct FTraitRecordDataCustomization

/**
 * A custom trait record drawer for the Details panel.
 */
class APPARATUSEDITOR_API FTraitRecordCustomization
  : public IPropertyTypeCustomization
{
  public:

	/**
	 * It is just a convenient helpers which will be used
	 * to register our customization. When the propertyEditor module
	 * find our FMyStruct property, it will use this static method
	 * to instantiate our customization object.
	 */
	static TSharedRef<IPropertyTypeCustomization>
	MakeInstance();

#pragma region IPropertyTypeCustomization

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
		class FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
		class IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

#pragma endregion IPropertyTypeCustomization

	/**
	 * Called when a trait's type is starting its change.
	 */
	void
	OnTypeChanging(TSharedRef<IPropertyHandle> RecordProperty,
				   TSharedRef<IPropertyHandle> TypeProperty);

	/**
	 * Called when a trait's type is changed.
	 */
	void
	OnTypeChanged(TSharedRef<IPropertyHandle> RecordProperty,
				  TSharedRef<IPropertyHandle> TypeProperty);

	~FTraitRecordCustomization() override;
};
