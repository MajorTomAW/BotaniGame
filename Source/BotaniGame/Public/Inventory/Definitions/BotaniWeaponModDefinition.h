// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniInventoryItemDefinition.h"
#include "BotaniWeaponModDefinition.generated.h"

/**
 * UBotaniWeaponModDefinition
 *
 * A modification that can be applied to a weapon.
 */
UCLASS(meta = (DisplayName = "Weapon Modification", ShortTooltip = "A modification that can be applied to a weapon."))
class BOTANIGAME_API UBotaniWeaponModDefinition : public UBotaniInventoryItemDefinition
{
	GENERATED_BODY()
};
