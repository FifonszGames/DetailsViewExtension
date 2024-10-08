// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "Components/Widget.h"
#include "PropertyPaths/VisiblePropertyPaths.h"
#include "TypeView.generated.h"

class SBorder;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPropertyChangedValue, FName, PropertyName);

USTRUCT(BlueprintType)
struct FDetailsViewParameters
{
	GENERATED_BODY()

	FDetailsViewArgs AsDetailsViewArgs() const;

	/** True if we allow filtering through search and the filter dropdown menu. */
	UPROPERTY(EditAnywhere)
	bool bAllowFiltering = false;

	/** If false, the current properties editor will never display the favorite system */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bAllowFiltering"))
	bool bAllowFavoriteSystem = false;

	/** True if you want to show the 'Show Only Modified Properties'. Only valid in conjunction with bAllowFiltering */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bAllowFiltering"))
	bool bShowModifiedPropertiesOption = false;

	/** True if you want to show the 'Show Only Keyable Properties'. Only valid in conjunction with bAllowFiltering */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bAllowFiltering"))
	bool bShowKeyablePropertiesOption = true;

	/** True if you want to show the 'Show Only Animated Properties'. Only valid in conjunction with bAllowFiltering */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bAllowFiltering"))
	bool bShowAnimatedPropertiesOption = true;

	/** The default column width */
	UPROPERTY(EditAnywhere, meta=(ClampMin=0.0f, ClampMax=1.0f))
	float ColumnWidth = 0.65f;

	/** The minimum width of the right column in Slate units. */
	UPROPERTY(EditAnywhere, meta=(ClampMin=0.0f))
	float RightColumnMinWidth = 22.f;

	/** If false, the details panel's scrollbar will always be hidden. Useful when embedding details panels in widgets that either grow to accommodate them, or with scrollbars of their own. */
	UPROPERTY(EditAnywhere)
	bool bShowScrollBar = true;
};

UCLASS(Abstract)
class DETAILSVIEWEXTENSION_API UTypeView : public UWidget, public FNotifyHook
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="TypeView")
	FOnPropertyChangedValue OnPropertyChangedValue;

	UFUNCTION(BlueprintCallable, Category="TypeView")
	void ToggleFilterArea(bool bInToVisible) const;
	UFUNCTION(BlueprintCallable, Category="TypeView")
	void ShowAllAdvancedProperties() const;
	UFUNCTION(BlueprintCallable, Category="TypeView")
	void ClearSearch() const;

	UFUNCTION(BlueprintCallable, Category="TypeView")
	void HighlightProperty(FName InPropertyName) const;
	UFUNCTION(BlueprintCallable, Category="TypeView")
	void ScrollPropertyIntoView(FName InPropertyName, bool bInExpandProperty = false) const;

	UFUNCTION(BlueprintCallable, Category="TypeView")
	void SetIsPropertyEditingEnabled(const bool bInIsEnabled) { bIsPropertyEditingEnabled = bInIsEnabled; }

	UFUNCTION(BlueprintCallable, Category="TypeView")
	void SetDisableCustomDetailLayouts(bool bInIsEnabled) const;

protected:
	FDetailsViewArgs CreateDetailsViewArgs();

	virtual TSharedRef<SWidget> CreateContentWidget() PURE_VIRTUAL(UTypeViewBase::GetContentWidget,return SNullWidget::NullWidget;)
	virtual IDetailsView* GetDetailsView() const PURE_VIRTUAL(UTypeViewBase::GetDetailsView, return nullptr;)
	virtual const UStruct* GetViewType() const PURE_VIRTUAL(UTypeViewBase::GetViewType, return nullptr;)
	virtual const FVisiblePropertyPaths& GetVisiblePropertyPaths() const PURE_VIRTUAL(UTypeViewBase::GetVisiblePropertyPaths, return FVisiblePropertyPaths::Invalid;)
	virtual TSet<FName> GetUpdatableMemberVariableNames() const;

	bool GetIsPropertyVisible(const FPropertyAndParent& InPropertyAndParent) const;

	UPROPERTY(EditAnywhere, Category="TypeView", BlueprintReadOnly)
	bool bIsPropertyEditingEnabled = true;

public:
	//~ UObject interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End of UObject interface

	//~ FNotifyHook interface
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;
	//~ End of FNotifyHook interface

	//~ UWidget interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual const FText GetPaletteCategory() override;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void PostInitProperties() override;
	//~ End of UWidget interface

	void TryBroadcastPropertyChanged(FName PropertyName) const;
	
	UPROPERTY(EditAnywhere, Category = "TypeView", BlueprintReadWrite)
	FDetailsViewParameters DetailsViewParameters;
	
private:
	void RefreshContentWidget();
	void RefreshContentWidgetAfterTimer();
	bool GetPropertyPath(FName InPropertyName, FPropertyPath& OutPath) const;

	FORCEINLINE bool GetIsPropertyEditingEnabled() const { return bIsPropertyEditingEnabled; }
	
	UPROPERTY(Transient)
	TObjectPtr<const UStruct> InitialClass;

	TSharedPtr<SBorder> DisplayedWidget;
	FTimerHandle RefreshHandle;
};
