// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "BotaniCameraComponent.generated.h"

struct FGameplayTag;
class UBotaniCameraModeStack;
class UBotaniCameraMode;

template <class TClass> class TSubclassOf;
DECLARE_DELEGATE_RetVal(TSubclassOf<UBotaniCameraMode>, FBotaniCameraModeDelegate);

/**
 * UBotaniCameraComponent
 *
 * The base camera component class used by this project.
 */
UCLASS()
class BOTANIGAME_API UBotaniCameraComponent : public UCameraComponent
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns the camera component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Botani|Camera")
	static UBotaniCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UBotaniCameraComponent>() : nullptr); }

	/** Returns the target actor that the camera is looking at. */
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	virtual void DrawDebug(UCanvas* Canvas) const;

	/** Delegate used to query for the best camera mode. */
	FBotaniCameraModeDelegate DetermineCameraModeDelegate;

	/** Gets the tag associated with the top layer and the blend weight of it. */
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:
	//~ Begin UActorComponent Interface
	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
	//~ End UActorComponent Interface

	virtual void UpdateCameraModes();

protected:
	/** Delegate used to query for the best camera mode. */
	UPROPERTY()
	TObjectPtr<UBotaniCameraModeStack> CameraModeStack;

	/** Offset applied to the field of view. The offset is only for one frame, it gets cleared once it is applied. */
	float FieldOfViewOffset;
};
