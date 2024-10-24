// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BotaniGameplayAbility.h"
#include "BotaniMovementAbility_Sprint.generated.h"

/**
 * UBotaniMovementAbility_Sprint
 *
 * Gameplay ability used for character sprinting.
 */
UCLASS(Abstract, MinimalAPI, meta = (ShortTooltip = "Gameplay ability used for character sprinting."))
class UBotaniMovementAbility_Sprint : public UBotaniGameplayAbility
{
	GENERATED_UCLASS_BODY()

protected:
	/**
	 * Called to make the character sprint.
	 *
	 * @param bForceSprint If true, the character will sprint even if the current movement state doesn't allow sprinting, such as when the character is crouching.
	 * In this case, the character will be forced to un-crouch before sprinting.
	 */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability")
	BOTANIGAME_API void StartSprinting(bool bForceSprint = false);
	
	/** Called to make the character stop sprinting. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability")
	BOTANIGAME_API void StopSprinting();

	/** Called to directly set the character's sprinting state. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability")
	BOTANIGAME_API void SetSprinting(bool bNewSprinting, bool bForceSprint = false);

protected:
	//~ Begin UGameplayAbility Interface
	BOTANIGAME_API virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	BOTANIGAME_API virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility Interface
};
