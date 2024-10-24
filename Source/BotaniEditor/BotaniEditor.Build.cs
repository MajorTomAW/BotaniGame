using UnrealBuildTool;

public class BotaniEditor : ModuleRules
{
    public BotaniEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "AssetTools",
                "BotaniGame",
                "UnrealEd",
                "ContentBrowser",
                "DeveloperSettings",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UMGEditor",
                "UMG",
                "Kismet",
                "PlacementMode",
                "GameplayAbilities",
                "ToolMenus",
                "ToolWidgets",
                "AssetDefinition",
                "GamePlaysets",
                "BotaniUI",
                "PropertyEditor",
                "EditorStyle",
                "BotaniAI",
                "EngineAssetDefinitions",
                "ToolWidgets",
                "AIGraph",
                "BehaviorTreeEditor"
            }
        );
    }
}