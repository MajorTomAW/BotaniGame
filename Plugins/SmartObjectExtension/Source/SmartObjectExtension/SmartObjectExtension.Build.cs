// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SmartObjectExtension : ModuleRules
{
	public SmartObjectExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"AIModule",
				"SmartObjectsModule",
				"GameplayBehaviorsModule",
				"GameplayTags",
				"GameplayBehaviorSmartObjectsModule",
			});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
			});
	}
}
