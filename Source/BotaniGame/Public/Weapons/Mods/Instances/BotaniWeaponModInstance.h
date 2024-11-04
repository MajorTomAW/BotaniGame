// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Instances/BotaniItemInstance.h"
#include "BotaniWeaponModInstance.generated.h"

class UBotaniWeaponEquipmentInstance;
/**
 * 
 */
UCLASS()
class BOTANIGAME_API UBotaniWeaponModInstance : public UBotaniItemInstance
{
	GENERATED_BODY()

public:
	virtual void TickWeaponMod(float DeltaTime, UBotaniWeaponEquipmentInstance* WeaponEquipmentInstance, APawn* OwningPawn);
};
