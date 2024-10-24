// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Player/BotaniLocalPlayer.h"

#include "Settings/BotaniSettingsShared.h"

UBotaniSettingsShared* UBotaniLocalPlayer::GetSharedSettings() const
{
	if (SharedSettings != nullptr)
	{
		return SharedSettings;
	}

	// COMPUTER can use sync load because it only checks the disk
	bool bCanLoadBeforeLogin = PLATFORM_DESKTOP;

	if (bCanLoadBeforeLogin)
	{
		SharedSettings = UBotaniSettingsShared::LoadOrCreateSettings(this);
	}
	else
	{
		//@TODO: For now its fine as we are not planning on using anything than desktop
	}

	return SharedSettings;
}
