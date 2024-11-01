// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Weapons/Mods/WeaponMod_BouncingProjectile.h"

UWeaponMod_BouncingProjectile::UWeaponMod_BouncingProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Target = EBotaniWeaponModTarget::Transmitter;
}