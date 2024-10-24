// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniItemInstance.h"
#include "AbilitySystem/BotaniAbilitySet.h"
#include "BotaniWeaponItemInstance.generated.h"

/**
 * UBotaniWeaponItemInstance
 *
 * A virtual instance of a weapon item.
 */
UCLASS()
class BOTANIGAME_API UBotaniWeaponItemInstance : public UBotaniItemInstance
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UBotaniItemInstance Interface
	virtual void OnInstanceCreated(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext) override;
	virtual void OnInstanceRemoved(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext) override;
	//~ End UBotaniItemInstance Interface
};
