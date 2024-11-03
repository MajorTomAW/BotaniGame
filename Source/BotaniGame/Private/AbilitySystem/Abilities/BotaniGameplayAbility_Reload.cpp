// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Abilities/BotaniGameplayAbility_Reload.h"

#include "GameplayTags/BotaniGameplayTags.h"
#include "Inventory/Definitions/BotaniAmmoDefinition.h"
#include "Inventory/Instances/BotaniWeaponEquipmentInstance.h"
#include "Inventory/Instances/BotaniWeaponItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniGameplayAbility_Reload)

UBotaniGameplayAbility_Reload::UBotaniGameplayAbility_Reload(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(BotaniGameplayTags::Ability::Descriptor::Action::TAG_AbilityAction_Reload);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = BotaniGameplayTags::Input::TAG_InputTag_Reload;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UBotaniGameplayAbility_Reload::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(BotaniGameplayTags::Ability::Restrict::TAG_AbilityRestrict_Weapon_PrimaryFire, 1);
	}
}

void UBotaniGameplayAbility_Reload::EndAbility(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(BotaniGameplayTags::Ability::Restrict::TAG_AbilityRestrict_Weapon_PrimaryFire, 1);
	}
}

bool UBotaniGameplayAbility_Reload::CheckCost(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
	{
		return false;
	}

	// We can't reload if the magazine is full
	if (IsMagazineFull())
	{
		OptionalRelevantTags->AddTag(BotaniGameplayTags::Ability::ActivationFail::TAG_AbilityActivationFail_MagazineFull);
		return false;
	}

	// If we have no spare ammo, we can't reload
	if (GetSpareAmmo() <= 0)
	{
		OptionalRelevantTags->AddTag(BotaniGameplayTags::Ability::ActivationFail::TAG_AbilityActivationFail_OutOfAmmo);
		return false;
	}

	return true;
}

void UBotaniGameplayAbility_Reload::ApplyCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}

int32 UBotaniGameplayAbility_Reload::GetSpareAmmo() const
{
	if (UGameplayInventoryManager* InventoryManager = GetInventoryManager())
	{
		UBotaniAmmoDefinition* AmmoToUse = GetWeaponEquipmentInstance()->GetActiveWeaponMode()->GetAmmoToConsume();
		if (AmmoToUse == nullptr)
		{
			return INDEX_NONE;
		}

		return InventoryManager->GetTotalItemCountByDefinition(AmmoToUse);
	}

	return INDEX_NONE;
}

int32 UBotaniGameplayAbility_Reload::GetMagazineAmmo() const
{
	return GetWeaponItemInstance()->GetGameplayTagStackCount(BotaniGameplayTags::Item::Weapon::Stat::TAG_WeaponStat_MagazineAmmo);
}

bool UBotaniGameplayAbility_Reload::IsMagazineFull() const
{
	const int32 MagazineAmmo = GetMagazineAmmo();
	const int32 MagazineSize = GetWeaponItemInstance()->GetGameplayTagStackCount(BotaniGameplayTags::Item::Weapon::Stat::TAG_WeaponStat_MagazineSize);
	return MagazineAmmo >= MagazineSize;
}
