// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Instances/BotaniWeaponItemInstance.h"

#include "Inventory/Definitions/BotaniWeaponDefinition.h"
#include "Net/UnrealNetwork.h"

UBotaniWeaponItemInstance::UBotaniWeaponItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

float UBotaniWeaponItemInstance::GetDistanceAttenuation(
	float Distance, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
	return 0.f;
}

float UBotaniWeaponItemInstance::GetPhysicalMaterialAttenuation(
	const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
	return 0.f;
}

void UBotaniWeaponItemInstance::OnInstanceCreated(
	const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext)
{
	Super::OnInstanceCreated(InHandle, InItemContext);

	UBotaniWeaponDefinition* WeaponDef = GetItemDefinition<UBotaniWeaponDefinition>();
	if (!ensure(WeaponDef))
	{
		return;
	}
}

void UBotaniWeaponItemInstance::OnInstanceRemoved(
	const FGameplayInventoryItemSpecHandle& InHandle, const FGameplayInventoryItemContext* InItemContext)
{
	Super::OnInstanceRemoved(InHandle, InItemContext);
}
