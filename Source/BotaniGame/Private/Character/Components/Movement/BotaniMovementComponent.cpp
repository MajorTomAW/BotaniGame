// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Character/Components/Movement/BotaniMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BotaniLogChannels.h"
#include "Character/BotaniCharacter.h"
#include "Character/Components/Movement/BotaniMovementTypes.h"
#include "Components/CapsuleComponent.h"
#include "GameplayTags/BotaniGameplayTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniMovementComponent)

UBotaniMovementComponent::UBotaniMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BotaniCharacterOwner = nullptr;

	// Sprinting
	MovementBlockedTags.AddTag(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockMoving);
	Sprint_MaxSpeed = 1000.0f;
	bAllowSprintingWhenCrouched = false;

	// Wall Running
	WallRunBlockedTags.AddTag(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockWallRunning);
	bAlwaysStayOnWall = 1;
	WallRunSide = Wall_Error;
	WallRun_MinRequiredSpeed = 500.f;
	WallRun_MinRequiredHeight = 100.f;
	WallRun_MinRequiredAngle = 64.f;
	WallRun_MaxVerticalSpeed = 400.f;
	WallRun_MaxSpeed = 800.f;
	WallRun_PullAwayAngle = 72.f;
	WallRun_AttractionForce = 120.f;
	WallRun_BrakingDeceleration = 800.f;
	WallRun_SurfaceFrictionFactor = 0.02f;
	bDrawWallRunDebug = 1;
	bResetTimerOnlyOnLand = 1;
	WallRunTime = 0.f;
	WallRun_MinTimeBetweenRuns = 0.5f;

	// Wall Jumping
	WallJumpBlockedTags.AddTag(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockWallJumping);
	bAllowWallJump = true;
	bOverrideWallJumpZVelocity = 1;
	bOverrideWallJumpXYVelocity = 0;
	WallJump_Force = 420.f;
	WallJump_ArcadeForce = FVector::ZeroVector;

	// Crouching
	bPrevWantsToCrouch = false;

	// Sliding
	SlideBlockedTags.AddTag(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockSliding);
	Slide_BrakingDeceleration = 300.f;
	Slide_EnterMinRequiredSpeed = 500.f;
	Slide_ExitMinRequiredSpeed = 200.f;
	Slide_MaxSpeed = 2048.f;
	Slide_Control = 0.06f;
	Slide_SurfaceFrictionFactor = 0.04f;
	Slide_GravityScale = 4.0f;
	Slide_EnterImpulse = 120.f;
	Slide_ForceDirection = FVector(0.f, 0.f, -1.f);

	// Dashing
	DashBlockedTags.AddTag(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockDashing);
	VelocityOnFinish = ERootMotionFinishVelocityMode::ClampVelocity;
	Dash_Speed = 1800.f;
	Dash_Duration = 0.2f;
	Dash_ArcadeDirection = FVector(0.f, 0.f, 0.1f);
	Dash_EndVelocity = 1000.f;
	bDash_AdditiveVelocity = true;
	bDash_RequiresGround = false;
	bDash_Uncrouch = true;
	bDash_StopSliding = true;
	bDash_ApplyGravity = false;

	// Grappling
	Grapple_MaxTraceDistance = 1000.f;
	Grapple_PullForce = 0.f;
	Grapple_MaxSpeed = 1000.f;
	Grapple_SurfaceFrictionFactor = 0.4f;
	GrappleRopeLength = 0.f;
	
	Grapple_RopeConfig.Grapple_RopeLength = 1500.f;
	Grapple_RopeConfig.Grapple_RopeElasticity = 0.f;
}

void UBotaniMovementComponent::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	Super::SetUpdatedComponent(NewUpdatedComponent);

	BotaniCharacterOwner = Cast<ABotaniCharacter>(PawnOwner);	
}

FNetworkPredictionData_Client* UBotaniMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		auto MutableThis = const_cast<UBotaniMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_BotaniCharacter(*this);
	}
	
	return ClientPredictionData;
}

void UBotaniMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	if (!IsValid(BotaniCharacterOwner))
	{
		return;
	}

	// Wall Run
	const bool bWasPressingWallRun = BotaniCharacterOwner->bWantsToWallRun;

	// Slide
	const bool bWasPressingSlide = BotaniCharacterOwner->bWantsToSlide;
	BotaniCharacterOwner->bWantsToSlide = ((Flags & FSavedMove_BotaniCharacter::FLAG_WantsToSlide) != 0);

	// Grapple
	const bool bWasPressingGrapple = BotaniCharacterOwner->bWantsToGrapple;
	BotaniCharacterOwner->bWantsToGrapple = ((Flags & FSavedMove_BotaniCharacter::FLAG_WantsToGrapple) != 0);
	

	const ENetRole NetRole = BotaniCharacterOwner->GetLocalRole();
	if (NetRole < ROLE_Authority)
	{
		return;
	}

	// Detect change of the state in wall run.
	const bool bWantsToWallRun = BotaniCharacterOwner->bWantsToWallRun;
	if (bWantsToWallRun && !bWasPressingWallRun)
	{
		BotaniCharacterOwner->SetCanWallRun(true);
	}
	else if (!bWantsToWallRun)
	{
		BotaniCharacterOwner->SetCanWallRun(false);
	}

	// Detect change of the state in grapple.
	const bool bWantsToGrapple = BotaniCharacterOwner->bWantsToGrapple;
	if (bWantsToGrapple && !bWasPressingGrapple)
	{
		
	}
	else if (!bWantsToGrapple)
	{
		
	}
}

void UBotaniMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if (!IsValid(BotaniCharacterOwner))
	{
		return;
	}

	// Prevent custom movement on simulated proxies.
	const ENetRole NetRole = BotaniCharacterOwner->GetLocalRole();
	if (NetRole == ROLE_SimulatedProxy)
	{
		return;
	}

	switch (CustomMovementMode)
	{
	case CMOVE_WallRun:
		{
			PhysWallRun(deltaTime, Iterations);
		}
		break;
	case CMOVE_Slide:
		{
			PhysSlide(deltaTime, Iterations);
		}
		break;
	case CMOVE_Grapple:
		{
			PhysGrapple(deltaTime, Iterations);
		}
		break;
	default:
		{
			BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::PhysCustom: Unknown custom movement mode %d."), CustomMovementMode);
		}
		break;
	}
	
	Super::PhysCustom(deltaTime, Iterations);
}

void UBotaniMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (!IsGrappling() && CanGrapple() && BotaniCharacterOwner->bWantsToGrapple)
	{
		const bool bSuccess = TryGrapple();

		if (!bSuccess)
		{
			BotaniCharacterOwner->bWantsToGrapple = false;
		}
	}
	else if (IsGrappling() && !BotaniCharacterOwner->bWantsToGrapple)
	{
		SetMovementMode(MOVE_Falling);
	}

	if (MovementMode == MOVE_Walking && !bWantsToCrouch && bPrevWantsToCrouch)
	{
		if (!IsSliding() && CanSlideInCurrentState())
		{
			SetCustomMovementMode(CMOVE_Slide);
		}
	}
	else if (IsCustomMovementMode(CMOVE_Slide) && !bWantsToCrouch)
	{
		SetMovementMode(MOVE_Walking);
	}

	/*if (!IsSliding() && CanSlideInCurrentState() && BotaniCharacterOwner->bWantsToSlide)
	{
		BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::UpdateCharacterStateBeforeMovement: Entering slide."));
		
		SetCustomMovementMode(CMOVE_Slide);
	}
	else if (IsCustomMovementMode(CMOVE_Slide) && !BotaniCharacterOwner->bWantsToSlide)
	{
		SetMovementMode(MOVE_Walking);
	}*/

	if (IsFalling())
	{
		TryWallRun();
	}
	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UBotaniMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Slide)
	{
		ExitSliding();
	}

	if (IsCustomMovementMode(CMOVE_Slide))
	{
		DoSlide();
	}

	if (!IsValid(BotaniCharacterOwner))
	{
		return;
	}

	if (BotaniCharacterOwner->IsWallRunning() && GetOwnerRole() == ROLE_SimulatedProxy)
	{
		const FVector Start = UpdatedComponent->GetComponentLocation();
		const FVector End = Start + UpdatedComponent->GetRightVector() * CapR() * 2;
		const FCollisionQueryParams QueryParams = BotaniCharacterOwner->GetIgnoreCharacterQueryParams();

		FHitResult WallHit;
		GetWorld()->LineTraceSingleByChannel(WallHit, Start, End, ECC_Camera, QueryParams);
		WallRunSide = DetermineWallSide(WallHit.ImpactNormal);
	}
}

void UBotaniMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	bPrevWantsToCrouch = bWantsToCrouch;
}

FRotator UBotaniMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CharacterOwner))
	{
		if (ASC->HasMatchingGameplayTag(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockMoving))
		{
			return FRotator::ZeroRotator;
		}
	}
	
	return Super::GetDeltaRotation(DeltaTime);
}

float UBotaniMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CharacterOwner))
	{
		if (ASC->HasMatchingGameplayTag(BotaniGameplayTags::Movement::Modifier::TAG_MovementModifier_BlockMoving))
		{
			return 0.f;
		}
	}

	if (IsValid(BotaniCharacterOwner))
	{
		if (BotaniCharacterOwner->IsSprinting())
		{
			return Sprint_MaxSpeed;
		}

		if (BotaniCharacterOwner->IsWallRunning())
		{
			return WallRun_MaxSpeed;
		}

		if (BotaniCharacterOwner->IsSliding())
		{
			return Slide_MaxSpeed;
		}

		if (BotaniCharacterOwner->IsGrappling())
		{
			return Grapple_MaxSpeed;
		}
	}

	return MaxSpeed;
}

