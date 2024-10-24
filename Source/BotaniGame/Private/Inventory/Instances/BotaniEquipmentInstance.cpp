// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Instances/BotaniEquipmentInstance.h"

#include "AbilitySystemGlobals.h"
#include "GameplayInventoryLogChannels.h"
#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniEquipmentInstance)

UBotaniEquipmentInstance::UBotaniEquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UBotaniAbilitySystemComponent* UBotaniEquipmentInstance::GetBotaniASC() const
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn());
	return ASC ? Cast<UBotaniAbilitySystemComponent>(ASC) : nullptr;
}
