// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BotaniWeaponHostWidget.generated.h"


class UBotaniWeaponEquipmentInstance;

UCLASS(MinimalAPI)
class UBotaniWeaponHostWidget : public UCommonUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	//~ Being UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget Interface

	/** Called when the weapon has changed */
	UFUNCTION(BlueprintImplementableEvent, Category = "Botani|Weapons", meta = (DisplayName = "On Weapon Changed"))
	void OnWeaponChanged_BP(UBotaniWeaponEquipmentInstance* OldWeapon, UBotaniWeaponEquipmentInstance* NewWeapon);

private:
	/** Called to rebuild the widget from the current weapon */
	void RebuildWidgetFromWeapon();

private:
	UPROPERTY(Transient)
	TObjectPtr<UBotaniWeaponEquipmentInstance> CurrentWeapon;
};
