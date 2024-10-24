// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Instance/GameplayEquipmentInstance.h"
#include "BotaniEquipmentInstance.generated.h"

class UBotaniAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class BOTANIGAME_API UBotaniEquipmentInstance : public UGameplayEquipmentInstance
{
	GENERATED_UCLASS_BODY()

protected:
	UBotaniAbilitySystemComponent* GetBotaniASC() const;
};
