// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "TypeView.h"
#include "PropertyPaths/VisiblePropertyPaths.h"
#include "ClassView.generated.h"

UCLASS()
class DETAILSVIEWEXTENSION_API UClassView : public UTypeView
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category="ClassView")
	const UClass* GetViewClass() const { return ClassPropertyPaths.GetSourceClass(); }
	UFUNCTION(BlueprintCallable, Category="ClassView")
	void SetViewClass(const UClass* InNewClass);
	
	UFUNCTION(BlueprintCallable, Category="ClassView")
	void CopyValuesFromObject(UObject* InNewObject);
	UFUNCTION(BlueprintPure, Category="ClassView")
	UObject* GetObject() const { return CurrentObject; }

	template<typename  T>
	T* GetObject() const { return Cast<T>(CurrentObject); }
	
protected:
	UPROPERTY(EditAnywhere, Category="ClassView")
	FClassPropertyPaths ClassPropertyPaths;
	
	//~ UTypeView interface
	virtual IDetailsView* GetDetailsView() const override { return ClassDetailsView.Get(); }
	virtual TSharedRef<SWidget> CreateContentWidget() override;
	virtual TSet<FName> GetUpdatableMemberVariableNames() const override;
	virtual const UStruct* GetViewType() const override;
	virtual const FVisiblePropertyPaths& GetVisiblePropertyPaths() const override { return ClassPropertyPaths; }
	// End of UTypeView interface

public:
	//~ UWidget interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UWidget interface

private:
	void RemoveCurrentObject() const;
	void RecreateCurrentObject(const UObject* InFromObject = nullptr);

	UPROPERTY(Transient)
	TObjectPtr<UObject> CurrentObject;
	
	TSharedPtr<IDetailsView> ClassDetailsView;
};
