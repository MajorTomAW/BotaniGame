// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Abilities/BotaniGameplayAbility_Weapon.h"

#include "Inventory/Instances/BotaniItemInstance.h"
#include "Inventory/Instances/BotaniWeaponEquipmentInstance.h"
#include "Inventory/Instances/BotaniWeaponItemInstance.h"


UBotaniGameplayAbility_Weapon::UBotaniGameplayAbility_Weapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UBotaniWeaponDefinition* UBotaniGameplayAbility_Weapon::GetWeaponDefinition() const
{
	UBotaniItemInstance* Item = GetAssociatedItem();
	return Cast<UBotaniWeaponDefinition>(Item->GetItemDefinition());
}

UBotaniWeaponEquipmentInstance* UBotaniGameplayAbility_Weapon::GetWeaponEquipmentInstance() const
{
	return Cast<UBotaniWeaponEquipmentInstance>(GetAssociatedEquipment());
}

UBotaniWeaponItemInstance* UBotaniGameplayAbility_Weapon::GetWeaponItemInstance() const
{
	return Cast<UBotaniWeaponItemInstance>(GetAssociatedItem());
}

const UBotaniWeaponMode* UBotaniGameplayAbility_Weapon::GetActiveWeaponMode() const
{
	if (const UBotaniWeaponEquipmentInstance* WeaponInstance = GetWeaponEquipmentInstance())
	{
		return WeaponInstance->GetActiveWeaponMode();
	}

	return nullptr;
}
