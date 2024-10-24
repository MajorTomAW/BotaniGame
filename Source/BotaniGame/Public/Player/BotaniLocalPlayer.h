// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonLocalPlayer.h"
#include "BotaniLocalPlayer.generated.h"

class UBotaniSettingsShared;
/**
 * UBotaniLocalPlayer
 *
 * Local player class for Botani, that each player has their own instance of. 
 */
UCLASS()
class BOTANIGAME_API UBotaniLocalPlayer : public UCommonLocalPlayer
{
	GENERATED_BODY()

public:
	/** Gets the shared setting for this player, this is read using the save game system so may not be correct until after user login */
	UFUNCTION()
	UBotaniSettingsShared* GetSharedSettings() const;

private:
	UPROPERTY(Transient)
	mutable TObjectPtr<UBotaniSettingsShared> SharedSettings;
};
