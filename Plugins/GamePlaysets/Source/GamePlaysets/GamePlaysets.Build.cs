// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GamePlaysets : ModuleRules
{
	public GamePlaysets(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"Slate",
			"SlateCore",
			"GameplayTags",
		});
			
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
		});
	}
}
