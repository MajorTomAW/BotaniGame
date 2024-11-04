// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AIUtilities : ModuleRules
{
	public AIUtilities(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"AIModule",
			"GameplayTags",
			"NavigationSystem",
			"GameplayAbilities"
		});
			
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("Settings");
		}
	}
}
