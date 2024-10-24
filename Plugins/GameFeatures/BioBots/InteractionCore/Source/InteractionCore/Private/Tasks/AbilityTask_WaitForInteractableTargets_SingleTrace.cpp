// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Tasks/AbilityTask_WaitForInteractableTargets_SingleTrace.h"

#include "InteractionStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityTask_WaitForInteractableTargets_SingleTrace)

UAbilityTask_WaitForInteractableTargets_SingleTrace::UAbilityTask_WaitForInteractableTargets_SingleTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAbilityTask_WaitForInteractableTargets_SingleTrace* UAbilityTask_WaitForInteractableTargets_SingleTrace::WaitForInteractableTargets_SingleTrace(
	UGameplayAbility* OwningAbility, FBotaniInteractionQuery InteractionQuery, FCollisionProfileName TraceProfile, FGameplayAbilityTargetingLocationInfo StartLocation,
	float InteractionScanRange, float InteractionScanRate, bool bShowDebug)
{
	UAbilityTask_WaitForInteractableTargets_SingleTrace* NewTask = NewAbilityTask<UAbilityTask_WaitForInteractableTargets_SingleTrace>(OwningAbility);
	NewTask->InteractionScanRange = InteractionScanRange;
	NewTask->InteractionScanRate = InteractionScanRate;
	NewTask->StartLocation = StartLocation;
	NewTask->InteractionQuery = InteractionQuery;
	NewTask->TraceProfile = TraceProfile;
	NewTask->bShowDebug = bShowDebug;

	return NewTask;
}

void UAbilityTask_WaitForInteractableTargets_SingleTrace::Activate()
{
	SetWaitingOnAvatar();

	const UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::PerformTrace, InteractionScanRate, true);
}

void UAbilityTask_WaitForInteractableTargets_SingleTrace::OnDestroy(bool AbilityEnded)
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}
	
	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_WaitForInteractableTargets_SingleTrace::PerformTrace()
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!AvatarActor)
	{
		return;
	}

	UWorld* World = GetWorld();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);

	constexpr bool bTraceComplex = false;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(UAbilityTask_WaitForInteractableTargets_SingleTrace), bTraceComplex);
	Params.AddIgnoredActors(ActorsToIgnore);

	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;
	AimWithPlayerController(AvatarActor, Params, TraceStart, InteractionScanRange, OUT TraceEnd);

	FHitResult OutHitResult;
	LineTrace(OutHitResult, World, TraceStart, TraceEnd, TraceProfile.Name, Params);

	TArray<TScriptInterface<IInteractableTarget>> InteractableTargets;
	UInteractionStatics::AppendInteractableTargetsFromHitResult(OutHitResult, InteractableTargets);

	UpdateInteractableOptions(InteractionQuery, InteractableTargets);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		FColor DebugColor = OutHitResult.bBlockingHit ? FColor::Red : FColor::Green;
		if (OutHitResult.bBlockingHit)
		{
			DrawDebugLine(World, TraceStart, OutHitResult.Location, DebugColor, false, 0.1f, 0, 1.0f);
			DrawDebugPoint(World, OutHitResult.Location, 10.0f, DebugColor, false, 0.1f, 0);
		}
		else
		{
			DrawDebugLine(World, TraceStart, TraceEnd, DebugColor, false, 0.1f, 0, 1.0f);
		}
	}
#endif
}
