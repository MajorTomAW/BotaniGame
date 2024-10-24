// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ModularPlayerState.h"
#include "AbilitySystem/BotaniAbilitySystemTypes.h"
#include "GameFeatures/Data/BotaniPawnData.h"
#include "Teams/BotaniTeamAgentInterface.h"
#include "BotaniPlayerState.generated.h"

class UBotaniExperienceDefinition;
class UBotaniAbilitySystemComponent;
/**
 * ABotaniPlayerState
 *
 * Base player state class used by this project.
 */
UCLASS(Config = Game)
class BOTANIGAME_API ABotaniPlayerState : public AModularPlayerState, public IAbilitySystemInterface, public IBotaniTeamAgentInterface
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Botani|PlayerState")
	UBotaniAbilitySystemComponent* GetBotaniAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//~ Begin APlayerState interface
	virtual void ClientInitialize(AController* Controller) override;
	virtual void PostInitializeComponents() override;
	//~ End APlayerState interface

	/** Returns the replicated view rotation for the player. */
	FORCEINLINE virtual FRotator GetReplicatedViewRotation() const { return ReplicatedViewRotation; }

	/** Sets the replicated view rotation for the player and marks it as dirty. */
	virtual void SetReplicatedViewRotation(const FRotator& NewRotation);
	

public:
	/**
	 * Template function to get the pawn data for the player.
	 * @tparam T The type of the pawn data to get.
	 */
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	/** Sets the pawn data for the player. Only the server should call this. */
	void SetPawnData(const UBotaniPawnData* InPawnData);

	static const FName NAME_BotaniAbilityReady;
	
protected:
	/** The pawn data for the player. */
	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UBotaniPawnData> PawnData;
	
	UFUNCTION()
	void OnRep_PawnData();

private:
	void OnExperienceLoaded(const UBotaniExperienceDefinition* CurrentExperience);
	
private:
	/** The ability system component for the player. */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBotaniAbilitySystemComponent> AbilitySystemComponent;

	/** Attribute set for health. */
	UPROPERTY()
	TObjectPtr<const class UBotaniHealthSet> HealthSet;

	/** Attribute set for combat. */
	UPROPERTY()
	TObjectPtr<const class UBotaniCombatSet> CombatSet;

	/** Movement set for the player movement. */
	UPROPERTY()
	TObjectPtr<const class UBotaniMovementSet> MovementSet;

	/** Damage set for damage. */
	UPROPERTY()
	TObjectPtr<const class UBotaniDamageSet> DamageSet;

private:
	/** The team ID for the player. There will mostly be two teams, Player (0) and Bots (1). */
	UPROPERTY(ReplicatedUsing = OnRep_MyTeamId)
	FGenericTeamId MyTeamId;

	/** The squad ID for the player. Each player and bot can belong to a squad. */
	UPROPERTY(ReplicatedUsing = OnRep_MySquadId)
	int32 MySquadId;

	/** The replicated view rotation for the player. */
	UPROPERTY(Replicated)
	FRotator ReplicatedViewRotation;

	UFUNCTION()
	void OnRep_MyTeamId(FGenericTeamId OldTeamId);

	UFUNCTION()
	void OnRep_MySquadId();
};
