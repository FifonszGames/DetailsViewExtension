// Copyright FifonszGames All Rights Reserved.

#include "PropertyPaths/VisiblePropertyPathsCustomization.h"

#include "ClassViewerModule.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "StructViewerModule.h"
#include "PropertyPaths/PropertyPathsHelpers.h"
#include "PropertyPaths/PropertyPathsTypeFilter.h"
#include "PropertyPaths/SVisiblePropertyPathsCombo.h"
#include "PropertyPaths/VisiblePropertyPaths.h"
#include "Styling/SlateIconFinder.h"

namespace StructPropertyPathsCustomizationUtils
{
	TSharedRef<SWidget> GenerateStructPicker(const TSharedRef<IPropertyHandle> PropertyPathHandle, const TSharedRef<IPropertyHandle> StructTypeHandle, TSharedRef<SComboButton> Combo)
	{
		const bool bShowTreeView = StructTypeHandle->HasMetaData(PropertyPathHelpers::Get::Meta::ShowTreeView());

		FStructViewerInitializationOptions Options;
		Options.bShowNoneOption = !(StructTypeHandle->GetMetaDataProperty()->PropertyFlags & CPF_NoClear);
		Options.StructFilter = MakeShared<FPropertyPathsStructFilter>(PropertyPathHandle);
		Options.NameTypeToDisplay = EStructViewerNameTypeToDisplay::DisplayName;
		Options.DisplayMode = bShowTreeView ? EStructViewerDisplayMode::TreeView : EStructViewerDisplayMode::ListView;
		Options.bAllowViewOptions = !StructTypeHandle->HasMetaData(PropertyPathHelpers::Get::Meta::HideViewOptions());
		Options.SelectedStruct = PropertyPathHelpers::GetValueFromHandle<UScriptStruct>(StructTypeHandle);
		Options.PropertyHandle = StructTypeHandle;

		return SNew(SBox)
			.WidthOverride(280)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.MaxHeight(500)
				[
					PropertyPathHelpers::Get::StructViewer().CreateStructViewer(Options,
						FOnStructPicked::CreateLambda([StructTypeHandle, Combo](const UScriptStruct* SelectedStruct)
						{
							StructTypeHandle->SetValue(SelectedStruct);
							Combo->SetIsOpen(false);
						}))
				]
			];
	}

	const FSlateBrush* GetDisplayValueIcon(const TSharedRef<IPropertyHandle> StructTypeHandle)
	{
		return FSlateIconFinder::FindIconBrushForClass(PropertyPathHelpers::GetValueFromHandle<UStruct>(StructTypeHandle), "ClassIcon.Object");
	}

	FText GetDisplayValueString(const TSharedRef<IPropertyHandle> StructTypeHandle)
	{
		const UStruct* Type = PropertyPathHelpers::GetValueFromHandle<UStruct>(StructTypeHandle);
		return Type ? Type->GetDisplayNameText() : FText::FromString(TEXT("None"));
	}

	FText GetTooltipText(const TSharedRef<IPropertyHandle> StructTypeHandle)
	{
		const UStruct* Type = PropertyPathHelpers::GetValueFromHandle<UStruct>(StructTypeHandle);
		return Type ? Type->GetToolTipText() : FText::FromString(TEXT("None"));
	}
}

FVisiblePropertyPathsCustomizationBase::FVisiblePropertyPathsCustomizationBase()
{
}

