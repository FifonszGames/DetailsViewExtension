// Copyright FifonszGames All Rights Reserved.


#include "DetailsViewExtensionUtils.h"

class FPropertyEditorModule;
class FStructViewerModule;
class FClassViewerModule;

namespace DetailsViewExtensionUtils
{
	void ForeachProperty(const UStruct* InStruct, const TFunctionRef<void(const FProperty& Property)>& InAction, const EFieldIterationFlags InIterationFlags)
	{
		for (TFieldIterator<FProperty> PropIt(InStruct, InIterationFlags); PropIt; ++PropIt)
		{
			if (const FProperty* Property = *PropIt)
			{
				InAction(*Property);
			}
		}
	}

	FName GetFieldFName(const FField& InField, const bool bInAccountForDisplayNameMeta)
	{
		if (bInAccountForDisplayNameMeta)
		{
			const FString* DisplayName = InField.FindMetaData(Get::Meta::DisplayName());
			if (DisplayName && !DisplayName->IsEmpty())
			{
				return FName(*DisplayName);
			}
		}
		return InField.GetFName();
	}

	FString GetFieldNameString(const FField& InField, const bool bInAccountForDisplayNameMeta)
	{
		return GetFieldFName(InField, bInAccountForDisplayNameMeta).ToString();
	}

	namespace Get
	{
		FPropertyEditorModule& PropertyEditor()
		{
			return FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		}

		FClassViewerModule& ClassViewer()
		{
			return FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");
		}

		FStructViewerModule& StructViewer()
		{
			return FModuleManager::LoadModuleChecked<FStructViewerModule>("StructViewer");
		}
	}
}
