// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ThumbnailExtractor : ModuleRules
{
	public ThumbnailExtractor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"ToolMenusEditor",
				"ContentBrowser",
				"UnrealEd",
				"ImageWrapper",
				"GamePlaysets",
				"EditorScriptingUtilities",
			}
			);
	}
}
