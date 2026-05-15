// Copyright FifonszGames All Rights Reserved.

#include "Views/ClassView.h"

#include "DetailsViewExtensionUtils.h"

void UClassView::SetViewClass(const UClass* InNewClass)
{
	if (InNewClass == ClassPropertyPaths.GetSourceClass())
	{
		return;
	}

	ClassPropertyPaths.SetSourceClass(InNewClass);
	RecreateCurrentObject();
}

void UClassView::CopyValuesFromObject(UObject* InNewObject)
{
	if (InNewObject)
	{
		if (CurrentObject)
		{
			const UClass* NewObjectClass = InNewObject->GetClass();
			if (NewObjectClass == CurrentObject->GetClass())
			{
				if (ClassPropertyPaths.GetSourceClass() != NewObjectClass)
				{
					ClassPropertyPaths.SetSourceClass(NewObjectClass);
				}
				UEngine::CopyPropertiesForUnrelatedObjects(InNewObject, CurrentObject);
				return;
			}
		}
		RecreateCurrentObject(InNewObject);
	}
}

TSharedRef<SWidget> UClassView::CreateContentWidget()
{
	ClassDetailsView.Reset();

	if (const UClass* SourceClass = ClassPropertyPaths.GetSourceClass())
	{
		FPropertyEditorModule& PropertyEditorModule = DetailsViewExtensionUtils::Get::PropertyEditor();

		FDetailsViewArgs DetailsViewArgs = CreateDetailsViewArgs();
		DetailsViewArgs.bShowCustomFilterOption = false;

		ClassDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
		if (ClassDetailsView.IsValid())
		{
			if (!CurrentObject || CurrentObject->GetClass() != SourceClass)
			{
				RecreateCurrentObject();
			}

			ClassDetailsView->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateUObject(this, &UClassView::GetIsPropertyVisible));
			ClassDetailsView->SetObject(CurrentObject.Get());

			return ClassDetailsView.ToSharedRef();
		}
	}
	RemoveCurrentObject();
	return SNew(STextBlock)
		.Text(FText::FromString(FString::Printf(TEXT("Failed to create view. Check if class in [%s] is empty"),
			GET_MEMBER_NAME_STRING_CHECKED(UClassView, ClassPropertyPaths))));
}

TSet<FName> UClassView::GetUpdatableMemberVariableNames() const
{
	TSet<FName> Names = Super::GetUpdatableMemberVariableNames();
	Names.Add(GET_MEMBER_NAME_CHECKED(UClassView, ClassPropertyPaths));

	return Names;
}

const UStruct* UClassView::GetViewType() const
{
	return CurrentObject ? CurrentObject->GetClass() : nullptr;
}

void UClassView::ReleaseSlateResources(const bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	CurrentObject = nullptr;
}

void UClassView::RemoveCurrentObject() const
{
	if (UObject* Object = CurrentObject.Get())
	{
		Object->MarkAsGarbage();
		Object = nullptr;
	}
}

void UClassView::RecreateCurrentObject(const UObject* InFromObject)
{
	RemoveCurrentObject();
	const UObject* CopyFrom = nullptr;
	if (InFromObject)
	{
		ClassPropertyPaths.SetSourceClass(InFromObject->GetClass());
		CopyFrom = InFromObject;
	}
	else if (const UClass* SourceClass = ClassPropertyPaths.GetSourceClass())
	{
		CopyFrom = SourceClass->GetDefaultObject();
	}
	if (CopyFrom)
	{
		CurrentObject = StaticDuplicateObject(CopyFrom, GetTransientPackage());
	}
	ClassDetailsView->SetObject(CurrentObject);
}
