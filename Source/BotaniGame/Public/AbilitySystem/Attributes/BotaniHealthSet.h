// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BotaniAttributeSet.h"
#include "BotaniHealthSet.generated.h"

/**
 * UBotaniHealthSet
 *
 * Health attribute set class for the project.
 * Used to define the health attribute and its functions for specific actors or players.
 */
UCLASS(BlueprintType)
class BOTANIGAME_API UBotaniHealthSet : public UBotaniAttributeSet
{
	GENERATED_BODY()

public:
	UBotaniHealthSet();

	/** Attribute accessors */
	ATTRIBUTE_ACCESSORS(ThisClass, Health);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, Shield);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxShield);
	ATTRIBUTE_ACCESSORS(ThisClass, Healing);
	ATTRIBUTE_ACCESSORS(ThisClass, Damage);

	/** Delegate when health changes due to damage/healing, some information may be missing on the client */
	mutable FBotaniAttributeEvent OnHealthChanged;

	/** Delegate when max health changes */
	mutable FBotaniAttributeEvent OnMaxHealthChanged;

	/** Delegate when shield changed due to damage/healing, some information may be missing on the client */
	mutable FBotaniAttributeEvent OnShieldChanged;

	/** Delegate when max shield changes */
	mutable FBotaniAttributeEvent OnMaxShieldChanged;

	/** Delegate to broadcast when the health attribute reaches zero */
	mutable FBotaniAttributeEvent OnOutOfHealth;

	/** Delegate to broadcast when the shield attribute reaches zero */
	mutable FBotaniAttributeEvent OnOutOfShield;

protected:
	/** OnRep function that gets called when the health attribute is replicated */
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);

	/** OnRep function that gets called when the max health attribute is replicated */
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	/** OnRep function that gets called when the shield attribute is replicated */
	UFUNCTION()
	virtual void OnRep_Shield(const FGameplayAttributeData& OldValue);

	/** OnRep function that gets called when the max shield attribute is replicated */
	UFUNCTION()
	virtual void OnRep_MaxShield(const FGameplayAttributeData& OldValue);

	/** Called to clamp the value of an attribute */
	virtual void ClampAttribute(const FGameplayAttribute& InAttribute, float& OutValue) const override;
	
	//~ Begin UAttributeSet Interface
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	//~ End UAttributeSet Interface

private:
	/**
	 * The current health attribute.
	 * Will be capped by the max health attribute.
	 * Health is hidden from modifiers as it should only be modified by executions.
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Botani|Health", meta = (HideFromModifiers, AllowPrivateAccess = "true"))
	FGameplayAttributeData Health;

	/**
	 * The current max health attribute.
	 * Works as an attribute since gameplay effects can modify it.
	 * Caps the health attribute.
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Botani|Health", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxHealth;

	/**
	 * The current shield attribute.
	 * Will be capped by the max shield attribute.
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Botani|Health", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Shield;

	/**
	 * The current max shield attribute.
	 * Works as an attribute since gameplay effects can modify it.
	 * Caps the shield attribute.
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Botani|Health", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxShield;
	

	
	/************************************************************************
	 * Meta Attributes (Non replicated Attributes that aren't 'stateful')														
	 ************************************************************************/

	/**
	 * The incoming healing attribute.
	 * Mapped directly to the health attribute as it is a meta attribute.
	 * Increases the health attribute when modified.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Botani|Health", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Healing;

	/**
	 * The incoming damage attribute.
	 * Mapped directly to the health attribute as it is a meta attribute.
	 * Decreases the health attribute when modified.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Botani|Health", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Damage;

	/** Used to track when the health reaches 0 */
	bool bOutOfHealth;

	/** Used to track when the shield reaches 0 */
	bool bOutOfShield;

	/** Store the health before any changes */
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;

	/** Store the shield before any changes */
	float MaxShieldBeforeAttributeChange;
	float ShieldBeforeAttributeChange;
};
