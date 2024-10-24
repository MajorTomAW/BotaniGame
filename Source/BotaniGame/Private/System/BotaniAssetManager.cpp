// Copyright © 2024 Botanibots Team. All rights reserved.


#include "System/BotaniAssetManager.h"
#include "BotaniLogChannels.h"
#include "AbilitySystem/BotaniGameplayCueManager.h"

#define LOCTEXT_NAMESPACE "BotaniAssetManager"

const FName FBotaniBundles::Equipped("Equipped");

//////////////////////////////////////////////////////////////////////////

static FAutoConsoleCommand CVarDumpLoadedAssets(
	TEXT("botani.DumpLoadedAssets"),
	TEXT("Shows all assets that were loaded via the asset manager and are currently in memory."),
	FConsoleCommandDelegate::CreateStatic(UBotaniAssetManager::DumpLoadedAssets)
);

//////////////////////////////////////////////////////////////////////////

#define STARTUP_JOB_WEIGHTED(JobFunc, JobWeight) StartupJobs.Add(FBotaniAssetManagerStartupJob(#JobFunc, [this](const FBotaniAssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle){JobFunc;}, JobWeight))
#define STARTUP_JOB(JobFunc) STARTUP_JOB_WEIGHTED(JobFunc, 1.f)

//////////////////////////////////////////////////////////////////////////

UBotaniAssetManager::UBotaniAssetManager()
{
	DefaultPawnData = nullptr;
}

UBotaniAssetManager& UBotaniAssetManager::Get()
{
	check(GEngine);

	if (UBotaniAssetManager* AssetManager = Cast<UBotaniAssetManager>(GEngine->AssetManager))
	{
		return *AssetManager;
	}

	BOTANI_LOG(Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini. It must be set to BotaniAssetManager!"));
	return *NewObject<UBotaniAssetManager>();
}

const UBotaniGameData& UBotaniAssetManager::GetGameData()
{
	return GetOrLoadTypedGameData<UBotaniGameData>(BotaniGameDataPath);
}

const UBotaniPawnData* UBotaniAssetManager::GetDefaultPawnData() const
{
	return GetAsset(DefaultPawnData);
}

void UBotaniAssetManager::DumpLoadedAssets()
{
	BOTANI_LOG(Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		BOTANI_LOG(Log, TEXT("	%s"), *GetNameSafe(LoadedAsset));
	}

	BOTANI_LOG(Log, TEXT("... %d assets in loaded pool."), Get().LoadedAssets.Num());
	BOTANI_LOG(Log, TEXT("========== End Dumping Loaded Assets =========="));
}

UPrimaryDataAsset* UBotaniAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);

	if (!DataClassPath.IsNull())
	{
#if WITH_EDITOR
		FScopedSlowTask SlowTask(0, FText::Format(NSLOCTEXT("BotaniEditor", "BeginLoadingGameDataTask", "Loading GameData {0}"), FText::FromName(DataClass->GetFName())));
		SlowTask.MakeDialog(false, true);
#endif

		BOTANI_LOG(Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("		... GameData loaded!"), nullptr);

		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				// No timeout since its required having loaded game data for this game
				Handle->WaitUntilComplete(0.f, false);
				Asset = CastChecked<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		BOTANI_LOG(Fatal, TEXT("Failed to load GameData: %s"), *DataClassPath.ToString());
	}

	return Asset;
}

UObject* UBotaniAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (!AssetPath.IsValid())
	{
		return nullptr;
	}

	if (ShouldLogAssetLoads())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr = MakeUnique<FScopeLogTime>(
			*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr,
			FScopeLogTime::ScopeLog_Seconds);
	}

	if (IsInitialized())
	{
		return GetStreamableManager().LoadSynchronous(AssetPath, false);
	}

	// If the asset manager isn't loaded, load the object blocking
	return AssetPath.TryLoad();
}

bool UBotaniAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void UBotaniAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

void UBotaniAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("UBotaniAssetManager::StartInitialLoading");
	
	Super::StartInitialLoading();

	// Load cue manager
	STARTUP_JOB(InitializeGameplayCueManager());

	// Load base game data asset
	STARTUP_JOB_WEIGHTED(GetGameData(), 25.f);

	// Run all the queued startup jobs
	DoAllStartupJobs();
}

void UBotaniAssetManager::DoAllStartupJobs()
{
	SCOPED_BOOT_TIMING("UBotaniAssetManager::DoAllStartupJobs");
	const double AllStartupJobsStartTime = FPlatformTime::Seconds();

	if (IsRunningDedicatedServer())
	{
		for (const FBotaniAssetManagerStartupJob& StartupJob : StartupJobs)
		{
			StartupJob.DoJob();
		}
	}
	else
	{
		if (StartupJobs.Num() > 0)
		{
			float TotalJobValue = 0.f;
			for (const FBotaniAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				TotalJobValue += StartupJob.JobWeight;
			}

			float AccumulatedJobValue = 0.f;
			for (FBotaniAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				const float JobValue = StartupJob.JobWeight;
				StartupJob.SubstepProgressDelegate.BindLambda([This = this, AccumulatedJobValue, JobValue, TotalJobValue] (float NewProgress)
				{
					const float SubstepAmount = FMath::Clamp(NewProgress, 0.0f, 1.0f) * JobValue;
					const float OverallPercentWithSubstep = (AccumulatedJobValue + SubstepAmount) / TotalJobValue;

					This->UpdateInitialGameContentLoadPercent(OverallPercentWithSubstep);
				});

				StartupJob.DoJob();
				StartupJob.SubstepProgressDelegate.Unbind();

				AccumulatedJobValue += JobValue;
				UpdateInitialGameContentLoadPercent(AccumulatedJobValue / TotalJobValue);
			}
		}
		else
		{
			UpdateInitialGameContentLoadPercent(1.f);
		}
	}

	StartupJobs.Empty();
	BOTANI_LOG(Display, TEXT("All startup jobs took %.2f seconds to complete"), FPlatformTime::Seconds() - AllStartupJobsStartTime);
}

void UBotaniAssetManager::InitializeGameplayCueManager()
{
	SCOPED_BOOT_TIMING("UBotaniAssetManager::InitializeGameplayCueManager");

	UBotaniGameplayCueManager* GCM = UBotaniGameplayCueManager::Get();
	check(GCM);

	GCM->LoadAlwaysLoadedCues();
}

void UBotaniAssetManager::UpdateInitialGameContentLoadPercent(float GameContentPercent)
{
}

#if WITH_EDITOR
void UBotaniAssetManager::PreBeginPIE(bool bStartSimulate)
{
	Super::PreBeginPIE(bStartSimulate);

	FScopedSlowTask SlowTask(0, NSLOCTEXT("BotaniEditor", "BeginLoadingPIEData", "Loading PIE Data"));
	SlowTask.MakeDialog(false, true);

	const UBotaniGameData& LocalGameData = GetGameData();
	
	// Intentionally after GetGameData to avoid counting GameData time in this timer
	SCOPE_LOG_TIME_IN_SECONDS(TEXT("PreBeginPIE asset preloading complete"), nullptr);

	// Maybe we could add preloading of anything else needed for the experience we'll be using here. Not sure if we even need this atm lol.
    // (e.g., by grabbing the default experience from the world settings + the experience override in developer settings)
}
#endif
#undef LOCTEXT_NAMESPACE