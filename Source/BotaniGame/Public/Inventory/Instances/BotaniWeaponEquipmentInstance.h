// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniEquipmentInstance.h"
#include "AbilitySystem/BotaniAbilitySet.h"
#include "BotaniWeaponEquipmentInstance.generated.h"

/**
 * UBotaniWeaponEquipmentInstance
 *
 * Piece of equipment that represents a weapon.
 */
UCLASS()
class BOTANIGAME_API UBotaniWeaponEquipmentInstance : public UBotaniEquipmentInstance
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UBotaniEquipmentInstance interface
	virtual void OnEquipped(const FGameplayEquipmentSpec& EquipmentSpec, const FGameplayInventoryItemContext& Context) override;
	virtual void OnUnequipped(const FGameplayEquipmentSpec& EquipmentSpec) override;
	//~ End UBotaniEquipmentInstance interface

	/**
	 * Called to activate a weapon mode.
	 *
	 * @returns true if the weapon mode was activated, false otherwise.
	 */
	virtual bool ActivateWeaponMode(const FGameplayTag& NewModeTag);

	/**
	 * Called to deactivate the currently active weapon mode.
	 */
	virtual void DeactivateWeaponMode(const FGameplayTag& OldModeTag);

	/**
	 * Called to get the currently active weapon mode.
	 */
	virtual const class UBotaniWeaponMode* GetActiveWeaponMode() const;

	/**
	 * Called to store the granted handles for a specific weapon mode.
	 * @param ModeTag		The tag of the weapon mode.
	 * @param GrantedHandles	The granted handles for the weapon mode.
	 */
	virtual void CacheModeSpecificHandles(const FGameplayTag& ModeTag, const FBotaniAbilitySet_GrantedHandles& GrantedHandles);

protected:
	/** Called when the active weapon mode changes. */
	UFUNCTION()
	virtual void OnRep_ActiveWeaponMode(FGameplayTag OldWeaponMode);
	
private:
	double TimeLastEquipped = 0.0f;
	double TimeLastFired = 0.0f;

private:
	/** Cache of granted handles per weapon mode. */
	UPROPERTY()
	TMap<FGameplayTag, FBotaniAbilitySet_GrantedHandles> GrantedModeHandles;

	/** The currently active weapon mode. */
	UPROPERTY(ReplicatedUsing = OnRep_ActiveWeaponMode)
	FGameplayTag ActiveModeTag;
};
