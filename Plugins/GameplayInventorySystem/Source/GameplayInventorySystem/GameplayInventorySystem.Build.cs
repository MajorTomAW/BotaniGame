// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameplayInventorySystem : ModuleRules
{
	public GameplayInventorySystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"GamePlaysets",
			"GameplayTags",
			"GameplayAbilities",
			"GameplayTagStacks",
			"GameplayMessageRuntime",
			"Iris",
			"Niagara",
			"GameplayDebugger"
		});
			
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"NetCore",
			"CoreUObject",
			"Engine",
		});
		
		SetupIrisSupport(Target);
	}
}
