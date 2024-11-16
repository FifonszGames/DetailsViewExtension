// Copyright FifonszGames. All Rights Reserved.

#include "PropertyPaths/PropertyPathsHelpers.h"
#include "PropertyPaths/VisiblePropertyPaths.h"

namespace PropertyPathHelpers
{
	void RemovePath(const TSharedPtr<IPropertyHandle>& PropertyHandle, const FString& InPath)
	{
		if(const FVisiblePropertyPaths* ValuePtr = GetPathsFromHandle(PropertyHandle))
		{
			TSharedPtr<IPropertyHandleArray> PathsArray = GetPathsHandleArray(PropertyHandle);
			if(!PathsArray.IsValid())
			{
				return;
			}
			RemovePath(PathsArray.ToSharedRef(), *ValuePtr, InPath);
		}
	}

	void RemovePath(const TSharedRef<IPropertyHandleArray>& ArrayHandle, const FVisiblePropertyPaths& PathsArray, const FString& InPath)
	{
		const int32 PathIndex = PathsArray.GetPaths().Find(InPath);
		if(PathIndex > INDEX_NONE)
		{
			ArrayHandle->DeleteItem(PathIndex);
		}
	}

	void RemovePaths(const TSharedPtr<IPropertyHandle>& PropertyHandle, const TArray<FString>& InPaths)
	{
		if(const FVisiblePropertyPaths* ValuePtr = GetPathsFromHandle(PropertyHandle))
		{
			TSharedPtr<IPropertyHandleArray> PathsArray = GetPathsHandleArray(PropertyHandle);
			if(!PathsArray.IsValid())
			{
				return;
			}
			TSharedRef<IPropertyHandleArray> ArrayHandle = PathsArray.ToSharedRef();
			
			for (const FString& Path : InPaths)
			{
				RemovePath(ArrayHandle, *ValuePtr, Path);
			}
		}
	}

	void AddPaths(const TSharedPtr<IPropertyHandle>& PropertyHandle, const TArray<FString>& InPaths)
	{
		TSharedPtr<IPropertyHandleArray> PathsArray = GetPathsHandleArray(PropertyHandle);
		if(!PathsArray.IsValid())
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
		FPropertyAccess::Result AddResult = PropertyHandle->AddItem();
		check(AddResult == FPropertyAccess::Result::Success);
		FPropertyAccess::Result SetValueResult = PropertyHandle->GetElement(NumElements)->SetValue(InPath);
		check(SetValueResult == FPropertyAccess::Result::Success);
	}

	const FVisiblePropertyPaths* GetPathsFromHandle(const TSharedPtr<IPropertyHandle>& InPropertyHandle)
	{
		if (InPropertyHandle.IsValid())
		{
			void* RawData = nullptr;
			FPropertyAccess::Result Result = InPropertyHandle->GetValueData(RawData);
			if (Result == FPropertyAccess::Success)
			{
				return static_cast<FVisiblePropertyPaths*>(RawData);
			}
		}
		return nullptr;
	}

	void RemoveAllPaths(const TSharedPtr<IPropertyHandle>& PropertyHandle)
	{
		TSharedPtr<IPropertyHandleArray> PathsArray = GetPathsHandleArray(PropertyHandle);
		if(!PathsArray.IsValid())
		{
			return;
		}
		PathsArray->EmptyArray();
	}

	void AddAllChildren(const TSharedPtr<IPropertyHandle>& PropertyHandle, const TSharedPtr<FPropertyPathNode>& ParentNode)
	{
		if(!ParentNode.IsValid())
		{
			return;
		}
		TSharedPtr<IPropertyHandleArray> PathsArray = GetPathsHandleArray(PropertyHandle);
		if(!PathsArray.IsValid())
		{
			return;
		}
		TSharedRef<IPropertyHandleArray> PathsArrayRef = PathsArray.ToSharedRef();
		TArray<TSharedPtr<FPropertyPathNode>> OutItems;
		ParentNode->FillWithOutmostChildren(OutItems);
		
		PathsArrayRef->EmptyArray();
		for (const TSharedPtr<FPropertyPathNode>& PropertyPathNode : OutItems)
		{
			const FString Path = PropertyPathNode->GetTotalPath();
			AddPath(PathsArrayRef, Path);
		} 
	}

	TSharedPtr<IPropertyHandleArray> GetPathsHandleArray(const TSharedPtr<IPropertyHandle>& PropertyHandle)
	{
		TSharedPtr<IPropertyHandle> Handle = PropertyHandle->GetChildHandle(FVisiblePropertyPaths::GetPathsPropertyName());
		return Handle.IsValid() ? Handle->AsArray() : nullptr;
	}

	FName GetFieldFName(const FField& InField, const bool bInAccountForDisplayNameMeta)
	{
		if(bInAccountForDisplayNameMeta)
		{
			const FString* DisplayName = InField.FindMetaData(PropertyPathHelpers::Get::Meta::DisplayName());
			if(DisplayName && !DisplayName->IsEmpty())
			{
				return FName(*DisplayName);	
			}
		}
		return InField.GetFName();
	}

	FString GetFieldNameString(const FField& InField, const bool bInAccountForDisplayNameMeta)
	{
		return GetFieldFName(InField, bInAccountForDisplayNameMeta).ToString();
	}

	void ForeachProperty(const UStruct* InStruct, const TFunctionRef<void(const FProperty& Property)>& InAction, const EFieldIterationFlags InIterationFlags)
	{
		for (TFieldIterator<FProperty> PropIt(InStruct, InIterationFlags); PropIt; ++PropIt)
		{
			if(const FProperty* Property = *PropIt)
			{
				InAction(*Property);
			}
		}
	}

	FPropertyEditorModule& Get::PropertyEditor()
	{
		return FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	}
}
