// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/BotaniGameplayCueManager.h"

#include "AbilitySystemGlobals.h"
#include "BotaniLogChannels.h"
#include "GameplayCueSet.h"
#include "GameplayTagsManager.h"
#include "Engine/AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniGameplayCueManager)

enum class EBotaniEditorLoadMode
{
	/**
	 * Loads all cues upfront.
	 * Longer loading time in the editor but short PIE times and effects never fail to play.
	 */ 
	LoadUpFront,

	/**
	 * Outside the editor: Async loads as cue tag are registered.
	 * In editor: Async loads when cues are invoked.
	 *
	 * @note This can cause some "why didn't I see the effect for X" issues in PIE and is good for iteration speed but otherwise bad for designers.
	 */
	PreloadAsCuesAreReferenced_GameOnly,

	/**
	 * Async loads as cue tags are registered
	 */
	PreloadAsCuesAreReferenced,
};


namespace BotaniGameplayCueManagerCVars
{
	static FAutoConsoleCommand CVarDumpGameplayCues
		(
			TEXT("Botani.DumpGameplayCues"),
			TEXT("Shows all assets that were loaded via the BotaniGameplayCueManager and are currently in memory."),
			FConsoleCommandWithArgsDelegate::CreateStatic(UBotaniGameplayCueManager::DumpGameplayCues)
		);

	static EBotaniEditorLoadMode LoadMode = EBotaniEditorLoadMode::LoadUpFront;
}

constexpr bool bPreloadEvenInEditor = true;

struct FGameplayCueTagThreadSynchronizeGraphTask : public FAsyncGraphTaskBase
{
	TFunction<void()> TheTask;
	
	FGameplayCueTagThreadSynchronizeGraphTask(TFunction<void()>&& Task)
		: TheTask(MoveTemp(Task))
	{}

	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	{
		TheTask();
	}
	ENamedThreads::Type GetDesiredThread()
	{
		return ENamedThreads::GameThread;
	}
};

UBotaniGameplayCueManager::UBotaniGameplayCueManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UBotaniGameplayCueManager* UBotaniGameplayCueManager::Get()
{
	return Cast<UBotaniGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}

void UBotaniGameplayCueManager::DumpGameplayCues(const TArray<FString>& Args)
{
	UBotaniGameplayCueManager* GCM = Get();
	if (!GCM)
	{
		BOTANI_GAS_LOG(Error, TEXT("[%hs] failed. No BotaniGameplayCueManager found."), __FUNCTION__);
		return;
	}

	const bool bIncludeRefs = Args.Contains(TEXT("Refs"));

	BOTANI_GAS_LOG(Log, TEXT("=========== Dumping Always Loaded Gameplay Cue Notifies ==========="));
	for (UClass* CueClass : GCM->AlwaysLoadedCues)
	{
		BOTANI_GAS_LOG(Log, TEXT("	%s"), *GetPathNameSafe(CueClass));
	}

	BOTANI_GAS_LOG(Log, TEXT("=========== Dumping Preloaded Gameplay Cue Notifies ==========="));
	for (UClass* CueClass : GCM->PreloadedCues)
	{
		TSet<FObjectKey>* ReferencerSet = GCM->PreloadedCueReferences.Find(CueClass);
		int32 NumRefs = ReferencerSet ? ReferencerSet->Num() : 0;

		BOTANI_GAS_LOG(Log, TEXT("	%s (%d refs)"), *GetPathNameSafe(CueClass), NumRefs);

		if (bIncludeRefs && ReferencerSet)
		{
			for (const FObjectKey& Ref : *ReferencerSet)
			{
				UObject* RefObj = Ref.ResolveObjectPtr();
				BOTANI_GAS_LOG(Log, TEXT("	^-	%s"), *GetPathNameSafe(RefObj));
			}
		}
	}

	BOTANI_GAS_LOG(Log, TEXT("=========== Dumping Gameplay Cue Notifies loaded on demand ==========="));
	int32 NumMissingCuesLoaded = 0;
	if (GCM->RuntimeGameplayCueObjectLibrary.CueSet)
	{
		for (const auto& CueData : GCM->RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData)
		{
			if (CueData.LoadedGameplayCueClass &&
				!GCM->AlwaysLoadedCues.Contains(CueData.LoadedGameplayCueClass) &&
				!GCM->PreloadedCues.Contains(CueData.LoadedGameplayCueClass))
			{
				NumMissingCuesLoaded++;
				BOTANI_GAS_LOG(Log, TEXT("	%s"), *GetPathNameSafe(CueData.LoadedGameplayCueClass));
			}
		}
	}

	BOTANI_GAS_LOG(Log, TEXT("=========== Summary ==========="));
	BOTANI_GAS_LOG(Log, TEXT("	...%d cues in always loaded list."), GCM->AlwaysLoadedCues.Num());
	BOTANI_GAS_LOG(Log, TEXT("	...%d cues in preloaded list."), GCM->PreloadedCues.Num());
	BOTANI_GAS_LOG(Log, TEXT("	...%d cues loaded on demand."), NumMissingCuesLoaded);
	BOTANI_GAS_LOG(Log, TEXT("	...%d cues in total."), GCM->AlwaysLoadedCues.Num() + GCM->PreloadedCues.Num() + NumMissingCuesLoaded);
}

