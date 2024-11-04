// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Player/BotaniBotCoreController.h"

#include "GameFramework/PlayerState.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BotaniLogChannels.h"
#include "Components/AIUtilityManager.h"
#include "Game/BotaniGameModeBase.h"
#include "Perception/AIPerceptionComponent.h"


ABotaniBotCoreController::ABotaniBotCoreController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsPlayerState = true;
	bStopAILogicOnUnposses = true;

	BotPerceptionComponent = ObjectInitializer.CreateDefaultSubobject<UAIPerceptionComponent>(this, TEXT("AIPerceptionComponent"));
	BotPerceptionComponent->SetAutoActivate(true);

	UtilityManager = ObjectInitializer.CreateDefaultSubobject<UAIUtilityManager>(this, TEXT("AIUtilityManager"));
	UtilityManager->SetAutoActivate(true);
}

void ABotaniBotCoreController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	BOTANI_LOG(Error, TEXT("You can't set the team ID on a Botani bot controller (%s); it's driven by the associated player state"), *GetPathNameSafe(this));
}

FGenericTeamId ABotaniBotCoreController::GetGenericTeamId() const
{
	if (IBotaniTeamAgentInterface* TeamAgent = Cast<IBotaniTeamAgentInterface>(PlayerState))
	{
		return TeamAgent->GetGenericTeamId();
	}

	return FGenericTeamId::NoTeam;
}

FOnBotaniTeamIndexChangedDelegate* ABotaniBotCoreController::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamIndexChangedDelegate;
}

ETeamAttitude::Type ABotaniBotCoreController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* OtherPawn = Cast<APawn>(&Other);
	if (OtherPawn == nullptr)
	{
		return ETeamAttitude::Neutral;
	}

	if (const IBotaniTeamAgentInterface* TeamAgent = Cast<IBotaniTeamAgentInterface>(OtherPawn->GetController()))
	{
		const FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();

		// Checking other pawn team ID to define attitude
		if (OtherTeamID.GetId() != GetGenericTeamId().GetId())
		{
			return ETeamAttitude::Hostile;
		}

		return ETeamAttitude::Friendly;
	}

	return ETeamAttitude::Neutral;
}

void ABotaniBotCoreController::OnUnPossess()
{
	// Make sure the pawn being unpossessed doesn't remain our ASC's avatar
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}
	
	Super::OnUnPossess();
}

void ABotaniBotCoreController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetPerceptionComponent(*BotPerceptionComponent);
}

void ABotaniBotCoreController::ServerRestartController()
{
	if (GetNetMode() == NM_Client)
	{
		return;
	}

	ensure(GetPawn() == nullptr && IsInState(NAME_Inactive));

	if (IsInState(NAME_Inactive) || IsInState(NAME_Spectating))
	{
		ABotaniGameModeBase* const GameMode = GetWorld()->GetAuthGameMode<ABotaniGameModeBase>();
		if (GameMode == nullptr)
		{
			return;
		}

		if (!GameMode->ControllerCanRestart(this))
		{
			return;
		}

		// If we're still attached to a pawn, leave it
		if (GetPawn() != nullptr)
		{
			UnPossess();
		}

		// Re-enable input, similar to code in ClientRestart
		ResetIgnoreInputFlags();

		GameMode->RestartPlayer(this);
	}
}
