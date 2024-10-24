// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "BotaniSettingsShared.generated.h"

class UBotaniLocalPlayer;
/**
 * UBotaniSettingsShared
 *
 * The "Shared" settings are stored as part of the USaveGame system, these settings aren't machine-specific
 * like the local settings, and are safe to store in the cloud – and 'share' them.
 * Using the save game system, we can also store settings per player, so things like controller keybinding preferences should go here,
 * because if those are stored in the local settings, all users would get them.
 */
UCLASS()
class BOTANIGAME_API UBotaniSettingsShared : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:
	UFUNCTION()
	bool GetInvertVerticalAxis() const { return bInvertVerticalAxis; }

	UFUNCTION()
	bool GetInvertHorizontalAxis() const { return bInvertHorizontalAxis; }

private:
	/************************************************************
	 * Inversion settings
	 ************************************************************/
	
	/** If true, the vertical look-axis will be inverted. */
	UPROPERTY()
	bool bInvertVerticalAxis = true;

	/** If true, the horizontal look-axis will be inverted. */
	UPROPERTY()
	bool bInvertHorizontalAxis = false;

	/************************************************************
	 * Sensitivity settings
	 ************************************************************/

	/** Holds the mouse horizontal sensitivity */
	UPROPERTY()
	double MouseSensitivityX = 1.0;

	/** Holds the mouse vertical sensitivity */
	UPROPERTY()
	double MouseSensitivityY = 1.0;

public:
	static UBotaniSettingsShared* LoadOrCreateSettings(const UBotaniLocalPlayer* LocalPlayer);
};
