// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Abilities/BotaniWeaponAbility.h"

#include "Inventory/Instances/BotaniItemInstance.h"
#include "Inventory/Instances/BotaniWeaponEquipmentInstance.h"
#include "Inventory/Instances/BotaniWeaponItemInstance.h"


UBotaniWeaponAbility::UBotaniWeaponAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UBotaniWeaponDefinition* UBotaniWeaponAbility::GetWeaponDefinition() const
{
	UBotaniItemInstance* Item = GetAssociatedItem();
	return Cast<UBotaniWeaponDefinition>(Item->GetItemDefinition());
}

UBotaniWeaponEquipmentInstance* UBotaniWeaponAbility::GetWeaponEquipmentInstance() const
{
	return Cast<UBotaniWeaponEquipmentInstance>(GetAssociatedEquipment());
}

UBotaniWeaponItemInstance* UBotaniWeaponAbility::GetWeaponItemInstance() const
{
	return Cast<UBotaniWeaponItemInstance>(GetAssociatedItem());
}

const UBotaniWeaponMode* UBotaniWeaponAbility::GetActiveWeaponMode() const
{
	if (const UBotaniWeaponEquipmentInstance* WeaponInstance = GetWeaponEquipmentInstance())
	{
		return WeaponInstance->GetActiveWeaponMode();
	}

	return nullptr;
}
