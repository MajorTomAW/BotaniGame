// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniAssetManagerStartupJob.h"
#include "Engine/AssetManager.h"
#include "GameFeatures/Data/BotaniGameData.h"
#include "GameFeatures/Data/BotaniPawnData.h"
#include "BotaniAssetManager.generated.h"

struct FBotaniBundles
{
	static const FName Equipped;
};

/**
 * UBotaniAssetManager
 *
 *	Game implementation of the asset manager that overrides functionality and stores game-specific types.
 *	It is expected that most games will want to override AssetManager as it provides a good place for game-specific loading logic.
 *	This class is used by setting 'AssetManagerClassName' in DefaultEngine.ini.
 */
UCLASS(Config = Game)
class BOTANIGAME_API UBotaniAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	UBotaniAssetManager();

	/** Returns the game data asset */
	UFUNCTION(BlueprintCallable, Category = "Botani|AssetManager", meta = (DisplayName = "Get Game Data"))
	static const UBotaniGameData* K2_GetGameData() { return &Get().GetGameData(); }

	/** Static getter for the asset manager singleton */
	static UBotaniAssetManager& Get();

	const UBotaniGameData& GetGameData();
	const UBotaniPawnData* GetDefaultPawnData() const;

	/** Logs all assets currently loaded and tracked by the asset manager. */
	static void DumpLoadedAssets();
	
	template <typename GameDataClass>
	const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath);

	/** Returns the asset referenced by a TSoftObjectPtr. This will synchronously load the asset if it is not already loaded. */
	template <typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPtr, bool bKeepInMemory = true);

	/** Returns the subclass referenced by a TSoftClassPtr. This will synchronously load the asset if it is not already loaded. */
	template <typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType);
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);
	static bool ShouldLogAssetLoads();

	// Thread safe way of adding a loaded asset to keep in memory.
	void AddLoadedAsset(const UObject* Asset);

	//~UAssetManager interface
	virtual void StartInitialLoading() override;
#if WITH_EDITOR
	virtual void PreBeginPIE(bool bStartSimulate) override;
#endif
	//~End of UAssetManager interface

protected:
	// Global game data asset to use
	UPROPERTY(Config)
	TSoftObjectPtr<UBotaniGameData> BotaniGameDataPath;

	// Map of loaded editions of game data assets
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;

	/** Pawn data used when spawning a player pawn if there isn't one set in the player state */
	UPROPERTY(Config)
	TSoftObjectPtr<UBotaniPawnData> DefaultPawnData;

private:
	// Assets loaded and tracked by the asset manager.
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Used for a scope lock when modifying the list of load assets.
	FCriticalSection LoadedAssetsCritical;

	/** Flushes the StartupJobs array. Processes all startup work. */
	void DoAllStartupJobs();

	/** Sets up the ability system. */
	void InitializeGameplayCueManager();

	/* Called periodically during loads, could be used to feed the status to a loading screen */
	void UpdateInitialGameContentLoadPercent(float GameContentPercent);

	/** List of tasks to execute on startup */
	TArray<FBotaniAssetManagerStartupJob> StartupJobs;
	
};


template <typename GameDataClass>
const GameDataClass& UBotaniAssetManager::GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
{
	if (TObjectPtr<UPrimaryDataAsset> const * pResult = GameDataMap.Find(GameDataClass::StaticClass()))
	{
		return *CastChecked<GameDataClass>(*pResult);
	}

	// If it is not loaded, we must load it, use a blocking load for now
	return *CastChecked<const GameDataClass>(LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
}

template <typename AssetType>
AssetType* UBotaniAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPtr, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPtr.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPtr.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset %s"), *AssetPath.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template <typename AssetType>
TSubclassOf<AssetType> UBotaniAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class %s"), *AssetPath.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}
