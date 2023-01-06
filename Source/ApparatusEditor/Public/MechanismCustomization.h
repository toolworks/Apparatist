// MyCustomization.h
#pragma once

#include "IDetailCustomization.h"

class APPARATUSEDITOR_API FMechanismCustomization
  : public IDetailCustomization
{
  public:

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	//

	static TSharedRef<IDetailCustomization> MakeInstance();
};
