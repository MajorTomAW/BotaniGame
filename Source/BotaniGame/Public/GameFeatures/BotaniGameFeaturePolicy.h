// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFeaturesProjectPolicies.h"
#include "GameFeatureStateChangeObserver.h"

#include "BotaniGameFeaturePolicy.generated.h"

/**
 * UBotaniGameFeaturePolicy
 *
 * Manager to keep track of the state machines that bring a game feature plugin into memory and active
 * This class discovers plugins either that are built-in and distributed with the game,
 * or are reported externally (that is, in other words, by a web service or other endpoint)
 */
UCLASS(MinimalAPI, Config = Game)
class UBotaniGameFeaturePolicy : public UDefaultGameFeaturesProjectPolicies
{
	GENERATED_UCLASS_BODY()

public:
	BOTANIGAME_API static UBotaniGameFeaturePolicy& Get();
	
	//~UGameFeaturesProjectPolicies interface
	virtual void InitGameFeatureManager() override;
	virtual void ShutdownGameFeatureManager() override;
	virtual TArray<FPrimaryAssetId> GetPreloadAssetListForGameFeature(const UGameFeatureData* GameFeatureToLoad, bool bIncludeLoadedAssets = false) const override;
	virtual bool IsPluginAllowed(const FString& PluginURL) const override;
	virtual const TArray<FName> GetPreloadBundleStateForGameFeature() const override;
	virtual void GetGameFeatureLoadingMode(bool& bLoadClientData, bool& bLoadServerData) const override;
	//~End of UGameFeaturesProjectPolicies interface

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> Observers;
};


/**
 * UBotaniGameFeature_AddGameplayCuePaths
 *
 * Feature to add gameplay cue paths to the gameplay cue manager
 */
UCLASS()
class UBotaniGameFeature_AddGameplayCuePaths : public UObject, public IGameFeatureStateChangeObserver
{
	GENERATED_BODY()

public:
	//~ Begin IGameFeatureStateChangeObserver Interface
	virtual void OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
	virtual void OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
	//~ End IGameFeatureStateChangeObserver Interface
};
