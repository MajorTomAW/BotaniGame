// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Abilities/BotaniGameplayAbility_RangedWeapon.h"

#include "BotaniLogChannels.h"
#include "GameplayTags/BotaniGameplayTags.h"
#include "Inventory/Definitions/BotaniWeaponDefinition.h"
#include "Weapons/Modes/WeaponMode_RangedWeapon.h"
#include "Inventory/Instances/BotaniItemInstance.h"
#include "Inventory/Instances/BotaniWeaponEquipmentInstance.h"

UBotaniGameplayAbility_RangedWeapon::UBotaniGameplayAbility_RangedWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// AbilityTags.AddTag(BotaniGameplayTags::Ac)
	
	SourceBlockedTags.AddTag(BotaniGameplayTags::Gameplay::TAG_Gameplay_AbilityInputBlocked);
}

const UWeaponMode_RangedWeapon* UBotaniGameplayAbility_RangedWeapon::GetActiveRangedWeaponMode() const
{
	const UBotaniWeaponMode* ActiveMode = GetActiveWeaponMode();
	return ActiveMode ? Cast<UWeaponMode_RangedWeapon>(ActiveMode) : nullptr;
}

FBotaniWeaponStatData UBotaniGameplayAbility_RangedWeapon::GetRangedWeaponModeStats(bool& Success) const
{
	const UWeaponMode_RangedWeapon* Mode = GetActiveRangedWeaponMode();
	if (Mode == nullptr)
	{
		Success = false;
		return FBotaniWeaponStatData();
	}

	Success = true;
	return *Mode->WeaponStats.GetRow<FBotaniWeaponStatData>(TEXT(""));
}

bool UBotaniGameplayAbility_RangedWeapon::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	if (bResult)
	{
		if (GetWeaponDefinition() == nullptr || GetWeaponEquipmentInstance() == nullptr || GetActiveWeaponMode() == nullptr)
		{
			BOTANI_GAS_LOG(Error, TEXT("Weapon ability %s cannot be activated because the weapon definition [%s], instance [%s], or active mode [%s] is invalid."),
				*GetName(), GetWeaponDefinition() ? *GetWeaponDefinition()->GetPathName() : TEXT("null"),
				GetWeaponEquipmentInstance() ? *GetWeaponEquipmentInstance()->GetPathName() : TEXT("null"),
				GetActiveWeaponMode() ? *GetActiveWeaponMode()->GetPathName() : TEXT("null"));
			
			bResult = false;
		}
	}

	return bResult;
}

bool UBotaniGameplayAbility_RangedWeapon::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
	{
		return false;
	}

	// Check if the ability should automatically check for ammo
	if (!bAutoCheckAmmo)
	{
		return true;
	}

	const UWeaponMode_RangedWeapon* Mode = GetActiveWeaponMode<UWeaponMode_RangedWeapon>();
	if (Mode == nullptr)
	{
		return true; // Not sure abt this
	}

	const int32 AvailableAmmo = GetAssociatedItem()->GetGameplayTagStackCount(BotaniGameplayTags::Item::Weapon::Stat::TAG_WeaponStat_MagazineAmmo);
	const FBotaniWeaponStatData Stats = *Mode->WeaponStats.GetRow<FBotaniWeaponStatData>(TEXT(""));
	const uint8 AmmoCost = Stats.AmmoCostPerFire;

	return AvailableAmmo >= AmmoCost;
}

void UBotaniGameplayAbility_RangedWeapon::ApplyCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	if (!ActorInfo->IsNetAuthority())
	{
		return;
	}

	if (!bAutoConsumeAmmo)
	{
		return;
	}

	const UWeaponMode_RangedWeapon* Mode = GetActiveWeaponMode<UWeaponMode_RangedWeapon>();
	if (Mode == nullptr)
	{
		return;
	}

	const uint8 AmmoCost = Mode->WeaponStats.GetRow<FBotaniWeaponStatData>(TEXT(""))->AmmoCostPerFire;
	
	UBotaniItemInstance* ItemInstance =  GetAssociatedItem();
	if (ensure(ItemInstance))
	{
		ItemInstance->RemoveGameplayTagStack(BotaniGameplayTags::Item::Weapon::Stat::TAG_WeaponStat_MagazineAmmo, AmmoCost);
	}
}
