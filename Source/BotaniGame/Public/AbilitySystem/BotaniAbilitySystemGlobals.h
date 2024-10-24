// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "BotaniAbilitySystemGlobals.generated.h"

/**
 * UBotaniAbilitySystemGlobals
 *
 * The global settings for the Botani ability system.
 */
UCLASS(Config = Game)
class BOTANIGAME_API UBotaniAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UAbilitySystemGlobals Interface
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	//~ End UAbilitySystemGlobals Interface
};
