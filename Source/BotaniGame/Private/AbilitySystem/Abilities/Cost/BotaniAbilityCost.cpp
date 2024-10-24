// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Abilities/Cost/BotaniAbilityCost.h"

UBotaniAbilityCost::UBotaniAbilityCost(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bOnlyApplyCostOnHit = false;

/*#if WITH_EDITORONLY_DATA
	EditorDisplayName = K2_GetDisplayName();
#endif*/
}

bool UBotaniAbilityCost::CheckCost(const UBotaniGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	return K2_CheckCost(Ability, Handle, *ActorInfo, *OptionalRelevantTags);
}

bool UBotaniAbilityCost::K2_CheckCost_Implementation(const UBotaniGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, FGameplayTagContainer& OptionalRelevantTags) const
{
	return true;
}

void UBotaniAbilityCost::ApplyCost(const UBotaniGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	K2_ApplyCost(Ability, Handle, *ActorInfo, ActivationInfo);
}

/*
#if WITH_EDITORONLY_DATA
FText UBotaniAbilityCost::K2_GetDisplayName_Implementation() const
{
	return FText::FromString(TEXT("Ability Cost"));
}
#endif
*/
