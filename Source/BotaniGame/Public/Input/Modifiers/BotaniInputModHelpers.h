// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once
#include "EnhancedPlayerInput.h"
#include "Player/BotaniLocalPlayer.h"

namespace BotaniInputModHelpers
{
	/** Returns the owning BotaniLocalPlayer of the given EnhancedPlayerInput. */
	static UBotaniLocalPlayer* GetLocalPlayer(const UEnhancedPlayerInput* PlayerInput)
	{
		if (PlayerInput == nullptr)
		{
			return nullptr;
		}

		if (APlayerController* PlayerController = Cast<APlayerController>(PlayerInput->GetOuter()))
		{
			return Cast<UBotaniLocalPlayer>(PlayerController->GetLocalPlayer());
		}

		return nullptr;
	}
}
