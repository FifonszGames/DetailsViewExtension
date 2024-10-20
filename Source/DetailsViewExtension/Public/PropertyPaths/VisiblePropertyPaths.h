// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "VisiblePropertyPaths.generated.h"

USTRUCT()
struct FVisiblePropertyPaths
{
	GENERATED_BODY()
	
	virtual ~FVisiblePropertyPaths() = default;
	explicit FVisiblePropertyPaths() = default;

	static const FVisiblePropertyPaths Invalid;
	
	void AddPath(const FString& PropertyPath);

	bool Contains(const FPropertyAndParent& InProperty) const;

	bool HasMatchingPathExact(const FString& PropertyPath) const;
	bool HasMatchingPath(const FString& PropertyPath, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase) const;
	bool HasLeaf(const FString& PropertyPath, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase) const;

	void RemovePath(const FString& InPath) { VisiblePropertyPaths.Remove(InPath); }
	
	const TArray<FString>& GetPaths() const { return VisiblePropertyPaths; }
	bool IsEmpty() const { return VisiblePropertyPaths.IsEmpty(); }
	virtual const UStruct* GetSourceStruct() const { return nullptr;}

	bool ShowEditablePropertiesOnly() const { return bEditablePropertiesOnly; }
	
	static FName GetClassPropertyName() { return FName(TEXT("SourceClass")); }
	static FName GetPathsPropertyName() { return GET_MEMBER_NAME_CHECKED(FVisiblePropertyPaths, VisiblePropertyPaths); }
	static FName GetEditablePropertiesOnlyName() { return GET_MEMBER_NAME_CHECKED(FVisiblePropertyPaths, bEditablePropertiesOnly); }

	static TArray<FString> CreatePathsFrom(const UStruct& InSourceClass, const FString& InParentPropertyName = TEXT(""));
	static FString CreatePropertyPath(const FPropertyAndParent& InFromProperty);
	static FString GetPropertyName(const FProperty& InProperty, const bool bInAccountForDisplayMeta = true);

private:
	UPROPERTY(EditAnywhere, meta=(EditFixedOrder), Category="VisiblePropertyPaths")
	TArray<FString> VisiblePropertyPaths;
	UPROPERTY(EditAnywhere, Category="VisiblePropertyPaths")
	bool bEditablePropertiesOnly = true;
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
	const FString& GetPropertyName() const { return PropertyName; }
	FString GetTotalPath() const;

	TArray<TSharedPtr<FPropertyPathNode>> GetChildren(const FString& InFilterString = TEXT("")) const;
	bool PassesFilter(const FString& FilterString) const;

	void Initialize(const TSharedRef<IPropertyHandle>& SourceHandle, const bool bEditablePropertiesOnly);
	
	void FillWithOutmostChildren(TArray<TSharedPtr<FPropertyPathNode>>& OutItems, const bool bIncludeSelf = false);

	TSharedPtr<FPropertyPathNode> GetPropertyByPath(const FString& InPath, const bool bInCountSelf = false) const;
	bool IsEditableProperty() const { return bIsEditableProperty;}

private:
	void Initialize(const FProperty& InSourceProperty, const UStruct& InOutMostParentClass, const bool bInEditablePropertiesOnly);
	void Initialize(const FProperty& InSourceProperty, const UStruct& InOutMostParentClass, const TWeakPtr<const FPropertyPathNode>& InParent, const bool bInEditablePropertiesOnly, const bool bInIsEditable);
	
	void CreateChildren(const UStruct& OutMostParentClass, const UStruct* InFromClass, const bool bInEditablePropertiesOnly);
	void CreateChildren(const UStruct& InOutMostParentClass, const UStruct& InFromClass, const bool bInEditablePropertiesOnly);
	
	void AppendPath(FString& OutPath) const;
	TSharedPtr<FPropertyPathNode> FindChild(const FString& InPath) const;

	FString PropertyName;
	TArray<TSharedPtr<FPropertyPathNode>> Children;
	TWeakPtr<const FPropertyPathNode> Parent;
	TWeakObjectPtr<const UStruct> SourceClass;
	bool bIsEditableProperty = false;
};
