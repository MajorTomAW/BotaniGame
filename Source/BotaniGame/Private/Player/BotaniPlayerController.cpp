// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Player/BotaniPlayerController.h"

#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "Camera/BotaniCameraManager.h"
#include "Net/UnrealNetwork.h"
#include "Player/BotaniPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniPlayerController)


ABotaniPlayerController::ABotaniPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ABotaniCameraManager::StaticClass();
}

void ABotaniPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Thx to Lyra Starter Game for this :)
	
	// Disable replicating the PC target view as it doesn't work well for replays or client-side spectating.
	// The engine TargetViewRotation is only set in APlayerController::TickActor if the server knows ahead of time that 
	// a specific pawn is being spectated and it only replicates down for COND_OwnerOnly.
	// In client-saved replays, COND_OwnerOnly is never true and the target pawn is not always known at the time of recording.
	// To support client-saved replays, the replication of this was moved to ReplicatedViewRotation and updated in PlayerTick.
	DISABLE_REPLICATED_PROPERTY(APlayerController, TargetViewRotation);
}

ABotaniPlayerState* ABotaniPlayerController::GetBotaniPlayerState() const
{
	return CastChecked<ABotaniPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UBotaniAbilitySystemComponent* ABotaniPlayerController::GetBotaniAbilitySystemComponent() const
{
	const ABotaniPlayerState* BotaniPlayerState = GetBotaniPlayerState();
	return BotaniPlayerState ? BotaniPlayerState->GetBotaniAbilitySystemComponent() : nullptr;
}

void ABotaniPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UBotaniAbilitySystemComponent* BotaniAbilitySystemComponent = GetBotaniAbilitySystemComponent())
	{
		BotaniAbilitySystemComponent->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void ABotaniPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	ABotaniPlayerState* BotaniPlayerState = GetBotaniPlayerState();
	if (!BotaniPlayerState || !PlayerCameraManager)
	{
		return;
	}

	APawn* TargetPawn = PlayerCameraManager->GetViewTargetPawn();
	if (!TargetPawn)
	{
		return;
	}

	// Update view rotation on the server so it replicates
	if (HasAuthority() ||TargetPawn->IsLocallyControlled())
	{
		BotaniPlayerState->SetReplicatedViewRotation(TargetPawn->GetViewRotation());
	}

	// Update the local view rotation from the server for simulated proxies
	if (!TargetPawn->IsLocallyControlled())
	{
		BotaniPlayerState = TargetPawn->GetPlayerState<ABotaniPlayerState>();
		if (BotaniPlayerState)
		{
			// Get it from the spectated pawn's player state
			TargetViewRotation = BotaniPlayerState->GetReplicatedViewRotation();
		}
	}
}
