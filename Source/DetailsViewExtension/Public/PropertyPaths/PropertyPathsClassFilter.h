#pragma once

#include "CoreMinimal.h"
#include "ClassViewerFilter.h"
#include "StructViewerFilter.h"

namespace PropertyPathsFilterUtils
{
	constexpr EClassFlags DefaultFilteredClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_Interface | CLASS_Hidden;
}

class FPropertyPathsClassFilter final : public IClassViewerFilter
{
public:
	explicit FPropertyPathsClassFilter(const TSharedRef<IPropertyHandle> InPropertyPathHandle,
		EClassFlags InFilteredClassFlags = PropertyPathsFilterUtils::DefaultFilteredClassFlags) :
		PropertyPathHandle(InPropertyPathHandle), FilteredClassFlags(MoveTemp(InFilteredClassFlags))
	{
	}

	bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass,
		TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override;

	bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions,
		const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData,
		TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override;

private:
	TSharedRef<IPropertyHandle> PropertyPathHandle;
	const EClassFlags FilteredClassFlags;
};


class FPropertyPathsStructFilter final : public IStructViewerFilter
{
public:
	explicit FPropertyPathsStructFilter(const TSharedRef<IPropertyHandle> InPropertyPathHandle) : PropertyPathHandle(
		InPropertyPathHandle)
	{
	}

	bool IsStructAllowed(const FStructViewerInitializationOptions& InInitOptions, const UScriptStruct* InStruct,
		TSharedRef<FStructViewerFilterFuncs> InFilterFuncs) override;

	bool IsUnloadedStructAllowed(const FStructViewerInitializationOptions& InInitOptions,
		const FSoftObjectPath& InStructPath, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs) override { return true; }

private:
	TSharedRef<IPropertyHandle> PropertyPathHandle;
};
