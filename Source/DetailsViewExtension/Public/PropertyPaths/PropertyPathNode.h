// Copyright FifonszGames All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FPropertyPathNode : TSharedFromThis<FPropertyPathNode>
{
	void Initialize(const TSharedRef<IPropertyHandle>& TypeHandle, const bool bEditablePropertiesOnly);
	void Initialize(const UStruct& InSourceClass, FString InPropertyName, const bool bEditablePropertiesOnly);

	TArray<TSharedPtr<FPropertyPathNode>> GetChildren(const FString& InFilterString = TEXT("")) const;
	void FillWithOutermostChildren(TArray<TSharedPtr<FPropertyPathNode>>& OutItems, const bool bIncludeSelf = false);

	TSharedPtr<const FPropertyPathNode> GetPropertyByPath(const FString& InPath, const bool bInCountSelf = false) const;
	FString GetTotalPath() const;
	const FString& GetPropertyName() const { return PropertyName; }

	bool PassesFilter(const FString& FilterString) const;
	bool IsEditableProperty() const { return bIsEditableProperty; }

private:
	void Initialize(const FProperty& InSourceProperty, const UStruct& InOutMostParentClass, const TWeakPtr<const FPropertyPathNode>& InParent,
		const bool bInEditablePropertiesOnly, const bool bInIsEditable);

	void CreateChildren(const UStruct& InOutMostParentClass, const UStruct& InFromClass, const bool bInEditablePropertiesOnly);

	void AppendPath(FString& OutPath) const;
	TSharedPtr<const FPropertyPathNode> FindChild(const FString& InPath) const;

	FString PropertyName;
	TArray<TSharedPtr<FPropertyPathNode>> Children;
	TWeakPtr<const FPropertyPathNode> Parent;
	TWeakObjectPtr<const UStruct> SourceClass;
	bool bIsEditableProperty = false;
};