float UBotaniMovementComponent::GetMaxBrakingDeceleration() const
{
	if (MovementMode != MOVE_Custom)
	{
		return Super::GetMaxBrakingDeceleration();
	}

	switch (CustomMovementMode)
	{
	case CMOVE_WallRun:
		{
			return WallRun_BrakingDeceleration;
		}
	case CMOVE_Slide:
		{
			return Slide_BrakingDeceleration;
		}
	case CMOVE_Grapple:
		{
			return 0.f;
		}
	default:
		BOTANI_MOVEMENT_LOG(Error, TEXT("UBotaniMovementComponent::GetMaxBrakingDeceleration: Unknown custom movement mode %d."), CustomMovementMode);
		return -1.f;
	}
}

bool UBotaniMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(CMOVE_Slide);
}

bool UBotaniMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

bool UBotaniMovementComponent::CanAttemptJump() const
{
	if (!IsValid(BotaniCharacterOwner))
	{
		return false;
	}
	
	bool Result = Super::CanAttemptJump();

	// If super is false, make sure if any botani specific conditions are met.
	if (Result == false)
	{
		// Check if the character is wall running and can wall jump.
		if (BotaniCharacterOwner->IsWallRunning() && bAllowWallJump)
		{
			Result = true;
		}
	}
	
	return Result;
}

bool UBotaniMovementComponent::DoJump(bool bReplayingMoves)
{
	if (IsValid(BotaniCharacterOwner) && (BotaniCharacterOwner->IsWallRunning()))
	{
		return DoWallJump(bReplayingMoves);
	}
	
	return Super::DoJump(bReplayingMoves);
}

void UBotaniMovementComponent::PhysFlying(float deltaTime, int32 Iterations)
{
	Super::PhysFlying(deltaTime, Iterations);
}

bool UBotaniMovementComponent::CanDash() const
{
	if (!IsValid(BotaniCharacterOwner))
	{
		return false;
	}

	if (bDash_RequiresGround)
	{
		return IsMovingOnGround();
	}

	return true;
}

UCurveFloat* UBotaniMovementComponent::GetDashSpeedCurve()
{
	if (DashSpeedCurve)
	{
		return DashSpeedCurve;
	}
	
	if (!Dash_SpeedCurve.GetRichCurveConst()->HasAnyData())
	{
		return nullptr;
	}

	if (Dash_SpeedCurve.ExternalCurve)
	{
		return Dash_SpeedCurve.ExternalCurve;
	}

	// Create a new curve from the internal curve.
	DashSpeedCurve = NewObject<UCurveFloat>();
	DashSpeedCurve->FloatCurve = *const_cast<FRichCurve*>(Dash_SpeedCurve.GetRichCurveConst());
	return DashSpeedCurve;
}


bool UBotaniMovementComponent::IsCustomMovementMode(EBotaniCustomMovementMode InCustomMode) const
{
	return ((MovementMode == MOVE_Custom) &&
		(CustomMovementMode == InCustomMode));
}

void UBotaniMovementComponent::SetCustomMovementMode(EBotaniCustomMovementMode NewCustomMode)
{
	SetMovementMode(MOVE_Custom, NewCustomMode);
}


bool UBotaniMovementComponent::CanAttemptSprint() const
{
	bool Result = true;

	if (!bAllowSprintingWhenCrouched)
	{
		Result = !bWantsToCrouch;
	}

	return Result && IsMovingOnGround();
}

