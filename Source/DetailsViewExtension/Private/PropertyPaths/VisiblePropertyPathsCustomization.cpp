// Copyright FifonszGames. All Rights Reserved.

#include "PropertyPaths/VisiblePropertyPathsCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyPaths/SVisiblePropertyPathsCombo.h"
#include "PropertyPaths/VisiblePropertyPaths.h"

#define LOCTEXT_NAMESPACE "MyStructCustomization"

FVisiblePropertyPathsCustomization::FVisiblePropertyPathsCustomization()
{
}

TSharedRef<IPropertyTypeCustomization> FVisiblePropertyPathsCustomization::MakeInstance()
{
	return MakeShareable(new FVisiblePropertyPathsCustomization());
}

void FVisiblePropertyPathsCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	PropertyHandle = StructPropertyHandle;
	HeaderRow
	.NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNullWidget::NullWidget
	];
}

void FVisiblePropertyPathsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	ChildBuilder.AddProperty(InStructPropertyHandle->GetChildHandle(FVisiblePropertyPaths::GetClassPropertyName()).ToSharedRef());
	ChildBuilder.AddProperty(InStructPropertyHandle->GetChildHandle(FVisiblePropertyPaths::GetHiddenPropertyName()).ToSharedRef());
		
	TSharedRef<IPropertyHandle> PathsHandle = InStructPropertyHandle->GetChildHandle(FVisiblePropertyPaths::GetPathsPropertyName()).ToSharedRef();
	FDetailWidgetRow& CustomRow = ChildBuilder.AddCustomRow(PathsHandle->GetPropertyDisplayName());
	CustomRow.NameContent()
	[
		PathsHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SBox)
		.Padding(FMargin(0,2,0,2))
		[
			SNew(SVisiblePropertyPathsCombo)
			.PropertyHandle(InStructPropertyHandle)
		]
	];
}

#undef LOCTEXT_NAMESPACE