void UBotaniGameplayCueManager::OnCreated()
{
	Super::OnCreated();

	UpdateDelayLoadDelegateListeners();
}

bool UBotaniGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries() const
{
	switch (BotaniGameplayCueManagerCVars::LoadMode) {
	case EBotaniEditorLoadMode::LoadUpFront:
		{
			return true;
		}
	case EBotaniEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
		{
#if WITH_EDITOR
			if (GIsEditor)
			{
				return false;
			}
#endif
			break;
		}
	case EBotaniEditorLoadMode::PreloadAsCuesAreReferenced:
		{
			break;
		}
	}

	return !ShouldDelayLoadGameplayCues();
}

bool UBotaniGameplayCueManager::ShouldSyncLoadMissingGameplayCues() const
{
	return false;
}

bool UBotaniGameplayCueManager::ShouldAsyncLoadMissingGameplayCues() const
{
	return true;
}

void UBotaniGameplayCueManager::LoadAlwaysLoadedCues()
{
	if (ShouldDelayLoadGameplayCues())
	{
		UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
		TArray<FName> AdditionalAlwaysLoadedCueTags; //@TODO: Filter for "GameplayCue" tag?

		for (const FName& CueTagName : AdditionalAlwaysLoadedCueTags)
		{
			FGameplayTag CueTag = TagsManager.RequestGameplayTag(CueTagName, false);
			if (CueTag.IsValid())
			{
				ProcessTagToPreload(CueTag, nullptr);
			}
			else
			{
				BOTANI_GAS_LOG(Warning, TEXT("[%hs] Could not find tag %s."), __FUNCTION__, *CueTagName.ToString());
			}
		}
	}
}

void UBotaniGameplayCueManager::OnGameplayTagLoaded(const FGameplayTag& Tag)
{
	FScopeLock ScopeLock(&LoadedGameplayTagsToProcessCS);
	const bool bStartTask = LoadedGameplayTagsToProcess.Num() == 0;

	FUObjectSerializeContext* LoadContext = FUObjectThreadContext::Get().GetSerializeContext();
	UObject* OwningObj = LoadContext ? LoadContext->SerializedObject : nullptr;
	LoadedGameplayTagsToProcess.Emplace(Tag, OwningObj);

	if (bStartTask)
	{
		TGraphTask<FGameplayCueTagThreadSynchronizeGraphTask>::CreateTask().ConstructAndDispatchWhenReady([]()
		{
			if (GIsRunning)
			{
				if (UBotaniGameplayCueManager* StrongThis = Get())
				{
					// If we're garbage collection, we can't call StaticFindObject (...), so we will just wait until the GC is over.
					if (IsGarbageCollecting())
					{
						StrongThis->bProcessLoadedTagsAfterGC = true;
					}
					else
					{
						StrongThis->ProcessLoadedTags();
					}
				}
			}
		});
	}
}

