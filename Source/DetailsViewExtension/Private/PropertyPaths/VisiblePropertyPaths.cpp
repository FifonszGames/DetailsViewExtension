// Copyright FifonszGames. All Rights Reserved.

#include "PropertyPaths/VisiblePropertyPaths.h"

#include "DetailsViewExtensionLibrary.h"
#include "Algo/AnyOf.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "PropertyPaths/PropertyPathsHelpers.h"

const FVisiblePropertyPaths FVisiblePropertyPaths::Invalid;

bool FVisibleCustomProperties::Contains(const FName& PropertyName, const FName& PropertyCategory) const
{
	if(Properties.IsEmpty() && Categories.IsEmpty())
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
	return VisiblePropertyPaths.ContainsByPredicate([&PropertyPath, &SearchCase](const FString& Path){ return Path.StartsWith(PropertyPath, SearchCase); });
}

FString FVisiblePropertyPaths::CreatePropertyPath(const FPropertyAndParent& InFromProperty)
{
	FString PropertyPath;
	bool bWasBroken = false;
	for (int32 i = InFromProperty.ParentProperties.Num() -1 ; i >= 0 ; --i)
	{
		const FProperty* ParentProperty = InFromProperty.ParentProperties[i];
		if(ParentProperty && InFromProperty.ParentArrayIndices[i] < 0)
		{
			PropertyPath.Append(FString::Printf(TEXT("%s%s"), *GetPropertyName(*ParentProperty), *PropertyPathHelpers::Get::Separator()));
			continue;
		}
		bWasBroken = true;
		break;
	}
	if(!bWasBroken && InFromProperty.ArrayIndex <= INDEX_NONE)
	{
		PropertyPath.Append(GetPropertyName(InFromProperty.Property));
	}
	else
	{
		PropertyPath.RemoveAt(PropertyPath.Len() -1, 1);
	}
	return PropertyPath;
}

FString FVisiblePropertyPaths::GetPropertyName(const FProperty& InProperty, const bool bInAccountForDisplayMeta)
{
	if(bInAccountForDisplayMeta)
	{
		if(const FString* Meta = InProperty.FindMetaData(PropertyPathHelpers::Get::Meta::DisplayName()))
		{
			return *Meta;
		}
	}
	return InProperty.GetName();
}

void FPropertyPathNode::Initialize(const TSharedRef<IPropertyHandle>& SourceHandle, const bool bEditablePropertiesOnly)
{
	bIsEditableProperty = bEditablePropertiesOnly;
	const UObject* Value = nullptr;
	SourceHandle->GetValue(Value);
	if(const UStruct* Class = Cast<UStruct>(Value))
	{
		PropertyName = UDetailsViewExtensionLibrary::GetFieldNameString(*SourceHandle->GetProperty(), true);
		SourceClass = Class;
		CreateChildren(*Class, *Class, bEditablePropertiesOnly);
	}
}

TArray<TSharedPtr<FPropertyPathNode>> FPropertyPathNode::GetChildren(const FString& InFilterString) const
{
	return Children.FilterByPredicate([InFilterString](const TSharedPtr<FPropertyPathNode>& Child)
	{
		return Child->PassesFilter(InFilterString);
	});
}

void FPropertyPathNode::FillWithOutmostChildren(TArray<TSharedPtr<FPropertyPathNode>>& OutItems, const bool bIncludeSelf)
{
	if(bIncludeSelf)
	{
		OutItems.Add(AsShared());
	}
	for (const TSharedPtr<FPropertyPathNode>& Child : Children)
	{
		Child->FillWithOutmostChildren(OutItems, Child->GetChildren().IsEmpty());
	}
}

