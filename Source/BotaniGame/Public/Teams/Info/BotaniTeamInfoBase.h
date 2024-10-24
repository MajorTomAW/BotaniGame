// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "GameplayTagStackContainer.h"
#include "BotaniTeamInfoBase.generated.h"

UCLASS(Abstract)
class BOTANIGAME_API ABotaniTeamInfoBase : public AInfo
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns the team ID for this team */
	int32 GetTeamId() const { return TeamId; }

	//~ Begin AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor interface

	/** The team tags for this team */
	UPROPERTY(Replicated)
	FGameplayTagStackContainer TeamTags;

protected:
	virtual void RegisterWithTeamSubsystem(class UBotaniTeamSubsystem* Subsystem);
	void TryRegisterWithTeamSubsystem();

private:
	/** Sets the team ID for this team */
	void SetTeamId(int32 NewTeamId);
	
	/** The team ID for this team */
	UPROPERTY(ReplicatedUsing = OnRep_TeamId)
	int32 TeamId;

	
	UFUNCTION()
	virtual void OnRep_TeamId();
};