void UBotaniGameplayCueManager::HandlePostGarbageCollect()
{
	if (bProcessLoadedTagsAfterGC)
	{
		ProcessLoadedTags();
	}

	bProcessLoadedTagsAfterGC = false;
}

void UBotaniGameplayCueManager::ProcessLoadedTags()
{
	TArray<FLoadedGameplayTagToProcessData> TaskLoadedGameplayTagsToProcess;
	{
		FScopeLock TaskScopeLock(&LoadedGameplayTagsToProcessCS);
		TaskLoadedGameplayTagsToProcess = LoadedGameplayTagsToProcess;
		LoadedGameplayTagsToProcess.Empty();
	}

	// This might return during shutdown, and we don't want to process tags then.
	if (GIsRunning)
	{
		if (RuntimeGameplayCueObjectLibrary.CueSet)
		{
			for (const FLoadedGameplayTagToProcessData& LoadedTagData : TaskLoadedGameplayTagsToProcess)
			{
				if (RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueDataMap.Contains(LoadedTagData.Tag))
				{
					if (!LoadedTagData.WeakOwner.IsStale())
					{
						ProcessTagToPreload(LoadedTagData.Tag, LoadedTagData.WeakOwner.Get());
					}
				}
			}
		}
		else
		{
			BOTANI_GAS_LOG(Warning, TEXT("[%hs] process loaded tag(s) but RuntimeGameplayCueObjectLibrary.CueSet was null. Skipping processing..."), __FUNCTION__);
		}
	}
}

void UBotaniGameplayCueManager::ProcessTagToPreload(const FGameplayTag& Tag, UObject* OwningObject)
{
	switch (BotaniGameplayCueManagerCVars::LoadMode) {
	case EBotaniEditorLoadMode::LoadUpFront:
		{
			return;
		}
	case EBotaniEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
		{
#if WITH_EDITOR
			if (GIsEditor)
			{
				return;
			}
#endif
			break;
		}
	case EBotaniEditorLoadMode::PreloadAsCuesAreReferenced:
		break;
	}

	check(RuntimeGameplayCueObjectLibrary.CueSet);

	int32* DataIdx = RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueDataMap.Find(Tag);
	if (DataIdx && RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData.IsValidIndex(*DataIdx))
	{
		const FGameplayCueNotifyData& CueData = RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData[*DataIdx];

		UClass* LoadedGameplayCueClass = FindObject<UClass>(nullptr, *CueData.GameplayCueNotifyObj.ToString());
		if (LoadedGameplayCueClass)
		{
			RegisterPreloadedCue(LoadedGameplayCueClass, OwningObject);
		}
		else
		{
			bool bAlwaysLoadedCue = OwningObject == nullptr;
			TWeakObjectPtr<UObject> WeakOwner = OwningObject;
			StreamableManager.RequestAsyncLoad(CueData.GameplayCueNotifyObj, FStreamableDelegate::CreateUObject(this, &ThisClass::OnPreloadCueComplete, CueData.GameplayCueNotifyObj, WeakOwner, bAlwaysLoadedCue), FStreamableManager::DefaultAsyncLoadPriority, false, false, TEXT("GameplayCueManager"));
		}
	}
}

void UBotaniGameplayCueManager::OnPreloadCueComplete(
	FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject, bool bAlwaysLoadedCue)
{
	if (bAlwaysLoadedCue || OwningObject.IsValid())
	{
		if (UClass* LoadedCueClass = Cast<UClass>(Path.ResolveObject()))
		{
			RegisterPreloadedCue(LoadedCueClass, OwningObject.Get());
		}
	}
}

