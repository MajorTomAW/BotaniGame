using UnrealBuildTool;

public class BotaniCheats : ModuleRules
{
    public BotaniCheats(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "EngineSettings",
                "GameplayInventorySystem",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "BotaniGame",
            }
        );
        
        PrivateIncludePathModuleNames.AddRange(
            new string[]
            {
                "BotaniCheats",
            }
        );
    }
}