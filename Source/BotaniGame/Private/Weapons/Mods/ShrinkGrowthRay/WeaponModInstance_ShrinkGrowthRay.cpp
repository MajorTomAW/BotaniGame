// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Weapons/Mods/ShrinkGrowthRay/WeaponModInstance_ShrinkGrowthRay.h"

#include "Weapons/Mods/ShrinkGrowthRay/WeaponMod_ShrinkGrowthRay.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WeaponModInstance_ShrinkGrowthRay)

UWeaponModInstance_ShrinkGrowthRay::UWeaponModInstance_ShrinkGrowthRay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UWeaponModInstance_ShrinkGrowthRay::OnInstanceCreated(
	const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext)
{
	Super::OnInstanceCreated(InHandle, InItemContext);

	TotalSizeChangeTime = 10.f;
	DesiredAction = GetItemDefinition<UWeaponMod_ShrinkGrowthRayBase>()->GetDesiredAction(InHandle);
	SizeChangeStartTime = GetWorld()->GetTimeSeconds();
}

void UWeaponModInstance_ShrinkGrowthRay::OnInstanceRemoved(
	const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext)
{
	Super::OnInstanceRemoved(InHandle, InItemContext);
}

void UWeaponModInstance_ShrinkGrowthRay::TickWeaponMod(
	float DeltaTime, UBotaniWeaponEquipmentInstance* WeaponEquipmentInstance, APawn* OwningPawn)
{
	// If the total size change time is less than or equal to 0, then return.
	if (TotalSizeChangeTime <= 0.f)
	{
		return;
	}
	
	if (DesiredAction == EBotaniShrinkGrowthAction::Shrinking)
	{
		DoShrink(DeltaTime);
	}

	if (DesiredAction == EBotaniShrinkGrowthAction::Growing)
	{
		DoGrowth(DeltaTime);
	}

	if (GetRemainingShrinkTime() <= 0.f)
	{
		//@TODO: Clear shrinking state
	}
}

void UWeaponModInstance_ShrinkGrowthRay::DoShrink(float DeltaTime)
{
	CurrentAction = EBotaniShrinkGrowthAction::Shrinking;

	const FVector NewScale = FVector(GetItemDefinition<UWeaponMod_ShrinkGrowthRayBase>()->GetDesiredTargetScale());
	GetOwningActor()->SetActorScale3D(NewScale);
}

void UWeaponModInstance_ShrinkGrowthRay::DoGrowth(float DeltaTime)
{
	CurrentAction = EBotaniShrinkGrowthAction::Growing;

	const FVector NewScale = FVector(GetItemDefinition<UWeaponMod_ShrinkGrowthRayBase>()->GetDesiredTargetScale());
	GetOwningActor()->SetActorScale3D(NewScale);
}

float UWeaponModInstance_ShrinkGrowthRay::GetRemainingShrinkTime() const
{
	return FMath::Max(0.0f, TotalSizeChangeTime - (GetWorld()->GetTimeSeconds() - SizeChangeStartTime));
}
