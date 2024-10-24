// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Abilities/GameplayAbilityTargetActor_Interact.h"

#include "GameFramework/LightWeightInstanceSubsystem.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayAbilityTargetActor_Interact)

AGameplayAbilityTargetActor_Interact::AGameplayAbilityTargetActor_Interact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FHitResult AGameplayAbilityTargetActor_Interact::PerformTrace(AActor* InSourceActor)
{
	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(InSourceActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AGameplayAbilityTargetActor_Interact), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActors(ActorsToIgnore);

	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();			// InSourceActor->GetActorLocation();
	FVector TraceEnd;
	AimWithPlayerController(InSourceActor, Params, TraceStart, TraceEnd);			// Effective on server and launching client only

	FHitResult Hit;
	LineTraceWithFilter(Hit, InSourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceProfile.Name, Params);

	// Default to end of trace line if we don't hit anything.
	if (!Hit.bBlockingHit)
	{
		Hit.Location = TraceEnd;
	}

	if (AGameplayAbilityWorldReticle* LocalReticle = ReticleActor.Get())
	{
		const bool bHitActor = (Hit.bBlockingHit && (Hit.HitObjectHandle.IsValid()));
		const FVector ReticleLocation = (bHitActor && LocalReticle->bSnapToTargetedActor) ? FLightWeightInstanceSubsystem::Get().GetLocation(Hit.HitObjectHandle) : Hit.Location;

		LocalReticle->SetActorLocation(ReticleLocation);
		LocalReticle->SetIsTargetAnActor(bHitActor);
	}

#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue);
		DrawDebugSphere(GetWorld(), TraceEnd, 100.f, 16, FColor::Blue);
	}
#endif

	return Hit;
}
