// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Definitions/BotaniInventoryItemDefinition.h"

#include "Inventory/Instances/BotaniItemInstance.h"

UBotaniInventoryItemDefinition::UBotaniInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemInstanceClass = UBotaniItemInstance::StaticClass();
	Rarity = EBotaniItemRarity::Common;
	bHasDurability = false;
}
