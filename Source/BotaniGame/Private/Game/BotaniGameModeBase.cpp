// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Game/BotaniGameModeBase.h"

#include "BotaniLogChannels.h"
#include "EngineUtils.h"
#include "Character/BotaniCharacter.h"
#include "Character/Components/BotaniPawnExtensionComponent.h"
#include "Development/BotaniDeveloperSettings.h"
#include "Engine/PlayerStartPIE.h"
#include "Game/BotaniGameStateBase.h"
#include "Game/BotaniWorldSettings.h"
#include "Game/Components/BotaniExperienceManagerComponent.h"
#include "Game/Experience/BotaniExperienceDefinition.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BotaniBotCoreController.h"
#include "Player/BotaniPlayerController.h"
#include "Player/BotaniPlayerState.h"
#include "System/BotaniAssetManager.h"
#include "UI/BotaniHUD.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniGameModeBase)


ABotaniGameModeBase::ABotaniGameModeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = ABotaniGameStateBase::StaticClass();
	GameSessionClass = AGameSession::StaticClass();
	PlayerControllerClass = ABotaniPlayerController::StaticClass();
	PlayerStateClass = ABotaniPlayerState::StaticClass();
	DefaultPawnClass = ABotaniCharacter::StaticClass();
	HUDClass = ABotaniHUD::StaticClass();
}

const UBotaniPawnData* ABotaniGameModeBase::GetPawnDataForController(const AController* InController) const
{
	// Check if pawn data is already set on the player state
	if (InController != nullptr)
	{
		if (const ABotaniPlayerState* BotaniPlayerState = InController->GetPlayerState<ABotaniPlayerState>())
		{
			if (const UBotaniPawnData* PawnData = BotaniPlayerState->GetPawnData<UBotaniPawnData>())
			{
				return PawnData;
			}
		}
	}

	// If not, fall back to the default pawn data for the current experience
	check(GameState);

	const UBotaniExperienceManagerComponent* ExperienceManager = UBotaniExperienceManagerComponent::Get(GameState);
	check(ExperienceManager);

	if (!ExperienceManager->IsExperienceLoaded())
	{
		BOTANI_EXP_LOG(Error, TEXT("[%hs] called, before experience is loaded!"), __FUNCTION__);
		return nullptr;
	}

	const UBotaniExperienceDefinition* Experience = ExperienceManager->GetCurrentExperienceChecked();
	if (Experience->DefaultPawnData != nullptr)
	{
		BOTANI_EXP_LOG(Warning, TEXT("Found pawn data (%s) for controller [%s] in experience [%s]"), *Experience->DefaultPawnData->GetName(), *InController->GetName(), *Experience->GetName());
		return Experience->DefaultPawnData;
	}

	// Experience is loaded, but there is no pawn data yet, fall back to the default one for now
	BOTANI_EXP_LOG(Error, TEXT("No pawn data found for controller [%s] in experience [%s], falling back to default pawn data"), *InController->GetName(), *Experience->GetName());
	return UBotaniAssetManager::Get().GetDefaultPawnData();
}

void ABotaniGameModeBase::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
	else if (ABotaniBotCoreController* BotController = Cast<ABotaniBotCoreController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(BotController, &ABotaniBotCoreController::ServerRestartController);
	}
}

void ABotaniGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Wait for the next frame to give time to initialize startup settings
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void ABotaniGameModeBase::InitGameState()
{
	Super::InitGameState();

	// Listen for the experience load to complete
	UBotaniExperienceManagerComponent* ExperienceManager = UBotaniExperienceManagerComponent::Get(GameState);
	check(ExperienceManager);

	ExperienceManager->CallOrRegister_OnExperienceLoaded(FOnBotaniExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

UClass* ABotaniGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UBotaniPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* ABotaniGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	if (PawnClass == nullptr)
	{
		BOTANI_LOG(Error, TEXT("GameMode was unable to spawn Pawn due to NULL pawn class!"))
		return nullptr;
	}

	if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
	{
		if (UBotaniPawnExtensionComponent* PawnExtComp = UBotaniPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
		{
			if (const UBotaniPawnData* PawnData = GetPawnDataForController(NewPlayer))
			{
				PawnExtComp->SetPawnData(PawnData);
			}
			else
			{
				BOTANI_EXP_LOG(Error, TEXT("GameMode was unable to set PawnData on the spawned pawn [%s]!"), *SpawnedPawn->GetName());
			}
		}

		SpawnedPawn->FinishSpawning(SpawnTransform);
		return SpawnedPawn;
	}

	BOTANI_LOG(Error, TEXT("GameMode was unable to spawn Pawn of class [%s]!"), *PawnClass->GetName());
	return nullptr;
}

AActor* ABotaniGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
#if WITH_EDITOR
	//@Sebastian, always prefer the first "Play from Here" start spot in PIE
	const UWorld* const World = GetWorld();
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		APlayerStart* const PlayerStart = *It;
		if (PlayerStart->IsA<APlayerStartPIE>())
		{
			return PlayerStart;
		}
	}
#endif
	
	if (DeterminePlayerStartSpot.IsBound())
	{
		if (AActor* PotStartSpot = DeterminePlayerStartSpot.Execute(Player))
		{
			return PotStartSpot;
		}
	}
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

bool ABotaniGameModeBase::ShouldSpawnAtStartSpot(AController* Player)
{
	// We never want to use the default implementation, always use spawn manager component
	if (DeterminePlayerStartSpot.IsBound())
	{
		return false;
	}

	return true;
}

bool ABotaniGameModeBase::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

bool ABotaniGameModeBase::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	return Super::UpdatePlayerStartSpot(Player, Portal, OutErrorMessage);
}

void ABotaniGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Delay starting new players until the experience has been loaded
	if (!IsExperienceLoaded())
	{
		return;
	}

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

void ABotaniGameModeBase::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

void ABotaniGameModeBase::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);

	OnGameModePlayerInitialized.Broadcast(this, NewPlayer);
}

void ABotaniGameModeBase::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);

	// If we tried to restart a player and failed, try again next frame
	if (GetDefaultPawnClassForController(NewPlayer) == nullptr)
	{
		return;
	}

	if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
	{
		if (PlayerCanRestart(NewPC))
		{
			RequestPlayerRestartNextFrame(NewPlayer, false);
		}
	}
	else
	{
		RequestPlayerRestartNextFrame(NewPlayer, false);
	}
}

bool ABotaniGameModeBase::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (!Super::PlayerCanRestart_Implementation(PlayerController))
		{
			return false;
		}
	}
	else
	{
		// Check restart for bots
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}
	//@TODO: Player spawning manager

	return true;
}

void ABotaniGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	// Precedence order (highest wins)
	//  – Matchmaking assignment (if present)
	//  – URL Options override
	//  – Developer Settings (PIE only)
	//  – Command Line override
	//  – World Settings
	//  – Dedicated server (not implemented yet)
	//  – Default experience

	UWorld* World = GetWorld();

	// URL Options override
	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UBotaniExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
		ExperienceIdSource = TEXT("OptionsString");
	}

	// Developer Settings (PIE only)
	if (!ExperienceId.IsValid() && World->IsPlayInEditor())
	{
		ExperienceId = UBotaniDeveloperSettings::Get()->ExperienceOverride;
		ExperienceIdSource = TEXT("DeveloperSettings");
	}

	// Command Line override
	if (!ExperienceId.IsValid())
	{
		FString ExperienceFromCommandLine;
		if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
		{
			ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
			ExperienceIdSource = TEXT("CommandLine");
			if (!ExperienceId.PrimaryAssetType.IsValid())
			{
				ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UBotaniExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromCommandLine));
			}
		}
	}

	// World Settings
	if (!ExperienceId.IsValid())
	{
		if (ABotaniWorldSettings* TypedWorldSettings = Cast<ABotaniWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
			ExperienceIdSource = TEXT("WorldSettings");
		}
	}

	// Validate the experience
	UBotaniAssetManager& AssetManager = UBotaniAssetManager::Get();
	FAssetData Dummy;
	if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	{
		BOTANI_EXP_LOG(Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default!"), *ExperienceId.ToString());
		ExperienceId = FPrimaryAssetId();
	}

	// Final fallback to default experience
	if (!ExperienceId.IsValid())
	{
		ExperienceId = UAssetManager::Get().GetPrimaryAssetIdForPath(DefaultExperience.ToSoftObjectPath());
		if (!ExperienceId.IsValid())
		{
			BOTANI_EXP_LOG(Error, TEXT("%s.DefaultGameplayExperience is %s but that failed to resolve into an asset ID (you might need to add a path to the Asset Rules in your game feature plugin or project settings"),
			*GetPathNameSafe(this), *DefaultExperience.ToString());
		}
		
		ExperienceIdSource = TEXT("Default");
	}

	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

void ABotaniGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	if (ExperienceId.IsValid())
	{
		BOTANI_EXP_LOG(Log, TEXT("Identified experience: %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);

		UBotaniExperienceManagerComponent* ExperienceManager = UBotaniExperienceManagerComponent::Get(GameState);
		check(ExperienceManager);
		ExperienceManager->SetCurrentExperience(ExperienceId);
	}
	else
	{
		BOTANI_EXP_LOG(Error, TEXT("Failed to identify experience, match wont start!"));
	}
}

void ABotaniGameModeBase::OnExperienceLoaded(const UBotaniExperienceDefinition* CurrentExperience)
{
	// Spawn any players that are already attached to the game
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* Controller = Cast<APlayerController>(*Iterator);
		if (Controller == nullptr)
		{
			continue;
		}

		if (Controller->GetPawn() != nullptr)
		{
			continue;
		}

		if (PlayerCanRestart(Controller))
		{
			RestartPlayer(Controller);
		}
	}
}

bool ABotaniGameModeBase::IsExperienceLoaded()
{
	check(GameState);

	const UBotaniExperienceManagerComponent* ExperienceManager = UBotaniExperienceManagerComponent::Get(GameState);
	check(ExperienceManager);

	return ExperienceManager->IsExperienceLoaded();
}