// Copyright FifonszGames All Rights Reserved.

#pragma once

#include "IPropertyTypeCustomization.h"

class FVisiblePropertyPathsCustomizationBase : public IPropertyTypeCustomization
{
public:
	FVisiblePropertyPathsCustomizationBase();

	void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& ChildBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

protected:
	virtual TSharedRef<SWidget> CreateClassPropertyValueContent(TSharedRef<IPropertyHandle> PropertyPathHandle) const = 0;

private:
	TSharedPtr<IPropertyHandle> PropertyHandle;
};

class FClassPropertyPathsCustomization final : public FVisiblePropertyPathsCustomizationBase
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

protected:
	TSharedRef<SWidget> CreateClassPropertyValueContent(TSharedRef<IPropertyHandle> PropertyPathHandle) const override;
};

class FStructPropertyPathsCustomization final : public FVisiblePropertyPathsCustomizationBase
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

protected:
	TSharedRef<SWidget> CreateClassPropertyValueContent(TSharedRef<IPropertyHandle> PropertyPathHandle) const override;
};
