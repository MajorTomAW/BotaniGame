// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Settings/BotaniSettingsShared.h"
#include "Player/BotaniLocalPlayer.h"

static FString SHARED_SETTINGS_SLOT_NAME = TEXT("SharedGameSettings");


UBotaniSettingsShared* UBotaniSettingsShared::LoadOrCreateSettings(const UBotaniLocalPlayer* LocalPlayer)
{
	UBotaniSettingsShared* SharedSettings = Cast<UBotaniSettingsShared>(LoadOrCreateSaveGameForLocalPlayer(StaticClass(), LocalPlayer, SHARED_SETTINGS_SLOT_NAME));
	// SharedSettings->ApplySettings(); @TODO
	return SharedSettings;
}
