// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniTeamInfoBase.h"
#include "BotaniTeamPrivateInfo.generated.h"

UCLASS()
class BOTANIGAME_API ABotaniTeamPrivateInfo : public ABotaniTeamInfoBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABotaniTeamPrivateInfo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
