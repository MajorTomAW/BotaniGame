// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputModifiers.h"
#include "BotaniInputMod_AimInversion.generated.h"

/**
 * Applies an inversion of axis values based on a user setting. 
 */
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Aim Inversion Setting"))
class UBotaniInputMod_AimInversion : public UInputModifier
{
	GENERATED_BODY()

protected:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
};
