// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniItemInstance.h"
#include "AbilitySystem/BotaniAbilitySet.h"
#include "AbilitySystem/BotaniAbilitySourceInterface.h"
#include "BotaniWeaponItemInstance.generated.h"

/**
 * UBotaniWeaponItemInstance
 *
 * A virtual instance of a weapon item.
 */
UCLASS()
class BOTANIGAME_API UBotaniWeaponItemInstance : public UBotaniItemInstance, public IBotaniAbilitySourceInterface
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin IBotaniAbilitySourceInterface Interface
	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const override;
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const override;
	//~ End IBotaniAbilitySourceInterface Interface

public:
	//~ Begin UBotaniItemInstance Interface
	virtual void OnInstanceCreated(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext) override;
	virtual void OnInstanceRemoved(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext) override;
	//~ End UBotaniItemInstance Interface
};
