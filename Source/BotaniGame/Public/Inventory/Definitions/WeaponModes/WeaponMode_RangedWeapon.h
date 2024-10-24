// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Definitions/BotaniWeaponDefinition.h"
#include "WeaponMode_RangedWeapon.generated.h"

/**
 * FBotaniWeaponSpreadData
 *
 * Stores data about the spread of a weapon.
 */
USTRUCT(BlueprintType)
struct FBotaniWeaponSpreadData
{
	GENERATED_BODY()

public:
	/**
	 * Spread exponent affects how tightly shots will cluster around the center line
	 * when the weapon has spread (non-perfect accuracy).
	 * Higher values will cause shots to be closer to the center line.
	 * (default is 1.0 which means uniformly within the spread cone)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float SpreadExponent = 1.0f;

	/**
	 * The time since firing before the spread starts to recover.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread", meta = (ClampMin = 0.0f, UIMin = 0.0f, Units = "s"))
	float SpreadRecoveryCooldownDelay = 0.f;

	/**
	 * Should the weapon have perfect accuracy when both player and weapon spread are at their minimum value?
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	uint8 bAllowFirstShotAccuracy : 1 = false;

	/**
	 * A curve that maps the heat to the spread angle.
	 * 
	 * – The X axis of this curve typically sets the min/max heat range of the weapon.
	 * 
	 * – The Y axis of this curve is used to define the min and maximum spread angle.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	FRuntimeFloatCurve HeatToSpreadCurve;

	/**
	 * A curve that maps the current heat to the amount a single shot will further 'heat up'.
	 * This is typically a flat curve with a single data point indicating how much heat a shot adds.
	 *
	 * This can be other shapes to do things like punish overheating by adding progressively more heat.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	FRuntimeFloatCurve HeatToHeatPerShotCurve;

	/**
	 * A curve that maps the current heat to the heat cooldown rate per second.
	 * This is typically a flat curve with a single data point indicating how fast the heat wears off,
	 * but there can be other shapes to do things like punish overheating by slowing down recovery at a high heat.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	FRuntimeFloatCurve HeatToCooldownPerSecondCurve;

	/** The multiplier for the spread when standing still. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread|Multiplier", meta = (ClampMin = 0.0f, UIMin = 0.0f, ForceUnits = "x"))
	float StandingStillSpreadMultiplier = 1.0f;

	/** The multiplier for the spread when walking. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread|Multiplier", meta = (ClampMin = 0.0f, UIMin = 0.0f, ForceUnits = "x"))
	float WalkingSpreadMultiplier = 1.0f;

	/** The multiplier for the spread when crouching. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread|Multiplier", meta = (ClampMin = 0.0f, UIMin = 0.0f, ForceUnits = "x"))
	float CrouchingSpreadMultiplier = 1.0f;

	/** The multiplier for the spread when sprinting. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread|Multiplier", meta = (ClampMin = 0.0f, UIMin = 0.0f, ForceUnits = "x"))
	float SprintingSpreadMultiplier = 1.0f;

	/** The multiplier for the spread when falling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread|Multiplier", meta = (ClampMin = 0.0f, UIMin = 0.0f, ForceUnits = "x"))
	float FallingSpreadMultiplier = 1.0f;
};

/**
 * FBotaniWeaponStatData
 *
 * Stores data for the stats of a weapon.
 */
USTRUCT(BlueprintType)
struct FBotaniWeaponStatData : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** Bullets that are shot per cartridge. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = 1, UIMin = 1))
	uint8 BulletsPerCartridge = 1;

	/** Ammo cost per fire. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = 0, UIMin = 0))
	uint8 AmmoCostPerFire = 1;

	/** The rate of fire. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Firing", meta = (ClampMin = 0.0f, UIMin = 0.0f, Units = "RevolutionsPerMinute"))
	float FiringRate = 600.0f;

	/** The sweep radius. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Firing", meta = (ClampMin = 0.0f, UIMin = 0.0f, Units = "cm"))
	float SweepRadius = 0.0f;

	/** The spread data. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Data")
	FBotaniWeaponSpreadData SpreadData;
};

/**
 * FBotaniRangedWeaponAmmoData
 *
 * Stores data for the ammo of a ranged weapon.
 */
USTRUCT(BlueprintType)
struct FBotaniRangedWeaponAmmoData
{
	GENERATED_BODY()

	/** The stat tag for the ammo. */
	UPROPERTY(BlueprintReadOnly, Category = "Ammo", meta = (Categories = "Weapon.Stat"))
	FGameplayTag StatTag;

#if WITH_EDITORONLY_DATA
	/** The name of this stat. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Ammo")
	FName StatName;
#endif

	/** The stack count of the ammo. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (UIMin = 0, ClampMin = -1))
	int32 StackCount = 0;

	/** Determines if this is a const stat or if it can be changed during gameplay. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (DisplayName = "Const ?"))
	uint32 bIsConstStat : 1 = false;
};

/**
 * EBotaniRangedWeaponFireMethod
 *
 * Determines the fire method of a ranged weapon.
 */
UENUM()
enum EBotaniRangedWeaponFireMethod
{
	/** Custom fire method. */
	FireMethod_Custom = 1 << 0 UMETA(DisplayName = "Custom"),
	
	/** Shoots a projectile that travels in a straight line. */
	FireMethod_Projectile = 1 << 1 UMETA(DisplayName = "Projectile"),

	/** Shoots a HitScan that instantly hits the target. */
	FireMethod_HitScan = 1 << 2 UMETA(DisplayName = "Hit Scan"),
};

/**
 * UWeaponMode_RangedWeapon
 */
UCLASS(meta = (DisplayName = "Ranged Weapon"))
class BOTANIGAME_API UWeaponMode_RangedWeapon : public UBotaniWeaponMode
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns the low ammo percentage normalized to 0-1. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Weapon")
	float GetLowAmmoPercentageNormalized() const;

	/** Returns a specific ammo stat. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Weapon")
	FBotaniRangedWeaponAmmoData GetAmmoStat(const FGameplayTag StatTag, bool& Success) const;

public:
	/** The stats for the weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (RowType = "/Script/BotaniGame.BotaniWeaponStatData"))
	FDataTableRowHandle WeaponStats;
	
	/** The fire method of the weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Firing")
	TEnumAsByte<EBotaniRangedWeaponFireMethod> FireMethod = FireMethod_Projectile;

	/** The projectile class to spawn when firing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Firing", meta = (EditCondition = "FireMethod == EBotaniRangedWeaponFireMethod::FireMethod_Projectile", EditConditionHides))
	TSoftClassPtr<class ABotaniProjectile> ProjectileClass;
	
	/** The ammo stats for the weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo", EditFixedSize, meta = (EditFixedOrder, TitleProperty = "StatName", DisplayAfter = "Weapon"))
	TArray<FBotaniRangedWeaponAmmoData> AmmoStats;

	/** The ammo type that the weapon uses. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	TObjectPtr<class UBotaniAmmoDefinition> AmmoType;

	/** Determines at what percentage the weapon is considered "low on ammo". */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 100.0f, UIMax = 100.0f, Units = "%"))
	float LowAmmoPercentage = 20.0f;

	/** Determines whether the ammo should be displayed as percentage instead of a number. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	uint32 bShowAmmoAsPercentage : 1 = false;

protected:
	virtual void OnModeActivated(APawn* Avatar, const class UBotaniWeaponDefinition* InWeaponDef, class UBotaniWeaponEquipmentInstance* InWeaponEquipmentInstance) const override;
};
