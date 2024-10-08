// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class DetailsViewExtension : ModuleRules
{
	public DetailsViewExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(
			new [] { "Core", }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new []
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UMG",
				"GameplayTagsEditor",
				"InputCore",
				"UnrealEd",
				"GameplayTags",
				"PropertyEditor"
			});
        PrivateIncludePaths.Add(Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Editor/PropertyEditor");
	}
}
