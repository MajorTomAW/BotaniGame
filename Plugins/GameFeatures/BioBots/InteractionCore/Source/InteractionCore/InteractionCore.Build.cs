using UnrealBuildTool;

public class InteractionCore : ModuleRules
{
    public InteractionCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new []
		{
 			"Core",
            "UMG",
            "GameplayInventorySystem",
            "GameplayAbilities",
            "GameplayTagStacks",
            "GameplayTasks",
            "GameplayTags",
		});
        
        PublicDependencyModuleNames.AddRange(new []
		{
			nameof(BotaniGame),
		});

        PrivateDependencyModuleNames.AddRange(new []
        {
            "CoreUObject",
            "Engine",
        });
        
        PrivateDependencyModuleNames.AddRange(new []
        {
            nameof(BotaniUI),
        });
    }
}