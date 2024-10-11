// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "TypeView.h"
#include "PropertyPaths/VisiblePropertyPaths.h"
#include "StructView.generated.h"

UCLASS()
class DETAILSVIEWEXTENSION_API UStructView : public UTypeView
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="StructView")
	void SetStructType(const UScriptStruct* InNewStructType);
	
	const UScriptStruct* GetSourceStruct() const { return StructPropertyPaths.GetSourceClass(); }
	
	UFUNCTION(BlueprintCallable, Category="StructView")
	void ResetStructValue();
	
	template<typename StructType>
	const StructType* GetData() const
	{
		return CurrentStruct.IsValid() ? CastField<StructType>(CurrentStruct->GetStructMemory()) : nullptr;
	}

protected:
	//~ UTypeViewBase interface
	virtual IDetailsView* GetDetailsView() const override;
	virtual const UStruct* GetViewType() const override;
	virtual TSharedRef<SWidget> CreateContentWidget() override;
	virtual TSet<FName> GetUpdatableMemberVariableNames() const override;
	virtual const FVisiblePropertyPaths& GetVisiblePropertyPaths() const override { return StructPropertyPaths; }
	// End of UTypeViewBase interface
	
public:
	//~ UWidget interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UWidget interface
	
private:
	UPROPERTY(EditAnywhere)
	FText CustomName;
	UPROPERTY(EditAnywhere, Category="ClassView")
	FStructPropertyPaths StructPropertyPaths;
	
	TSharedRef<IStructureDetailsView> CreateStructureDetailView();
	void ResetCurrentStruct();
	
	FORCEINLINE const uint8* GetStructMemory() const { return CurrentStruct.IsValid() ? CurrentStruct->GetStructMemory() : nullptr; }
	FORCEINLINE uint8* GetStructMemoryVolatile() const { return CurrentStruct.IsValid() ? CurrentStruct->GetStructMemory() : nullptr; }

	TSharedPtr<FStructOnScope> CurrentStruct;
	TSharedPtr<IStructureDetailsView> StructDetailsView;
	
	friend class UDetailsViewExtensionLibrary;
};
