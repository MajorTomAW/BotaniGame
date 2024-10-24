// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Bot/Modifiers/CharacterModifier_Movement.h"

#include "Character/Components/Movement/BotaniMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterModifier_Movement)

UCharacterModifier_Movement::UCharacterModifier_Movement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	EditorFriendlyName = TEXT("Modifier: Movement");
#endif
}

void FCharacterMovementModifier_Speeds::ApplyToMovementComponent(UCharacterMovementComponent* MovementComponent) const
{
	if (MovementComponent == nullptr)
	{
		return;
	}

	MovementComponent->BrakingDecelerationWalking = Deceleration;
	MovementComponent->MaxAcceleration = Acceleration;
	MovementComponent->MaxWalkSpeed = WalkSpeed;

	if (UBotaniMovementComponent* BotaniMoveComp = Cast<UBotaniMovementComponent>(MovementComponent))
	{
		BotaniMoveComp->Sprint_MaxSpeed = RunSpeed;
	}
}

void FCharacterMovementModifier_World::ApplyToMovementComponent(UCharacterMovementComponent* MovementComponent) const
{
	if (MovementComponent == nullptr)
	{
		return;
	}

	MovementComponent->GravityScale = GravityScale;
}

void UCharacterModifier_Movement::ApplyToCharacter(ACharacter* Character) const
{
	Super::ApplyToCharacter(Character);

	if (Character == nullptr)
	{
		return;
	}

	World.ApplyToMovementComponent(Character->GetCharacterMovement());
	Speeds.ApplyToMovementComponent(Character->GetCharacterMovement());
}
