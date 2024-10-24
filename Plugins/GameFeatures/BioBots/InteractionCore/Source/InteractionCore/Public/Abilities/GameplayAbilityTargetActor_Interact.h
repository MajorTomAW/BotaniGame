// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "GameplayAbilityTargetActor_Interact.generated.h"

/**
 * AGameplayAbilityTargetActor_Interact
 *
 * Low-level base class for all interaction target actors.
 */
UCLASS(Blueprintable, MinimalAPI)
class AGameplayAbilityTargetActor_Interact : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin AGameplayAbilityTargetActor Interface
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;
	//~ End AGameplayAbilityTargetActor Interface
};
