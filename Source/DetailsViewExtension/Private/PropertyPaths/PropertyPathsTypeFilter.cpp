// Copyright FifonszGames All Rights Reserved.

#include "PropertyPaths/PropertyPathsTypeFilter.h"

#include "DetailsViewExtensionUtils.h"
#include "PropertyPaths/VisiblePropertyPaths.h"

namespace PropertyPathsTypeFilterUtils
{
	bool HasAnyProperties(const UStruct* InStruct, const TSharedRef<IPropertyHandle> PropertyPathHandle)
	{
		bool bHasProperties = false;
		bool bInEditableOnly = false;
		PropertyPathHandle->GetChildHandle(FVisiblePropertyPaths::GetEditablePropertiesOnlyName())->GetValue(bInEditableOnly);
		DetailsViewExtensionUtils::ForeachProperty(InStruct, [bInEditableOnly, &bHasProperties](const FProperty& Property)
		{
			if (!bInEditableOnly || Property.HasAnyPropertyFlags(CPF_Edit))
			{
				bHasProperties |= true;
			}
		});
		return bHasProperties;
	}

	bool IsClassAllowed(const UStruct* InStruct, const TSharedRef<IPropertyHandle> PropertyPathHandle)
	{
		return InStruct
		&& !InStruct->HasMetaData(DetailsViewExtensionUtils::Get::Meta::Abstract())
		&& !InStruct->HasMetaData(DetailsViewExtensionUtils::Get::Meta::Hidden())
		&& HasAnyProperties(InStruct, PropertyPathHandle);
	}
}

bool FPropertyPathsTypeFilter::IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions,
	const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs)
{
	return PropertyPathsTypeFilterUtils::IsClassAllowed(InClass, PropertyPathHandle);
}

bool FPropertyPathsTypeFilter::IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs)
{
	return !InUnloadedClassData->HasAnyClassFlags(FilteredClassFlags);
}

bool FPropertyPathsStructFilter::IsStructAllowed(const FStructViewerInitializationOptions& InInitOptions,
	const UScriptStruct* InStruct, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs)
{
	return !InStruct->IsStructTrashed() && PropertyPathsTypeFilterUtils::IsClassAllowed(InStruct, PropertyPathHandle);
}
