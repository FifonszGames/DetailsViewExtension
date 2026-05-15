// Copyright FifonszGames All Rights Reserved.

using UnrealBuildTool;

public class DetailsViewExtension : ModuleRules
{
	public DetailsViewExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new []{
			"Core"
		});
		
		PrivateDependencyModuleNames.AddRange(new []{
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"UMG",
			"InputCore",
			"UnrealEd",
			"PropertyEditor",
			"ClassViewer",
			"StructViewer"
		});
	}
}
