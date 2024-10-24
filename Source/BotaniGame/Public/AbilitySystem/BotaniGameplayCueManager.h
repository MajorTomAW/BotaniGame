// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "BotaniGameplayCueManager.generated.h"

/**
 * UBotaniGameplayCueManager
 *
 * Game-specific gameplay cue manager for gameplay cues.
 */
UCLASS()
class UBotaniGameplayCueManager : public UGameplayCueManager
{
	GENERATED_UCLASS_BODY()

public:
	/** Static access to the Botani gameplay cue manager. */
	static UBotaniGameplayCueManager* Get();

	/** Dumps all gameplay cues to the log. */
	static void DumpGameplayCues(const TArray<FString>& Args);

	//~ Begin UGameplayCueManager Interface
	virtual void OnCreated() override;
	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override;
	virtual bool ShouldSyncLoadMissingGameplayCues() const override;
	virtual bool ShouldAsyncLoadMissingGameplayCues() const override;
	//~ End UGameplayCueManager Interface

	/** When delay loading cues, this will load the cues that must always be loaded anyway. */
	virtual void LoadAlwaysLoadedCues();

	/** Updates the bundles for a singular gameplay cue primary asset. */
	virtual void RefreshGameplayCuePrimaryAsset();

private:
	void OnGameplayTagLoaded(const FGameplayTag& Tag);
	void HandlePostGarbageCollect();
	void ProcessLoadedTags();
	void ProcessTagToPreload(const FGameplayTag& Tag, UObject* OwningObject);
	void OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject, bool bAlwaysLoadedCue);
	void RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject);
	void HandlePostLoadMap(UWorld* NewWorld);
	void UpdateDelayLoadDelegateListeners();
	bool ShouldDelayLoadGameplayCues() const;

private:
	struct FLoadedGameplayTagToProcessData
	{
		FLoadedGameplayTagToProcessData()
		{}

		FLoadedGameplayTagToProcessData(const FGameplayTag& InTag, UObject* InOwner)
			: Tag(InTag)
			, WeakOwner(InOwner)
		{}

		FLoadedGameplayTagToProcessData(const FGameplayTag& InTag, const TWeakObjectPtr<UObject>& InWeakOwner)
			: Tag(InTag)
			, WeakOwner(InWeakOwner)
		{}
		
		FGameplayTag Tag;
		TWeakObjectPtr<UObject> WeakOwner;
	};
	
	/** Cues that were preloaded on the client due to being referenced by other assets. */
	UPROPERTY(Transient)
	TSet<TObjectPtr<UClass>> PreloadedCues;
	TMap<FObjectKey, TSet<FObjectKey>> PreloadedCueReferences;

	/** Cues that were preloaded on the client and will always be loaded (code referenced or explicitly always loaded) */
	UPROPERTY(Transient)
	TSet<TObjectPtr<UClass>> AlwaysLoadedCues;

	/** Tags that are loaded and should be used for processing. */
	TArray<FLoadedGameplayTagToProcessData> LoadedGameplayTagsToProcess;
	FCriticalSection LoadedGameplayTagsToProcessCS;
	bool bProcessLoadedTagsAfterGC = false;
};
