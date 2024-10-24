// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Fragments/GameplayEquipmentFragment.h"
#include "EquipmentFragment_GrantAbilitySet.generated.h"

/**
 * Grants a set of abilities when equipped which get removed when unequipped.
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Grant Equipment Ability Set"))
class UEquipmentFragment_GrantAbilitySet : public UGameplayEquipmentFragment
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UGameplayEquipmentFragment Interface
	virtual bool CanApplyInventoryFragment(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, FGameplayTagContainer* OutBlockingTags) const override;
	virtual void OnEquipmentInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, class UGameplayEquipmentInstance* EquipmentInstance) const override;
	virtual void OnEquipmentInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, class UGameplayEquipmentInstance* EquipmentInstance) const override;
	//~ End UGameplayEquipmentFragment Interface

protected:
	/** A list of ability sets to grant when this equipment is equipped. */
	UPROPERTY(EditDefaultsOnly, Category = "Ability Set")
	TArray<TObjectPtr<const class UBotaniAbilitySet>> AbilitySetsToGrant;
};
