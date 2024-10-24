// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffect.h"
#include "BotaniAttributeSet.generated.h"

class UBotaniAbilitySystemComponent;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

#define LAZY_ATTRIBUTE_REPNOTIFY(ClassName, PropertyName, OldValue) \
{ \
	static FProperty* ThisProperty = FindFieldChecked<FProperty>(ClassName::StaticClass(), GET_MEMBER_NAME_CHECKED(ClassName, PropertyName)); \
	if (!GetOwningAbilitySystemComponent()) \
	{ \
		if (ABotaniAbilityActor* AbilityActor = Cast<ABotaniAbilityActor>(GetOwningActor()) \
		{ \
			AbilityActor->SetPendingAttributeFromReplication(ThisProperty, OldValue); \
		} \
	} \
	else \
	{ \
		GetOwningAbilitySystemComponent()->SetBaseAttributeValueFromReplication(FGameplayAttribute(ThisProperty), PropertyName, OldValue); \
	} \
}


/**
 * Delegate used to broadcast attribute events, some of these parameters may be null on clients:
 * @param EffectInstigator	The original instigating actor for this event
 * @param EffectCauser		The physical actor that caused the change
 * @param EffectSpec		The full effect spec for this change
 * @param EffectMagnitude	The raw magnitude, this is before clamping
 * @param OldValue			The value of the attribute before it was changed
 * @param NewValue			The value after it was changed
 */
DECLARE_MULTICAST_DELEGATE_SixParams(FBotaniAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);


/**
 * UBotaniAttributeSet
 *
 * Base attribute set class for the project.
 */
UCLASS(Abstract)
class BOTANIGAME_API UBotaniAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBotaniAttributeSet();
	virtual UBotaniAbilitySystemComponent* GetBotaniAbilitySystemComponent() const;
	virtual void ClampAttribute(const FGameplayAttribute& InAttribute, float& OutValue) const;

	//~ Begin UObject Interface
	virtual UWorld* GetWorld() const override;
	//~ End UObject Interface
};
