// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Definitions/BotaniWeaponModDefinition.h"

#include "Weapons/Mods/Instances/BotaniWeaponModInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniWeaponModDefinition)

UBotaniWeaponModDefinition::UBotaniWeaponModDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Target = EBotaniWeaponModTarget::Source;
	ItemInstanceClass = UBotaniWeaponModInstance::StaticClass();
}