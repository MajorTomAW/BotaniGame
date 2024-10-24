// Copyright © 2024 MajorT. All rights reserved.

using UnrealBuildTool;

public class CommonMaps : ModuleRules
{
	public CommonMaps(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"DeveloperSettings",
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"UnrealEd",
			"AssetTools",
			"ToolMenus",
			"ContentBrowser"
		});
	}
}
