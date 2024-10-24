// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BotaniAttributeSet.h"
#include "BotaniCombatSet.generated.h"

/**
 * UBotaniCombatSet
 *
 * Class for combat-related attributes.
 */
UCLASS(BlueprintType)
class BOTANIGAME_API UBotaniCombatSet : public UBotaniAttributeSet
{
	GENERATED_BODY()

public:
	UBotaniCombatSet();

	/** Attribute accessors */
	ATTRIBUTE_ACCESSORS(ThisClass, BaseDamage);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseHeal);

protected:
	/** OnRep function that gets called when the base damage attribute is replicated */
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	/** OnRep function that gets called when the base heal attribute is replicated */
	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

private:
	/**
	 * The base amount of damage to apply in the damage execution calculation.
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Botani|Combat", Meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData BaseDamage;

	/**
	 * The base amount of healing to apply in the heal execution calculation.
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Botani|Combat", Meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData BaseHeal;
};
