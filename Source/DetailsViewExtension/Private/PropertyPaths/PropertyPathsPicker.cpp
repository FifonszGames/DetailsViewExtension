// Copyright FifonszGames All Rights Reserved.

#include "PropertyPaths/PropertyPathsPicker.h"

#include "PropertyPaths/PropertyPathsHelpers.h"
#include "PropertyPaths/VisiblePropertyPaths.h"
#include "Widgets/Input/SSearchBox.h"

void SPropertyPathsPicker::Construct(const FArguments& InArgs)
{
	if (!InArgs._PropertyHandle.IsValid() || !InArgs._ParentNode.IsValid())
	{
		ChildSlot
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Invalid Property Handle")))
		];
		return;
	}
	PropertyHandle = InArgs._PropertyHandle;
	ParentNode = InArgs._ParentNode;
	PropertyHandle = InArgs._PropertyHandle;
	MaxHeight = InArgs._MaxHeight;

	FillAllPathItems();

	TWeakPtr<SPropertyPathsPicker> WeakSelf = SharedThis(this);

	const TSharedRef<SWidget> Picker =
		SNew(SBorder)
		.Padding(InArgs._Padding)
		.BorderImage(FStyleDefaults::GetNoBrush())
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Top)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.FillWidth(1.f)
				.Padding(0, 1, 5, 1)
				[
					SAssignNew(SearchPathBox, SSearchBox)
					.HintText(FText::FromString(TEXT("Search for a property path")))
					.OnTextChanged(this, &SPropertyPathsPicker::OnFilterTextChanged)
				]
			]
			+ SVerticalBox::Slot()
			.MaxHeight(MaxHeight)
			.FillHeight(1)
			[
				SNew(SBorder)
				.BorderImage(FStyleDefaults::GetNoBrush())
				[
					SAssignNew(PropertiesTreeWidget, STreeView<TSharedPtr<FPropertyPathNode>>)
					.TreeItemsSource(&AllPathItems)
					.OnGenerateRow(this, &SPropertyPathsPicker::OnGenerateRow)
					.OnGetChildren(this, &SPropertyPathsPicker::OnGetChildren)
					.SelectionMode(ESelectionMode::Single)
				]
			]
		];

	FMenuBuilder MenuBuilder(false, nullptr);

	MenuBuilder.BeginSection(FName(), FText::FromString(TEXT("Property Paths")));

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Clear all properties")), FText::GetEmpty(), FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.X"),
		FUIAction(FExecuteAction::CreateRaw(this, &SPropertyPathsPicker::OnClearAllClicked))
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Select all properties")), FText::GetEmpty(), FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Plus"),
		FUIAction(FExecuteAction::CreateRaw(this, &SPropertyPathsPicker::OnSelectAllClicked))
	);

	MenuBuilder.AddSeparator();

	const TSharedRef<SWidget> MenuContent =
		SNew(SBox)
		.WidthOverride(300.0f)
		.HeightOverride(MaxHeight)
		[
			Picker
		];

	MenuBuilder.AddWidget(MenuContent, FText::GetEmpty(), true);
	MenuBuilder.EndSection();

	ChildSlot
	[
		MenuBuilder.MakeWidget()
	];
}

TSharedPtr<SWidget> SPropertyPathsPicker::GetWidgetToFocusOnOpen()
{
	return SearchPathBox;
}

void SPropertyPathsPicker::OnFilterTextChanged(const FText& Text)
{
	FilterString = Text.ToString();
	FillAllPathItems();
	PropertiesTreeWidget->RequestTreeRefresh();
}

TSharedRef<ITableRow> SPropertyPathsPicker::OnGenerateRow(const TSharedPtr<FPropertyPathNode> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FPropertyPathNode>>, OwnerTable)
		.Style(FAppStyle::Get(), "GameplayTagTreeView")
		[
			SNew(SCheckBox)
			.OnCheckStateChanged(this, &SPropertyPathsPicker::OnPropertyPathCheckStatusChanged, InItem)
			.IsChecked(this, &SPropertyPathsPicker::IsPropertyChecked, InItem)
			.IsEnabled(true)
			.HAlign(HAlign_Left)
			.CheckBoxContentUsesAutoWidth(false)
			[
				SNew(STextBlock)
				.Text(FText::FromString(InItem->GetPropertyName()))
			]
		];
}

void SPropertyPathsPicker::OnPropertyPathCheckStatusChanged(const ECheckBoxState CheckBoxState, const TSharedPtr<FPropertyPathNode> PropertyPathNode) const
{
	if (!PropertyPathNode.IsValid())
	{
		return;
	}
	if (const FVisiblePropertyPaths* Paths = PropertyPathHelpers::GetPathsFromHandle(PropertyHandle))
	{
		const FString TotalPath = PropertyPathNode->GetTotalPath();
		switch (CheckBoxState)
		{
			case ECheckBoxState::Unchecked:
				{
					TArray<FString> PathsToRemove = Paths->GetPaths().FilterByPredicate([&TotalPath](const FString& Path)
					{
						return Path.Len() > TotalPath.Len() && Path.StartsWith(TotalPath);
					});
					PathsToRemove.Add(TotalPath);
					PropertyPathHelpers::RemovePaths(PropertyHandle, PathsToRemove);

					int32 Index = INDEX_NONE;
					if (TotalPath.FindLastChar(PropertyPathHelpers::Get::SeparatorAsChar(), Index))
					{
						const FString ChildPath = TotalPath.Left(Index);
						if (!Paths->HasMatchingPath(ChildPath))
						{
							PropertyPathHelpers::AddPath(PropertyHandle, ChildPath);
						}
					}
					break;
				}
			case ECheckBoxState::Checked:
				{
					const TArray<FString> PathsToRemove = Paths->GetPaths().FilterByPredicate([&TotalPath](const FString& Path)
					{
						return Path.Len() < TotalPath.Len() && TotalPath.StartsWith(Path);
					});
					PropertyPathHelpers::RemovePaths(PropertyHandle, PathsToRemove);
					PropertyPathHelpers::AddPath(PropertyHandle, TotalPath);
					break;
				}
			case ECheckBoxState::Undetermined: break;
		}
	}
}

ECheckBoxState SPropertyPathsPicker::IsPropertyChecked(const TSharedPtr<FPropertyPathNode> PropertyPathNode) const
{
	if (!PropertyPathNode.IsValid())
	{
		return ECheckBoxState::Unchecked;
	}

	const FVisiblePropertyPaths* Paths = PropertyPathHelpers::GetPathsFromHandle(PropertyHandle);
	return Paths && Paths->HasMatchingPath(PropertyPathNode->GetTotalPath()) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SPropertyPathsPicker::OnGetChildren(const TSharedPtr<FPropertyPathNode> InItem, TArray<TSharedPtr<FPropertyPathNode>>& OutChildren) const
{
	OutChildren.Append(InItem->GetChildren(FilterString));
}

void SPropertyPathsPicker::FillAllPathItems()
{
	AllPathItems = ParentNode->GetChildren(FilterString);
}

void SPropertyPathsPicker::OnClearAllClicked() const
{
	FScopedTransaction Transaction(FText::FromString(TEXT("Clear all paths")));
	PropertyPathHelpers::RemoveAllPaths(PropertyHandle);
	PropertiesTreeWidget->RequestTreeRefresh();
}

void SPropertyPathsPicker::OnSelectAllClicked() const
{
	FScopedTransaction Transaction(FText::FromString(TEXT("Select all paths")));
	PropertyPathHelpers::AddAllChildren(PropertyHandle, ParentNode);
	PropertiesTreeWidget->RequestTreeRefresh();
}
