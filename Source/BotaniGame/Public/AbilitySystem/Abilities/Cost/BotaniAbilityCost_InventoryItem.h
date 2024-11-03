// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniAbilityCost.h"
#include "BotaniAbilityCost_InventoryItem.generated.h"


/**
 * UBotaniAbilityCost_InventoryItem
 *
 * Represents a cost that requires an inventory item to be consumed.
 */
UCLASS(meta = (DisplayName = "Inventory Item"))
class UBotaniAbilityCost_InventoryItem : public UBotaniAbilityCost
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UBotaniAbilityCost Interface
	virtual bool CheckCost(const UBotaniGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const UBotaniGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~ End of UBotaniAbilityCost Interface

protected:
	/** Determines how much of the item to consume */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AbilityCost)
	FScalableFloat Quantity;

	/** Determines the item to consume */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AbilityCost)
	TSoftObjectPtr<class UBotaniInventoryItemDefinition> ItemDefinition;
};
