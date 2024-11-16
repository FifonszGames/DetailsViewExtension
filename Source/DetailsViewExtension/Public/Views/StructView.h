// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "TypeView.h"
#include "PropertyPaths/VisiblePropertyPaths.h"
#include "StructView.generated.h"

UENUM()
enum class ECopyMethod : uint8
{
	FromViewToData,
	FromDataToView
};

UENUM()
enum class EOperationResult : uint8
{
	Success,
	Failure
};

UCLASS()
class DETAILSVIEWEXTENSION_API UStructView : public UTypeView
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, CustomThunk, meta=(CustomStructureParam="OutData", ExpandEnumAsExecs="OutOperationResult"))
	static void GetStructValue(const UStructView* InFromView, EOperationResult& OutOperationResult, int32& OutData);
	UFUNCTION(BlueprintCallable, CustomThunk, meta=(CustomStructureParam="InSourceData", ExpandEnumAsExecs="OutOperationResult"))
	static void SetStructValue(const UStructView* InTargetView, EOperationResult& OutOperationResult, const int32& InSourceData);
	
private:
	DECLARE_FUNCTION(execGetStructValue);
	DECLARE_FUNCTION(execSetStructValue);
	
	static void ExecuteStructCopying(const UObject* Context, FFrame& Stack, ECopyMethod InCopyMethod);
	static bool AreMatchingTypes(const UScriptStruct* InFirstType, const UScriptStruct* InSecondType);
	
public:
	UFUNCTION(BlueprintCallable, Category="StructView")
	void SetStructType(const UScriptStruct* InNewStructType);
	UFUNCTION(BlueprintCallable, Category="StructView")
	void ResetStructValue();
	
	const UScriptStruct* GetSourceStruct() const { return StructPropertyPaths.GetSourceClass(); }
	
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
	virtual void TryUpdateOnPostEditChange(const FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual const FVisiblePropertyPaths& GetVisiblePropertyPaths() const override { return StructPropertyPaths; }
	// End of UTypeViewBase interface
	
public:
	//~ UWidget interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UWidget interface
	
private:
	UPROPERTY(EditAnywhere, Category="StructView")
	FText CustomName;
	UPROPERTY(EditAnywhere, Category="StructView")
	FStructPropertyPaths StructPropertyPaths;
	
	TSharedRef<IStructureDetailsView> CreateStructureDetailView();
	void ResetCurrentStruct();
	
	FORCEINLINE const uint8* GetStructMemory() const { return CurrentStruct.IsValid() ? CurrentStruct->GetStructMemory() : nullptr; }
	FORCEINLINE uint8* GetStructMemoryVolatile() const { return CurrentStruct.IsValid() ? CurrentStruct->GetStructMemory() : nullptr; }

	TSharedPtr<FStructOnScope> CurrentStruct;
	TSharedPtr<IStructureDetailsView> StructDetailsView;
	
	friend class UDetailsViewExtensionLibrary;
};
