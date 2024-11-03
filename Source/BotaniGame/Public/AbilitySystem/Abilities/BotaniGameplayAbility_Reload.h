// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniGameplayAbility_Weapon.h"
#include "BotaniGameplayAbility_Reload.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Weapon Reload Ability"))
class BOTANIGAME_API UBotaniGameplayAbility_Reload : public UBotaniGameplayAbility_Weapon
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	//~ End UGameplayAbility Interface

protected:
	virtual int32 GetSpareAmmo() const;
	virtual int32 GetMagazineAmmo() const;
	virtual bool IsMagazineFull() const;
};
