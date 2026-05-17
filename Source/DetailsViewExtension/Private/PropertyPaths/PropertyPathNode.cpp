// Copyright FifonszGames All Rights Reserved.

#include "PropertyPaths/PropertyPathNode.h"

#include "DetailsViewExtensionUtils.h"
#include "Algo/AnyOf.h"
#include "PropertyPaths/PropertyPathsHelpers.h"


void FPropertyPathNode::Initialize(const TSharedRef<IPropertyHandle>& SourceHandle, const bool bEditablePropertiesOnly)
{
	bIsEditableProperty = bEditablePropertiesOnly;
	const UObject* Value = nullptr;
	SourceHandle->GetValue(Value);
	if (const UStruct* Class = Cast<UStruct>(Value))
	{
		PropertyName = DetailsViewExtensionUtils::GetFieldNameString(*SourceHandle->GetProperty(), true);
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
	if (bIncludeSelf)
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
	if (bInCountSelf)
	{
		if (InPath.StartsWith(PropertyName))
		{
			if (InPath.Len() == PropertyName.Len())
			{
				return ConstCastSharedRef<FPropertyPathNode>(AsShared());
			}
			
			const FString NewPath = InPath.RightChop(PropertyName.Len() + PropertyPathHelpers::Separator().Len());
			return NewPath.IsEmpty() ? ConstCastSharedRef<FPropertyPathNode>(AsShared()) : FindChild(NewPath);
		}
		return nullptr;
	}
	return FindChild(InPath);
}

FString FPropertyPathNode::GetTotalPath() const
{
	FString Path = PropertyName;
	if (Parent.IsValid())
	{
		Parent.Pin()->AppendPath(Path);
	}
	return Path;
}

bool FPropertyPathNode::PassesFilter(const FString& FilterString) const
{
	return FilterString.IsEmpty() ||
	PropertyName.Contains(FilterString) ||
	Algo::AnyOf(Children, [&FilterString](const TSharedPtr<FPropertyPathNode>& Child) { return Child->PassesFilter(FilterString); });
}

void FPropertyPathNode::Initialize(const FProperty& InSourceProperty, const UStruct& InOutMostParentClass, const TWeakPtr<const FPropertyPathNode>& InParent,
	const bool bInEditablePropertiesOnly, const bool bInIsEditable)
{
	bIsEditableProperty = bInIsEditable;
	Parent = InParent;
	PropertyName = DetailsViewExtensionUtils::GetFieldNameString(InSourceProperty, true);

	bool bCreateChildren = !InSourceProperty.HasAnyPropertyFlags(CPF_TObjectPtr | CPF_UObjectWrapper);
	if (const FStructProperty* StructProperty = CastField<FStructProperty>(&InSourceProperty))
	{
		if (const UScriptStruct* Struct = StructProperty->Struct)
		{
			SourceClass = StructProperty->Struct;
			const FPropertyEditorModule& PropertyModule = DetailsViewExtensionUtils::Get::PropertyEditor();
			bCreateChildren = bCreateChildren && !PropertyModule.IsCustomizedStruct(Struct, {});
		}
	}
	else if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(&InSourceProperty))
	{
		SourceClass = ObjectProperty->PropertyClass;
		bCreateChildren = false;
	}
	if (bCreateChildren && SourceClass.IsValid())
	{
		CreateChildren(InOutMostParentClass, *SourceClass.Get(), bInEditablePropertiesOnly);
	}
}

void FPropertyPathNode::CreateChildren(const UStruct& InOutMostParentClass, const UStruct& InFromClass, const bool bInEditablePropertiesOnly)
{
	if (Parent.IsValid() && &InOutMostParentClass == &InFromClass)
	{
		return;
	}

	DetailsViewExtensionUtils::ForeachProperty(&InFromClass, [this, &InOutMostParentClass, bInEditablePropertiesOnly](const FProperty& Property)
	{
		const bool bIsEditable = Property.HasAnyPropertyFlags(CPF_Edit);
		if (bInEditablePropertiesOnly && !bIsEditable)
		{
			return;
		}

		const TSharedRef<FPropertyPathNode> Child = MakeShared<FPropertyPathNode>();
		Children.Add(Child);
		Child->Initialize(Property, InOutMostParentClass, AsWeak(), bInEditablePropertiesOnly, bIsEditable);
	});
}

void FPropertyPathNode::AppendPath(FString& OutPath) const
{
	if (Parent.IsValid())
	{
		OutPath.InsertAt(0, PropertyName + PropertyPathHelpers::Separator());
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