void UBotaniGameplayCueManager::RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject)
{
	check(LoadedGameplayCueClass);

	const bool bAlwaysLoadedCue = OwningObject == nullptr;

	if (bAlwaysLoadedCue)
	{
		AlwaysLoadedCues.Add(LoadedGameplayCueClass);
		PreloadedCues.Remove(LoadedGameplayCueClass);
		PreloadedCueReferences.Remove(LoadedGameplayCueClass);
	}
	else if ((OwningObject != LoadedGameplayCueClass) &&
		(OwningObject != LoadedGameplayCueClass->GetDefaultObject()) &&
		!AlwaysLoadedCues.Contains(LoadedGameplayCueClass))
	{
		PreloadedCues.Add(LoadedGameplayCueClass);
		TSet<FObjectKey>& ReferencerSet = PreloadedCueReferences.FindOrAdd(LoadedGameplayCueClass);
		ReferencerSet.Add(OwningObject);
	}
}

void UBotaniGameplayCueManager::HandlePostLoadMap(UWorld* NewWorld)
{
	if (RuntimeGameplayCueObjectLibrary.CueSet)
	{
		for (UClass* CueClass : AlwaysLoadedCues)
		{
			RuntimeGameplayCueObjectLibrary.CueSet->RemoveLoadedClass(CueClass);
		}

		for (UClass* CueClass : PreloadedCues)
		{
			RuntimeGameplayCueObjectLibrary.CueSet->RemoveLoadedClass(CueClass);
		}
	}

	for (auto CueIt = PreloadedCues.CreateIterator(); CueIt; ++CueIt)
	{
		TSet<FObjectKey>& ReferencerSet = PreloadedCueReferences.FindChecked(*CueIt);
		for (auto RefIt = ReferencerSet.CreateIterator(); RefIt; ++RefIt)
		{
			if (!RefIt->ResolveObjectPtr())
			{
				RefIt.RemoveCurrent();
			}
		}
		if (ReferencerSet.Num() == 0)
		{
			PreloadedCueReferences.Remove(*CueIt);
			CueIt.RemoveCurrent();
		}
	}
}

void UBotaniGameplayCueManager::UpdateDelayLoadDelegateListeners()
{
	UGameplayTagsManager::Get().OnGameplayTagLoadedDelegate.RemoveAll(this);
	FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	switch (BotaniGameplayCueManagerCVars::LoadMode) {
	case EBotaniEditorLoadMode::LoadUpFront:
		{
			return;
		}
	case EBotaniEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
		{
#if WITH_EDITOR
			if (GIsEditor)
			{
				return;
			}
#endif

			break;
		}
	case EBotaniEditorLoadMode::PreloadAsCuesAreReferenced:
		break;
	}

	UGameplayTagsManager::Get().OnGameplayTagLoadedDelegate.AddUObject(this, &ThisClass::OnGameplayTagLoaded);
	FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(this, &ThisClass::HandlePostGarbageCollect);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);
}

bool UBotaniGameplayCueManager::ShouldDelayLoadGameplayCues() const
{
	constexpr bool bClientDelayLoadedGameplayCues = true;
	return !IsRunningDedicatedServer() && bClientDelayLoadedGameplayCues;
}


const FPrimaryAssetType UBotaniAssetManager_GameplayCueRefsType = TEXT("GameplayCueRefs");
const FName UBotaniAssetManager_GameplayCueRefsName = TEXT("GameplayCueReferences");
const FName UBotaniAssetManager_LoadStateClient = FName(TEXT("Client"));

void UBotaniGameplayCueManager::RefreshGameplayCuePrimaryAsset()
{
	TArray<FSoftObjectPath> CuePaths;

	if (const UGameplayCueSet* RuntimeCueSet = GetRuntimeCueSet())
	{
		RuntimeCueSet->GetSoftObjectPaths(CuePaths);
	}

	FAssetBundleData Bundles;
	Bundles.AddBundleAssetsTruncated(UBotaniAssetManager_LoadStateClient, CuePaths);

	FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(UBotaniAssetManager_GameplayCueRefsType, UBotaniAssetManager_GameplayCueRefsName);
	UAssetManager::Get().AddDynamicAsset(PrimaryAssetId, FSoftObjectPath(), Bundles);
}