void UBotaniMovementComponent::PhysSlide(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CanSlideInCurrentState())
	{
		SetMovementMode(MOVE_Walking);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}

	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;

	float remainingTime = deltaTime;

	// Perform the move
	while((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Save the current state
		UPrimitiveComponent* const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != nullptr) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;

		// Ensure velocity is horizontal
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;

		FVector SlopeForce = CurrentFloor.HitResult.Normal;
		SlopeForce.Z = 0;
		Velocity += SlopeForce * GetGravityZ() * timeTick * (Slide_GravityScale * Slide_ForceDirection.Z);

		const FVector VelocityRight = (Velocity ^ FVector::UpVector);
		Acceleration = Acceleration.ProjectOnTo(VelocityRight.GetSafeNormal2D());
		Acceleration = GetSlideControl(timeTick, Slide_Control, Acceleration);

		// Apply acceleration
		CalcVelocity(timeTick, GroundFriction * Slide_SurfaceFrictionFactor, false, GetMaxBrakingDeceleration());

		// Compute move parameters
		const FVector MoveVelocity = Velocity;
		const FVector Delta = timeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		bool bFloorWalkable = CurrentFloor.IsWalkableFloor();

		
		FStepDownResult StepDownResult;
		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			// Try moving forward
			MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);

			// Pawn jumped or fell off
			if (IsFalling())
			{
				const float DesiredDist = Delta.Size();
				if (DesiredDist > KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size();
					remainingTime += timeTick * (1.f - FMath::Min(1.f, ActualDist / DesiredDist));
				}

				StartNewPhysics(remainingTime, Iterations);
				return;
			}

			// Entered water
			if (IsSwimming())
			{
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
				return;
			}
		}

		// Update the floor
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, nullptr);
		}

		// Check for ledges
		const bool bCheckLedges = !CanWalkOffLedges();
		if (bCheckLedges && !CurrentFloor.IsWalkableFloor())
		{
			// Calculate the possible alternate movement
			const FVector GravDir = FVector(0.f, 0.f, -1.f);
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, GravDir);

			if (!NewDelta.IsZero())
			{
				// First, revert to prev move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// Avoid repeated ledge moves
				bTriedLedgeMove = true;

				// Try new movement dir
				Velocity = NewDelta / timeTick; // v = dx / dt
				remainingTime += timeTick;
				continue;
			}

			
			bool bMustJump = bZeroDelta || (OldBase == nullptr) || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase));
			if ((bMustJump || !bCheckedFall) &&
				CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
			{
				return;
			}

			bCheckedFall = true;

			// Revert this move
			RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
			remainingTime = 0.f;
			break;
		}

		
		// Validate the floor
		if (CurrentFloor.IsWalkableFloor())
		{
			if (ShouldCatchAir(OldFloor, CurrentFloor))
			{
				HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);

				if (IsMovingOnGround())
				{
					// If still walking, then fall. If not, assume the user sets a different mode they want to keep.
					StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
				}

				return;
			}

			AdjustFloorHeight();
			SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
		}
		else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
		{
			// The floor check failed because it started in penetration
			// We don't want to try to move downward because the downward sweep failed.
			// Rather we would like to try to pop out of the floor.
			FHitResult Hit(CurrentFloor.HitResult);
			Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
			const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
			ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
			bForceNextFloorCheck = true;
		}

		// Check if we just entered water
		if (IsSwimming())
		{
			StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
			return;
		}

		// See if we need to start falling.
		if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
		{
			const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == nullptr || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
			if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump) )
			{
				return;
			}
			bCheckedFall = true;
		}

		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround() && bFloorWalkable)
		{
			// Make velocity reflect actual move
			if( !bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				// TODO-RootMotionSource: Allow this to happen during partial override Velocity, but only set allowed axes?
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
				MaintainHorizontalGroundVelocity();
			}
		}

		// If we didn't move at all this iteration, exit.
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
	}

	FHitResult Hit;
	FQuat NewQuat = FRotationMatrix::MakeFromXZ(Velocity.GetSafeNormal2D(), FVector::UpVector).ToQuat();
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewQuat, false, Hit);
}

bool UBotaniMovementComponent::IsSliding() const
{
	return IsCustomMovementMode(CMOVE_Slide);
}

bool UBotaniMovementComponent::ExitSliding()
{

	return true;
}

void UBotaniMovementComponent::DoSlide()
{
	bWantsToCrouch = true;
	if (bOrientRotationToMovement) bOrientRotationToMovement = false;

	Velocity += Velocity.GetSafeNormal2D() * Slide_EnterImpulse;
	FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, true, nullptr);
}

bool UBotaniMovementComponent::CanSlideInCurrentState() const
{
	if (!IsValid(BotaniCharacterOwner))
	{
		return false;
	}

	if (!BotaniCharacterOwner->CanSlide())
	{
		return false;
	}

	// Check if we have a surface to slide on.
	FHitResult SurfaceHit;
	const bool bDidHit = BotaniCharacterOwner->TraceSurfaceToSlideOn(SurfaceHit, bDrawWallRunDebug);

	// Check if our velocity is high enough to slide.
	const bool bFastEnough = IsSliding()
		? Velocity.SizeSquared2D() > pow(Slide_ExitMinRequiredSpeed, 2) // When sliding, use the exit speed
		: Velocity.SizeSquared2D() > pow(Slide_EnterMinRequiredSpeed, 2); // When not sliding, use the entering speed


	// BOTANI_MOVEMENT_LOG(Error, TEXT("UBotaniMovementComponent::CanSlideInCurrentState: DidHit: %d, FastEnough: %d"), bDidHit, bFastEnough);
	return bDidHit && SurfaceHit.IsValidBlockingHit() && bFastEnough;
}

FVector UBotaniMovementComponent::GetSlideControl(float DeltaTime, float TickSlideControl, const FVector& SlideAcceleration)
{
	return TickSlideControl * SlideAcceleration;
}

