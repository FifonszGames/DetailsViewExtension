// Copyright FifonszGames. All Rights Reserved.

#include "Views/StructView.h"

#include "IStructureDetailsView.h"
#include "PropertyEditorModule.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "PropertyPaths/PropertyPathsHelpers.h"

void UStructView::GetStructValue(const UStructView* InFromView, EOperationResult& OutOperationResult, int32& OutData)
{
	// We should never hit this!
	checkNoEntry();
}

DEFINE_FUNCTION(UStructView::execGetStructValue)
{
	ExecuteStructCopying(Context, Stack, ECopyMethod::FromViewToData);
}

void UStructView::SetStructValue(const UStructView* InTargetView, EOperationResult& OutOperationResult, const int32& InSourceData)
{
	// We should never hit this!
	checkNoEntry();
}

DEFINE_FUNCTION(UStructView::execSetStructValue)
{
	ExecuteStructCopying(Context, Stack, ECopyMethod::FromDataToView);
}

void UStructView::ExecuteStructCopying(const UObject* Context, FFrame& Stack, ECopyMethod InCopyMethod)
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

bool UStructView::AreMatchingTypes(const UScriptStruct* InFirstType, const UScriptStruct* InSecondType)
{
	return InFirstType && InSecondType && (InFirstType == InSecondType || (InFirstType->IsChildOf(InSecondType) && FStructUtils::TheSameLayout(InFirstType, InSecondType)));
}

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
