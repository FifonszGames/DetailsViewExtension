// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"

struct FPropertyPathNode;
class SPropertyPathsPicker;

enum class EPropertyPathChipState;

class DETAILSVIEWEXTENSION_API SVisiblePropertyPathsCombo : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SVisiblePropertyPathsCombo)
		: _PropertyHandle(nullptr)
		{}
		
		SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle)

	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);

private:
	void SetupInvalidChildSlot();
	
	void CreateSelectedPropertyPathsList();
	
	void InitializeParentNode(const TSharedRef<IPropertyHandle>& ClassPropHandle);
	void InitializeParentNode();
	
	EPropertyPathChipState GetChipState(const FString& InForPath) const;

	TSharedRef<ITableRow> MakePropertyPathListViewRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& TableViewBase);
	TSharedRef<SWidget> OnGetMenuContent();

	void OnEmptyClicked() const;

	void OnClearPathClicked(const FString PathToHighlight) const;
	
	bool IsValueEnabled() const;
	void OnPropertyValueChanged(const FPropertyChangedEvent& InEvent);
	
	TSharedPtr<IPropertyHandle> PropertyHandle;
	TSharedPtr<FPropertyPathNode> ParentNode;
	TArray<TSharedPtr<FString>> SelectedPropertyPaths;
	
	TSharedPtr<SListView<TSharedPtr<FString>>> PropertyPathsListView;
	TSharedPtr<SComboButton> ComboButton;
	TSharedPtr<SPropertyPathsPicker> TagPicker;

	bool bEditablePropertiesOnly = false;
};
