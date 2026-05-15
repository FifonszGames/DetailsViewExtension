// Copyright FifonszGames All Rights Reserved.

#pragma once

struct FPropertyPathNode;
struct FVisiblePropertyPaths;

namespace PropertyPathHelpers
{
	void RemovePath(const TSharedPtr<IPropertyHandle>& PropertyHandle, const FString& InPath);
	void RemovePaths(const TSharedPtr<IPropertyHandle>& PropertyHandle, const TArray<FString>& InPaths);
	void RemovePath(const TSharedRef<IPropertyHandleArray>& ArrayHandle, const FVisiblePropertyPaths& PathsArray, const FString& InPath);

	void AddPaths(const TSharedPtr<IPropertyHandle>& PropertyHandle, const TArray<FString>& InPaths);
	void AddPath(const TSharedPtr<IPropertyHandle>& PropertyHandle, const FString& InPath);
	void AddPath(const TSharedRef<IPropertyHandleArray>& PropertyHandle, const FString& InPath);

	const FVisiblePropertyPaths* GetPathsFromHandle(const TSharedPtr<IPropertyHandle>& InPropertyHandle);

	void RemoveAllPaths(const TSharedPtr<IPropertyHandle>& PropertyHandle);
	void AddAllChildren(const TSharedPtr<IPropertyHandle>& PropertyHandle, const TSharedPtr<FPropertyPathNode>& ParentNode);

	inline FString Separator() { return TEXT("."); }
	inline char SeparatorAsChar() { return Separator()[0]; }
}
