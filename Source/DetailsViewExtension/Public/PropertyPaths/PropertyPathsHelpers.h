// Copyright FifonszGames All Rights Reserved.

#pragma once

#include "ClassViewerModule.h"
#include "StructViewerModule.h"

struct FPropertyPathNode;
struct FVisiblePropertyPaths;

namespace PropertyPathHelpers
{
	void RemovePath(const TSharedPtr<IPropertyHandle>& PropertyHandle, const FString& InPath);
	void RemovePaths(const TSharedPtr<IPropertyHandle>& PropertyHandle, const TArray<FString>& InPaths);
	void RemovePath(const TSharedRef<IPropertyHandleArray>& ArrayHandle, const FVisiblePropertyPaths& PathsArray,
		const FString& InPath);

	void AddPaths(const TSharedPtr<IPropertyHandle>& PropertyHandle, const TArray<FString>& InPaths);
	void AddPath(const TSharedPtr<IPropertyHandle>& PropertyHandle, const FString& InPath);
	void AddPath(const TSharedRef<IPropertyHandleArray>& PropertyHandle, const FString& InPath);

	const FVisiblePropertyPaths* GetPathsFromHandle(const TSharedPtr<IPropertyHandle>& InPropertyHandle);

	void RemoveAllPaths(const TSharedPtr<IPropertyHandle>& PropertyHandle);
	void AddAllChildren(const TSharedPtr<IPropertyHandle>& PropertyHandle,
		const TSharedPtr<FPropertyPathNode>& ParentNode);

	TSharedPtr<IPropertyHandleArray> GetPathsHandleArray(const TSharedPtr<IPropertyHandle>& PropertyHandle);

	FName GetFieldFName(const FField& InField, const bool bInAccountForDisplayNameMeta = false);
	FString GetFieldNameString(const FField& InField, const bool bInAccountForDisplayNameMeta = false);
	void ForeachProperty(const UStruct* InStruct, const TFunctionRef<void(const FProperty& Property)>& InAction,
		const EFieldIterationFlags InIterationFlags = EFieldIterationFlags::Default);

	template <typename T>
	TEnableIf<!TIsDerivedFrom<T, UObject>::IsDerived, T*>::Type GetValueFromHandle(
		const TSharedRef<IPropertyHandle> Handle)
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
	TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, T*>::Type GetValueFromHandle(
		const TSharedRef<IPropertyHandle> Handle)
	{
		UObject* RawData = nullptr;
		const FPropertyAccess::Result Result = Handle->GetValue(RawData);
		if (Result == FPropertyAccess::Success)
		{
			return Cast<T>(RawData);
		}
		return nullptr;
	}

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

		inline FString Separator() { return TEXT("."); }
		inline char SeparatorAsChar() { return Separator()[0]; }
	}
}
