// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/BotaniCameraMode.h"
#include "CameraMode_DefaultFirstPerson.generated.h"

/**
 * UCameraMode_DefaultFirstPerson
 *
 * A basic first-person (fps) camera mode.
 */
UCLASS(Abstract, Blueprintable, MinimalAPI)
class UCameraMode_DefaultFirstPerson : public UBotaniCameraMode
{
	GENERATED_BODY()

public:
	UCameraMode_DefaultFirstPerson();

protected:
	//~ Begin UBotaniCameraMode Interface
	virtual void UpdateView(float DeltaTime) override;
	virtual void DrawDebug(UCanvas* Canvas) const override;
	//~ End UBotaniCameraMode Interface

protected:
	/** The camera tilt to be applied while wall running. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Run", meta = (ClampMin = "0.0", Units = "deg"))
	float WallRunCameraTiltAngle = 10.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Run", meta = (ClampMin = "0.0", Units = "deg/s"))
	float WallRunTiltInterpSpeed = 1.0f;

	/** Camera tilt to apply while sliding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slide")
	FVector SlideCameraOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slide", meta = (ClampMin = "0.0", ForceUnits = "cm/s"))
	float SlideTiltInterpSpeed = 1.0f;
};
