#include "PropertyPaths/PropertyPathsClassFilter.h"

#include "PropertyPaths/PropertyPathsHelpers.h"
#include "PropertyPaths/VisiblePropertyPaths.h"


namespace PropertyPathsFilterUtils
{
	bool HasAnyProperties(const UStruct* InStruct, const TSharedRef<IPropertyHandle> PropertyPathHandle)
	{
		bool bHasProperties = false;
		bool bInEditableOnly = false;
		PropertyPathHandle->GetChildHandle(FVisiblePropertyPaths::GetEditablePropertiesOnlyName())->GetValue(bInEditableOnly);
		PropertyPathHelpers::ForeachProperty(InStruct, [bInEditableOnly, &bHasProperties](const FProperty& Property)
		{
			if (!bInEditableOnly || Property.HasAnyPropertyFlags(CPF_Edit))
			{
				bHasProperties |= true;
			}
		});
		return bHasProperties;
	}
}

bool FPropertyPathsClassFilter::IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions,
	const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs)
{
	return PropertyPathsFilterUtils::HasAnyProperties(InClass, PropertyPathHandle);
}

bool FPropertyPathsClassFilter::IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs)
{
	return !InUnloadedClassData->HasAnyClassFlags(FilteredClassFlags);
}

bool FPropertyPathsStructFilter::IsStructAllowed(const FStructViewerInitializationOptions& InInitOptions,
	const UScriptStruct* InStruct, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs)
{
	return !InStruct->IsStructTrashed() && PropertyPathsFilterUtils::HasAnyProperties(InStruct, PropertyPathHandle);
}
