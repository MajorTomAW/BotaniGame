// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InteractionStatics.generated.h"

UCLASS(MinimalAPI)
class UInteractionStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UInteractionStatics();

public:
	/** Get the actor from an interactable target. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Interaction")
	static INTERACTIONCORE_API AActor* GetActorFromInteractableTarget(TScriptInterface<class IInteractableTarget> InteractableTarget);

	static INTERACTIONCORE_API void AppendInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& OverlapResults, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);
	static INTERACTIONCORE_API void AppendInteractableTargetsFromHitResult(const FHitResult& HitResult, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);
};
