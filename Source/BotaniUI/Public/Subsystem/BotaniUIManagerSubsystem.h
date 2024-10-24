// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameUIManagerSubsystem.h"
#include "GameUIManagerSubsystem.h"
#include "BotaniUIManagerSubsystem.generated.h"

/**
 * UBotaniUIManagerSubsystem
 *
 * UIManager subsystem for BotaniGame.
 */
UCLASS(Config = Game)
class BOTANIUI_API UBotaniUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()

public:
	UBotaniUIManagerSubsystem();
	
	//~ Begin UGameUIManagerSubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End UGameUIManagerSubsystem Interface

protected:
	bool Tick(float DeltaTime);
	void SyncRootLayoutVisibilityToShowHUD();

	FTSTicker::FDelegateHandle TickHandle;
};
