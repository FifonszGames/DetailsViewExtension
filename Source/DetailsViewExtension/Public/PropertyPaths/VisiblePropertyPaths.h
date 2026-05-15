// Copyright FifonszGames All Rights Reserved.

#pragma once

#include "VisiblePropertyPaths.generated.h"

USTRUCT()
struct FVisibleCustomProperties
{
	GENERATED_BODY()

	bool Contains(const FName& PropertyName, const FName& PropertyCategory) const;

private:
	UPROPERTY(EditAnywhere, Category="VisibleCustomProperties")
	TArray<FString> Properties;
	UPROPERTY(EditAnywhere, Category="VisibleCustomProperties")
	TArray<FString> Categories;
};

USTRUCT()
struct FVisiblePropertyPaths
{
	GENERATED_BODY()

	virtual ~FVisiblePropertyPaths() = default;
	explicit FVisiblePropertyPaths() = default;

	static const FVisiblePropertyPaths Invalid;

	bool Contains(const FPropertyAndParent& InProperty) const;
	bool ContainsCustomRow(const FName& PropertyName, const FName& PropertyCategory) const;
	bool HasMatchingPath(const FString& PropertyPath, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase) const;

	bool IsEmpty() const { return VisiblePropertyPaths.IsEmpty(); }

	const TArray<FString>& GetPaths() const { return VisiblePropertyPaths; }
	virtual const UStruct* GetSourceStruct() const { return nullptr; }

	bool ShowEditablePropertiesOnly() const { return bEditablePropertiesOnly; }

	static FName GetClassPropertyName() { return FName(TEXT("SourceClass")); }
	static FName GetPathsPropertyName() { return GET_MEMBER_NAME_CHECKED(FVisiblePropertyPaths, VisiblePropertyPaths); }
	static FName GetEditablePropertiesOnlyName() { return GET_MEMBER_NAME_CHECKED(FVisiblePropertyPaths, bEditablePropertiesOnly); }
	static FName GetVisibleCustomRowsName() { return GET_MEMBER_NAME_CHECKED(FVisiblePropertyPaths, VisibleCustomProperties); }

private:
	static FString CreatePropertyPath(const FPropertyAndParent& InFromProperty);

	UPROPERTY(EditAnywhere, Category="VisiblePropertyPaths")
	TArray<FString> VisiblePropertyPaths;
	UPROPERTY(EditAnywhere, Category="VisiblePropertyPaths")
	bool bEditablePropertiesOnly = true;
	UPROPERTY(EditAnywhere, Category="VisiblePropertyPaths")
	FVisibleCustomProperties VisibleCustomProperties;
};

USTRUCT()
struct FStructPropertyPaths : public FVisiblePropertyPaths
{
	GENERATED_BODY()

	void SetSourceClass(const UScriptStruct* InClass) { SourceClass = InClass; }
	const UScriptStruct* GetSourceClass() const { return SourceClass; }

	const UStruct* GetSourceStruct() const override { return SourceClass; }

private:
	UPROPERTY(EditAnywhere, Category="VisiblePropertyPaths")
	TObjectPtr<const UScriptStruct> SourceClass;
};

USTRUCT()
struct FClassPropertyPaths : public FVisiblePropertyPaths
{
	GENERATED_BODY()

	void SetSourceClass(const UClass* InClass) { SourceClass = InClass; }
	const UClass* GetSourceClass() const { return SourceClass; }

	const UStruct* GetSourceStruct() const override { return SourceClass; }

private:
	UPROPERTY(EditAnywhere, Category="VisiblePropertyPaths")
	TObjectPtr<const UClass> SourceClass;
};
