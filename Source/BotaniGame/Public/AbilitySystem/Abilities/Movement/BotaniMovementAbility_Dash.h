// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BotaniGameplayAbility.h"
#include "BotaniMovementAbility_Dash.generated.h"

/**
 * UBotaniMovementAbility_Dash
 *
 * Gameplay ability used for character dashing.
 */
UCLASS(Abstract, MinimalAPI, meta = (ShortTooltip = "Gameplay ability used for character dashing."))
class UBotaniMovementAbility_Dash : public UBotaniGameplayAbility
{
	GENERATED_UCLASS_BODY()

protected:
	/** Called to determine the character dash direction. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Botani|Ability|Dash")
	BOTANIGAME_API FVector GetDashDirection() const;

protected:
	/** Called to perform the dash. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Botani|Ability|Dash")
	BOTANIGAME_API void PerformDash(const FVector DashDirection, const float DashSpeed, const float DashDuration);
	
	//~ Begin UGameplayAbility Interface
	BOTANIGAME_API virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	BOTANIGAME_API virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//~ End UGameplayAbility Interface
};
