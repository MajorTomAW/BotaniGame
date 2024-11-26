// Copyright © 2024 Game Shifters Studio. All rights reserved.

using UnrealBuildTool;


public class GizmoHelperTool : ModuleRules
{
	public GizmoHelperTool(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		
		PublicDependencyModuleNames.AddRange(new string[]
		{ 
			"Core",
			"HTTP",
			"Engine",
			"DeveloperSettings",
			"EditorSubsystem",
			"EditorFramework",
			"InputCore",
		});
		
		PrivateDependencyModuleNames.AddRange(new string[] 
		{
			"CoreUObject",
			"Slate",
			"SlateCore",
			"Json",
			"JsonUtilities",
			"Projects",
			"UnrealEd",
			"Blutility",
		});
		
		PublicDefinitions.Add("IS_GIZMO_DEBUG=0");
	}
}


public class GizmoEditorTarget : TargetRules
{
	public GizmoEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
	}
}
