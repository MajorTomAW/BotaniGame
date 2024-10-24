// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/BotaniAbilitySystemGlobals.h"

#include "AbilitySystem/Effects/BotaniGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniAbilitySystemGlobals)

UBotaniAbilitySystemGlobals::UBotaniAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UBotaniAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FBotaniGameplayEffectContext();
}
