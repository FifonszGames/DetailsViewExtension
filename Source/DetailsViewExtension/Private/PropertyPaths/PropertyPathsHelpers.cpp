// Copyright FifonszGames All Rights Reserved.

#include "PropertyPaths/PropertyPathsHelpers.h"

#include "DetailsViewExtensionUtils.h"
#include "PropertyPaths/PropertyPathNode.h"
#include "PropertyPaths/VisiblePropertyPaths.h"

namespace PropertyPathHelpers
{
	TSharedPtr<IPropertyHandleArray> GetPathsHandleArray(const TSharedPtr<IPropertyHandle>& PropertyHandle)
	{
		const TSharedPtr<IPropertyHandle> Handle = PropertyHandle->GetChildHandle(
			FVisiblePropertyPaths::GetPathsPropertyName());
		return Handle.IsValid() ? Handle->AsArray() : nullptr;
	}

	void RemovePath(const TSharedPtr<IPropertyHandle>& PropertyHandle, const FString& InPath)
	{
		if (const FVisiblePropertyPaths* ValuePtr = GetPathsFromHandle(PropertyHandle))
		{
			const TSharedPtr<IPropertyHandleArray> PathsArray = GetPathsHandleArray(PropertyHandle);
			if (!PathsArray.IsValid())
			{
				return;
			}
			RemovePath(PathsArray.ToSharedRef(), *ValuePtr, InPath);
		}
	}

	void RemovePaths(const TSharedPtr<IPropertyHandle>& PropertyHandle, const TArray<FString>& InPaths)
	{
		if (const FVisiblePropertyPaths* ValuePtr = GetPathsFromHandle(PropertyHandle))
		{
			const TSharedPtr<IPropertyHandleArray> PathsArray = GetPathsHandleArray(PropertyHandle);
			if (!PathsArray.IsValid())
			{
				return;
			}
			const TSharedRef<IPropertyHandleArray> ArrayHandle = PathsArray.ToSharedRef();

			for (const FString& Path : InPaths)
			{
				RemovePath(ArrayHandle, *ValuePtr, Path);
			}
		}
	}

	void RemovePath(const TSharedRef<IPropertyHandleArray>& ArrayHandle, const FVisiblePropertyPaths& PathsArray,
		const FString& InPath)
	{
		const int32 PathIndex = PathsArray.GetPaths().Find(InPath);
		if (PathIndex > INDEX_NONE)
		{
			ArrayHandle->DeleteItem(PathIndex);
		}
	}

	void AddPaths(const TSharedPtr<IPropertyHandle>& PropertyHandle, const TArray<FString>& InPaths)
	{
		const TSharedPtr<IPropertyHandleArray> PathsArray = GetPathsHandleArray(PropertyHandle);
		if (!PathsArray.IsValid())
		{
			return;
		}
		for (const FString& Path : InPaths)
		{
			AddPath(PathsArray.ToSharedRef(), Path);
		}
	}

	void AddPath(const TSharedPtr<IPropertyHandle>& PropertyHandle, const FString& InPath)
	{
		AddPaths(PropertyHandle, TArray{InPath});
	}

	void AddPath(const TSharedRef<IPropertyHandleArray>& PropertyHandle, const FString& InPath)
	{
		uint32 NumElements = 0;
		PropertyHandle->GetNumElements(NumElements);
		const FPropertyAccess::Result AddResult = PropertyHandle->AddItem();
		check(AddResult == FPropertyAccess::Result::Success);
		const FPropertyAccess::Result SetValueResult = PropertyHandle->GetElement(NumElements)->SetValue(InPath);
		check(SetValueResult == FPropertyAccess::Result::Success);
	}

	const FVisiblePropertyPaths* GetPathsFromHandle(const TSharedPtr<IPropertyHandle>& InPropertyHandle)
	{
		return InPropertyHandle.IsValid() ? DetailsViewExtensionUtils::GetValueFromHandle<FVisiblePropertyPaths>(InPropertyHandle.ToSharedRef()) : nullptr;
	}

	void RemoveAllPaths(const TSharedPtr<IPropertyHandle>& PropertyHandle)
	{
		const TSharedPtr<IPropertyHandleArray> PathsArray = GetPathsHandleArray(PropertyHandle);
		if (!PathsArray.IsValid())
		{
			return;
		}
		PathsArray->EmptyArray();
	}

	void AddAllChildren(const TSharedPtr<IPropertyHandle>& PropertyHandle,
		const TSharedPtr<FPropertyPathNode>& ParentNode)
	{
		if (!ParentNode.IsValid())
		{
			return;
		}
		const TSharedPtr<IPropertyHandleArray> PathsArray = GetPathsHandleArray(PropertyHandle);
		if (!PathsArray.IsValid())
		{
			return;
		}
		const TSharedRef<IPropertyHandleArray> PathsArrayRef = PathsArray.ToSharedRef();
		TArray<TSharedPtr<FPropertyPathNode>> OutItems;
		ParentNode->FillWithOutmostChildren(OutItems);

		PathsArrayRef->EmptyArray();
		for (const TSharedPtr<FPropertyPathNode>& PropertyPathNode : OutItems)
		{
			const FString Path = PropertyPathNode->GetTotalPath();
			AddPath(PathsArrayRef, Path);
		}
	}
}
