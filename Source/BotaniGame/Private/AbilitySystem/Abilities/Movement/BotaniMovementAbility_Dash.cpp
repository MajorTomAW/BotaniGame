// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Abilities/Movement/BotaniMovementAbility_Dash.h"

#include "Character/BotaniCharacter.h"
#include "Character/Components/Movement/BotaniMovementComponent.h"

UBotaniMovementAbility_Dash::UBotaniMovementAbility_Dash(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

FVector UBotaniMovementAbility_Dash::GetDashDirection_Implementation() const
{
	const ABotaniCharacter* BotaniCharacter = GetBotaniCharacterFromActorInfo();
	if (!BotaniCharacter)
	{
		return FVector::ZeroVector;
	}

	const FVector Fwd = BotaniCharacter->GetActorForwardVector();
	const FVector ArcadeDirection = BotaniCharacter->GetBotaniMoveComp()->Dash_ArcadeDirection.RotateAngleAxis(BotaniCharacter->GetActorRotation().Yaw, FVector::UpVector);
	
	return Fwd + ArcadeDirection;
}

bool UBotaniMovementAbility_Dash::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo ||!ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const ABotaniCharacter* BotaniCharacter = Cast<ABotaniCharacter>(ActorInfo->AvatarActor.Get());
	if (!BotaniCharacter || !BotaniCharacter->GetBotaniMoveComp()->CanDash())
	{
		return false;
	}
	
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UBotaniMovementAbility_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ABotaniCharacter* BotaniCharacter = GetBotaniCharacterFromActorInfo();
	UBotaniMovementComponent* BotaniMoveComp = BotaniCharacter ? BotaniCharacter->GetBotaniMoveComp() : nullptr;
	if (!BotaniMoveComp)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Only dash when received from a local player.
	if (!IsLocallyControlled())
	{
		return;
	}

	// Exit any movement states that would interfere with the dash.
	if (BotaniMoveComp->IsCrouching() && BotaniMoveComp->bDash_Uncrouch)
	{
		BotaniCharacter->UnCrouch();
	}

	if (BotaniMoveComp->IsSliding() && BotaniMoveComp->bDash_StopSliding)
	{
		BotaniMoveComp->ExitSliding();
	}

	PerformDash(GetDashDirection(), BotaniMoveComp->Dash_Speed, BotaniMoveComp->Dash_Duration);
}
