// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "IPropertyTypeCustomization.h"

class FVisiblePropertyPathsCustomization : public IPropertyTypeCustomization
{
public:
	FVisiblePropertyPathsCustomization();

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:
	TSharedPtr<IPropertyHandle> PropertyHandle;
};