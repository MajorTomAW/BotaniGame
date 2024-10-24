// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Attributes/BotaniCombatSet.h"

#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniCombatSet)


UBotaniCombatSet::UBotaniCombatSet()
	: BaseDamage(0.f)
	, BaseHeal(0.f)
{
}

void UBotaniCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

void UBotaniCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, BaseDamage, OldValue);
}

void UBotaniCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, BaseHeal, OldValue);
}
