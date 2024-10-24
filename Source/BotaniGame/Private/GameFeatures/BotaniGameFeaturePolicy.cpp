// Copyright © 2024 Botanibots Team. All rights reserved.


#include "GameFeatures/BotaniGameFeaturePolicy.h"

#include "AbilitySystemGlobals.h"
#include "GameFeatureAction.h"
#include "GameFeatureData.h"
#include "GameplayCueSet.h"
#include "AbilitySystem/BotaniGameplayCueManager.h"
#include "GameFeatures/Actions/GameFeatureAction_AddGameplayCuePath.h"


UBotaniGameFeaturePolicy::UBotaniGameFeaturePolicy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UBotaniGameFeaturePolicy& UBotaniGameFeaturePolicy::Get()
{
	return UGameFeaturesSubsystem::Get().GetPolicy<UBotaniGameFeaturePolicy>();
}

void UBotaniGameFeaturePolicy::InitGameFeatureManager()
{
	//Observers.Add(NewObject<UBotaniGameFeature_HotfixManager>());
	Observers.Add(NewObject<UBotaniGameFeature_AddGameplayCuePaths>());

	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.AddObserver(Observer);
	}

	Super::InitGameFeatureManager();
}

void UBotaniGameFeaturePolicy::ShutdownGameFeatureManager()
{
	Super::ShutdownGameFeatureManager();

	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.RemoveObserver(Observer);
	}
	Observers.Empty();
}

TArray<FPrimaryAssetId> UBotaniGameFeaturePolicy::GetPreloadAssetListForGameFeature(const UGameFeatureData* GameFeatureToLoad, bool bIncludeLoadedAssets) const
{
	return Super::GetPreloadAssetListForGameFeature(GameFeatureToLoad, bIncludeLoadedAssets);
}

const TArray<FName> UBotaniGameFeaturePolicy::GetPreloadBundleStateForGameFeature() const
{
	return Super::GetPreloadBundleStateForGameFeature();
}

void UBotaniGameFeaturePolicy::GetGameFeatureLoadingMode(bool& bLoadClientData, bool& bLoadServerData) const
{
	// Editor will load both, this can cause hitching as the bundles are set to not preload in editor
	bLoadClientData = !IsRunningDedicatedServer();
	bLoadServerData = !IsRunningClientOnly();
}

bool UBotaniGameFeaturePolicy::IsPluginAllowed(const FString& PluginURL) const
{
	return Super::IsPluginAllowed(PluginURL);
}


//////////////////////////////////////////////////////////////////////////
/// UBotaniGameFeature_AddGameplayCuePaths


void UBotaniGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering(
	const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UBotaniGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering);

	const FString PluginRootPath = TEXT("/") + PluginName;
	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
	{
		if (const UGameFeatureAction_AddGameplayCuePath* CueFeatureAction = Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
		{
			const TArray<FDirectoryPath>& Paths = CueFeatureAction->GetDirectoryPathsToAdd();

			if (UBotaniGameplayCueManager* GCM = UBotaniGameplayCueManager::Get())
			{
				UGameplayCueSet* RuntimeGCSet = GCM->GetRuntimeCueSet();
				const int32 PreInitializeNumCues = RuntimeGCSet ? RuntimeGCSet->GameplayCueData.Num() : 0;

				// Add the paths to the cue manager
				for (const auto& Dir : Paths)
				{
					FString MutablePath = Dir.Path;
					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
					GCM->AddGameplayCueNotifyPath(MutablePath, false);
				}

				// Rebuild the runtime library with the new paths
				if (!Paths.IsEmpty())
				{
					GCM->InitializeRuntimeObjectLibrary();
				}

				const int32 PostInitializeNumCues = RuntimeGCSet ? RuntimeGCSet->GameplayCueData.Num() : 0;
				if (PreInitializeNumCues != PostInitializeNumCues)
				{
					GCM->RefreshGameplayCuePrimaryAsset();
				}
			}
		}
	}
}

void UBotaniGameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering(
	const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
{
	const FString PluginRootPath = TEXT("/") + PluginName;
	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
	{
		if (const UGameFeatureAction_AddGameplayCuePath* CueFeatureAction = Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
		{
			const TArray<FDirectoryPath>& Paths = CueFeatureAction->GetDirectoryPathsToAdd();

			if (UGameplayCueManager* GCM = UAbilitySystemGlobals::Get().GetGameplayCueManager())
			{
				int32 NumRemoved = 0;
				for (const auto& Dir : Paths)
				{
					FString MutablePath = Dir.Path;
					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
					NumRemoved += GCM->RemoveGameplayCueNotifyPath(MutablePath, false);
				}

				ensure(NumRemoved == Paths.Num());

				// Rebuild the runtime library only if there is a need to
				if (NumRemoved > 0)
				{
					GCM->InitializeRuntimeObjectLibrary();
				}
			}
		}
	}
}