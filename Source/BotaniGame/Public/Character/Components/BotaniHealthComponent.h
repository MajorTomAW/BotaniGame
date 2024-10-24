// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "BotaniHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBotaniDeathEventSignature, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FBotaniHealthAttributeChangedSignature, class UBotaniHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

//////////////////////////////////////////////////////////////////////////
/// EBotaniDeathState
//////////////////////////////////////////////////////////////////////////

struct FGameplayEffectSpec;
/**
 * EBotaniDeathState
 *
 *	Defines current state of death.
 */
UENUM(BlueprintType)
enum class EBotaniDeathState : uint8
{
	/** The actor is not dead, aka he is alive. */
    NotDead = 0,

	/** The actor is in the process of dying. */
	DeathStarted,

	/** The actor has finished dying. And is fully dead. */
    DeathFinished
};

//////////////////////////////////////////////////////////////////////////
/// UBotaniHealthComponent
//////////////////////////////////////////////////////////////////////////

/**
 * UBotaniHealthComponent
 *
 * An actor component used to handle anything related to health.
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent), Config = Game)
class BOTANIGAME_API UBotaniHealthComponent : public UGameFrameworkComponent
{
	GENERATED_UCLASS_BODY()

public:
	/** Static getter for the health component. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Health")
	static UBotaniHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UBotaniHealthComponent>() : nullptr); }

	/** Initialize the component using an ability system component. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Health")
	void InitializeWithAbilitySystem(class UBotaniAbilitySystemComponent* InASC);

	/** Uninitialize the component, clearing any references to the ability system. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Health")
	void UninitializeFromAbilitySystem();


	/** Begins the death process for the owner. */
	virtual void StartDeath();

	/** Ends the death process for the owner. */
	virtual void FinishDeath();

	/** Applies enough damage to kill the owner. */
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);
	


	/** Returns the current health value. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Health")
	float GetHealth() const;

	/** Returns the current maximum health value. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Health")
	float GetMaxHealth() const;

	/** Returns the current health in the range [0.0, 1.0]. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Health")
	float GetHealthNormalized() const;

	/** Returns the current death state. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Health")
	EBotaniDeathState GetDeathState() const { return DeathState; }

	/** Returns true if the actor is dead or in the process of dying. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > EBotaniDeathState::NotDead); }

public:
	/** Delegate fired when health value has changed. This is called on the client, but the instigator may not be valid */
	UPROPERTY(BlueprintAssignable, Category = "Botani|Health")
	FBotaniHealthAttributeChangedSignature OnHealthChanged;

	/** Delegate fired when max health value has changed. This is called on the client, but the instigator may not be valid */
	UPROPERTY(BlueprintAssignable, Category = "Botani|Health")
	FBotaniHealthAttributeChangedSignature OnMaxHealthChanged;

	/** Delegate fired when the death sequence has started. */
	UPROPERTY(BlueprintAssignable, Category = "Botani|Health")
	FBotaniDeathEventSignature OnDeathStarted;

	/** Delegate fired when the death sequence has finished. */
	UPROPERTY(BlueprintAssignable, Category = "Botani|Health")
	FBotaniDeathEventSignature OnDeathFinished;

protected:
	//~ Begin UActorComponent Interface
	virtual void OnUnregister() override;
	//~ End UActorComponent Interface

	void ClearGameplayTags() const;

	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	
protected:
	/** Replicated state used to determine if the actor is dead or not. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DeathState, Category = "Botani|Health")
	EBotaniDeathState DeathState;

	UFUNCTION()
	virtual void OnRep_DeathState(EBotaniDeathState OldDeathState);

private:
	/** Pointer to the ability system used to handle health changes. */
	UPROPERTY()
	TObjectPtr<class UBotaniAbilitySystemComponent> AbilitySystemComponent;

	/** Pointer to the health set used to define the health attributes. */
	UPROPERTY()
	TObjectPtr<const class UBotaniHealthSet> HealthSet;
};
