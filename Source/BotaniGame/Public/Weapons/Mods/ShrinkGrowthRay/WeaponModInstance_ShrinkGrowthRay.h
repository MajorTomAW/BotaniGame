// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WeaponMod_ShrinkGrowthRay.h"
#include "Weapons/Mods/Instances/BotaniWeaponModInstance.h"
#include "WeaponModInstance_ShrinkGrowthRay.generated.h"

/**
 * 
 */
UCLASS()
class UWeaponModInstance_ShrinkGrowthRay : public UBotaniWeaponModInstance
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UGameplayInventoryItemInstance Interface
	virtual void OnInstanceCreated(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext) override;
	virtual void OnInstanceRemoved(const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext) override;
	//~ End UGameplayInventoryItemInstance Interface

	//~ Begin UBotaniWeaponModInstance Interface
	virtual void TickWeaponMod(float DeltaTime, UBotaniWeaponEquipmentInstance* WeaponEquipmentInstance, APawn* OwningPawn) override;
	//~ End UBotaniWeaponModInstance Interface

protected:
	virtual void DoShrink(float DeltaTime);
	virtual void DoGrowth(float DeltaTime);

	float GetRemainingShrinkTime() const;

private:
	EBotaniShrinkGrowthAction DesiredAction = EBotaniShrinkGrowthAction::None;
	EBotaniShrinkGrowthAction CurrentAction = EBotaniShrinkGrowthAction::None;

	/** Time that keeps track of how long the shrink/growth effect has been applied. */
	double TotalSizeChangeTime = 0.0f;
	double SizeChangeStartTime = 0.0f;
};
