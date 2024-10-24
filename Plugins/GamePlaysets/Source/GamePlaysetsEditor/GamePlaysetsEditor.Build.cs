using UnrealBuildTool;

public class GamePlaysetsEditor : ModuleRules
{
    public GamePlaysetsEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        { 
            "Core",
            "AssetTools"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        { 
            "CoreUObject", 
            "Engine",
            "Slate",
            "SlateCore",
            "InputCore",
            "AssetDefinition",
            "DeveloperSettings",
            "GamePlaysets",
            "Projects",
            "ContentBrowser",
            "EditorFramework",
            "TypedElementFramework",
            "TypedElementRuntime",
            "EditorStyle",
            "ToolMenus",
            "ToolWidgets",
            "EditorWidgets",
            "ClassViewer",
            "WorkspaceMenuStructure",
            "ContentBrowserData"
        });
        
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] {
                "LevelEditor",
                "UnrealEd",
                "EditorFramework"
            });
        }
    }
}