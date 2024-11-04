// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniInventoryItemDefinition.h"
#include "BotaniWeaponModDefinition.generated.h"

/**
 * EBotaniWeaponModTarget
 *
 * Defines what the target of the weapon modification is.
 */
UENUM(BlueprintType)
enum class EBotaniWeaponModTarget : uint8
{
	/** The weapon itself. */
	Source,

	/** The target that was hit by the weapon. */
	Target,

	/** The transmitting object, such as a projectile. */
	Transmitter,
};

/**
 * UBotaniWeaponModDefinition
 *
 * A modification that can be applied to a weapon.
 */
UCLASS(meta = (DisplayName = "Weapon Modification", ShortTooltip = "A modification that can be applied to a weapon."), HideCategories = ("Equipment", "Item|Equipment"))
class BOTANIGAME_API UBotaniWeaponModDefinition : public UBotaniInventoryItemDefinition
{
	GENERATED_UCLASS_BODY()

public:

protected:
	/** The target of the weapon modification. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Modification")
	EBotaniWeaponModTarget Target;
};
