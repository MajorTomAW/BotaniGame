// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Player/BotaniPlayerStart.h"

#include "GameFramework/GameModeBase.h"

ABotaniPlayerStart::ABotaniPlayerStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsClaimable = true;
	ClaimingController = nullptr;
	ExpirationCheckInterval = 1.f;

	bRespawnAlivePlayers = true;
}

#if WITH_EDITOR
void ABotaniPlayerStart::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property == nullptr)
	{
		return;
	}
	
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ABotaniPlayerStart, OptStartPointTags))
	{
	};

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ThisClass, SpawnpointTag))
	{
		if (PlayerStartTag.IsValid())
		{
			PlayerStartTag = SpawnpointTag.GetTagName();	
		}
		else
		{
			PlayerStartTag = NAME_None;
		}
	}
}
#endif

bool ABotaniPlayerStart::IsClaimed() const
{
	return ClaimingController != nullptr;
}

bool ABotaniPlayerStart::TryClaim(AController* OccupyingController)
{
	if (OccupyingController == nullptr)
	{
		return false;
	}

	if (IsClaimed())
	{
		return false;
	}

	ClaimingController = OccupyingController;

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ExpirationTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::CheckUnclaimed), ExpirationCheckInterval, true);
	}

	return true;
}

EBotaniPlayerStartLocationOccupancy::Type ABotaniPlayerStart::GetLocationOccupancy( AController* const ControllerPawnToFit) const
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return EBotaniPlayerStartLocationOccupancy::Full;
	}

	if (!HasAuthority())
	{
		return EBotaniPlayerStartLocationOccupancy::Full;
	}

	if (AGameModeBase* AuthGameMode = World->GetAuthGameMode())
	{
		TSubclassOf<APawn> PawnClass = AuthGameMode->GetDefaultPawnClassForController(ControllerPawnToFit);
		const APawn* const PawnToFit = PawnClass ? GetDefault<APawn>(PawnClass) : nullptr;

		FVector ActorLocation = GetActorLocation();
		const FRotator ActorRotation = GetActorRotation();

		if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation, nullptr))
		{
			return EBotaniPlayerStartLocationOccupancy::Empty;
		}

		if (World->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
		{
			return EBotaniPlayerStartLocationOccupancy::Partial;
		}
	}

	return EBotaniPlayerStartLocationOccupancy::Full;
}

void ABotaniPlayerStart::CheckUnclaimed()
{
	if (ClaimingController == nullptr)
	{
		return;
	}

	// No pawn spawned yet, meaning this player start is still claimed
	if (ClaimingController->GetPawn() == nullptr)
	{
		return;
	}

	if (GetLocationOccupancy(ClaimingController) == EBotaniPlayerStartLocationOccupancy::Empty)
	{
		ClaimingController = nullptr;
		if (const UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(ExpirationTimerHandle);
		}
	}
}
