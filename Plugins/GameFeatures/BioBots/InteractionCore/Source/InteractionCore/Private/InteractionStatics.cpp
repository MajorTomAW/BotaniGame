// Copyright © 2024 Botanibots Team. All rights reserved.


#include "InteractionStatics.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/OverlapResult.h"
#include "Interfaces/InteractableTarget.h"
#include "UObject/ScriptInterface.h"

UInteractionStatics::UInteractionStatics()
	: Super(FObjectInitializer::Get())
{
}

AActor* UInteractionStatics::GetActorFromInteractableTarget(TScriptInterface<class IInteractableTarget> InteractableTarget)
{
	if (UObject* Object = InteractableTarget.GetObject())
	{
		if (AActor* Actor = Cast<AActor>(Object))
		{
			return Actor;
		}

		if (UActorComponent* ActorComponent = Cast<UActorComponent>(Object))
		{
			return ActorComponent->GetOwner();
		}
		
		unimplemented();
	}

	return nullptr;
}

void UInteractionStatics::AppendInteractableTargetsFromOverlapResults(
	const TArray<FOverlapResult>& OverlapResults, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{
	for (const auto& Overlap : OverlapResults)
	{
		TScriptInterface<IInteractableTarget> InteractableActor(Overlap.GetActor());
		if (InteractableActor)
		{
			OutInteractableTargets.AddUnique(InteractableActor);
		}

		TScriptInterface<IInteractableTarget> InteractableComponent(Overlap.GetComponent());
		if (InteractableComponent)
		{
			OutInteractableTargets.AddUnique(InteractableComponent);
		}
	}
}

void UInteractionStatics::AppendInteractableTargetsFromHitResult(
	const FHitResult& HitResult, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{
	TScriptInterface<IInteractableTarget> InteractableActor(HitResult.GetActor());
	if (InteractableActor)
	{
		OutInteractableTargets.AddUnique(InteractableActor);
	}

	TScriptInterface<IInteractableTarget> InteractableComponent(HitResult.GetComponent());
	if (InteractableComponent)
	{
		OutInteractableTargets.AddUnique(InteractableComponent);
	}
}
