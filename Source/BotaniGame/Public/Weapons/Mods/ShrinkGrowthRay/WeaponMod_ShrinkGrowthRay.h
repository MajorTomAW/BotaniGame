// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Definitions/BotaniWeaponModDefinition.h"
#include "WeaponMod_ShrinkGrowthRay.generated.h"

struct FGameplayInventoryItemSpecHandle;
/**
 * EBShrinkGrowthState
 *
 * Enum defining shrink states.
 */
UENUM(BlueprintType)
enum class EBotaniShrinkGrowthState : uint8
{
	/** Normal state. */
	Normal = 0,

	/** Target is shrinking. */
	Shrinking,

	/** Target is growing. */
	Growing,

	/** Target has finished shrinking and is now shrunk. */
	Shrunk,

	/** Target has finished growing and is now grown. */
	Grown
};

/**
 * EBotaniShrinkGrowthAction
 *
 */
UENUM(BlueprintType)
enum class EBotaniShrinkGrowthAction : uint8
{
	None = 0,
	Shrinking,
	Growing,
};


UCLASS(Abstract)
class UWeaponMod_ShrinkGrowthRayBase : public UBotaniWeaponModDefinition
{
	GENERATED_UCLASS_BODY()

public:
	virtual EBotaniShrinkGrowthAction GetDesiredAction(const FGameplayInventoryItemSpecHandle& InHandle);
	virtual float GetDesiredTargetScale() const;

protected:
	/** Determines the time it takes to shrink/grow the target. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Modification|ShrinkGrowth", meta = (Units = "s"))
	float SizeChangeTime = 1.0f;

	/** Determines the maximum duration of the shrink/growth effect. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Modification|ShrinkGrowth", meta = (Units = "s"))
	float SizeChangeDuration = 10.0f;
};


UCLASS()
class UWeaponMod_ShrinkRay : public UWeaponMod_ShrinkGrowthRayBase
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UBotaniWeaponModDefinition Interface
	virtual EBotaniShrinkGrowthAction GetDesiredAction(const FGameplayInventoryItemSpecHandle& InHandle) override;
	virtual float GetDesiredTargetScale() const override { return DesiredShrinkScale; }
	//~ End UBotaniWeaponModDefinition Interface

protected:
	/** Determines the desired scale for the target when finished shrinking. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Modification|Shrink", meta = (Units = "x"))
	float DesiredShrinkScale = 0.1f;
};


UCLASS()
class UWeaponMod_GrowthRay : public UWeaponMod_ShrinkGrowthRayBase
{
	GENERATED_UCLASS_BODY()
	
public:
	//~ Begin UBotaniWeaponModDefinition Interface
	virtual EBotaniShrinkGrowthAction GetDesiredAction(const FGameplayInventoryItemSpecHandle& InHandle) override;
	virtual float GetDesiredTargetScale() const override { return DesiredGrowthScale; }
	//~ End UBotaniWeaponModDefinition Interface

protected:
	/** Determines the desired scale for the target when finished growing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Modification|Growth", meta = (Units = "x"))
	float DesiredGrowthScale = 2.0f;
};


UCLASS()
class UWeaponMod_ShrinkGrowthRay : public UWeaponMod_ShrinkGrowthRayBase
{
	GENERATED_UCLASS_BODY()
};