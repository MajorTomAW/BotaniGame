// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputModifiers.h"
#include "BotaniInputMod_SettingBasedScalar.generated.h"

/**
 * Scales input based on a double property in the user settings.
 */
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "Setting Based Scalar"))
class UBotaniInputMod_SettingBasedScalar : public UInputModifier
{
	GENERATED_BODY()

public:
	/** Name of the property that will be used to clamp the X Axis of this value */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Settings")
	FName XAxisScalarSettingName = NAME_None;

	/** Name of the property that will be used to clamp the Y Axis of this value */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Settings")
	FName YAxisScalarSettingName = NAME_None;

	/** Name of the property that will be used to clamp the Z Axis of this value */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Settings")
	FName ZAxisScalarSettingName = NAME_None;

	/** Set the maximum value of this setting on each axis */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Settings")
	FVector MaxValueClamp = FVector(10.0f, 10.0f, 10.0f);

	/** Set the minimum value of this setting on each axis */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Settings")
	FVector MinValueClamp = FVector::ZeroVector;

protected:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;

	/** FProperty Cache that will be populated with any found FProperty's on the settings class so that we don't need to look them up each frame */
	TArray<const FProperty*> PropertyCache;
};
