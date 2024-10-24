// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Abilities/Movement/BotaniMovementAbility_Sprint.h"

#include "Character/BotaniCharacter.h"
#include "Character/Components/Movement/BotaniMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniMovementAbility_Sprint)

UBotaniMovementAbility_Sprint::UBotaniMovementAbility_Sprint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UBotaniMovementAbility_Sprint::StartSprinting(bool bForceSprint)
{
	ABotaniCharacter* BotaniCharacter = GetBotaniCharacterFromActorInfo();
	if (BotaniCharacter == nullptr)
	{
		return;
	}

	if (BotaniCharacter->IsLocallyControlled() && !BotaniCharacter->bWantsToSprint)
	{
		if (!BotaniCharacter->GetBotaniMoveComp()->IsCrouching() || bForceSprint)
		{
			BotaniCharacter->UnCrouch();
			BotaniCharacter->Sprint();
		}
	}
}

void UBotaniMovementAbility_Sprint::StopSprinting()
{
	ABotaniCharacter* BotaniCharacter = GetBotaniCharacterFromActorInfo();
	if (BotaniCharacter == nullptr)
	{
		return;
	}

	if (BotaniCharacter->IsLocallyControlled() && BotaniCharacter->bWantsToSprint)
	{
		BotaniCharacter->StopSprinting();
	}
}

void UBotaniMovementAbility_Sprint::SetSprinting(bool bNewSprinting, bool bForceSprint)
{
	if (bNewSprinting)
	{
		StartSprinting(bForceSprint);
	}
	else
	{
		StopSprinting();
	}
}

bool UBotaniMovementAbility_Sprint::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const ABotaniCharacter* BotaniCharacter = Cast<ABotaniCharacter>(ActorInfo->AvatarActor.Get());
	if (!BotaniCharacter ||!BotaniCharacter->CanSprint())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UBotaniMovementAbility_Sprint::EndAbility(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop sprinting when the ability ends.
	StopSprinting();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
