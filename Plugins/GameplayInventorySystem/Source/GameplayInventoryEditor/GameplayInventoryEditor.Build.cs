using UnrealBuildTool;

public class GameplayInventoryEditor : ModuleRules
{
    public GameplayInventoryEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Slate",
                "SlateCore",
                "GamePlaysetsEditor",
                "DeveloperSettings",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "EngineAssetDefinitions",
                "GameplayInventorySystem",
                "UnrealEd",
                "AssetDefinition",
                "ContentBrowser",
            }
        );
    }
}