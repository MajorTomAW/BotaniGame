// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniEquipmentAbility.h"
#include "Inventory/Definitions/BotaniWeaponDefinition.h"
#include "BotaniWeaponAbility.generated.h"

class UBotaniWeaponItemInstance;
/**
 * 
 */
UCLASS(MinimalAPI, meta = (ShortTooltip = "An ability granted by and associated with a weapon instance.", DisplayName = "Weapon Ability"))
class UBotaniWeaponAbility : public UBotaniEquipmentAbility
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns the weapon definition associated with this ability */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Ability")
	const UBotaniWeaponDefinition* GetWeaponDefinition() const;

	/** Returns the weapon equipment instance associated with this ability */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Ability")
	UBotaniWeaponEquipmentInstance* GetWeaponEquipmentInstance() const;

	/** Returns  the weapon item instance associated with this ability */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Ability")
	UBotaniWeaponItemInstance* GetWeaponItemInstance() const;

	/** Returns the currently active weapon mode */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Ability")
	const UBotaniWeaponMode* GetActiveWeaponMode() const;

protected:
	/** Returns the weapon definition associated with this ability */
	template <typename T>
	const T* GetWeaponDefinition() const;

	/** Returns the weapon instance associated with this ability */
	template <typename T>
	T* GetWeaponEquipmentInstance() const;

	/** Returns the weapon item instance associated with this ability */
	template <typename T>
	T* GetWeaponItemInstance() const;
	
	/** Returns the currently active weapon mode */
	template <typename T>
	const T* GetActiveWeaponMode() const;
};

template <typename T>
const T* UBotaniWeaponAbility::GetWeaponDefinition() const
{
	return Cast<T>(GetAssociatedItem());
}

template <typename T>
T* UBotaniWeaponAbility::GetWeaponEquipmentInstance() const
{
	return Cast<T>(GetAssociatedEquipment());
}

template <typename T>
T* UBotaniWeaponAbility::GetWeaponItemInstance() const
{
	return Cast<T>(GetAssociatedItem());
}

template <typename T>
const T* UBotaniWeaponAbility::GetActiveWeaponMode() const
{
	return GetActiveWeaponMode() ? Cast<T>(GetActiveWeaponMode()) : nullptr;
}
