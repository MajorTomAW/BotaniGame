// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModularAIController.h"
#include "Teams/BotaniTeamAgentInterface.h"
#include "BotaniBotCoreController.generated.h"

namespace EAIFocusPriority
{
	typedef uint8 Type;

	/** Custom focus flag. */	
	inline constexpr Type Custom = 3;
}

/**
 * ABotaniBotCoreController
 *
 *	The controller class used by Botani bots in this project.
 */
UCLASS(Blueprintable, Config = Game, Abstract)
class BOTANIGAME_API ABotaniBotCoreController : public AModularAIController, public IBotaniTeamAgentInterface
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin IBotaniTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnBotaniTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//~ End of IBotaniTeamAgentInterface interface

	//~ Begin AController interface
	virtual void OnUnPossess() override;
	//~ End AController interface

	//~ Begin AActor interface
	virtual void PostInitializeComponents() override;
	//~ End AActor interface

	void ServerRestartController();

protected:
	/** The perception component for this bot */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAIPerceptionComponent> BotPerceptionComponent;

	/** The Utility Manager for this bot */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (DisplayName = "AI Utility Manager"))
	TObjectPtr<class UAIUtilityManager> UtilityManager;

private:
	/** Delegate for when the player state changes team */
	UPROPERTY()
	FOnBotaniTeamIndexChangedDelegate OnTeamIndexChangedDelegate;

	/** The last player state that was seen by this bot */
	UPROPERTY()
	TObjectPtr<APlayerState> LastSeenPlayerState;
};
