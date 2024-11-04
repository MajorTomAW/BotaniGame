// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Weapons/Mods/ShrinkGrowthRay/WeaponMod_ShrinkGrowthRay.h"

#include "Weapons/Mods/ShrinkGrowthRay/WeaponModInstance_ShrinkGrowthRay.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WeaponMod_ShrinkGrowthRay)

UWeaponMod_ShrinkGrowthRayBase::UWeaponMod_ShrinkGrowthRayBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemInstanceClass = UWeaponModInstance_ShrinkGrowthRay::StaticClass();
	Target = EBotaniWeaponModTarget::Target;
}

EBotaniShrinkGrowthAction UWeaponMod_ShrinkGrowthRayBase::GetDesiredAction(
	const FGameplayInventoryItemSpecHandle& InHandle)
{
	return EBotaniShrinkGrowthAction::None;
}

float UWeaponMod_ShrinkGrowthRayBase::GetDesiredTargetScale() const
{
	return 1.0f;
}

UWeaponMod_ShrinkRay::UWeaponMod_ShrinkRay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

EBotaniShrinkGrowthAction UWeaponMod_ShrinkRay::GetDesiredAction(const FGameplayInventoryItemSpecHandle& InHandle)
{
	return EBotaniShrinkGrowthAction::Shrinking;
}

UWeaponMod_GrowthRay::UWeaponMod_GrowthRay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

EBotaniShrinkGrowthAction UWeaponMod_GrowthRay::GetDesiredAction(const FGameplayInventoryItemSpecHandle& InHandle)
{
	return EBotaniShrinkGrowthAction::Growing;
}

UWeaponMod_ShrinkGrowthRay::UWeaponMod_ShrinkGrowthRay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}