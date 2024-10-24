// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModularPawn.h"
#include "Teams/BotaniTeamAgentInterface.h"
#include "BotaniPawn.generated.h"

/**
 * ABotaniPawn
 *
 * The base class for all Botani pawns.
 */
UCLASS(Config = Game, meta = (ShortTooltip = "The base pawn class that supports team assignment."))
class BOTANIGAME_API ABotaniPawn : public AModularPawn, public IBotaniTeamAgentInterface
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin AActor interface
	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor interface

	//~ Begin APawn interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	//~ End APawn interface

	//~ Begin IBotaniTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnBotaniTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~ End IBotaniTeamAgentInterface interface

protected:
	/** Called to determine what happens to the team ID when possession ends */
	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
	{
		// This could be changed to return, e.g., OldTeamID if you want to keep it assigned afterwards, or return an ID for some neutral faction, or etc...
		return FGenericTeamId::NoTeam;
	}

private:
	UPROPERTY()
	FOnBotaniTeamIndexChangedDelegate OnTeamChangedDelegate;
	
	/** Replicated team ID */
	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);

	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);
};
