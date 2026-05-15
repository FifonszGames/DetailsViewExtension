// Copyright FifonszGames. All Rights Reserved.

#include "DetailsViewExtension.h"

#include "DetailsViewExtensionUtils.h"
#include "PropertyPaths/VisiblePropertyPaths.h"
#include "PropertyPaths/VisiblePropertyPathsCustomization.h"

void FDetailsViewExtensionModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = DetailsViewExtensionUtils::Get::PropertyEditor();

	PropertyModule.RegisterCustomPropertyTypeLayout(FStructPropertyPaths::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStructPropertyPathsCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout(FClassPropertyPaths::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FClassPropertyPathsCustomization::MakeInstance));
}

void FDetailsViewExtensionModule::ShutdownModule()
{
	FPropertyEditorModule& PropertyModule = DetailsViewExtensionUtils::Get::PropertyEditor();

	PropertyModule.UnregisterCustomPropertyTypeLayout(FStructPropertyPaths::StaticStruct()->GetFName());
	PropertyModule.UnregisterCustomPropertyTypeLayout(FClassPropertyPaths::StaticStruct()->GetFName());
}

IMPLEMENT_MODULE(FDetailsViewExtensionModule, DetailsViewExtension)
