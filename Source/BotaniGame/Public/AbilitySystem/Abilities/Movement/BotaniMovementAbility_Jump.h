// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BotaniGameplayAbility.h"
#include "BotaniMovementAbility_Jump.generated.h"

/**
 * UBotaniMovementAbility_Jump
 *
 * Gameplay ability used for character jumping.
 */
UCLASS(Abstract, MinimalAPI, meta = (ShortTooltip = "Gameplay ability used for character jumping."))
class UBotaniMovementAbility_Jump : public UBotaniGameplayAbility
{
	GENERATED_UCLASS_BODY()

protected:
	/** Called to make the character jump. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability")
	BOTANIGAME_API void StartJumping();

	/** Called to make the character stop jumping. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability")
	BOTANIGAME_API void StopJumping();

protected:
	//~ Begin UGameplayAbility Interface
	BOTANIGAME_API virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	BOTANIGAME_API virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility Interface
};
