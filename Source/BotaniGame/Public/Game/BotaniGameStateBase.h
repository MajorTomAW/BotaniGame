// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ModularGameState.h"
#include "BotaniGameStateBase.generated.h"

class UBotaniAbilitySystemComponent;
class UBotaniExperienceManagerComponent;

/**
 * ABotaniGameStateBase
 *
 * The base game state class used by this project.
 */
UCLASS(Config = Game)
class BOTANIGAME_API ABotaniGameStateBase : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin AActor Interface
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
	//~ End of AActor Interface

	//~ Begin IAbilitySystemInterface Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End of IAbilitySystemInterface Interface

	/** Gets the server*s FPS, replicated to clients */
	float GetServerFPS() const { return ServerFPS; }

protected:
	UPROPERTY(Replicated)
	float ServerFPS;

private:
	/** Handles loading and managing the current gameplay experience */
	UPROPERTY()
	TObjectPtr<UBotaniExperienceManagerComponent> ExperienceManagerComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBotaniAbilitySystemComponent> AbilitySystemComponent;
};
