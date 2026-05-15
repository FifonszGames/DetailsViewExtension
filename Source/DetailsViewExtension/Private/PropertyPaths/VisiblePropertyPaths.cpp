// Copyright FifonszGames All Rights Reserved.

#include "PropertyPaths/VisiblePropertyPaths.h"

#include "DetailsViewExtensionUtils.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "PropertyPaths/PropertyPathsHelpers.h"

bool FVisibleCustomProperties::Contains(const FName& PropertyName, const FName& PropertyCategory) const
{
	if (Properties.IsEmpty() && Categories.IsEmpty())
	{
		return true;
	}
	if (!PropertyName.IsNone() && Properties.Contains(PropertyName))
	{
		return true;
	}
	if (!PropertyCategory.IsNone() && Categories.Contains(PropertyCategory))
	{
		return true;
	}
	return false;
}

const FVisiblePropertyPaths FVisiblePropertyPaths::Invalid;

bool FVisiblePropertyPaths::Contains(const FPropertyAndParent& InProperty) const
{
	const FString PropertyPath = CreatePropertyPath(InProperty);
	return HasMatchingPath(PropertyPath);
}

bool FVisiblePropertyPaths::ContainsCustomRow(const FName& PropertyName, const FName& PropertyCategory) const
{
	return VisibleCustomProperties.Contains(PropertyName, PropertyCategory);
}

bool FVisiblePropertyPaths::HasMatchingPath(const FString& PropertyPath, ESearchCase::Type SearchCase) const
{
	return VisiblePropertyPaths.ContainsByPredicate([&PropertyPath, &SearchCase](const FString& Path) { return Path.StartsWith(PropertyPath, SearchCase); });
}

FString FVisiblePropertyPaths::CreatePropertyPath(const FPropertyAndParent& InFromProperty)
{
	FString PropertyPath;
	bool bWasBroken = false;
	for (int32 i = InFromProperty.ParentProperties.Num() - 1; i >= 0; --i)
	{
		const FProperty* ParentProperty = InFromProperty.ParentProperties[i];
		if (ParentProperty && InFromProperty.ParentArrayIndices[i] < 0)
		{
			PropertyPath.Append(FString::Printf(TEXT("%s%s"), *DetailsViewExtensionUtils::GetFieldNameString(*ParentProperty, true), *PropertyPathHelpers::Separator()));
			continue;
		}
		bWasBroken = true;
		break;
	}
	if (!bWasBroken && InFromProperty.ArrayIndex <= INDEX_NONE)
	{
		PropertyPath.Append(DetailsViewExtensionUtils::GetFieldNameString(InFromProperty.Property, true));
	}
	else
	{
		PropertyPath.RemoveAt(PropertyPath.Len() - 1, 1);
	}
	return PropertyPath;
}
