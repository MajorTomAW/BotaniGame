// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Abilities/Movement/BotaniMovementAbility_Jump.h"

#include "Character/BotaniCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniMovementAbility_Jump)

UBotaniMovementAbility_Jump::UBotaniMovementAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UBotaniMovementAbility_Jump::StartJumping()
{
	ABotaniCharacter* BotaniCharacter = GetBotaniCharacterFromActorInfo();
	if (BotaniCharacter == nullptr)
	{
		return;
	}

	if (BotaniCharacter->IsLocallyControlled() && !BotaniCharacter->bPressedJump)
	{
		BotaniCharacter->UnCrouch();
		BotaniCharacter->Jump();
	}
}

void UBotaniMovementAbility_Jump::StopJumping()
{
	ABotaniCharacter* BotaniCharacter = GetBotaniCharacterFromActorInfo();
	if (BotaniCharacter == nullptr)
	{
		return;
	}

	if (BotaniCharacter->IsLocallyControlled() && BotaniCharacter->bPressedJump)
	{
		BotaniCharacter->StopJumping();
	}
}

bool UBotaniMovementAbility_Jump::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const ABotaniCharacter* BotaniCharacter = Cast<ABotaniCharacter>(ActorInfo->AvatarActor.Get());
	if (!BotaniCharacter || !BotaniCharacter->CanJump())
	{
		return false;
	}
	
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UBotaniMovementAbility_Jump::EndAbility(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping in case the ability blueprint doesn't call it.
	StopJumping();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
