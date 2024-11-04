// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Definitions/BotaniWeaponDefinition.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/BotaniAbilitySet.h"
#include "Inventory/Instances/BotaniWeaponEquipmentInstance.h"
#include "Inventory/Instances/BotaniWeaponItemInstance.h"

//////////////////////////////////////////////////////////////////////////////
/// UBotaniWeaponMode

UBotaniWeaponMode::UBotaniWeaponMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBotaniWeaponMode::OnModeActivated(APawn* Avatar, const UBotaniWeaponDefinition* InWeaponDef, UBotaniWeaponEquipmentInstance* InWeaponEquipmentInstance) const
{
	check(Avatar && InWeaponDef && InWeaponEquipmentInstance);

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Avatar);
	UBotaniAbilitySystemComponent* BotaniASC = ASC ? Cast<UBotaniAbilitySystemComponent>(ASC) : nullptr;

	if (BotaniASC == nullptr)
	{
		return;
	}

	// Grant the weapon mode's abilities
	for (const auto& Set : AbilitySets)
	{
		if (Set == nullptr)
		{
			continue;
		}

		// Grant the ability set and cache the handles
		FBotaniAbilitySet_GrantedHandles Handles;
		Set->GiveToAbilitySystem(BotaniASC, &Handles, InWeaponEquipmentInstance);

		// Cache the granted handles in the equipment
		InWeaponEquipmentInstance->CacheModeSpecificHandles(WeaponModeTag, Handles);
	}
}

//////////////////////////////////////////////////////////////////////////
// UBotaniWeaponDefinition 

UBotaniWeaponDefinition::UBotaniWeaponDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAllowSecondaryFireToInterruptPrimaryFire = false;
	bAutoReload = true;
	bADSPreventsReload = false;

	ItemInstanceClass = UBotaniWeaponItemInstance::StaticClass();
}

bool UBotaniWeaponDefinition::ActivateWeaponMode(
	APawn* Avatar, const FGameplayTag& NewModeTag, UBotaniWeaponEquipmentInstance* InWeaponEquipmentInstance) const
{
	const UBotaniWeaponMode* Mode = FindWeaponMode(NewModeTag, true);
	if (Mode == nullptr)
	{
		return false;
	}

	Mode->OnModeActivated(Avatar, this, InWeaponEquipmentInstance);
	return true;
}

void UBotaniWeaponDefinition::DeactivateWeaponMode(
	APawn* Avatar, const FGameplayTag& OldModeTag, UBotaniWeaponEquipmentInstance* InWeaponEquipmentInstance) const
{
}

const UBotaniWeaponMode* UBotaniWeaponDefinition::FindWeaponMode(
	const FGameplayTag& ModeTag, const bool bExactMatch) const
{
	const UBotaniWeaponMode* FoundMode = nullptr;

	if (!ModeTag.IsValid())
	{
		return FoundMode;
	}

	for (const auto& Mode : WeaponModes)
	{
		if (Mode == nullptr || !Mode->WeaponModeTag.IsValid())
		{
			continue;
		}

		if (bExactMatch)
		{
			if (Mode->WeaponModeTag.MatchesTagExact(ModeTag))
			{
				FoundMode = Mode;
				break;
			}
		}
		else
		{
			if (Mode->WeaponModeTag.MatchesTag(ModeTag))
			{
				FoundMode = Mode;
				break;
			}
		}
	}

	return FoundMode;
}

const UBotaniWeaponMode* UBotaniWeaponDefinition::GetWeaponMode(const TSubclassOf<UBotaniWeaponMode> ModeClass) const
{
	for (const UBotaniWeaponMode* Mode : WeaponModes)
	{
		if (Mode == nullptr)
		{
			continue;
		}

		if (Mode->IsA(ModeClass))
		{
			return Mode;
		}
	}

	return nullptr;
}
