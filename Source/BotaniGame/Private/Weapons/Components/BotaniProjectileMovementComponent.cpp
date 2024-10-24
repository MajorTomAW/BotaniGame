// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Weapons/Components/BotaniProjectileMovementComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniProjectileMovementComponent)

UBotaniProjectileMovementComponent::UBotaniProjectileMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxBounces = INDEX_NONE;
	CurrentBounces = 0;
}

void UBotaniProjectileMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	bool bStopSimulating = false;

	if (CanBounce())
	{
		const FVector OldVelocity = Velocity;
		Velocity = ComputeBounceResult(Hit, TimeSlice, MoveDelta);

		// Trigger bounce events
		OnProjectileBounce.Broadcast(Hit, OldVelocity);

		// Event may modify velocity or threshold, so check velocity threshold now.
		Velocity = LimitVelocity(Velocity);
		if (IsVelocityUnderSimulationThreshold())
		{
			bStopSimulating = true;
		}
		else
		{
			CurrentBounces++;
		}
	}
	else
	{
		bStopSimulating = true;
	}


	if (bStopSimulating)
	{
		StopSimulating(Hit);
	}
}

bool UBotaniProjectileMovementComponent::CanBounce() const
{
	if (bShouldBounce)
	{
		return (CurrentBounces < MaxBounces || MaxBounces == INDEX_NONE);
	}

	return false;
}
