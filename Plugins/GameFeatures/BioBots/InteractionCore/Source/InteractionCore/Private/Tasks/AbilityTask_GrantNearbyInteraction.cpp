// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Tasks/AbilityTask_GrantNearbyInteraction.h"

#include "BotaniCollisionChannels.h"
#include "InteractionStatics.h"
#include "Interfaces/InteractableTarget.h"
#include "AbilitySystemComponent.h"
#include "InteractionQuery.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"
#include "Components/PrimitiveComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityTask_GrantNearbyInteraction)


UAbilityTask_GrantNearbyInteraction::UAbilityTask_GrantNearbyInteraction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InteractionScanRange = 100.f;
	InteractionScanRate = 0.f;
}

UAbilityTask_GrantNearbyInteraction* UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate)
{
	UAbilityTask_GrantNearbyInteraction* NewTask = NewAbilityTask<UAbilityTask_GrantNearbyInteraction>(OwningAbility);
	NewTask->InteractionScanRange = InteractionScanRange;
	NewTask->InteractionScanRate = InteractionScanRate;
	return NewTask;
}

void UAbilityTask_GrantNearbyInteraction::Activate()
{
	SetWaitingOnAvatar();
	
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::QueryInteractables, InteractionScanRate, true);
	}
}

void UAbilityTask_GrantNearbyInteraction::OnDestroy(bool AbilityEnded)
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(QueryTimerHandle);
	}
	
	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_GrantNearbyInteraction::QueryInteractables()
{
	const UWorld* World = GetWorld();
	AActor* ActorOwner = GetAvatarActor();

	if (!World || !ActorOwner)
	{
		return;
	}

	const FCollisionQueryParams Params(SCENE_QUERY_STAT(UAbilityTask_GrantNearbyInteraction), false);
	TArray<FOverlapResult> OverlapResults;

	World->OverlapMultiByChannel(OUT OverlapResults, ActorOwner->GetActorLocation(), FQuat::Identity, BOTANI_TRACE_CHANNEL_INTERACTION, FCollisionShape::MakeSphere(InteractionScanRange), Params);

	if (OverlapResults.Num() > 0)
	{
		TArray<TScriptInterface<IInteractableTarget>> InteractableTargets;
		UInteractionStatics::AppendInteractableTargetsFromOverlapResults(OverlapResults, OUT InteractableTargets);

		FBotaniInteractionQuery InteractionQuery;
		InteractionQuery.RequestingAvatar = ActorOwner;
		InteractionQuery.RequestingController = Cast<AController>(ActorOwner->GetOwner());

		// Gather all interaction options from the interactable targets.
		TArray<FBotaniInteractionOption> Options;
		for (TScriptInterface<IInteractableTarget>& InteractiveTarget : InteractableTargets)
		{
			FBotaniInteractionOptionBuilder Builder(InteractiveTarget, Options);
			InteractiveTarget->GatherInteractionOptions(InteractionQuery, Builder);
		}

		// Check if any of the options need to grant the ability to the user before they can be used.
		for (FBotaniInteractionOption& Option : Options)
		{
			if (!Option.InteractionAbilityToGrant)
			{
				continue;
			}

			// Grant the ability to the user, otherwise it won't be able to do whatever the interaction is.
			FObjectKey ObjectKey(Option.InteractionAbilityToGrant);
			if (!InteractionAbilityCache.Find(ObjectKey))
			{
				FGameplayAbilitySpec Spec(Option.InteractionAbilityToGrant, 1, INDEX_NONE, this);
				FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);

				InteractionAbilityCache.Add(ObjectKey, Handle);
			}
		}
	}
}
