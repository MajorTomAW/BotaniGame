// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "GameFramework/CheatManager.h"
#include "BotaniPlayerController.generated.h"

/**
 * ABotaniPlayerController
 *
 * The base player controller class used by this project.
 */
UCLASS(Config = Game)
class BOTANIGAME_API ABotaniPlayerController : public ACommonPlayerController
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns the Botani player state for this controller. */
	UFUNCTION(BlueprintCallable, Category = "Botani|PlayerController")
	class ABotaniPlayerState* GetBotaniPlayerState() const;

	/** Returns the Botani ability system component for this controller. */
	UFUNCTION(BlueprintCallable, Category = "Botani|PlayerController")
	class UBotaniAbilitySystemComponent* GetBotaniAbilitySystemComponent() const;

protected:
	//~ Begin APlayerController interface
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End APlayerController interface
};
