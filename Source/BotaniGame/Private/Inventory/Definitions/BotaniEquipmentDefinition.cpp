// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Definitions/BotaniEquipmentDefinition.h"

#include "Inventory/Instances/BotaniEquipmentInstance.h"

UBotaniEquipmentDefinition::UBotaniEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EquipmentInstanceClass = UBotaniEquipmentInstance::StaticClass();
	DefaultInstanceClass = UBotaniEquipmentInstance::StaticClass();
}
