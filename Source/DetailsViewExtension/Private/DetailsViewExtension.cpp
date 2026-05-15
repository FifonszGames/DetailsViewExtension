#include "DetailsViewExtension.h"

#include "PropertyPaths/PropertyPathsHelpers.h"
#include "PropertyPaths/VisiblePropertyPaths.h"
#include "PropertyPaths/VisiblePropertyPathsCustomization.h"

#define LOCTEXT_NAMESPACE "FDetailsViewExtensionModule"

void FDetailsViewExtensionModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = PropertyPathHelpers::Get::PropertyEditor();

	PropertyModule.RegisterCustomPropertyTypeLayout(FStructPropertyPaths::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FStructPropertyPathsCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout(FClassPropertyPaths::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FClassPropertyPathsCustomization::MakeInstance));
}

void FDetailsViewExtensionModule::ShutdownModule()
{
	FPropertyEditorModule& PropertyModule = PropertyPathHelpers::Get::PropertyEditor();

	PropertyModule.UnregisterCustomPropertyTypeLayout(FStructPropertyPaths::StaticStruct()->GetFName());
	PropertyModule.UnregisterCustomPropertyTypeLayout(FClassPropertyPaths::StaticStruct()->GetFName());
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDetailsViewExtensionModule, DetailsViewExtension)
