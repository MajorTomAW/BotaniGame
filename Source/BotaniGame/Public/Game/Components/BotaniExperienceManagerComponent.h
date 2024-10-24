// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LoadingProcessInterface.h"
#include "Components/GameStateComponent.h"
#include "BotaniExperienceManagerComponent.generated.h"

namespace UE::GameFeatures { struct FResult; }
class UBotaniExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBotaniExperienceLoaded, const UBotaniExperienceDefinition* /*Experience*/);


/** The loading states for the experience system. */
enum class EBotaniExperienceLoadingState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	LoadingChaosTestingDelay,
	ExecutingActions,
	Loaded,
	Deactivating
};

/**
 * UBotaniExperienceManagerComponent
 *
 * Manages the experience system for the game.
 */
UCLASS()
class UBotaniExperienceManagerComponent final : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_UCLASS_BODY()

public:
	static UBotaniExperienceManagerComponent* Get(const AGameStateBase* InGameState);
	
	//~ Begin UActorComponent Interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent Interface
	
	//~ Begin ILoadingProcessInterface Interface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~ End ILoadingProcessInterface Interface

	/** Tries to set the current experience. Can be a UI or gameplay one. */
	void SetCurrentExperience(FPrimaryAssetId ExperienceId);

	/** Returns the current experience if it is fully loaded, asserting otherwise. */
	const UBotaniExperienceDefinition* GetCurrentExperienceChecked() const;

	/** Returns true if the current experience is fully loaded. */
	bool IsExperienceLoaded() const;

	/**
	 * Ensures the delegate is called once the experience has been loaded, before others are called.
	 * However, if the experience has already loaded, calls the delegate immediately.
	 */
	void CallOrRegister_OnExperienceLoaded_HighPriority(FOnBotaniExperienceLoaded::FDelegate&& Delegate);

	/**
	 * Ensures the delegate is called once the experience has been loaded.
	 * If the experience has already loaded, calls the delegate immediately.
	 */
	void CallOrRegister_OnExperienceLoaded(FOnBotaniExperienceLoaded::FDelegate&& Delegate);

	/**
	 * Ensures the delegate is called once the experience has been loaded.
	 * If the experience has already loaded, calls the delegate immediately.
	 */
	void CallOrRegister_OnExperienceLoaded_LowPriority(FOnBotaniExperienceLoaded::FDelegate&& Delegate);
	
private:
	UFUNCTION()
	void OnRep_CurrentExperience();

	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();

	void OnActionDeactivationCompleted();
	void OnAllActionsDeactivated();
	
private:
	/** The current experience that is being loaded. */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentExperience)
	TObjectPtr<const UBotaniExperienceDefinition> CurrentExperience;

	/** The loading state of the experience system. */
	EBotaniExperienceLoadingState LoadingState = EBotaniExperienceLoadingState::Unloaded;

	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;
	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;

	/**
	 * Delegate called when the experience has finished loading just before others.
	 * (e.g., subsystems that need the experience to be loaded first)
	 */
	FOnBotaniExperienceLoaded OnExperienceLoaded_HighPriority;

	/** Delegate called when the experience has finished loading. */
	FOnBotaniExperienceLoaded OnExperienceLoaded;

	/** Delegate called when the experience has finished loading after others. */
	FOnBotaniExperienceLoaded OnExperienceLoaded_LowPriority;
};
