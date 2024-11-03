// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Abilities/Cost/BotaniAbilityCost_InventoryItem.h"

#include "AbilitySystem/Abilities/BotaniGameplayAbility.h"
#include "Inventory/Components/BotaniInventoryManager.h"
#include "Inventory/Definitions/BotaniInventoryItemDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniWeaponItemInstance)

UBotaniAbilityCost_InventoryItem::UBotaniAbilityCost_InventoryItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Quantity.SetValue(1.0f);
}

bool UBotaniAbilityCost_InventoryItem::CheckCost(
	const UBotaniGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (AController* PC = Ability->GetControllerFromActorInfo())
	{
		if (UBotaniInventoryManager* InventoryManager = PC->GetComponentByClass<UBotaniInventoryManager>())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
			const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);

			return InventoryManager->GetTotalItemCountByDefinition(ItemDefinition.LoadSynchronous()) >= NumItemsToConsume;
		}
	}

	return false;
}

void UBotaniAbilityCost_InventoryItem::ApplyCost(const UBotaniGameplayAbility* Ability,
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (AController* PC = Ability->GetControllerFromActorInfo())
		{
			if (UBotaniInventoryManager* InventoryManager = PC->GetComponentByClass<UBotaniInventoryManager>())
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
				const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);

				const FGameplayInventoryItemContext Context = InventoryManager->MakeItemContext(ItemDefinition.LoadSynchronous(), NumItemsToConsume);
				InventoryManager->ConsumeItem(Context);
			}
		}
	}
}
