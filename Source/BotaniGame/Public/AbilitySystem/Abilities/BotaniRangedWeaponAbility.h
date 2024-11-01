// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniWeaponAbility.h"
#include "Weapons/Modes/WeaponMode_RangedWeapon.h"
#include "BotaniRangedWeaponAbility.generated.h"

/**
 * UBotaniRangeWeaponAbility
 *
 * An ability granted by and associated with a ranged weapon instance.
 */
UCLASS(MinimalAPI, meta = (ShortTooltip = "An ability granted by and associated with a ranged weapon instance.", DisplayName = "Ranged Weapon Ability"))
class UBotaniRangedWeaponAbility : public UBotaniWeaponAbility
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns the active weapon mode as a ranged weapon mode */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Ability")
	const UWeaponMode_RangedWeapon* GetActiveRangedWeaponMode() const;

	/** Returns the stats of the ranged weapon mode */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Ability")
	FBotaniWeaponStatData GetRangedWeaponModeStats(bool& Success) const;

protected:
	//~ Begin UGameplayAbility interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	//~ End UGameplayAbility interface

protected:
	/** Determines if the ability automatically checks for valid ammo */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	uint32 bAutoCheckAmmo : 1 = true;

	/** Determines if the ability automatically consumes ammo */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	uint32 bAutoConsumeAmmo : 1 = true;
};
