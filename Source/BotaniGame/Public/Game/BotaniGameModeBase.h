// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameMode.h"
#include "BotaniGameModeBase.generated.h"

class UBotaniExperienceDefinition;
class UBotaniPawnData;
class AGameModeBase;
class AController;
class APlayerController;

/**
 * Delegate for getting the start spot for a player.
 */
DECLARE_DELEGATE_RetVal_OneParam(AActor* /** Start Spot */, FOnBotaniGetStartSpot, AController* /** Player */);

/**
 * Post-login event, triggered when a player or bot joins the game as well as after seamless and non-seamless travel.
 * This is called after the player has finished initialization.
 *
 * @param GameMode The game mode that the player is joining.
 * @param NewPlayer The player that has joined the game right now.
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBotaniGameModePlayerInitialized, AGameModeBase* /*GameMode*/, AController* /*NewPlayer*/);

/**
 * ABotaniGameModeBase
 *
 * The base game mode class used by this project.
 */
UCLASS(Config = Game, meta = (ShortTooltip = "The base game mode class used by this project."))
class BOTANIGAME_API ABotaniGameModeBase : public AModularGameModeBase
{
	GENERATED_UCLASS_BODY()
	
public:
	/** Delegate called on a completed player initialization. */
	FOnBotaniGameModePlayerInitialized OnGameModePlayerInitialized;

	/** Delegate for determining the player start spot. */
	FOnBotaniGetStartSpot DeterminePlayerStartSpot;

	/** Returns the pawn data for the specified controller. Can be overriden by the experience */
	UFUNCTION(BlueprintCallable, Category = "Botani|Pawn")
	const UBotaniPawnData* GetPawnDataForController(const AController* InController) const;

	/** Restart (respawn) the specified player or bot next frame. */
	UFUNCTION(BlueprintCallable, Category = "Botani|GameMode")
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset = false);

	//~ Begin AGameModeBase Interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	virtual bool UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage) override;
	
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	virtual void GenericPlayerInitialization(AController* NewPlayer) override;
	virtual void FailedToRestartPlayer(AController* NewPlayer) override;
	//~ Begin AGameModeBase Interface

	// Agnostic version of PlayerCanRestart that can be used for any controller type
	virtual bool ControllerCanRestart(AController* Controller);

protected:
	void OnExperienceLoaded(const UBotaniExperienceDefinition* CurrentExperience);

	bool IsExperienceLoaded();

	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource);

	void HandleMatchAssignmentIfNotExpectingOne();

private:
	UPROPERTY(Config)
	TSoftObjectPtr<UBotaniExperienceDefinition> DefaultExperience;
};
