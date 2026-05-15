// Copyright FifonszGames All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FPropertyEditorModule;
class FStructViewerModule;
class FClassViewerModule;

namespace DetailsViewExtensionUtils
{
	void ForeachProperty(const UStruct* InStruct, const TFunctionRef<void(const FProperty& Property)>& InAction,
		const EFieldIterationFlags InIterationFlags = EFieldIterationFlags::Default);

	template <typename T>
	TEnableIf<!TIsDerivedFrom<T, UObject>::IsDerived, T*>::Type GetValueFromHandle(const TSharedRef<IPropertyHandle> Handle)
	{
		void* RawData = nullptr;
		const FPropertyAccess::Result Result = Handle->GetValueData(RawData);
		if (Result == FPropertyAccess::Success)
		{
			return static_cast<T*>(RawData);
		}
		return nullptr;
	}

	template <typename T>
	TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, T*>::Type GetValueFromHandle(const TSharedRef<IPropertyHandle> Handle)
	{
		UObject* RawData = nullptr;
		const FPropertyAccess::Result Result = Handle->GetValue(RawData);
		if (Result == FPropertyAccess::Success)
		{
			return Cast<T>(RawData);
		}
		return nullptr;
	}

	FName GetFieldFName(const FField& InField, const bool bInAccountForDisplayNameMeta = false);
	FString GetFieldNameString(const FField& InField, const bool bInAccountForDisplayNameMeta = false);

	namespace Get
	{
		FPropertyEditorModule& PropertyEditor();
		FClassViewerModule& ClassViewer();
		FStructViewerModule& StructViewer();

		namespace Meta
		{
			inline FName DisplayName() { return TEXT("DisplayName"); }
			inline FName Category() { return TEXT("Category"); }
			inline FName HideViewOptions() { return TEXT("HideViewOptions"); }
			inline FName ShowTreeView() { return TEXT("ShowTreeView"); }
		}
	}
}
