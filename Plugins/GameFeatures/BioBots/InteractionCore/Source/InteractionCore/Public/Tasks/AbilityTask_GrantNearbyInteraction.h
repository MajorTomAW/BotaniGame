// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once


#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_GrantNearbyInteraction.generated.h"

class UGameplayAbility;
class UObject;
struct FFrame;
struct FGameplayAbilitySpecHandle;
struct FObjectKey;

/**
 * UAbilityTask_GrantNearbyInteraction
 *
 * Grants abilities to any reachable interactables.
 */
UCLASS()
class INTERACTIONCORE_API UAbilityTask_GrantNearbyInteraction : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UAbilityTask Interface
	virtual void Activate() override;
	//~ End UAbilityTask Interface

	/** Wait until an overlap occurs. This will need to be better fleshed out, so we can specify game-specific collision requirements */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_GrantNearbyInteraction* GrantAbilitiesForNearbyInteractors(UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate);

private:
	//~ Begin UAbilityTask Interface
	virtual void OnDestroy(bool AbilityEnded) override;
	//~ End UAbilityTask Interface

	void QueryInteractables();

private:
	float InteractionScanRange;
	float InteractionScanRate;

	FTimerHandle QueryTimerHandle;
	TMap<FObjectKey, FGameplayAbilitySpecHandle> InteractionAbilityCache;
};