void UBotaniMovementComponent::PhysGrapple(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	bJustTeleported = false;
	float remainingTime = deltaTime;

	FVector FallAcceleration = GetFallingLateralAcceleration(deltaTime);
	const FVector GravityRelativeFallAcceleration = RotateWorldToGravity(FallAcceleration);
	FallAcceleration = RotateGravityToWorld(FVector(GravityRelativeFallAcceleration.X, GravityRelativeFallAcceleration.Y, 0.f));

	// perform the move
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && BotaniCharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FVector OldVel = Velocity;

		const float DesiredRopeLength = Grapple_RopeConfig.bGrapple_AlwaysUseFullLength ?
			Grapple_RopeConfig.Grapple_RopeLength :
			FVector::Distance(OldLocation, GrappleHitResult.ImpactPoint);

		// End grapple if the wall is invalid
		if (!GrappleHitResult.IsValidBlockingHit())
		{
			ExitGrapple();
			StartNewPhysics(remainingTime, deltaTime);
			return;
		}

		// Break the rope if we're too far away
		constexpr float RopeThreshold = 100.f;
		if (Grapple_RopeConfig.bGrapple_BreakRopeOnMaxDistance &&
			(FVector::Distance(OldLocation, GrappleHitResult.ImpactPoint) > DesiredRopeLength + RopeThreshold))
		{
			ExitGrapple();
			StartNewPhysics(remainingTime, deltaTime);
			return;
		}

		const FVector UnitDir = UKismetMathLibrary::GetDirectionUnitVector(OldLocation, GrappleHitResult.ImpactPoint);
		
		DrawDebugLine(GetWorld(), OldLocation, OldLocation + UnitDir * 100.f, FColor::Red, false, 0.f, 0, 1.f);
		DrawDebugLine(GetWorld(), OldLocation, GrappleHitResult.ImpactPoint, FColor::Purple, false, 0.f, 0, 0.25f);

		// Draw a debug sphere that represents the grapple hit point
		DrawDebugSphere(GetWorld(), GrappleHitResult.ImpactPoint, 10.f, 8, FColor::Red, false, 0.f, 0, 1.f);

		// Draw a debug sphere that represents the rope length
		DrawDebugSphere(GetWorld(), GrappleHitResult.ImpactPoint, FVector::Distance(GrappleHitResult.ImpactPoint, OldLocation), 12, FColor::Green, false, 0.f, 0, 0.5f);

		
		// Clamp the acceleration to only be left/right if we are too far away
		if (FVector::Distance(OldLocation, GrappleHitResult.ImpactPoint) > DesiredRopeLength)
		{
			Acceleration = FVector::VectorPlaneProject(Acceleration, UnitDir);
		}

		FFindFloorResult Floor;
		FindFloor(OldLocation, Floor, false);
		

		CalcVelocity(timeTick, GroundFriction * Grapple_SurfaceFrictionFactor, false, GetMaxBrakingDeceleration());

		if (!Floor.IsWalkableFloor())
		{
			Velocity = NewFallVelocity(Velocity, -GetGravityDirection() * GetGravityZ(), timeTick);	
		}

		// Apply pull force
		if (Grapple_PullForce > 0.f && !Floor.IsWalkableFloor())
		{
			const FVector PullForce = Grapple_PullForce * UnitDir;
			Velocity += PullForce * timeTick;
		}

		if (Velocity.SizeSquared() > pow(Grapple_MaxSpeed, 2))
		{
			Velocity = Velocity.GetSafeNormal() * Grapple_MaxSpeed;
		}

		if (FVector::Distance(OldLocation, GrappleHitResult.ImpactPoint) > DesiredRopeLength)
		{
			Velocity = FVector::VectorPlaneProject(Velocity, UnitDir);
		}

		const FVector Adjusted = timeTick * Velocity;

		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);

		if (Hit.Time < 1.f)
		{
			HandleImpact(Hit, timeTick, Adjusted);
			SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
		}
	}
}

void UBotaniMovementComponent::ExitGrapple()
{
	BotaniCharacterOwner->bWantsToGrapple = false;
	
	SetCustomMovementMode(CMOVE_None);
	SetMovementMode(MOVE_Falling);
}

bool UBotaniMovementComponent::IsGrappling() const
{
	return IsCustomMovementMode(CMOVE_Grapple);
}

bool UBotaniMovementComponent::CanGrapple() const
{
	if (!IsValid(BotaniCharacterOwner))
	{
		return false;
	}

	return true;
}

bool UBotaniMovementComponent::TryGrapple()
{
	if (!IsValid(BotaniCharacterOwner))
	{
		return false;
	}

	// Check if we're already grappling.
	if (IsGrappling())
	{
		return false;
	}

	// Check the character can grapple.
	FHitResult GrappleHit;
	if (!BotaniCharacterOwner->CanGrapple(GrappleHit))
	{
		return false;
	}

	//@TODO: Check for angle, phys mat, ...

	GrappleHitResult = GrappleHit;

	SetCustomMovementMode(CMOVE_Grapple);
	BOTANI_MOVEMENT_LOG(Display, TEXT("UBotaniMovementComponent::TryGrapple: Grappling started!"));
	return true;
}

