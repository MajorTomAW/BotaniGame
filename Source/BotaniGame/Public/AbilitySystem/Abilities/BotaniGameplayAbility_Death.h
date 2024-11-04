// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniGameplayAbility.h"
#include "BotaniGameplayAbility_Death.generated.h"


/**
 * UBotaniGameplayAbility_Death
 *
 * Gameplay ability used for handling death.
 * Activates automatically via the "GameplayEvent.Death" ability trigger tag.
 */
UCLASS(Abstract)
class UBotaniGameplayAbility_Death : public UBotaniGameplayAbility
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility Interface

	/** Called to start the death sequence. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability")
	void StartDeathSequence();

	/** Called to finish the death sequence. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability")
	void FinishDeathSequence();

protected:
	/** Determines if the death sequence should start automatically. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	uint32 bAutoStartDeath : 1;
};
