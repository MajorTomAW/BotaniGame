// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Weapons/Modes/WeaponMode_RangedWeapon.h"

#include "GameplayTags/BotaniGameplayTags.h"
#include "Instance/GameplayInventoryItemInstance.h"
#include "Inventory/Instances/BotaniWeaponEquipmentInstance.h"

UWeaponMode_RangedWeapon::UWeaponMode_RangedWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Weapon Mode
	WeaponModeTag = BotaniGameplayTags::Item::Weapon::Mode::TAG_WeaponMode_Ranged;
	
	// Magazine Size
	{
		FBotaniRangedWeaponAmmoData Data;
		Data.StatName = "Magazine Size";
		Data.StatTag = BotaniGameplayTags::Item::Weapon::Stat::TAG_WeaponStat_MagazineSize;
		Data.bIsConstStat = true;
		Data.StackCount = 30;

		AmmoStats.Add(Data);
	}

	// Magazine Ammo
	{
		FBotaniRangedWeaponAmmoData Data;
		Data.StatName = "Magazine Ammo";
		Data.StatTag = BotaniGameplayTags::Item::Weapon::Stat::TAG_WeaponStat_MagazineAmmo;
		Data.bIsConstStat = false;
		Data.StackCount = 0;

		AmmoStats.Add(Data);
	}

	/*// Spare Ammo @note: Disabled for now, Spare Ammo will be handled by the Inventory System
	{
		FBotaniRangedWeaponAmmoData Data;
		Data.StatName = "Spare Ammo";
		Data.StatTag = BotaniGameplayTags::Item::Weapon::Stat::TAG_WeaponStat_SpareAmmo;
		Data.bIsConstStat = false;
		Data.StackCount = 0;

		AmmoStats.Add(Data);
	}*/

#if WITH_EDITORONLY_DATA
	WeaponModeName = TEXT("Mode: Ranged Weapon");
#endif
}

float UWeaponMode_RangedWeapon::GetLowAmmoPercentageNormalized() const
{
	return LowAmmoPercentage / 100.0f;
}

FBotaniRangedWeaponAmmoData UWeaponMode_RangedWeapon::GetAmmoStat(const FGameplayTag StatTag, bool& Success) const
{
	for (const auto& AmmoStat : AmmoStats)
	{
		if (AmmoStat.StatTag == StatTag)
		{
			Success = true;
			return AmmoStat;
		}
	}

	Success = false;
	return FBotaniRangedWeaponAmmoData();
}

void UWeaponMode_RangedWeapon::OnModeActivated(
	APawn* Avatar, const UBotaniWeaponDefinition* InWeaponDef, UBotaniWeaponEquipmentInstance* InWeaponEquipmentInstance) const
{
	UGameplayInventoryItemInstance* Instance = CastChecked<UGameplayInventoryItemInstance>(InWeaponEquipmentInstance->GetInstigator());

	for (const auto& KVP : AmmoStats)
	{
		// Only apply const stats
		if (!KVP.bIsConstStat)
		{
			continue;
		}

		Instance->SetGameplayTagStack(KVP.StatTag, KVP.StackCount);
		UE_LOG(LogTemp, Display, TEXT("Set const stat %s to %d"), *KVP.StatName.ToString(), KVP.StackCount);
	}
	
	Super::OnModeActivated(Avatar, InWeaponDef, InWeaponEquipmentInstance);
}
