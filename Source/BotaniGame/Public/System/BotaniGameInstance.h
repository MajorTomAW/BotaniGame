// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameInstance.h"

#include "BotaniGameInstance.generated.h"

/**
 * UBotaniGameInstance
 *
 * Single-ton object that handles global game instance data.
 */
UCLASS(Config = Game)
class BOTANIGAME_API UBotaniGameInstance : public UCommonGameInstance
{
	GENERATED_UCLASS_BODY()

public:
	virtual void HandlerUserInitialized(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext) override;

protected:
	//~ Begin UCommonInstance Interface
	virtual void Init() override;
	//~ End UCommonInstance Interface
};
