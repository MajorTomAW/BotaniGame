// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniInventoryItemDefinition.h"
#include "BotaniAmmoDefinition.generated.h"



/**
 * UBotaniAmmoDefinition
 *
 * Ammo definition that a weapon can use.
 */
UCLASS(meta = (DisplayName = "Ammo Definition", ShortTooltip = "Ammo definition that a weapon can use."))
class BOTANIGAME_API UBotaniAmmoDefinition : public UBotaniInventoryItemDefinition
{
	GENERATED_UCLASS_BODY()
};
