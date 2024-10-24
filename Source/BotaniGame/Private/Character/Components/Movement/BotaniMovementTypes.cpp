// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Character/Components/Movement/BotaniMovementTypes.h"

#include "Character/BotaniCharacter.h"
#include "Character/Components/Movement/BotaniMovementComponent.h"


//////////////////////////////////////////////////////////////////////////
/// FSavedMove_BotaniCharacter

FSavedMove_BotaniCharacter::FSavedMove_BotaniCharacter()
{
	// Movement safe flags.
	bWantsToSlide = 0;
	bWantsToGrapple = 0;

	// Un-safe flags.
	bWantsToWallRun = 0;
}

void FSavedMove_BotaniCharacter::Clear()
{
	// Movement safe flags.
	bWantsToSlide = 0;
	bWantsToGrapple = 0;

	// Un-safe flags.
	bWantsToWallRun = 0;
	
	FSavedMove_Character::Clear();
}

void FSavedMove_BotaniCharacter::SetMoveFor(
	ACharacter* Character, float InDeltaTime, const FVector& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	const ABotaniCharacter* BotaniCharacter = Cast<ABotaniCharacter>(Character);
	if (!IsValid(BotaniCharacter))
	{
		return;
	}

	// Movement safe flags.
	bWantsToSlide = BotaniCharacter->bWantsToSlide;
	bWantsToGrapple = BotaniCharacter->bWantsToGrapple;

	// Un-safe flags.
	bWantsToWallRun = BotaniCharacter->bWantsToWallRun;
}

void FSavedMove_BotaniCharacter::PrepMoveFor(ACharacter* Character)
{
	FSavedMove_Character::PrepMoveFor(Character);

	ABotaniCharacter* BotaniCharacter = Cast<ABotaniCharacter>(Character);
	if (!IsValid(BotaniCharacter))
    {
        return;
    }

	// Movement safe flags.
	BotaniCharacter->bWantsToSlide = bWantsToSlide;
	BotaniCharacter->bWantsToGrapple = bWantsToGrapple;

	// Un-safe flags.
	BotaniCharacter->bWantsToWallRun = bWantsToWallRun;
}

uint8 FSavedMove_BotaniCharacter::GetCompressedFlags() const
{
	uint8 Flags = FSavedMove_Character::GetCompressedFlags();
	
	if (bWantsToSlide)
	{
		Flags |= FLAG_WantsToSlide;
	}

	if (bWantsToGrapple)
	{
		Flags |= FLAG_WantsToGrapple;
	}

	return Flags;
}

bool FSavedMove_BotaniCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,float MaxDelta) const
{
	const FSavedMove_BotaniCharacter* BotaniMove = static_cast<FSavedMove_BotaniCharacter*>(NewMove.Get());
	
    if (bWantsToSlide != BotaniMove->bWantsToSlide)
    {
        return false;
    }

	if (bWantsToGrapple != BotaniMove->bWantsToGrapple)
	{
		return false;
	}
	
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

//////////////////////////////////////////////////////////////////////////
/// FSavedMove_BotaniCharacter

FNetworkPredictionData_Client_BotaniCharacter::FNetworkPredictionData_Client_BotaniCharacter(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_BotaniCharacter::AllocateNewMove()
{
	return MakeShared<FSavedMove_BotaniCharacter>();
}
