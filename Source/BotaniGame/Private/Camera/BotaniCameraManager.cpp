// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Camera/BotaniCameraManager.h"

#include "Camera/Component/BotaniCameraComponent.h"
#include "Engine/Canvas.h"

ABotaniCameraManager::ABotaniCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set the default camera values.
	DefaultFOV = BOTANI_CAMERA_DEFAULT_FOV;
	ViewPitchMin = BOTANI_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = BOTANI_CAMERA_DEFAULT_PITCH_MAX;
}

void ABotaniCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	// If the UI Camera is looking at something, let it have priority.
	
	
	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void ABotaniCameraManager::DisplayDebug(class UCanvas* Canvas, const class FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::Yellow);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("BotaniCameraManager: %s"), *GetNameSafe(this)));

	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);
	if (const UBotaniCameraComponent* CameraComponent = UBotaniCameraComponent::FindCameraComponent(Pawn))
	{
		CameraComponent->DrawDebug(Canvas);
	}
}
