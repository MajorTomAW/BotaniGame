// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilityTask_WaitForInteractableTargets.h"
#include "InteractionQuery.h"
#include "AbilityTask_WaitForInteractableTargets_SingleTrace.generated.h"

UCLASS(MinimalAPI)
class UAbilityTask_WaitForInteractableTargets_SingleTrace : public UAbilityTask_WaitForInteractableTargets
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UAbilityTask Interface
	virtual void Activate() override;
	//~ End UAbilityTask Interface

	/** Wait until we trace a new set of interactables. This task automatically loops. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitForInteractableTargets_SingleTrace* WaitForInteractableTargets_SingleTrace(UGameplayAbility* OwningAbility, FBotaniInteractionQuery InteractionQuery, FCollisionProfileName TraceProfile, FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionScanRange = 100, float InteractionScanRate = 0.100, bool bShowDebug = false);

protected:
	//~ Begin UAbilityTask Interface
	virtual void OnDestroy(bool AbilityEnded) override;
	//~ End UAbilityTask Interface

	void PerformTrace();

private:
	UPROPERTY()
	FBotaniInteractionQuery InteractionQuery;

	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;

	float InteractionScanRange = 100.f;
	float InteractionScanRate = 0.1f;
	bool bShowDebug = false;

	FTimerHandle TimerHandle;
};
