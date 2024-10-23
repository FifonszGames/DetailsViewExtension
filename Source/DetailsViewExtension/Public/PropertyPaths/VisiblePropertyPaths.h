// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "VisiblePropertyPaths.generated.h"

USTRUCT()
struct FVisibleCustomProperties
{
	GENERATED_BODY()
	
	bool Contains(const FName& PropertyName, const FName& PropertyCategory) const;

private:
	UPROPERTY(EditAnywhere)
	TArray<FString> Properties;
	UPROPERTY(EditAnywhere)
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
	virtual const UStruct* GetSourceStruct() const { return nullptr;}

	bool ShowEditablePropertiesOnly() const { return bEditablePropertiesOnly; }
	
	static FName GetClassPropertyName() { return FName(TEXT("SourceClass")); }
	static FName GetPathsPropertyName() { return GET_MEMBER_NAME_CHECKED(FVisiblePropertyPaths, VisiblePropertyPaths); }
	static FName GetEditablePropertiesOnlyName() { return GET_MEMBER_NAME_CHECKED(FVisiblePropertyPaths, bEditablePropertiesOnly); }
	static FName GetVisibleCustomRowsName() { return GET_MEMBER_NAME_CHECKED(FVisiblePropertyPaths, VisibleCustomProperties); }

private:
	static FString CreatePropertyPath(const FPropertyAndParent& InFromProperty);
	static FString GetPropertyName(const FProperty& InProperty, const bool bInAccountForDisplayMeta = true);
	
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
	
	void SetSourceClass(const UScriptStruct* InClass) { SourceClass = InClass;}
	const UScriptStruct* GetSourceClass() const { return SourceClass; }
	
	virtual const UStruct* GetSourceStruct() const override { return SourceClass;}

private:
	UPROPERTY(EditAnywhere, Category="VisiblePropertyPaths")
	TObjectPtr<const UScriptStruct> SourceClass;
};

USTRUCT()
struct FClassPropertyPaths : public FVisiblePropertyPaths
{
	GENERATED_BODY()
	
	void SetSourceClass(const UClass* InClass) { SourceClass = InClass;}
	const UClass* GetSourceClass() const { return SourceClass; }
	
	virtual const UStruct* GetSourceStruct() const override { return SourceClass;}
	
private:
	UPROPERTY(EditAnywhere, Category="VisiblePropertyPaths")
	TObjectPtr<const UClass> SourceClass;
};

struct FPropertyPathNode : TSharedFromThis<FPropertyPathNode>
{
	void Initialize(const TSharedRef<IPropertyHandle>& SourceHandle, const bool bEditablePropertiesOnly);
	
	TArray<TSharedPtr<FPropertyPathNode>> GetChildren(const FString& InFilterString = TEXT("")) const;
	void FillWithOutmostChildren(TArray<TSharedPtr<FPropertyPathNode>>& OutItems, const bool bIncludeSelf = false);
	
	TSharedPtr<FPropertyPathNode> GetPropertyByPath(const FString& InPath, const bool bInCountSelf = false) const;
	FString GetTotalPath() const;
	const FString& GetPropertyName() const { return PropertyName; }

	bool PassesFilter(const FString& FilterString) const;
	bool IsEditableProperty() const { return bIsEditableProperty;}

private:
	void Initialize(const FProperty& InSourceProperty, const UStruct& InOutMostParentClass, const TWeakPtr<const FPropertyPathNode>& InParent,
		const bool bInEditablePropertiesOnly, const bool bInIsEditable);
	
	void CreateChildren(const UStruct& InOutMostParentClass, const UStruct& InFromClass, const bool bInEditablePropertiesOnly);
	
	void AppendPath(FString& OutPath) const;
	TSharedPtr<FPropertyPathNode> FindChild(const FString& InPath) const;

	FString PropertyName;
	TArray<TSharedPtr<FPropertyPathNode>> Children;
	TWeakPtr<const FPropertyPathNode> Parent;
	TWeakObjectPtr<const UStruct> SourceClass;
	bool bIsEditableProperty = false;
};
