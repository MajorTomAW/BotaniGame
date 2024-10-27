// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Weapons/Components/BotaniWeaponStateComponent.h"

#include "Inventory/Components/BotaniEquipmentManager.h"
#include "Inventory/Instances/BotaniWeaponEquipmentInstance.h"
#include "Inventory/Instances/BotaniWeaponItemInstance.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniWeaponStateComponent)

UBotaniWeaponStateComponent::UBotaniWeaponStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UBotaniWeaponStateComponent::TickComponent(
	float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (APawn* OwnerPawn = GetPawn<APawn>())
	{
		if (UBotaniEquipmentManager* EquipmentManager = OwnerPawn->FindComponentByClass<UBotaniEquipmentManager>())
		{
			UBotaniWeaponEquipmentInstance* CurrentWeapon = EquipmentManager->GetFirstInstanceOfType<UBotaniWeaponEquipmentInstance>();
			if (CurrentWeapon == nullptr)
			{
				return;
			}

			CurrentWeapon->TickWeapon(DeltaTime);
		}
	}
}
