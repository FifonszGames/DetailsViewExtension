// Copyright FifonszGames. All Rights Reserved.

#include "DetailsViewExtensionLibrary.h"

#include "Blueprint/BlueprintExceptionInfo.h"
#include "PropertyPaths/PropertyPathsHelpers.h"
#include "Views/StructView.h"


void UDetailsViewExtensionLibrary::GetStructValue(const UStructView* InFromView, EOperationResult& OutOperationResult, int32& OutData)
{
	// We should never hit this!
	checkNoEntry();
}

DEFINE_FUNCTION(UDetailsViewExtensionLibrary::execGetStructValue)
{
	ExecuteStructCopying(Context, Stack, ECopyMethod::FromViewToData);
}

void UDetailsViewExtensionLibrary::SetStructValue(const UStructView* InTargetView, EOperationResult& OutOperationResult, const int32& InSourceData)
{
	// We should never hit this!
	checkNoEntry();
}

FName UDetailsViewExtensionLibrary::GetFieldFName(const FField& InField, bool bInAccountForDisplayNameMeta)
{
	if(bInAccountForDisplayNameMeta)
	{
		const FString* DisplayName = InField.FindMetaData(PropertyPathHelpers::Get::Meta::DisplayName());
		if(DisplayName && !DisplayName->IsEmpty())
		{
			return FName(*DisplayName);	
		}
	}
	return InField.GetFName();
}

FString UDetailsViewExtensionLibrary::GetFieldNameString(const FField& InField, bool bInAccountForDisplayNameMeta)
{
	return GetFieldFName(InField, bInAccountForDisplayNameMeta).ToString();
}

DEFINE_FUNCTION(UDetailsViewExtensionLibrary::execSetStructValue)
{
	ExecuteStructCopying(Context, Stack, ECopyMethod::FromDataToView);
}

void UDetailsViewExtensionLibrary::ExecuteStructCopying(const UObject* Context, FFrame& Stack, ECopyMethod InCopyMethod)
{
	P_GET_OBJECT(UStructView, InTargetView);
	P_GET_ENUM_REF(EOperationResult, OutOperationResult);

	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentPropertyContainer = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	
	const FStructProperty* ValueProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	OutOperationResult = EOperationResult::Failure;

	if (!ValueProp || !ValuePtr || !InTargetView || !InTargetView->GetSourceStruct())
	{
		const FBlueprintExceptionInfo ExceptionInfo(EBlueprintExceptionType::AccessViolation,FText::FromString(TEXT("Failed to resolve the value")));

		FBlueprintCoreDelegates::ThrowScriptException(Context, Stack, ExceptionInfo);
	}
	else
	{
		const UScriptStruct* ValueType = ValueProp->Struct;
		const UScriptStruct* StructViewType  = InTargetView->GetSourceStruct();

		if(AreMatchingTypes(ValueType, StructViewType))
		{
			P_NATIVE_BEGIN;
				const void* SourceData = InCopyMethod == ECopyMethod::FromDataToView ? ValuePtr : InTargetView->GetStructMemory();
				void* DestinationData = InCopyMethod == ECopyMethod::FromDataToView ? InTargetView->GetStructMemoryVolatile() : ValuePtr;
				ValueType->CopyScriptStruct(DestinationData, SourceData);
				OutOperationResult = EOperationResult::Success;
			P_NATIVE_END;	
		}
		else
		{
			const FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				FText::FromString(FString::Printf(TEXT("Value type [%s] and struct view type [%s] are not matching types"),
					ValueType ? *ValueType->GetName() : TEXT("Unknown output type"),
					StructViewType ? *StructViewType->GetName() : TEXT("Unknown struct view type"))));
			
			FBlueprintCoreDelegates::ThrowScriptException(Context, Stack, ExceptionInfo);
		}
	}
}

bool UDetailsViewExtensionLibrary::AreMatchingTypes(const UScriptStruct* InFirstType, const UScriptStruct* InSecondType)
{
	return InFirstType && InSecondType && (InFirstType == InSecondType || (InFirstType->IsChildOf(InSecondType) && FStructUtils::TheSameLayout(InFirstType, InSecondType)));
}

void UDetailsViewExtensionLibrary::ForeachProperty(const UStruct* InStruct, const TFunctionRef<void(const FProperty& Property)>& InAction, EFieldIterationFlags InIterationFlags)
{
	for (TFieldIterator<FProperty> PropIt(InStruct, InIterationFlags); PropIt; ++PropIt)
	{
		if(const FProperty* Property = *PropIt)
		{
			InAction(*Property);
		}
	}
}
