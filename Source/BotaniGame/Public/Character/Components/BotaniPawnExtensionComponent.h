// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "GameFeatures/Data/BotaniPawnData.h"
#include "BotaniPawnExtensionComponent.generated.h"


class UBotaniAbilitySystemComponent;
/**
 * UBotaniPawnExtensionComponent
 *
 * Component that adds functionality to all Pawn classes, so it can be used for characters/vehicles/etc.
 * This coordinates the initialization of other components.
 */
UCLASS()
class BOTANIGAME_API UBotaniPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_UCLASS_BODY()

public:
	/** Static getter that returns the pawn extension component if one exists on the specified actor. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Pawn")
	static UBotaniPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UBotaniPawnExtensionComponent>() : nullptr); }
	
	/** The name of this overall feature, this one depends on the other named component features */
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

	/** Template function to get the pawn data */
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	/** Sets the current pawn data */
	UFUNCTION(BlueprintCallable, Category = "Botani|Pawn")
	void SetPawnData(const UBotaniPawnData* InPawnData);

	/** Gets the pawn data, which is used to specify pawn properties in data */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Pawn")
	UBotaniAbilitySystemComponent* GetBotaniAbilitySystemComponent() const { return AbilitySystemComponent; }

	/** Should be called by the owning pawn to become the avatar of the ability system. */
	void InitializeAbilitySystem(UBotaniAbilitySystemComponent* InASC, AActor* InOwnerActor);

	/** Should be called by the owning pawn to remove itself as the avatar of the ability system. */
	void UninitializeAbilitySystem();

	/** Should be called by the owning pawn when the pawn's controller changes. */
	void HandleControllerChanged();

	/** Should be called by the owning pawn when the player state has been replicated. */
	void HandlePlayerStateReplicated();

	/** Should be called by the owning pawn when the input component is setup. */
	void SetupPlayerInputComponent();

	/** Register with the OnAbilitySystemInitialized delegate and broadcast if our pawn has been registered with the ability system component */
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	/** Register with the OnAbilitySystemUninitialized delegate fired when our pawn is removed as the ability system's avatar actor */
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

protected:
	//~ Begin UAActorComponent Interface
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UAActorComponent Interface
	
	/** Delegate fired when our pawn becomes the ability system's avatar actor */
	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	/** Delegate fired when our pawn is removed as the ability system's avatar actor */
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

	UFUNCTION()
	void OnRep_PawnData();

protected:
	/** Pointer to the ability system component that is cached for easy access */
	UPROPERTY(Transient)
	TObjectPtr<UBotaniAbilitySystemComponent> AbilitySystemComponent;

	/** Replicated pawn data used to create the pawn. Specified from a spawn function or on a placed instance */
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "Botani|Pawn")
	TObjectPtr<const UBotaniPawnData> PawnData;
};
