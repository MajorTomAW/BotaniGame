using UnrealBuildTool;

public class BotaniAI : ModuleRules
{
    public BotaniAI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "BotaniGame",
                "ModularGameplay",
                "ModularGameplayActors",
                "GameplayTags",
                "AIModule",
                "GameplayInteractionsModule",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "NavigationSystem"
            }
        );
    }
}