// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BotaniAbilitySystemTypes.h"
#include "GameFramework/Actor.h"
#include "BotaniAbilityActor.generated.h"

/**
 * ABotaniAbilityActor
 *
 * A base class for all actors that are meant to use the Ability System.
 * It provides a way to lazy-load the Ability System for optimal performance.
 */
UCLASS(Blueprintable, BlueprintType, Abstract, meta = (ShortTooltip = "Base class for all Ability Actors that support lazy loading of Ability System."))
class BOTANIGAME_API ABotaniAbilityActor : public AActor, public IAbilitySystemInterface
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin AActor Interface
	virtual void PreInitializeComponents() override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	//~ End AActor Interface

	//~ Begin IAbilitySystemInterface Interface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface Interface
	
	/** Called to initialize the Ability System of this Actor. */
	void InitializeAbilitySystem();
	virtual void OnAbilitySystemInitialized() {}

	virtual void SetPendingAttributeFromReplication(const FGameplayAttribute& Attribute, const FGameplayAttributeData& NewValue);
	
protected:
	/** The Ability System component of this Actor */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ability System")
	TObjectPtr<class UBotaniAbilitySystemComponent> AbilitySystemComponent;
	
	/** The class of the Ability System component to create */
	UPROPERTY(EditDefaultsOnly, Category = "Ability System")
	TSubclassOf<class UBotaniAbilitySystemComponent> AbilitySystemClass;

	/** The creation policy of this ability system */
	UPROPERTY(EditDefaultsOnly, Category = "Ability System")
	EBotaniAbilitySystemCreationPolicy AbilitySystemCreationPolicy;

protected:
	virtual void ApplyPendingAttributeReplications();
	
private:
	/** Called to create the Ability System of this Actor. */
	virtual void CreateAbilitySystem();
	
	/** A transient ability system that is used for replication */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_TransientAbilitySystem)
	TObjectPtr<class UBotaniAbilitySystemComponent> TransientAbilitySystem;

	UFUNCTION()
	void OnRep_TransientAbilitySystem();
	
	/** Pending attribute replication data */
	TArray<struct FPendingAttributeReplication> PendingAttributeReplications;
};
