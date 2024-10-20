// Copyright FifonszGames. All Rights Reserved.

#include "PropertyPaths/SVisiblePropertyPathsCombo.h"

#include "PropertyCustomizationHelpers.h"
#include "PropertyPaths/PropertyPathChip.h"
#include "PropertyPaths/PropertyPathsHelpers.h"
#include "PropertyPaths/PropertyPathsPicker.h"

#define LOCTEXT_NAMESPACE "VisiblePropertyPathsCombo"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SVisiblePropertyPathsCombo::Construct(const FArguments& InArgs)
{
	PropertyHandle = InArgs._PropertyHandle;
	
	if (!PropertyHandle.IsValid())
	{
		SetupInvalidChildSlot();
		return;
	}
	
	TSharedPtr<IPropertyHandle> ClassPropHandle = PropertyHandle->GetChildHandle(FVisiblePropertyPaths::GetClassPropertyName());
	PropertyHandle->SetOnChildPropertyValueChangedWithData(TDelegate<void(const FPropertyChangedEvent&)>::CreateSP(this, &SVisiblePropertyPathsCombo::OnPropertyValueChanged));
	PropertyHandle->GetChildHandle(FVisiblePropertyPaths::GetEditablePropertiesOnlyName())->GetValue(bEditablePropertiesOnly);
	InitializeParentNode(ClassPropHandle.ToSharedRef());
	
	CreateSelectedPropertyPathsList();

	TWeakPtr<SVisiblePropertyPathsCombo> WeakSelf = StaticCastWeakPtr<SVisiblePropertyPathsCombo>(AsWeak());
	
	constexpr int32 ItemHeight = 24.f;
	
	PropertyPathsListView = SNew(SListView<TSharedPtr<FString>>)
		.ListItemsSource(&SelectedPropertyPaths)
		.SelectionMode(ESelectionMode::None)
		.ItemHeight(ItemHeight)// FIX
		.ListViewStyle(&FAppStyle::Get().GetWidgetStyle<FTableViewStyle>("SimpleListView"))
		.Visibility(SelectedPropertyPaths.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible)
		.OnGenerateRow(this, &SVisiblePropertyPathsCombo::MakePropertyPathListViewRow);
	
	ChildSlot
	[
		SNew(SHorizontalBox)
							
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Top)
		[
			SAssignNew(ComboButton, SComboButton)
			.ComboButtonStyle(&FAppStyle::Get().GetWidgetStyle<FComboButtonStyle>("SimpleComboButton"))
			.HasDownArrow(true)
			.VAlign(VAlign_Top)
			.ContentPadding(0)
			.IsEnabled(this, &SVisiblePropertyPathsCombo::IsValueEnabled)
			.Clipping(EWidgetClipping::OnDemand)
			.OnGetMenuContent(this, &SVisiblePropertyPathsCombo::OnGetMenuContent)
			.ButtonContent()
			[
				SNew(SBox)
				.MaxDesiredHeight(ItemHeight*5)
				[
					SNew(SBorder)
					.Padding(FMargin(6,2))
					[
						PropertyPathsListView.ToSharedRef()
					]
				]
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			PropertyCustomizationHelpers::MakeEmptyButton(FSimpleDelegate::CreateSP(this, &SVisiblePropertyPathsCombo::OnEmptyClicked))
		]
	];
}

void SVisiblePropertyPathsCombo::SetupInvalidChildSlot()
{
	ChildSlot
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("Invalid Property Handle")))
	];
}

void SVisiblePropertyPathsCombo::CreateSelectedPropertyPathsList()
{
	SelectedPropertyPaths.Empty();
	if(const FVisiblePropertyPaths* StructPaths = PropertyPathHelpers::GetPathsFromHandle(PropertyHandle))
	{
		Algo::Transform(StructPaths->GetPaths(), SelectedPropertyPaths, [](const FString& Path) { return MakeShared<FString>(Path); });
	}
}

