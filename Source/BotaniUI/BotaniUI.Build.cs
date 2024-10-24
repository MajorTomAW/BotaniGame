using UnrealBuildTool;

public class BotaniUI : ModuleRules
{
    public BotaniUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "InputCore",
            "EnhancedInput",
            "CommonGame",
            "CommonUI",
            "BotaniGame",
            "ModularGameplay",
            "GameplayAbilities",
            "GameplayInventorySystem",
            "AsyncMixin",
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "GameplayTags",
            "CommonInput",
        });
    }
}