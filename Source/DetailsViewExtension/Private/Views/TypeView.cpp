// Copyright FifonszGames All Rights Reserved.

#include "Views/TypeView.h"

#include "DetailsViewExtensionUtils.h"
#include "PropertyPath.h"

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

void UTypeView::ToggleFilterArea(const bool bInToVisible) const
{
	if (IDetailsView* DetailsView = GetDetailsView())
	{
		DetailsView->HideFilterArea(bInToVisible);
	}
}

void UTypeView::ClearSearch() const
{
	if (IDetailsView* DetailsView = GetDetailsView())
	{
		DetailsView->ClearSearch();
	}
}

void UTypeView::HighlightProperty(const FName InPropertyName) const
{
	if (IDetailsView* DetailsView = GetDetailsView())
	{
		FPropertyPath PropertyPath = FPropertyPath::CreateEmpty().Get();
		GetPropertyPath(InPropertyName, PropertyPath);
		DetailsView->HighlightProperty(PropertyPath);
	}
}

void UTypeView::ScrollPropertyIntoView(const FName InPropertyName, const bool bInExpandProperty) const
{
	if (IDetailsView* DetailsView = GetDetailsView())
	{
		FPropertyPath PropertyPath;
		if (GetPropertyPath(InPropertyName, PropertyPath))
		{
			DetailsView->ScrollPropertyIntoView(PropertyPath, bInExpandProperty);
		}
	}
}

TSet<FName> UTypeView::GetUpdatableMemberVariableNames() const
{
	return {GET_MEMBER_NAME_CHECKED(UTypeView, DetailsViewParameters)};
}

FDetailsViewArgs UTypeView::CreateDetailsViewArgs()
{
	FDetailsViewArgs Args = DetailsViewParameters.AsDetailsViewArgs();
	Args.NotifyHook = this;
	Args.bForceHiddenPropertyVisibility = !GetVisiblePropertyPaths().ShowEditablePropertiesOnly();
	return Args;
}

bool UTypeView::GetIsPropertyVisible(const FPropertyAndParent& InPropertyAndParent) const
{
	const FVisiblePropertyPaths& Paths = GetVisiblePropertyPaths();
	if (!IsDesignTime())
	{
		//TODO:: add support to change visible paths after dynamically changing them
		if (InitialClass != Paths.GetSourceStruct())
		{
			return InPropertyAndParent.Property.HasAnyPropertyFlags(CPF_Edit);
		}
	}
	if (!Paths.IsEmpty() && Paths.GetSourceStruct() == GetViewType())
	{
		return Paths.Contains(InPropertyAndParent);
	}
	return true;
}

void UTypeView::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (IsDesignTime())
	{
		TryUpdateOnPostEditChange(PropertyChangedEvent);
	}
}

void UTypeView::PostInitProperties()
{
	Super::PostInitProperties();
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		InitialClass = GetVisiblePropertyPaths().GetSourceStruct();
	}
}

void UTypeView::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	FNotifyHook::NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);
	const FName PropertyName = PropertyThatChanged ? PropertyThatChanged->GetFName() : NAME_None;
	TryBroadcastPropertyChanged(PropertyName);
}

void UTypeView::ReleaseSlateResources(const bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	DisplayedWidget.Reset();
}

const FText UTypeView::GetPaletteCategory()
{
	return NSLOCTEXT("UMG", "Editor", "Editor");
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

void UTypeView::TryForceRefresh() const
{
	if (IDetailsView* DetailsView = GetDetailsView())
	{
		DetailsView->ForceRefresh();
	}
}

void UTypeView::TryBroadcastPropertyChanged(const FName PropertyName) const
{
	if (!PropertyName.IsNone())
	{
		OnPropertyChangedValue.Broadcast(PropertyName);
	}
}

void UTypeView::TryUpdateOnPostEditChange(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == FVisiblePropertyPaths::GetPathsPropertyName())
	{
		TryForceRefresh();
	}
	else
	{
		const TSet<FName> Names = GetUpdatableMemberVariableNames();
		if (Names.Contains(PropertyChangedEvent.GetMemberPropertyName()))
		{
			if (const UWorld* World = GetWorld(); World && !RefreshHandle.IsValid())
			{
				constexpr float RefreshRate = 0.15f;
				World->GetTimerManager().SetTimer(RefreshHandle, FTimerDelegate::CreateUObject(this, &UTypeView::RefreshContentWidgetAfterTimer), RefreshRate, false);
			}
		}
	}
}

bool UTypeView::IsCustomRowVisible(const FName PropertyName, const FName PropertyCategory) const
{
	return GetVisiblePropertyPaths().ContainsCustomRow(PropertyName, PropertyCategory);
}

void UTypeView::RefreshContentWidget()
{
	if (DisplayedWidget.IsValid())
	{
		DisplayedWidget->SetContent(CreateContentWidget());
		if (IDetailsView* DetailsView = GetDetailsView())
		{
			DetailsView->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateUObject(this, &UTypeView::GetIsPropertyEditingEnabled));
			DetailsView->SetIsCustomRowVisibleDelegate(FIsCustomRowVisible::CreateUObject(this, &UTypeView::IsCustomRowVisible));
			DetailsView->ForceRefresh();
		}
	}
	else
	{
		InvalidateLayoutAndVolatility();
	}
}

void UTypeView::RefreshContentWidgetAfterTimer()
{
	RefreshContentWidget();
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RefreshHandle);
	}
	RefreshHandle.Invalidate();
}

bool UTypeView::GetPropertyPath(const FName InPropertyName, FPropertyPath& OutPath) const
{
	if (const UStruct* Type = GetViewType())
	{
		for (FProperty* Property = Type->PropertyLink; Property != nullptr; Property = Property->PropertyLinkNext)
		{
			if (Property->GetFName() == InPropertyName || Property->GetMetaData(DetailsViewExtensionUtils::Get::Meta::DisplayName()) == InPropertyName)
			{
				OutPath = FPropertyPath::Create(Property).Get();
				return OutPath.IsValid();
			}
		}
	}
	return false;
}
