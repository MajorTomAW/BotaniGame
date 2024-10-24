// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitForInteractableTargets.generated.h"

struct FBotaniInteractionQuery;
class IInteractableTarget;
struct FBotaniInteractionOption;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableObjectsChangedSignature, const TArray<FBotaniInteractionOption>&, InteractableOptions);

/**
 * UAbilityTask_WaitForInteractableTargets
 *
 * Waits for interactable targets to be available
 */
UCLASS(Abstract)
class INTERACTIONCORE_API UAbilityTask_WaitForInteractableTargets : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

public:
	/** Event that is triggered when the interactable objects change */
	UPROPERTY(BlueprintAssignable)
	FInteractableObjectsChangedSignature InteractableObjectsChanged;

protected:
	/** Performs the interaction query */
	static void LineTrace(FHitResult& OutHit, const UWorld* World, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params);

	void AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, float MaxRange, FVector& OutTraceEnd, bool bIgnorePitch = false) const;

	static bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition);

	void UpdateInteractableOptions(const FBotaniInteractionQuery& InteractQuery, const TArray<TScriptInterface<IInteractableTarget>>& InteractableTargets);

public:
	FCollisionProfileName TraceProfile;

	/** Does the trace affect the aiming pitch */
	bool bTraceAffectsAimPitch = true;

	/** The current options available */
	TArray<FBotaniInteractionOption> CurrentOptions;
};