float UBotaniMovementComponent::CapR() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
}

float UBotaniMovementComponent::CapHH() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

//////////////////////////////////////////////////////////////////////////
/// Movement Logic: Wall Running 

void UBotaniMovementComponent::PhysWallRun(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	bJustTeleported = false;
	float remainingTime = deltaTime;

	// perform the move
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && BotaniCharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		FHitResult WallHit; //@TODO: Maybe use custom trace here
		BotaniCharacterOwner->TraceWallsToRunOn(WallHit, Wall_Both, bDrawWallRunDebug);

		const FVector WallTangent = WallHit.Normal.RotateAngleAxis(-90.f, WallHit.Normal.RotateAngleAxis(90.f, FVector::UpVector));

		if (bDrawWallRunDebug) UKismetSystemLibrary::DrawDebugArrow(this, UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentLocation() + (WallTangent * 150.f), 1.f, FLinearColor::Yellow, 10.f);

		// End wall run if conditions aren't met
		if (!WallHit.IsValidBlockingHit() || WantsToFallOffWall(WallHit))
		{
			//BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::PhysWallRun: Wants to pull away (%d) or no wall to run on.(%d)"), WantsToFallOffWall(WallHit), !WallHit.IsValidBlockingHit());
			
			ExitWallRunning();
			StartNewPhysics(remainingTime, Iterations);
			return;
		}

		// Clamp the acceleration
		Acceleration = FVector::VectorPlaneProject(Acceleration, WallHit.Normal);
		Acceleration.Z = 0.f; // No vertical acceleration

		// Apply the acceleration
		CalcVelocity(timeTick, GroundFriction * WallRun_SurfaceFrictionFactor, false, GetMaxBrakingDeceleration());
		Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
		
		float TangentAccel = Acceleration.GetSafeNormal() | Velocity.GetSafeNormal2D(); // Acceleration in the direction of velocity
		bool bVelUp = Velocity.Z > 0.f;

		// Not pushing with velocity says more gravity. Right?
		if (WallRun_GravityVelScaleCurve.GetRichCurve()->HasAnyData())
		{
			const float GravityAmount = GetGravityZ() * WallRun_GravityVelScaleCurve.GetRichCurve()->Eval(bVelUp ? 0.f : TangentAccel) * timeTick; 
			Velocity += GravityAmount * WallTangent;
		}

		// Get gravity scale for the wall run time
		if (WallRun_GravityTimeScaleCurve.GetRichCurve()->HasAnyData())
		{
			const float TimeDelta = (GetWorld()->GetTimeSeconds() - WallRunTime);
			const float TimedGravity = WallRun_GravityTimeScaleCurve.GetRichCurve()->Eval(bVelUp ? 0.f : TimeDelta);
			const float GravityAmount = GetGravityZ() * TimedGravity * timeTick;
			Velocity += GravityAmount * WallTangent;

			//FString NetRole = (BotaniCharacterOwner->GetLocalRole() == ROLE_Authority) ? TEXT("Server") : TEXT("Client");
			
			//BOTANI_MOVEMENT_LOG(Log, TEXT("[%s] UBotaniMovementComponent::PhysWallRun: TimedGravity: %f at Time: %f"), *(WallTangent.GetSafeNormal() * GravityAmount).ToString(), TimedGravity, TimeDelta);
		}

		//@TODO: add an option that lets the player stick to the wall if they are not pressing any movement keys
		if (!CanStayInWallRun(WallHit) || Velocity.Z < -WallRun_MaxVerticalSpeed)
		{
			BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::PhysWallRun: Cant stay in wall run. Or falling too fast."));
			
			ExitWallRunning();
			StartNewPhysics(remainingTime, Iterations);
			return;
		}

		// Compute new movement
		const FVector Delta = timeTick * Velocity;	// dx = v * dt
		const bool bZeroDelta = Delta.IsNearlyZero();
		if (bZeroDelta)
		{
			// No move needed
			remainingTime = 0.0f;
		}
		else
		{
			FHitResult Hit(1.f);
			SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);

			const float Dot = (WallHit.Normal | GetGravityDirection());

			if (bDrawWallRunDebug) BOTANI_MOVEMENT_LOG(Display, TEXT("UBotaniMovementComponent::PhysWallRun: Dot: %f"), Dot);

			if (Dot >= 0.f)
			{
				FVector WallAttractForce = -WallHit.Normal * WallRun_AttractionForce * timeTick;
				SafeMoveUpdatedComponent(WallAttractForce, UpdatedComponent->GetComponentQuat(), true, Hit);
			}

			if (Hit.Time < 1.f) //@TODO: Keep momentum when hitting the wall ?
			{
				HandleImpact(Hit, timeTick, Delta);
				// SlideAlongSurface(Delta, (1.f - Hit.Time), Hit.Normal, Hit, true);
			}
		}

		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}

		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;	// v = dx / dt
	}

	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + FVector::DownVector * (CapHH() + WallRun_MinRequiredHeight * 0.5f);
	FHitResult GroundHit;
	GetWorld()->LineTraceSingleByChannel(GroundHit, Start, End, ECC_Camera, BotaniCharacterOwner->GetIgnoreCharacterQueryParams());
	if (GroundHit.IsValidBlockingHit())
	{
		ExitWallRunning();
		return;
	}
	

	FHitResult WallHit; //@TODO: Maybe use custom trace here
	BotaniCharacterOwner->TraceWallsToRunOn(WallHit, Wall_Both, bDrawWallRunDebug);
	//@TODO: add an option that lets the player stick to the wall if they are not pressing any movement keys
	if (!CanStayInWallRun(WallHit))
	{
		BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::PhysWallRun: Cant stay in wall run."));
		
		ExitWallRunning();
		return;
	}
	
	if (!WallHit.IsValidBlockingHit())
	{
		// BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::PhysWallRun: No wall to run on."));
		
		ExitWallRunning();
	}
}

