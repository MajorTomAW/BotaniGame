// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Widgets/Weapons/BotaniWeaponHostWidget.h"

#include "Inventory/Components/BotaniEquipmentManager.h"
#include "Inventory/Instances/BotaniWeaponEquipmentInstance.h"
#include "Library/InventorySystemBlueprintLibrary.h"

UBotaniWeaponHostWidget::UBotaniWeaponHostWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentWeapon = nullptr;
}

void UBotaniWeaponHostWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBotaniWeaponHostWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UBotaniWeaponHostWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const APawn* Pawn = GetOwningPlayerPawn();
	if (Pawn == nullptr)
	{
		return;
	}

	UBotaniEquipmentManager* EquipmentManager = CastChecked<UBotaniEquipmentManager>(UInventorySystemBlueprintLibrary::FindEquipmentManager(Pawn));
	if (EquipmentManager == nullptr)
	{
		return;
	}

	if (UBotaniWeaponEquipmentInstance* NewWeapon = EquipmentManager->GetFirstInstanceOfType<UBotaniWeaponEquipmentInstance>())
	{
		if ((NewWeapon != CurrentWeapon) &&
			(NewWeapon->GetInstigator() != nullptr))
		{
			UBotaniWeaponEquipmentInstance* OldWeapon = CurrentWeapon;
			CurrentWeapon = NewWeapon;
			RebuildWidgetFromWeapon();
			OnWeaponChanged_BP(OldWeapon, CurrentWeapon);
		}
	}
	else
	{
		// Weapon was unequipped but none was equipped
		if (CurrentWeapon != nullptr)
		{
			UBotaniWeaponEquipmentInstance* OldWeapon = CurrentWeapon;
			CurrentWeapon = nullptr;
			RebuildWidgetFromWeapon();
			OnWeaponChanged_BP(OldWeapon, CurrentWeapon);
		}
	}
}

void UBotaniWeaponHostWidget::RebuildWidgetFromWeapon()
{
}
