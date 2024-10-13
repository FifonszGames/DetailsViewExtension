// Copyright FifonszGames. All Rights Reserved.

#include "Views/TypeView.h"

#include "PropertyPath.h"

#define LOCTEXT_NAMESPACE "UMG"

FDetailsViewArgs FDetailsViewParameters::AsDetailsViewArgs() const
{
	FDetailsViewArgs DetailsViewArgs;
	{
		DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
		DetailsViewArgs.bUpdatesFromSelection = false;
		DetailsViewArgs.bLockable = false;
		DetailsViewArgs.bShowPropertyMatrixButton = false;
	
		DetailsViewArgs.bAllowSearch = bAllowFiltering;
		DetailsViewArgs.bAllowFavoriteSystem = bAllowFavoriteSystem;
		DetailsViewArgs.bShowOptions = bAllowFiltering;
		DetailsViewArgs.bShowModifiedPropertiesOption = bShowModifiedPropertiesOption;
		DetailsViewArgs.bShowKeyablePropertiesOption = bShowKeyablePropertiesOption;
		DetailsViewArgs.bShowAnimatedPropertiesOption = bShowAnimatedPropertiesOption;
		DetailsViewArgs.bShowScrollBar = bShowScrollBar;
		DetailsViewArgs.ColumnWidth = ColumnWidth;
		DetailsViewArgs.RightColumnMinWidth = RightColumnMinWidth;
	}
	return DetailsViewArgs;
}

TSet<FName> UTypeView::GetUpdatableMemberVariableNames() const
{
	TSet BaseNames{GET_MEMBER_NAME_CHECKED(UTypeView, DetailsViewParameters), GET_MEMBER_NAME_CHECKED(UTypeView, bIsPropertyEditingEnabled),};
	return BaseNames;
}

void UTypeView::RefreshContentWidget()
{
	if(DisplayedWidget.IsValid())
	{
		DisplayedWidget->SetContent(CreateContentWidget());
		if(IDetailsView* DetailsView = GetDetailsView())
		{
			DetailsView->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateUObject(this, &UTypeView::GetIsPropertyEditingEnabled));
			DetailsView->ForceRefresh();
		}
	}
	else
	{
		InvalidateLayoutAndVolatility();
	}
}

void UTypeView::ToggleFilterArea(bool bInToVisible) const
{
	if(IDetailsView* DetailsView = GetDetailsView())
	{
		DetailsView->HideFilterArea(bInToVisible);
	}
}

void UTypeView::ShowAllAdvancedProperties() const
{
	if(IDetailsView* DetailsView = GetDetailsView())
	{
		DetailsView->ShowAllAdvancedProperties();
	}
}

void UTypeView::ClearSearch() const
{
	if(IDetailsView* DetailsView = GetDetailsView())
	{
		DetailsView->ClearSearch();
	}
}

void UTypeView::HighlightProperty(FName InPropertyName) const
{
	if(IDetailsView* DetailsView = GetDetailsView())
	{
		FPropertyPath PropertyPath;
		if(GetPropertyPath(InPropertyName, PropertyPath))
		{
			DetailsView->HighlightProperty(PropertyPath);
		}
	}
}

void UTypeView::ScrollPropertyIntoView(FName InPropertyName, bool bInExpandProperty) const
{
	if(IDetailsView* DetailsView = GetDetailsView())
	{
		FPropertyPath PropertyPath;
		if(GetPropertyPath(InPropertyName, PropertyPath))
		{
			DetailsView->ScrollPropertyIntoView(PropertyPath, bInExpandProperty);
		}
	}
}

void UTypeView::SetDisableCustomDetailLayouts(bool bInIsEnabled) const
{
	if(IDetailsView* DetailsView = GetDetailsView())
	{
		DetailsView->SetDisableCustomDetailLayouts(bInIsEnabled);
	}
}

void UTypeView::TryBroadcastPropertyChanged(FName PropertyName) const
{
	if(!PropertyName.IsNone())
	{
		OnPropertyChangedValue.Broadcast(PropertyName);
	}
}

void UTypeView::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (IsDesignTime())
	{
		if(PropertyChangedEvent.GetPropertyName() == FVisiblePropertyPaths::GetPathsPropertyName())
		{
			if(IDetailsView* DetailsView = GetDetailsView())
			{
				DetailsView->ForceRefresh();
			}
		}
		else
		{
			const TSet<FName> Names = GetUpdatableMemberVariableNames();
			if(Names.Contains(PropertyChangedEvent.GetMemberPropertyName()))
			{
				if(UWorld* World = GetWorld(); World && !RefreshHandle.IsValid())
				{
					constexpr float RefreshRate = 0.15f;
					World->GetTimerManager().SetTimer(RefreshHandle, FTimerDelegate::CreateUObject(this, &UTypeView::RefreshContentWidgetAfterTimer), RefreshRate, false);
				}
			}
		}
	}
}

FDetailsViewArgs UTypeView::CreateDetailsViewArgs()
{
	FDetailsViewArgs Args = DetailsViewParameters.AsDetailsViewArgs();
	Args.NotifyHook = this;
	Args.bForceHiddenPropertyVisibility = !GetVisiblePropertyPaths().ShowEditablePropertiesOnly();
	return Args;
}

void UTypeView::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	FNotifyHook::NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);
	const FName PropertyName = PropertyThatChanged ? PropertyThatChanged->GetFName() : NAME_None;
	TryBroadcastPropertyChanged(PropertyName);
}

void UTypeView::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	DisplayedWidget.Reset();
}

const FText UTypeView::GetPaletteCategory()
{
	return LOCTEXT("Editor", "Editor");
}

TSharedRef<SWidget> UTypeView::RebuildWidget()
{
	DisplayedWidget = SNew(SBorder)
	.Padding(0.0f)
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	.BorderImage(FAppStyle::GetBrush("NoBorder"));
	
	RefreshContentWidget();
	
	return DisplayedWidget.ToSharedRef();
}

void UTypeView::PostInitProperties()
{
	Super::PostInitProperties();
	if(!HasAnyFlags(RF_ClassDefaultObject))
	{
		InitialClass = GetVisiblePropertyPaths().GetSourceStruct();
	}
}

bool UTypeView::GetIsPropertyVisible(const FPropertyAndParent& InPropertyAndParent) const
{
	const FVisiblePropertyPaths& Paths = GetVisiblePropertyPaths();
	if(!IsDesignTime())
	{
		//TODO:: add support to change visible paths after dynamically changing them
		if(InitialClass != Paths.GetSourceStruct())
		{
			return true;
		}
	}
	if(!Paths.IsEmpty() && Paths.GetSourceStruct() == GetViewType())
	{
		return Paths.Contains(InPropertyAndParent);
	}
	return true;
}

void UTypeView::RefreshContentWidgetAfterTimer()
{
	RefreshContentWidget();
	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RefreshHandle);
	}
	RefreshHandle.Invalidate();
}

bool UTypeView::GetPropertyPath(FName InPropertyName, FPropertyPath& OutPath) const
{
	if(const UStruct* Type = GetViewType())
	{
		if(FProperty* Property = Type->FindPropertyByName(InPropertyName))
		{
			OutPath = FPropertyPath::Create(Property).Get();
			return OutPath.IsValid();
		}
	}
	return false;
}

#undef LOCTEXT_NAMESPACE