bool UBotaniMovementComponent::IsWallRunning() const
{
	return IsCustomMovementMode(CMOVE_WallRun);
}

bool UBotaniMovementComponent::TryWallRun()
{
	if (!IsValid(BotaniCharacterOwner))
	{
		return false;
	}

	if ((GetWorld()->GetTimeSeconds() - WallRunTime) < WallRun_MinTimeBetweenRuns)
	{
		return false;
	}

	// Check if the character is already wall running.
	if (BotaniCharacterOwner->IsWallRunning())
	{
		return false;
	}

	// We can only wall run if we're falling.
	if (!IsFalling())
	{
		return false;
	}

	if ((Velocity.SizeSquared2D() < pow(WallRun_MinRequiredSpeed, 2)) && !bAlwaysStayOnWall)
	{
		return false;
	}

	// Check the max vertical speed.
	if (Velocity.Z < -WallRun_MaxVerticalSpeed)
	{
		if (bDrawWallRunDebug) BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::TryWallRun: Character is falling too fast to start wall running."));
		return false;
	}
	

	// Check the character can wall run.
	FHitResult WallHit;
	if (!BotaniCharacterOwner->CanWallRun(&WallHit))
	{
		// BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::TryWallRun: No wall to run on."));
		return false;
	}

	// Check the angle of the wall.
	float Angle = FMath::RadiansToDegrees(FMath::Acos(WallHit.Normal | FVector::UpVector));
	if (Angle < WallRun_MinRequiredAngle)
	{
		if (bDrawWallRunDebug) BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::TryWallRun: Wall angle is too steep to start wall running."));
		return false;
	}

	// Check if the current velocity is facing away from the wall.
	{
		float VelDot = (Velocity.GetSafeNormal2D() | WallHit.Normal);
		if (VelDot > 0.f)
		{
			if (bDrawWallRunDebug) BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::TryWallRun: Character is moving away from the wall: %f"), VelDot);
			return false;
		}

		float AccelDot = (Acceleration.GetSafeNormal() | WallHit.Normal);
		if (AccelDot > 0.f)
		{
			if (bDrawWallRunDebug) BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::TryWallRun: Character is accelerating away from the wall. %f"), AccelDot);
			return false;
		}

		if (bDrawWallRunDebug) BOTANI_MOVEMENT_LOG(Log, TEXT("VelDot: %f, AccelDot: %f"), VelDot, AccelDot);
	}


	

	// Check if we're directly facing the wall
	float SinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallRun_PullAwayAngle));

	const FVector DirVector = Acceleration.GetSafeNormal().IsZero() ? BotaniCharacterOwner->GetActorForwardVector().GetSafeNormal2D() : Acceleration.GetSafeNormal();;
	
	if (((DirVector | WallHit.Normal) > SinPullAwayAngle))
	{
		//BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::TryWallRun: Character is looking away from the wall."));
		//return false;
	}

	if (WantsToFallOffWall(WallHit) || !WallHit.IsValidBlockingHit())
	{
		if (bDrawWallRunDebug) BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::TryWallRun: Character wants to fall off the wall."));
		return false;
	}

	WallRunSide = DetermineWallSide(WallHit.ImpactNormal);

	// Check the minimum height above the ground.
	{
		const FVector Start = UpdatedComponent->GetComponentLocation();
		const FVector End = Start + FVector::DownVector * (CapHH() + WallRun_MinRequiredHeight);
		const FCollisionQueryParams QueryParams = BotaniCharacterOwner->GetIgnoreCharacterQueryParams();
	
		FHitResult GroundHit;
		if (GetWorld()->LineTraceSingleByChannel(GroundHit, Start, End, ECC_Camera, QueryParams))
		{
			if (bDrawWallRunDebug) BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::TryWallRun: Character is too close to the ground to start wall running."));
			return false;
		}
	}

	// Check if we're directly facing the wall
	FVector PrjVel = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
	if (PrjVel.SizeSquared2D() < pow(WallRun_MinRequiredSpeed, 2))
	{
		//BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::TryWallRun: Character is not facing the wall to start wall running."));
		//return false;
	}

	Velocity = PrjVel;
	Velocity.Z = FMath::Clamp(Velocity.Z, 0.f, WallRun_MaxVerticalSpeed);
	
	SetCustomMovementMode(CMOVE_WallRun);
	BOTANI_MOVEMENT_LOG(Log, TEXT("UBotaniMovementComponent::StartWallRunning: Wall running started."));
	WallRunTime = GetWorld()->GetTimeSeconds();

	return true;
}

