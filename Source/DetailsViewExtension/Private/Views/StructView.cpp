// Copyright FifonszGames. All Rights Reserved.

#include "Views/StructView.h"

#include "IStructureDetailsView.h"
#include "PropertyEditorModule.h"
#include "PropertyPaths/PropertyPathsHelpers.h"

void UStructView::SetStructType(const UScriptStruct* InNewStructType)
{
	if(InNewStructType != StructPropertyPaths.GetSourceClass())
	{
		StructPropertyPaths.SetSourceClass(InNewStructType);
		ResetStructValue();
	}
}

void UStructView::ResetStructValue()
{
	if(CurrentStruct.IsValid())
	{
		CurrentStruct->Reset();
		CurrentStruct->Initialize(StructPropertyPaths.GetSourceClass());
	}
	else
	{
		CurrentStruct = MakeShareable(new FStructOnScope(StructPropertyPaths.GetSourceClass()));
	}
	if(StructDetailsView)
	{
		StructDetailsView->SetStructureData(CurrentStruct);
	}
}

IDetailsView* UStructView::GetDetailsView() const
{
	return StructDetailsView.IsValid() ? StructDetailsView->GetDetailsView() : nullptr;
}

const UStruct* UStructView::GetViewType() const
{
	return CurrentStruct.IsValid() ? CurrentStruct->GetStruct() : nullptr;
}

TSharedRef<SWidget> UStructView::CreateContentWidget()
{
	StructDetailsView.Reset();
	
	if(StructPropertyPaths.GetSourceClass())
	{
		ResetStructValue();
		
		StructDetailsView = CreateStructureDetailView();
		
		if (StructDetailsView.IsValid())
		{
			return StructDetailsView->GetWidget().ToSharedRef();
		}
	}
	
	ResetCurrentStruct();
	return SNew(STextBlock)
		.Text(FText::FromString(FString::Printf(TEXT("Failed to create view. Check if struct in [%s] is empty!"), GET_MEMBER_NAME_STRING_CHECKED(UStructView, StructPropertyPaths))));
}

TSet<FName> UStructView::GetUpdatableMemberVariableNames() const
{
	TSet<FName> Names = Super::GetUpdatableMemberVariableNames();
	Names.Add(GET_MEMBER_NAME_CHECKED(UStructView, StructPropertyPaths));
	return Names;
}

void UStructView::TryUpdateOnPostEditChange(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if(PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UStructView, CustomName))
	{
		if(StructDetailsView.IsValid())
		{
			StructDetailsView->SetCustomName(CustomName);
			TryForceRefresh();
		}
	}
	else
	{
		Super::TryUpdateOnPostEditChange(PropertyChangedEvent);
	}
}

void UStructView::ResetCurrentStruct()
{
	if(CurrentStruct.IsValid())
	{
		CurrentStruct->Reset();
		CurrentStruct.Reset();
	}
}

void UStructView::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	StructDetailsView.Reset();
	ResetCurrentStruct();
}

TSharedRef<IStructureDetailsView> UStructView::CreateStructureDetailView()
{
	FStructureDetailsViewArgs StructArgs;
	{
		StructArgs.bShowObjects = true;
		StructArgs.bShowAssets = true;
		StructArgs.bShowClasses = true;
		StructArgs.bShowInterfaces = true;
	}
	
	//creating with nullptr for delegates to correctly register
	FPropertyEditorModule& PropertyEditorModule = PropertyPathHelpers::Get::PropertyEditor();
	TSharedRef<IStructureDetailsView> DetailsView = PropertyEditorModule.CreateStructureDetailView(
		CreateDetailsViewArgs(),
		StructArgs,
		nullptr,
		CustomName);
	
	DetailsView->GetDetailsView()->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateUObject(this, &UStructView::GetIsPropertyVisible));
	DetailsView->SetStructureData(CurrentStruct);
	return DetailsView;
}