void SVisiblePropertyPathsCombo::InitializeParentNode(const TSharedRef<IPropertyHandle>& ClassPropHandle)
{
	ParentNode = MakeShared<FPropertyPathNode>();
	ParentNode->Initialize(ClassPropHandle, bEditablePropertiesOnly);
}

EPropertyPathChipState SVisiblePropertyPathsCombo::GetChipState(const FString& InForPath) const
{
	TSharedPtr<FPropertyPathNode> PropertyNode = ParentNode->GetPropertyByPath(*InForPath);
	return PropertyNode.IsValid() ? EPropertyPathChipState::Valid : EPropertyPathChipState::Invalid;
}

TSharedRef<ITableRow> SVisiblePropertyPathsCombo::MakePropertyPathListViewRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& TableViewBase)
{
	const EPropertyPathChipState State = GetChipState(*Item);
	
	return SNew(STableRow<TSharedPtr<FString>>, TableViewBase)
	.Style(&FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("SimpleTableView.Row"))
	.Padding(FMargin(0,2))
	[
		SNew(SPropertyPathChip)
		.ChipText(FText::FromString(*Item))
		.ChipState(State)
		.OnClearPressed(this, &SVisiblePropertyPathsCombo::OnClearPathClicked, *Item.Get())
	];
}

TSharedRef<SWidget> SVisiblePropertyPathsCombo::OnGetMenuContent()
{
	TagPicker = SNew(SPropertyPathsPicker)
		.MaxHeight(400.0f)
		.Padding(FMargin(2,0,2,0))
		.PropertyHandle(PropertyHandle)
		.ParentNode(ParentNode);

	if (TSharedPtr<SWidget> WidgetToFocus = TagPicker->GetWidgetToFocusOnOpen())
	{
		ComboButton->SetMenuContentWidgetToFocus(WidgetToFocus);
	}
	
	return TagPicker.ToSharedRef();
}

void SVisiblePropertyPathsCombo::OnEmptyClicked() const
{
	PropertyPathHelpers::RemoveAllPaths(PropertyHandle);
}

void SVisiblePropertyPathsCombo::OnClearPathClicked(const FString PathToHighlight) const
{
	PropertyPathHelpers::RemovePath(PropertyHandle, PathToHighlight);
}

bool SVisiblePropertyPathsCombo::IsValueEnabled() const
{
	return PropertyHandle.IsValid();
}

void SVisiblePropertyPathsCombo::InitializeParentNode()
{
	TSharedPtr<IPropertyHandle> ClassPropHandle = PropertyHandle->GetChildHandle(FVisiblePropertyPaths::GetClassPropertyName());
	InitializeParentNode(ClassPropHandle.ToSharedRef());
}

void SVisiblePropertyPathsCombo::OnPropertyValueChanged(const FPropertyChangedEvent& InEvent)
{
	if(FVisiblePropertyPaths::GetPathsPropertyName() == InEvent.GetPropertyName())
	{
		if(const FVisiblePropertyPaths* StructPaths = PropertyPathHelpers::GetPathsFromHandle(PropertyHandle))
		{
			if(!StructPaths->GetPaths().IsEmpty() && StructPaths->GetPaths().Last().IsEmpty())
			{
				return;
			}
			CreateSelectedPropertyPathsList();
			PropertyPathsListView->SetItemsSource(&SelectedPropertyPaths);
			PropertyPathsListView->RequestListRefresh();
			PropertyPathsListView->SetVisibility(SelectedPropertyPaths.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible);
		}
	}
	else if(FVisiblePropertyPaths::GetEditablePropertiesOnlyName() == InEvent.GetPropertyName())
	{
		PropertyHandle->GetChildHandle(FVisiblePropertyPaths::GetEditablePropertiesOnlyName())->GetValue(bEditablePropertiesOnly);
		if(ParentNode->IsEditableProperty() != bEditablePropertiesOnly)
		{
			InitializeParentNode();
			PropertyPathsListView->RebuildList();
		}
	}
	else if(FVisiblePropertyPaths::GetClassPropertyName() == InEvent.GetPropertyName())
	{
		InitializeParentNode();
		PropertyPathsListView->RebuildList();
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
