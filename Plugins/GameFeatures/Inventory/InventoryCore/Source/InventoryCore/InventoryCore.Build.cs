using UnrealBuildTool;

public class InventoryCore : ModuleRules
{
    public InventoryCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "BotaniGame",
                "BotaniUI",
                "CommonUI",
                "GameplayInventorySystem",
                "GameplayTags",
                "UMG",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "InputCore",
            }
        );
    }
}