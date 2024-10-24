// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "BotaniCheatManager.generated.h"

#ifndef USING_CHEAT_MANAGER
#define USING_CHEAT_MANAGER (1 && !UE_BUILD_SHIPPING)
#endif 

/**
 * UBotaniCheatManager
 *
 * Base chat manager class used by this project.
 */
UCLASS(Config = Game, Within = "PlayerController")
class BOTANICHEATS_API UBotaniCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UBotaniCheatManager();

	//~ Begin UCheatManager Interface
	virtual void InitCheatManager() override;
	//~ End UCheatManager Interface
};