void FVisiblePropertyPathsCustomizationBase::CustomizeHeader(const TSharedRef<IPropertyHandle> StructPropertyHandle,
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

void FVisiblePropertyPathsCustomizationBase::CustomizeChildren(const TSharedRef<IPropertyHandle> InStructPropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	const TSharedPtr<IPropertyHandle> ClassHandle = InStructPropertyHandle->GetChildHandle(FVisiblePropertyPaths::GetClassPropertyName());
	ChildBuilder.AddProperty(InStructPropertyHandle->GetChildHandle(FVisiblePropertyPaths::GetEditablePropertiesOnlyName()).ToSharedRef());

	const TSharedRef<IPropertyHandle> PathsHandle = InStructPropertyHandle->GetChildHandle(FVisiblePropertyPaths::GetPathsPropertyName()).ToSharedRef();
	FDetailWidgetRow& CustomRow = ChildBuilder.AddCustomRow(PathsHandle->GetPropertyDisplayName());
	CustomRow.NameContent()
	[
		PathsHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SBox)
		.Padding(FMargin(0, 2, 0, 2))
		[
			SNew(SVisiblePropertyPathsCombo)
			.PropertyHandle(InStructPropertyHandle)
		]
	];

	FClassViewerInitializationOptions Options;
	Options.ClassFilters.Add(MakeShared<FPropertyPathsTypeFilter>(InStructPropertyHandle));
	ChildBuilder.AddCustomRow(ClassHandle->GetPropertyDisplayName())
	.NameContent()
	[
		ClassHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		CreateClassPropertyValueContent(InStructPropertyHandle)
	];

	ChildBuilder.AddProperty(InStructPropertyHandle->GetChildHandle(FVisiblePropertyPaths::GetVisibleCustomRowsName()).ToSharedRef());
}

TSharedRef<IPropertyTypeCustomization> FClassPropertyPathsCustomization::MakeInstance()
{
	return MakeShared<FClassPropertyPathsCustomization>();
}

TSharedRef<SWidget> FClassPropertyPathsCustomization::CreateClassPropertyValueContent(
	const TSharedRef<IPropertyHandle> PropertyPathHandle) const
{
	const TSharedRef<IPropertyHandle> ClassHandle = PropertyPathHandle->GetChildHandle(FVisiblePropertyPaths::GetClassPropertyName()).ToSharedRef();
	return SNew(SClassPropertyEntryBox)
		.MetaClass(UObject::StaticClass())
		.HideViewOptions(ClassHandle->HasMetaData(PropertyPathHelpers::Get::Meta::HideViewOptions()))
		.ShowTreeView(ClassHandle->HasMetaData(PropertyPathHelpers::Get::Meta::ShowTreeView()))
		.SelectedClass_Lambda([ClassHandle] { return PropertyPathHelpers::GetValueFromHandle<UClass>(ClassHandle); })
		.OnSetClass_Lambda([ClassHandle](const UClass* SelectedClass) { ClassHandle->SetValue(SelectedClass); })
		.ClassViewerFilters({MakeShared<FPropertyPathsTypeFilter>(PropertyPathHandle)});
}

TSharedRef<IPropertyTypeCustomization> FStructPropertyPathsCustomization::MakeInstance()
{
	return MakeShared<FStructPropertyPathsCustomization>();
}

TSharedRef<SWidget> FStructPropertyPathsCustomization::CreateClassPropertyValueContent(
	const TSharedRef<IPropertyHandle> PropertyPathHandle) const
{
	const TSharedRef<IPropertyHandle> ClassHandle = PropertyPathHandle->GetChildHandle(
		FVisiblePropertyPaths::GetClassPropertyName()).ToSharedRef();

	TSharedRef<SComboButton> Combo = SNew(SComboButton)
		.ContentPadding(0)
		.IsEnabled(ClassHandle->IsEditable())
		.ButtonContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 4.0f, 0.0f)
			[
				SNew(SImage)
				.Image_Static(&StructPropertyPathsCustomizationUtils::GetDisplayValueIcon, ClassHandle)
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text_Static(&StructPropertyPathsCustomizationUtils::GetDisplayValueString, ClassHandle)
				.ToolTipText_Static(&StructPropertyPathsCustomizationUtils::GetTooltipText, ClassHandle)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		];

	Combo->SetOnGetMenuContent(FOnGetContent::CreateStatic(&StructPropertyPathsCustomizationUtils::GenerateStructPicker, PropertyPathHandle, ClassHandle, Combo));
	return Combo;
}
