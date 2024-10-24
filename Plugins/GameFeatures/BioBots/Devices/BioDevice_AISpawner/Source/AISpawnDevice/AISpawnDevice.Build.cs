using UnrealBuildTool;

public class AISpawnDevice : ModuleRules
{
    public AISpawnDevice(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "BotaniGame",
                "BioDevices",
                "BotaniAI",
                "GameplayTags",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "NavigationSystem",
            }
        );
    }
}