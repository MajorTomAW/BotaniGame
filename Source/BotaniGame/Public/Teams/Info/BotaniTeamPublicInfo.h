// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniTeamInfoBase.h"
#include "Teams/Data/BotaniTeamDisplayAsset.h"
#include "BotaniTeamPublicInfo.generated.h"

class UBotaniTeamDisplayAsset;

UCLASS(MinimalAPI)
class ABotaniTeamPublicInfo : public ABotaniTeamInfoBase
{
	GENERATED_UCLASS_BODY()

public:
	UBotaniTeamDisplayAsset* GetTeamDisplayAsset() const { return TeamDisplayAsset; }

protected:
	virtual void SetTeamDisplayAsset(TObjectPtr<UBotaniTeamDisplayAsset> NewDisplayAsset);

private:
	/** Replicated team display asset */
	UPROPERTY(ReplicatedUsing = OnRep_TeamDisplayAsset)
	TObjectPtr<UBotaniTeamDisplayAsset> TeamDisplayAsset;

	UFUNCTION()
	void OnRep_TeamDisplayAsset();
};
