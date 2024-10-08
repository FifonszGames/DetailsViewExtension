// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "DetailsViewExtensionLibrary.generated.h"

class UStructView;
class UScriptStruct;
class UObject;
struct FFrame;

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
class DETAILSVIEWEXTENSION_API UDetailsViewExtensionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "DetailsViewExtensionLibrary", meta=(CustomStructureParam="OutData", ExpandEnumAsExecs="OutOperationResult"))
	static void GetStructValue(const UStructView* InFromView, EOperationResult& OutOperationResult, int32& OutData);
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "DetailsViewExtensionLibrary", meta=(CustomStructureParam="InSourceData", ExpandEnumAsExecs="OutOperationResult"))
	static void SetStructValue(const UStructView* InTargetView, EOperationResult& OutOperationResult, const int32& InSourceData);

	static FName GetFieldFName(const FField& InField, bool bInAccountForDisplayNameMeta = false);
	static FString GetFieldNameString(const FField& InField, bool bInAccountForDisplayNameMeta = false);
		
	static bool AreMatchingTypes(const UScriptStruct* InFirstType, const UScriptStruct* InSecondType);
	static void ForeachProperty(const UStruct* InStruct, const TFunctionRef<void(const FProperty& Property)>& InAction, EFieldIterationFlags InIterationFlags = EFieldIterationFlags::Default);

private:
	DECLARE_FUNCTION(execGetStructValue);
	DECLARE_FUNCTION(execSetStructValue);

	static void ExecuteStructCopying(const UObject* Context, FFrame& Stack, ECopyMethod InCopyMethod);
};
