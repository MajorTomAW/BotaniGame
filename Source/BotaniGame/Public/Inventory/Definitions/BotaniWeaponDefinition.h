// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniInventoryItemDefinition.h"
#include "BotaniWeaponDefinition.generated.h"

/**
 * FBotaniReticleWidget
 *
 * Stores data about a reticle widget.
 */
USTRUCT(BlueprintType)
struct FBotaniReticleWidgetData
{
	GENERATED_BODY()

	FBotaniReticleWidgetData()
		: HUDExtensionPoint(FGameplayTag::EmptyTag)
		, ReticleWidgetClass(nullptr)
	{
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle", meta = (Categories = "HUD.Slot"))
	FGameplayTag HUDExtensionPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle", meta = (AllowedClasses = "/Script/BotaniUI.BotaniReticleWidget"))
	TSoftClassPtr<class UUserWidget> ReticleWidgetClass;
};

/**
 * FBotaniReticleData
 *
 * Stores data about all reticles and other reticle-related data.
 */
USTRUCT(BlueprintType)
struct FBotaniReticleData
{
	GENERATED_BODY()
	
	FBotaniReticleData()
		: bShowHitNotification(true)
		, HitNotificationDuration(0.5f)
	{
	}


public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle", meta = (TitleProperty = "HUDExtensionPoint"))
	TArray<FBotaniReticleWidgetData> ReticleWidgets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
	uint8 bShowHitNotification : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle", meta = (EditCondition = "bShowHitNotification"))
	float HitNotificationDuration;
};

/**
 * UBotaniWeaponMode
 *
 * A definition of a weapon mode that can be equipped by a player.
 */
UCLASS(Abstract, BlueprintType, Const, EditInlineNew, DefaultToInstanced)
class UBotaniWeaponMode : public UObject
{
	GENERATED_UCLASS_BODY()
	friend class UBotaniWeaponDefinition;
	
public:
	/** The tag that represents this weapon mode */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon Mode")
	FGameplayTag WeaponModeTag;

	/** List of ability sets that are granted when this weapon mode is equipped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TArray<TObjectPtr<class UBotaniAbilitySet>> AbilitySets;

protected:
	virtual void OnModeActivated(APawn* Avatar, const class UBotaniWeaponDefinition* InWeaponDef, class UBotaniWeaponEquipmentInstance* InWeaponEquipmentInstance) const;

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, meta = (EditCondition = "false", EditConditionHides))
	FString WeaponModeName;
#endif
};

/**
 * UBotaniWeaponDefinition
 *
 * A definition of a weapon that can be equipped by a player.
 */
UCLASS(meta = (DisplayName = "Weapon Definition", ShortTooltip = "A definition of a weapon that can be equipped by a player."))
class BOTANIGAME_API UBotaniWeaponDefinition : public UBotaniInventoryItemDefinition
{
	GENERATED_UCLASS_BODY()

public:
	/**
	 * Called to activate a weapon mode.
	 * @param Avatar		The pawn that is activating the weapon mode. 
	 * @param NewModeTag	The tag of the weapon mode to activate.
	 * @param InWeaponEquipmentInstance		The weapon equipment instance that is activating the weapon mode.
	 */
	virtual bool ActivateWeaponMode(APawn* Avatar, const FGameplayTag& NewModeTag, class UBotaniWeaponEquipmentInstance* InWeaponEquipmentInstance) const;

	/**
	 * Called to deactivate a weapon mode.
	 * @param Avatar		The pawn that is deactivating the weapon mode. 
	 * @param OldModeTag	The tag of the weapon mode to deactivate.
	 * @param InWeaponEquipmentInstance		The weapon equipment instance that is deactivating the weapon mode.
	 */
	virtual void DeactivateWeaponMode(APawn* Avatar, const FGameplayTag& OldModeTag, class UBotaniWeaponEquipmentInstance* InWeaponEquipmentInstance) const;


	/**
	 * Returns the specific weapon mode associated with the given tag.
	 * @param ModeTag		The tag of the weapon mode to find.
	 * @param bExactMatch	Whether to require an exact match for the tag.
	 * @returns The weapon mode associated with the given tag, or nullptr if not found.
	 */
	virtual const UBotaniWeaponMode* FindWeaponMode(const FGameplayTag& ModeTag, const bool bExactMatch = true) const;

	/**
	 * Returns the specific weapon mode associated with the given tag.
	 * @param ModeClass		The class of the weapon mode to find.
	 * @returns The weapon mode associated with the given tag, or nullptr if not found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Weapon", meta = (DisplayName = "Find Weapon Mode", DeterminesOutputType = "ModeClass"))
	const UBotaniWeaponMode* GetWeaponMode(const TSubclassOf<UBotaniWeaponMode> ModeClass) const;
	
public:
	/******************************************************************************************************
	 * FIRING
	 ******************************************************************************************************/
	
	/** Whether the secondary fire can interrupt the primary fire */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Firing")
	uint8 bAllowSecondaryFireToInterruptPrimaryFire : 1;

	/** Whether the weapon should automatically reload when empty */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Firing")
	uint8 bAutoReload : 1;

	/** Whether the weapon should prevent reloading while ADS */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Firing", meta = (DisplayName = "Prevent Reload While ADS"))
	uint8 bADSPreventsReload : 1;

	/******************************************************************************************************
	 * RETICLE
	 ******************************************************************************************************/

	/** Stores data about all reticles and other reticle-related data */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
	FBotaniReticleData ReticleData;

	/******************************************************************************************************
	 * WEAPON MODES
	 ******************************************************************************************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Weapon Modes", meta = (TitleProperty = "WeaponModeName"))
	TArray<TObjectPtr<UBotaniWeaponMode>> WeaponModes;
};
