// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "BotaniCameraManager.generated.h"

#define BOTANI_CAMERA_DEFAULT_FOV			(80.0f)
#define BOTANI_CAMERA_DEFAULT_PITCH_MIN		(-89.0f)
#define BOTANI_CAMERA_DEFAULT_PITCH_MAX		(89.0f)

/**
 * ABotaniCameraManager
 *
 * The base player camera manager class used by this project.
 */
UCLASS(NotPlaceable, MinimalAPI)
class ABotaniCameraManager : public APlayerCameraManager
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
	virtual void DisplayDebug(class UCanvas* Canvas, const class FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
};
