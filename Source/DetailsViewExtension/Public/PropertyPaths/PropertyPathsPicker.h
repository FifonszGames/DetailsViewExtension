// Copyright FifonszGames All Rights Reserved.

#pragma once

#include "PropertyPathNode.h"
#include "Widgets/SCompoundWidget.h"

class ITableRow;

class SPropertyPathsPicker final : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPropertyPathsPicker)
			: _PropertyHandle(nullptr)
			, _MaxHeight(260.0f)
			, _Padding(FMargin(2.0f))
		{
		}

		SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle)

		SLATE_ARGUMENT(TSharedPtr<FPropertyPathNode>, ParentNode)

		SLATE_ARGUMENT(float, MaxHeight)

		SLATE_ARGUMENT(FMargin, Padding)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	TSharedPtr<SWidget> GetWidgetToFocusOnOpen();

private:
	void OnFilterTextChanged(const FText& Text);

	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FPropertyPathNode> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	void OnPropertyPathCheckStatusChanged(ECheckBoxState CheckBoxState, TSharedPtr<FPropertyPathNode> PropertyPathNode) const;
	ECheckBoxState IsPropertyChecked(TSharedPtr<FPropertyPathNode> PropertyPathNode) const;

	void OnGetChildren(TSharedPtr<FPropertyPathNode> InItem, TArray<TSharedPtr<FPropertyPathNode>>& OutChildren) const;

	void FillAllPathItems();

	void OnClearAllClicked() const;
	void OnSelectAllClicked() const;

	TSharedPtr<IPropertyHandle> PropertyHandle;

	float MaxHeight = 0.f;
	FMargin Padding;

	TSharedPtr<FPropertyPathNode> ParentNode;
	TArray<TSharedPtr<FPropertyPathNode>> AllPathItems;
	TSharedPtr<SSearchBox> SearchPathBox;
	TSharedPtr<STreeView<TSharedPtr<FPropertyPathNode>>> PropertiesTreeWidget;
	FString FilterString;
};
