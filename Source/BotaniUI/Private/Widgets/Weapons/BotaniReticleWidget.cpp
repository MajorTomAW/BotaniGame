// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Widgets/Weapons/BotaniReticleWidget.h"

#include "Inventory/Instances/BotaniItemInstance.h"
#include "Inventory/Instances/BotaniWeaponEquipmentInstance.h"

UBotaniReticleWidget::UBotaniReticleWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FadeDuration = 0.f;
}

void UBotaniReticleWidget::InitializeFromWeapon(class UBotaniWeaponEquipmentInstance* InWeaponInstance)
{
	WeaponInstance = InWeaponInstance;
	InventoryInstance = nullptr;
	if (WeaponInstance)
	{
		InventoryInstance = Cast<UBotaniItemInstance>(WeaponInstance->GetInstigator());
	}

	OnWeaponInitialized_BP();
}

void UBotaniReticleWidget::RestartFadeTimer()
{
	// Reset and start the fade timer
	if (FadeDuration <= 0.f)
	{
		return;
	}
	
	if (FadeTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
	}

	GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::OnFadeOut_BP), FadeDuration, false);
	OnFadeIn_BP();
}

void UBotaniReticleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (FadeDuration <= 0.f)
	{
		return;
	}

	RestartFadeTimer();
}

void UBotaniReticleWidget::NativeDestruct()
{
	Super::NativeDestruct();

	FadeTimerHandle.Invalidate();
}
