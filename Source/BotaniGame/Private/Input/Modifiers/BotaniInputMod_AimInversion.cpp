// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Input/Modifiers/BotaniInputMod_AimInversion.h"

#include "Input/Modifiers/BotaniInputModHelpers.h"
#include "Player/BotaniLocalPlayer.h"
#include "Settings/BotaniSettingsShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniInputMod_AimInversion)


FInputActionValue UBotaniInputMod_AimInversion::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	UBotaniLocalPlayer* LocalPlayer = BotaniInputModHelpers::GetLocalPlayer(PlayerInput);
	if (LocalPlayer == nullptr)
	{
		return CurrentValue;
	}

	UBotaniSettingsShared* Settings = LocalPlayer->GetSharedSettings();
	ensure(Settings);

	FVector NewValue = CurrentValue.Get<FVector>();

	if (Settings->GetInvertVerticalAxis())
	{
		NewValue.Y *= -1.0f;
	}

	if (Settings->GetInvertHorizontalAxis())
	{
		NewValue.X *= -1.0f;
	}

	return NewValue;
}
