#pragma once

#include "IPropertyTypeCustomization.h"

//TODO:: change to base class and create a separate one for class and struct
class FVisiblePropertyPathsCustomizationBase : public IPropertyTypeCustomization
{
public:
	FVisiblePropertyPathsCustomizationBase();

	void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& ChildBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

protected:
	virtual TSharedRef<SWidget> CreateClassPropertyValueContent(TSharedRef<IPropertyHandle> PropertyPathHandle) const =
	0;

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

	static TSharedRef<SWidget> GenerateStructPicker(const TSharedRef<IPropertyHandle> PropertyPathHandle, TSharedRef<IPropertyHandle> StructTypeHandle);
	static const FSlateBrush* GetDisplayValueIcon(TSharedRef<IPropertyHandle> StructTypeHandle);
	static FText GetDisplayValueString(TSharedRef<IPropertyHandle> StructTypeHandle);
	static FText GetTooltipText(TSharedRef<IPropertyHandle> StructTypeHandle);

	static void OnStructPicked(const UScriptStruct* SelectedStruct, TSharedRef<IPropertyHandle> StructTypeHandle);
};
