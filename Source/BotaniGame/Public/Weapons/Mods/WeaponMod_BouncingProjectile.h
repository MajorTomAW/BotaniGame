// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Definitions/BotaniWeaponModDefinition.h"
#include "WeaponMod_BouncingProjectile.generated.h"

/**
 * 
 */
UCLASS()
class UWeaponMod_BouncingProjectile : public UBotaniWeaponModDefinition
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Modification|Bouncing")
	TArray<TEnumAsByte<EPhysicalSurface>> BounceSurfaces;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Modification|Bouncing")
	float Bounciness = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Modification|Bouncing")
	int32 MaxBounces = INDEX_NONE;
};