TSharedPtr<FPropertyPathNode> FPropertyPathNode::GetPropertyByPath(const FString& InPath, const bool bInCountSelf) const
{
	if(bInCountSelf)
	{
		if(InPath.StartsWith(PropertyName))
		{
			const FString NewPath = InPath.LeftChop(InPath.Len() + 1);
			return NewPath.IsEmpty() ? MakeShared<FPropertyPathNode>(*this) : FindChild(NewPath);
		}
		return nullptr;
	}
	return FindChild(InPath);
}

FString FPropertyPathNode::GetTotalPath() const
{
	FString Path = PropertyName;
	if(Parent.IsValid())
	{
		Parent.Pin()->AppendPath(Path);
	}
	return Path;
}

bool FPropertyPathNode::PassesFilter(const FString& FilterString) const
{
	return FilterString.IsEmpty() ||
		PropertyName.Contains(FilterString) ||
		Algo::AnyOf(Children, [&FilterString](const TSharedPtr<FPropertyPathNode>& Child){ return Child->PassesFilter(FilterString);});
}

void FPropertyPathNode::Initialize(const FProperty& InSourceProperty, const UStruct& InOutMostParentClass, const TWeakPtr<const FPropertyPathNode>& InParent,
	const bool bInEditablePropertiesOnly, const bool bInIsEditable)
{
	bIsEditableProperty = bInIsEditable;
	Parent = InParent;
	PropertyName = UDetailsViewExtensionLibrary::GetFieldNameString(InSourceProperty, true);
	
	bool bCreateChildren = !InSourceProperty.HasAnyPropertyFlags(CPF_TObjectPtr | CPF_UObjectWrapper);
	if(const FStructProperty* StructProperty = CastField<FStructProperty>(&InSourceProperty))
	{
		if(const UScriptStruct* Struct = StructProperty->Struct)
		{
			SourceClass = StructProperty->Struct;
			const FPropertyEditorModule& PropertyModule = PropertyPathHelpers::Get::PropertyEditor();
			bCreateChildren = bCreateChildren && !PropertyModule.IsCustomizedStruct(Struct, {});
		}
	}
	else if(const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(&InSourceProperty))
	{
		if(const UClass* Class = ObjectProperty->PropertyClass)
		{
			SourceClass = Class;
			bCreateChildren = false;
		}
	}
	if(bCreateChildren && SourceClass.IsValid())
	{
		CreateChildren(InOutMostParentClass, *SourceClass.Get(), bInEditablePropertiesOnly);
	}
}

void FPropertyPathNode::AppendPath(FString& OutPath) const
{
	if(Parent.IsValid())
	{
		OutPath.InsertAt(0, PropertyName + PropertyPathHelpers::Get::Separator());
		Parent.Pin()->AppendPath(OutPath);
	}
}

TSharedPtr<FPropertyPathNode> FPropertyPathNode::FindChild(const FString& InPath) const
{
	const TSharedPtr<FPropertyPathNode>* Child = Algo::FindByPredicate(Children, [&InPath](const TSharedPtr<FPropertyPathNode>& ChildNode)
	{
		return ChildNode.IsValid() && ChildNode->GetPropertyByPath(InPath, true);
	});
	return Child ? *Child : nullptr;
}

void FPropertyPathNode::CreateChildren(const UStruct& InOutMostParentClass, const UStruct& InFromClass, const bool bInEditablePropertiesOnly)
{
	if(Parent.IsValid() && &InOutMostParentClass == &InFromClass)
	{
		return;
	}
	
	UDetailsViewExtensionLibrary::ForeachProperty(&InFromClass, [this, &InOutMostParentClass, bInEditablePropertiesOnly](const FProperty& Property)
	{
		const bool bIsEditable = Property.HasAnyPropertyFlags(CPF_Edit);
		if(bInEditablePropertiesOnly && !bIsEditable)
		{
			return;
		}
		
		TSharedRef<FPropertyPathNode> Child = MakeShared<FPropertyPathNode>();
		Children.Add(Child);
		Child->Initialize(Property, InOutMostParentClass, AsWeak(), bInEditablePropertiesOnly, bIsEditable);
	});
}