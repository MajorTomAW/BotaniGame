// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Attributes/BotaniAttributeSet.h"
#include "AbilitySystem/BotaniAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniAttributeSet)

UBotaniAttributeSet::UBotaniAttributeSet()
{
}

UBotaniAbilitySystemComponent* UBotaniAttributeSet::GetBotaniAbilitySystemComponent() const
{
	return CastChecked<UBotaniAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

void UBotaniAttributeSet::ClampAttribute(const FGameplayAttribute& InAttribute, float& OutValue) const
{
}

UWorld* UBotaniAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}
