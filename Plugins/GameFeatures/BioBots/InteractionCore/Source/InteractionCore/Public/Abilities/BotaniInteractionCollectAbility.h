// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BotaniGameplayAbility.h"
#include "BotaniInteractionCollectAbility.generated.h"

class IPickupable;
/**
 * UBotaniInteractionCollectAbility
 */
UCLASS(Abstract)
class INTERACTIONCORE_API UBotaniInteractionCollectAbility : public UBotaniGameplayAbility
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//~ End UGameplayAbility Interface

protected:
	/** Called to add the item to the inventory. */
	UFUNCTION(BlueprintCallable, Category = "Interaction|Collect")
	void AddItemToInventory(TScriptInterface<IPickupable> Pickupable);
};
