// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Abilities/BotaniDeathAbility.h"

#include "BotaniLogChannels.h"
#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "Character/Components/BotaniHealthComponent.h"
#include "GameplayTags/BotaniGameplayTags.h"

UBotaniDeathAbility::UBotaniDeathAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	
	// Default to auto-starting death
	bAutoStartDeath = true;

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = BotaniGameplayTags::Gameplay::Event::TAG_GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UBotaniDeathAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);
	UBotaniAbilitySystemComponent* BotaniASC = GetBotaniAbilitySystemComponent();
	check(BotaniASC);

	// Cancel all abilities
	FGameplayTagContainer AbilitiesToIgnore;
	AbilitiesToIgnore.AddTag(BotaniGameplayTags::Ability::Behavior::TAG_Ability_Behavior_SurvivesDeath);
	BotaniASC->CancelAbilities(nullptr, &AbilitiesToIgnore, this);

	SetCanBeCanceled(false);

	if (!CanChangeActivationGroup(EBotaniAbilityActivationGroup::Exclusive_Blocking))
	{
		BOTANI_GAS_LOG(Error, TEXT("UBotaniDeathAbility::ActivateAbility: Ability [%s] failed to change activation group to blocking."), *GetName());
	}

	if (bAutoStartDeath)
	{
		StartDeathSequence();
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UBotaniDeathAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);
	FinishDeathSequence();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBotaniDeathAbility::StartDeathSequence()
{
	UBotaniHealthComponent* HealthComponent = UBotaniHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo());
	if (HealthComponent == nullptr)
	{
		BOTANI_GAS_LOG(Error, TEXT("UBotaniDeathAbility::StartDeathSequence: Failed to find health component for actor %s."), *GetAvatarActorFromActorInfo()->GetName());
		return;
	}

	if (HealthComponent->GetDeathState() == EBotaniDeathState::NotDead)
	{
		HealthComponent->StartDeath();
	}
}

void UBotaniDeathAbility::FinishDeathSequence()
{
	UBotaniHealthComponent* HealthComponent = UBotaniHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo());
	if (HealthComponent == nullptr)
	{
		BOTANI_GAS_LOG(Error, TEXT("UBotaniDeathAbility::FinishDeathSequence: Failed to find health component for actor %s."), *GetAvatarActorFromActorInfo()->GetName());
		return;
	}

	if (HealthComponent->GetDeathState() == EBotaniDeathState::DeathStarted)
	{
		HealthComponent->FinishDeath();
	}
}