bool UBotaniMovementComponent::CanStayInWallRun(FHitResult WallHit) const
{
	if (!IsValid(BotaniCharacterOwner))
	{
		return false;
	}
	
	if (bAlwaysStayOnWall)
	{
		return true;
	}
	
	float SinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallRun_PullAwayAngle));

	const FVector DirVector = Acceleration.GetSafeNormal().IsZero() ? BotaniCharacterOwner->GetActorForwardVector().GetSafeNormal2D() : Acceleration.GetSafeNormal();;
	
	if (((DirVector | WallHit.Normal) > SinPullAwayAngle))
	{
		BOTANI_MOVEMENT_LOG(Warning, TEXT("UBotaniMovementComponent::TryWallRun: Character is looking away from the wall."));
		return false;
	}

	return !(Velocity.SizeSquared2D() < pow(WallRun_MinRequiredSpeed, 2));
}

bool UBotaniMovementComponent::WantsToFallOffWall(const FHitResult& CurrentWall) const
{
	const float SinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallRun_PullAwayAngle));
	const bool bWantsToPullAway =
		CurrentWall.IsValidBlockingHit() &&												// We're still on the wall
		!Acceleration.IsNearlyZero() &&												// We're speeding up
		((Acceleration.GetSafeNormal() | CurrentWall.Normal) > SinPullAwayAngle) &&		// We're looking away from the wall
		!bAlwaysStayOnWall;															// We aren't forced to stay on the wall

	return bWantsToPullAway;
}

bool UBotaniMovementComponent::ExitWallRunning()
{
	WallRunTime = GetWorld()->GetTimeSeconds();
	SetMovementMode(MOVE_Falling);
	return true;
}

bool UBotaniMovementComponent::DoWallJump(bool bReplayingMoves)
{
	if (!IsValid(BotaniCharacterOwner))
	{
		return false;
	}
	
	const bool bWasWallRunning = BotaniCharacterOwner->IsWallRunning();
	if (bWasWallRunning)
	{
		FHitResult WallHit;
		BotaniCharacterOwner->TraceWallsToRunOn(WallHit, WallRunSide, bDrawWallRunDebug);

		const FVector RotatedArcadeForce = WallJump_ArcadeForce.RotateAngleAxis(BotaniCharacterOwner->GetControlRotation().Yaw, FVector::UpVector);
		// Velocity += WallHit.Normal * WallJump_Force + RotatedArcadeForce;

		FVector LaunchVelocity = RotatedArcadeForce + (WallHit.Normal.GetSafeNormal()) * WallJump_Force;
		ExitWallRunning();
		Acceleration = FVector::ZeroVector;

		const FVector WallNormal = WallHit.Normal.GetSafeNormal();
		const FVector WallNormalRight = (WallNormal ^ FVector::UpVector);
		Velocity = Velocity.ProjectOnTo(WallNormalRight);

		if (bOverrideWallJumpZVelocity && bOverrideWallJumpXYVelocity)
		{
			Velocity = LaunchVelocity;
		}
		else
		{
			Velocity += LaunchVelocity;
			
			if (bOverrideWallJumpZVelocity)
			{
				Velocity.Z = LaunchVelocity.Z;	
			}

			if (bOverrideWallJumpXYVelocity)
			{
				Velocity.X = LaunchVelocity.X;
				Velocity.Y = LaunchVelocity.Y;
			}

			if (Velocity.ContainsNaN())
			{
				Velocity = FVector::ZeroVector;
			}
		}
	}

	return true;
}

EBotaniWallRunSide UBotaniMovementComponent::DetermineWallSide(const FVector& InWallNormal) const
{
	if (!InWallNormal.IsNormalized())
	{
		return Wall_Error;
	}

	const FVector VelocityForward = Velocity.GetSafeNormal2D();
	const FVector VelocityRight = (VelocityForward ^ FVector::UpVector);
	const float Dot = (VelocityRight | InWallNormal);

	const EBotaniWallRunSide Side = (Dot > 0.f) ? Wall_Right : Wall_Left;

	return Side;
}
