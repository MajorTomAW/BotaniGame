// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Camera/Modes/CameraMode_DefaultFirstPerson.h"

#include "Character/BotaniCharacter.h"
#include "Character/Components/Movement/BotaniMovementComponent.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetMathLibrary.h"

UCameraMode_DefaultFirstPerson::UCameraMode_DefaultFirstPerson()
{
}

void UCameraMode_DefaultFirstPerson::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	const FRotator OldRot = View.Rotation;
	const FVector OldLoc = View.Location;

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	// Wall run camera til
	if (const ABotaniCharacter* BotaniCharacter = Cast<ABotaniCharacter>(GetTargetActor()))
	{
		float WallRunTilt = 0.0f;
		
		// If the character is wall running, tilt the camera
		if (BotaniCharacter->IsWallRunning())
		{
			FHitResult WallHit;
			BotaniCharacter->TraceWallsToRunOn(WallHit, Wall_Both);
			
			const FVector RightVector = BotaniCharacter->GetActorRightVector();
			const float Dot = FVector::DotProduct(WallHit.ImpactNormal, RightVector);
			WallRunTilt = Dot * WallRunCameraTiltAngle;
		}

		// Smoothly interpolate the camera tilt
		View.Rotation.Roll = UKismetMathLibrary::FInterpTo(OldRot.Roll, WallRunTilt, DeltaTime, WallRunTiltInterpSpeed);

		if (BotaniCharacter->GetMovementComponent()->IsMovingOnGround())
		{
			float SlideTilt = 0.0f;
			
			// If the character is sliding, tilt the camera
			if (BotaniCharacter->IsSliding())
			{
				const FVector RotatedOffset = SlideCameraOffset.RotateAngleAxis(PivotRotation.Yaw, FVector::UpVector);
				SlideTilt = RotatedOffset.Z;
			}
			
			View.Location.Z = UKismetMathLibrary::FInterpTo(OldLoc.Z, PivotLocation.Z + SlideTilt, DeltaTime, SlideTiltInterpSpeed);	
		}
	}
}

void UCameraMode_DefaultFirstPerson::DrawDebug(UCanvas* Canvas) const
{
	Super::DrawDebug(Canvas);
	
	check(Canvas);
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetDrawColor(FColor::White);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("		Camera Tilt: %s (%f)"), *GetName(), View.Rotation.Roll));
	DisplayDebugManager.DrawString(FString::Printf(TEXT("		Wall Run Tilt: %s (%f)"), *GetName(), WallRunCameraTiltAngle));
}
