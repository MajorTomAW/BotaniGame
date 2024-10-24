// Copyright © 2024 Botanibots Team. All rights reserved.


#include "PathFollowing/BotaniPathFollowingComponent.h"

#include "NavMesh/NavMeshPath.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniPathFollowingComponent)

UBotaniPathFollowingComponent::UBotaniPathFollowingComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OffsetFromCornersDistance = 0.0f;
}

FAIRequestID UBotaniPathFollowingComponent::RequestMove(const FAIMoveRequest& RequestData, FNavPathSharedPtr InPath)
{
	if (OffsetFromCornersDistance > 0.0f)
	{
		((FNavMeshPath*)InPath.Get())->OffsetFromCorners(OffsetFromCornersDistance);	
	}
	
	FAIRequestID Result = Super::RequestMove(RequestData, InPath);
	return Result;
}

void UBotaniPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
	if (!Path.IsValid() || MovementComp == nullptr)
	{
		return;
	}

	const FVector CurrentLocation = MovementComp->GetActorFeetLocation();
	const FVector CurrentTarget = GetCurrentTargetLocation();
	
	// set to false by default, we will set this back to true if appropriate
	bIsDecelerating = false;

	const bool bAccelerationBased = MovementComp->UseAccelerationForPathFollowing();
	if (bAccelerationBased)
	{
		CurrentMoveInput = (CurrentTarget - CurrentLocation).GetSafeNormal();

		if (bStopMovementOnFinish && (MoveSegmentStartIndex >= DecelerationSegmentIndex))
		{
			const FVector PathEnd = Path->GetEndLocation();
			const FVector::FReal DistToEndSq = FVector::DistSquared(CurrentLocation, PathEnd);
			const bool bShouldDecelerate = DistToEndSq < FMath::Square(CachedBrakingDistance);
			if (bShouldDecelerate)
			{
				bIsDecelerating = true;

				const FVector::FReal  SpeedPct = FMath::Clamp(FMath::Sqrt(DistToEndSq) / CachedBrakingDistance, 0., 1.);
				CurrentMoveInput *= SpeedPct;
			}
		}

		PostProcessMove.ExecuteIfBound(this, CurrentMoveInput);
		MovementComp->RequestPathMove(CurrentMoveInput);
	}
	else
	{
		FVector MoveVelocity = (CurrentTarget - CurrentLocation) / DeltaTime;

		const int32 LastSegmentStartIndex = Path->GetPathPoints().Num() - 2;
		const bool bNotFollowingLastSegment = (MoveSegmentStartIndex < LastSegmentStartIndex);

		PostProcessMove.ExecuteIfBound(this, MoveVelocity);
		MovementComp->RequestDirectMove(MoveVelocity, bNotFollowingLastSegment);
	}
}
