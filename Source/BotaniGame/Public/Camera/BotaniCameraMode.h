// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Component/BotaniCameraComponent.h"
#include "UObject/Object.h"
#include "BotaniCameraMode.generated.h"

/**
 * EBotaniCameraModeBlendFunction
 *
 * Blend function used for transition between camera modes.
 */
UENUM(BlueprintType)
enum class EBotaniCameraModeBlendFunction : uint8
{
	/** Does a simple linear interpolation. */
	Linear,

	/** Immediately speeds up but smoothly decelerates into the target. Ease amount controlled by the exponent. */
	EaseIn,

	/** Smoothly speeds up but doesn't decelerate into the target. Ease amount controlled by the exponent. */
	EaseOut,

	/** Smoothly speeds up and decelerates. Ease amount controlled by the exponent. */
	EaseInOut,
};

/**
 * FBotaniCameraModeView
 *
 * View data produced by the camera mode that is used to blend camera modes.
 */
struct FBotaniCameraModeView
{
public:
	FBotaniCameraModeView();

	void Blend(const FBotaniCameraModeView& Other, float OtherWeight);

public:
	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

/**
 * UBotaniCameraMode
 *
 * Base class for camera modes.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BOTANIGAME_API UBotaniCameraMode : public UObject
{
	GENERATED_BODY()

public:
	UBotaniCameraMode();
	
	/** Returns the camera component associated with this camera mode. */
	UBotaniCameraComponent* GetBotaniCameraComponent() const;

	/** Returns the target actor for the camera mode. */
	AActor* GetTargetActor() const;

	//~ Begin UObject Interface
	virtual UWorld* GetWorld() const override;
	//~ End UObject Interface

	/** Called when this camera mode is activated on the camera mode stack. */
	virtual void OnActivated() {}

	/** Called when this camera mode is deactivated on the camera mode stack. */
	virtual void OnDeactivated() {}

	void UpdateCameraMode(float DeltaTime);
	const FBotaniCameraModeView& GetCameraModeView() const { return View; }

	float GetBlendTime() const { return BlendTime; }
	float GetBlendWeight() const { return BlendWeight; }
	void SetBlendWeight(float InBlendWeight);
	FGameplayTag GetCameraModeTag() const { return CameraTypeTag; }
	

	virtual void DrawDebug(UCanvas* Canvas) const;

protected:
	virtual FVector GetPivotLocation() const;
	virtual FRotator GetPivotRotation() const;

	virtual void UpdateView(float DeltaTime);
	virtual void UpdateBlending(float DeltaTime);

protected:
	/**
	 * A tag that can be queried by gameplay code that cared when a kind of camera mode is active,
	 * without having to ask about the specific camera mode (e.g. "Is the camera in third-person mode?").
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Blending", meta = (Categories = "Gameplay.Camera"))
	FGameplayTag CameraTypeTag;

	/** View data produced by the camera mode that is used to blend camera modes. */
	FBotaniCameraModeView View;

	/** The horizontal field of view (in degrees). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "View", meta = (UIMin = "5.0", UIMax = "170.0", ClampMin = "5.0", ClampMax = "170.0", Units = "degrees"))
	float FieldOfView;

	/** The minimum view pitch (in degrees). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "View", meta = (UIMin = "-89.0", UIMax = "89.0", /*ClampMin = "-89.0", ClampMax = "89.0",*/ Units = "degrees"))
	float ViewPitchMin;

	/** The maximum view pitch (in degrees). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "View", meta = (UIMin = "-89.0", UIMax = "89.0", /*ClampMin = "-89.0", ClampMax = "89.0",*/ Units = "degrees"))
	float ViewPitchMax;

	/** The blend time (in seconds) used for transitioning between camera modes. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blending", meta = (UIMin = "0.0", ClampMin = "0.0", Units = "seconds"))
	float BlendTime;

	/** The function used for blending between camera modes. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blending")
	EBotaniCameraModeBlendFunction BlendFunction;

	/** The exponent used for blending between camera modes. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blending", meta = (UIMin = "0.0", ClampMin = "0.0"))
	float BlendExponent;

	/** Linear blend alpha used to determine the blend weight between camera modes. */
	float BlendAlpha;
	
	/** The blend weight used for transitioning between camera modes. */
	float BlendWeight;

protected:
	/** If true, skips all interpolations and puts camera in ideal location. Automatically set to false next frame. */
	UPROPERTY(Transient)
	uint32 bResetInterpolation : 1;
};

/**
 * UBotaniCameraModeStack
 *
 * Stack used for blending camera modes.
 */
UCLASS()
class UBotaniCameraModeStack : public UObject
{
	GENERATED_BODY()

public:
	UBotaniCameraModeStack();

	void ActivateStack();
	void DeactivateStack();

	bool IsStackActive() const { return bIsActive; }

	void PushCameraMode(TSubclassOf<UBotaniCameraMode> CameraModeClass);
	bool EvaluateStack(float DeltaTime, FBotaniCameraModeView& OutView);
	void DrawDebug(UCanvas* Canvas) const;

	/** Gets the tag associated with the top camera mode on the stack. */
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:
	UBotaniCameraMode* GetCameraModeInstance(TSubclassOf<UBotaniCameraMode> CameraModeClass);

	void UpdateStack(float DeltaTime);
	void BlendStack(FBotaniCameraModeView& OutView) const;

protected:
	bool bIsActive;

	UPROPERTY()
	TArray<TObjectPtr<UBotaniCameraMode>> CameraModeInstances;

	UPROPERTY()
	TArray<TObjectPtr<UBotaniCameraMode>> CameraModeStack;
};