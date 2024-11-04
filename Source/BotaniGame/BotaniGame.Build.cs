// Copyright © 2024 Botanibots Team. All rights reserved.

using UnrealBuildTool;

public class BotaniGame : ModuleRules
{
    public BotaniGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");
        PublicDefinitions.Add("USE_PEER_TO_PEER=1");
        
        PublicIncludePaths.AddRange(new []
        {
            "BotaniGame"
        });

        // System
        PublicDependencyModuleNames.AddRange(new[] 
        {
            "Core",
            "UMG",
            "Iris",
            "IrisCore",
            "PhysicsCore",
            "CommonGame",
            "CommonUI",
            "CommonUser",
            "GameFeatures",
            "ModularGameplay",
            "EnhancedInput",
            "ModularGameplayActors",
            "GameplayMessageRuntime",
            "GameplayInventorySystem",
            "SignificanceManager",
            "CommonLoadingScreen",
            "UIExtension",
            "GameplayTagStacks",
            "GamePlaysets",
            "AIModule",
            "AIUtilities",
            "Niagara",
        });
        
        // Gameplay
        PublicDependencyModuleNames.AddRange(new[]
        {
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks"
        });

        // Engine
        PrivateDependencyModuleNames.AddRange(new[]
        {
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "UMG",
            "DeveloperSettings",
            "EngineSettings",
            "NetCore",
            "Projects",
        });
        
        // Circular dependencies
        PrivateIncludePathModuleNames.AddRange(new[]
        {
            "BotaniCheats",
        });
        
        SetupGameplayDebuggerSupport(Target);
    }
}