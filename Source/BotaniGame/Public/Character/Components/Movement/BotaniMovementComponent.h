// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniMovementTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BotaniMovementComponent.generated.h"

/**
 * UBotaniMovementComponent
 *
 * An actor component class.
 */
UCLASS(BlueprintType, PrioritizeCategories = ("Character Movement (General Settings)"))
class BOTANIGAME_API UBotaniMovementComponent : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()
	friend class ABotaniCharacter;
	friend class FSavedMove_BotaniCharacter;

public:
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;

	virtual bool IsMovingOnGround() const override;
	virtual bool CanCrouchInCurrentState() const override;

	virtual bool CanAttemptJump() const override;
	virtual bool DoJump(bool bReplayingMoves, float DeltaTime) override;

	virtual void PhysFlying(float deltaTime, int32 Iterations) override;

public:
	/**
	 * Checks whether the given custom movement mode is currently active.
	 * @param InCustomMode  The movement mode to compare with the current movement mode.
	 * @return True if the given custom movement mode is active, false otherwise.
	 */
	virtual bool IsCustomMovementMode(EBotaniCustomMovementMode InCustomMode) const;

	/**
	 * Sets the movement mode to custom and updates the custom movement mode.
	 * @param NewCustomMode		The new custom movement mode to set.
	 */
	virtual void SetCustomMovementMode(EBotaniCustomMovementMode NewCustomMode);
	
	virtual EBotaniWallRunSide GetWallRunSide() const { return WallRunSide; }

protected:
	/** Pointer to the botani character owner. */
	UPROPERTY(Transient)
	TObjectPtr<ABotaniCharacter> BotaniCharacterOwner;

protected:
	/** Tags that block moving when present on the character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Walking")
	FGameplayTagContainer MovementBlockedTags;
	
	//////////////////////////////////////////////////////////////////////////
	/// Wall Run
	
	/** @note Movement update functions */
	virtual void PhysWallRun(float deltaTime, int32 Iterations);

	/** Returns true if the character is currently wall running. */
	virtual bool IsWallRunning() const;

	/** Tries to start wall running. */
	virtual bool TryWallRun();

	virtual bool CanStayInWallRun(FHitResult WallHit) const;
	virtual bool WantsToFallOffWall(const FHitResult& CurrentWall) const;

	virtual bool ExitWallRunning();

	/** Performs the wall jump. */ 
	virtual bool DoWallJump(bool bReplayingMoves, float DeltaTime);

	/**
	 * Determines which side of the wall is relatively to the velocity given by the normal.
	 * @param InWallNormal The Walls normal relatively which side is computed.
	 * @return Side the wall is relatively to the velocity.
	 *
	 * @note InWallNormal should be normalized.
	 */
	virtual EBotaniWallRunSide DetermineWallSide(const FVector& InWallNormal) const;

public:
	/** Enum telling which side of the wall the character is running on. */
	UPROPERTY(BlueprintReadOnly, Category = "Character Movement: Wall Running")
	TEnumAsByte<EBotaniWallRunSide> WallRunSide;

	/** Delta used for vector's tail in wall trace. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Trace")
	float WallTraceVectorsTailDelta = 30.f;

	/** Delta used for vector's head in wall trace. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Trace")
	float WallTraceVectorsHeadDelta = 90.f;

	/** Gameplay tag that blocks wall running when present on the character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running")
	FGameplayTagContainer WallRunBlockedTags;

	/** Whether the character should always stay on the wall. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Requirements")
	uint32 bAlwaysStayOnWall:1;

	/** The minimum speed the character must have to start wall running. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Requirements", meta = (ClampMin = "0.0", ForceUnits = "cm/s", EditCondition = "!bAlwaysStayOnWall"))
	float WallRun_MinRequiredSpeed;

	/** The minimum height of the character must be above the ground to start the wall running. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Requirements", meta = (ClampMin = "0.0", ForceUnits = "cm", DisplayName = "Min Required Wall Height"))
	float WallRun_MinRequiredHeight;

	/** The minimum angle of the wall required to start wall running. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Requirements", meta = (ClampMin = "0.0", Units = "deg"))
	float WallRun_MinRequiredAngle;

	/** The maximum vertical speed the character can have while wall running. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Requirements", meta = (ClampMin = "0.0", ForceUnits = "cm/s"))
	float WallRun_MaxVerticalSpeed;

	/** The maximum speed the character can move while wall running. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running", meta = (ClampMin = "0.0", ForceUnits = "cm/s", DisplayName = "Wall Run Max Speed"))
	float WallRun_MaxSpeed;

	/** The maximum angle the character can have relative to the wall before falling off. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running", meta = (ClampMin = "0.0", Units = "deg", ScriptName = "WallRunPullAwayAngle"))
	float WallRun_PullAwayAngle;

	/** The amount of force that pulls the character towards the wall. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running", meta = (ClampMin = "0.0", Units = "cm/s^2", ScriptName = "WallRunAttractionForce"))
	float WallRun_AttractionForce;

	/** The braking deceleration applied to the character when wall running.(will slow down the character) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running", meta = (ClampMin = "0.0", Units = "cm/s^2"))
	float WallRun_BrakingDeceleration;

	/** Friction factor applied to the ground friction while wall running. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running", meta = (ClampMin = "0.0", Units = "x", DisplayName = "Wall Run Friction Factor"))
	float WallRun_SurfaceFrictionFactor;

	/** The minimum amount of time that has to pass until we can wall run again. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running", meta = (ClampMin = "0.0", Units = "s"))
	float WallRun_MinTimeBetweenRuns;

	/** Runtime curve used to determine the gravity scale over velocity while wall running. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running", meta = (DisplayName = "Wall Run [Gravity x Velocity] Scale Curve", ScriptName = "WallRunGravityVelScaleCurve"))
	FRuntimeFloatCurve WallRun_GravityVelScaleCurve;

	/** Runtime curve used to determine the gravity scale over time while wall running. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running", meta = (DisplayName = "Wall Run [Gravity x Time] Scale Curve"))
	FRuntimeFloatCurve WallRun_GravityTimeScaleCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running")
	uint32 bResetTimerOnlyOnLand:1;
	
	/** Gameplay tag that blocks wall jumping when present on the character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Jumping")
	FGameplayTagContainer WallJumpBlockedTags;
	
	/** Whether the character is allowed to jump off a wall. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Jumping")
	uint32 bAllowWallJump:1;
	
	/** Force used to apply when jumping off a wall. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Jumping", meta = (ClampMin = "0.0", ForceUnits = "cm/s^2", EditCondition = "bAllowWallJump", ScriptName = "WallJumpForce"))
	float WallJump_Force;

	/** Force used to apply when jumping off a wall. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Jumping", meta = (ClampMin = "0.0", EditCondition = "bAllowWallJump", ScriptName = "WallJumpArcadeForce"))
	FVector WallJump_ArcadeForce;

	/** Whether to override the current z velocity when jumping off a wall. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Jumping", meta = (EditCondition = "bAllowWallJump", DisplayName = "Wall Jump Overrides Z Velocity"))
	uint32 bOverrideWallJumpZVelocity:1;

	/** Whether to override the current xy velocity when jumping off a wall. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Jumping", meta = (EditCondition = "bAllowWallJump", DisplayName = "Wall Jump Overrides XY Velocity"))
	uint32 bOverrideWallJumpXYVelocity:1;

	/** Whether to draw the debug lines in the editor */ 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Wall Running|Debug")
	uint32 bDrawWallRunDebug:1;

	/** Time WHEN the character started wall running. */
	UPROPERTY(BlueprintReadOnly, Category = "Character Movement: Wall Running|Debug")
	float WallRunTime;
	

	//////////////////////////////////////////////////////////////////////////
	/// Sprinting

	/** The sprinting speed velocity. */
	UPROPERTY(Category = "Character Movement: Sprinting", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ForceUnits = "cm/s", DisplayName = "Sprint Speed"))
	float Sprint_MaxSpeed;

	/** Whether the character is allowed to sprint when crouched. */
	UPROPERTY(Category = "Character Movement: Sprinting", EditAnywhere, BlueprintReadWrite)
	uint32 bAllowSprintingWhenCrouched:1;

	bool CanAttemptSprint() const;

	//////////////////////////////////////////////////////////////////////////
	/// Crouching

	bool bPrevWantsToCrouch;
	
	//////////////////////////////////////////////////////////////////////////
	/// Sliding

	/** @note Movement update functions */
	virtual void PhysSlide(float deltaTime, int32 Iterations);

	/** Returns true if the character is currently sliding. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Movement: Sliding")
	virtual bool IsSliding() const;

	virtual bool ExitSliding();
	virtual void DoSlide();

	virtual bool CanSlideInCurrentState() const;

	virtual FVector GetSlideControl(float DeltaTime, float TickSlideControl, const FVector& SlideAcceleration);

	/** Tags that block sliding when present on the character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Sliding")
	FGameplayTagContainer SlideBlockedTags;

	/** The overall Sliding friction. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Sliding", meta = (ClampMin = "0.0", Units = "x", DisplayName = "Sliding Friction Factor"))
	float Slide_SurfaceFrictionFactor;

	/** The minimum velocity the character must have to start sliding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Sliding|Requirements", meta = (ClampMin = "0.0", ForceUnits = "cm/s", DisplayName = "Sliding Min Required Entering Speed"))
	float Slide_EnterMinRequiredSpeed;

	/** The minimum velocity the character must have to stay sliding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Sliding|Requirements", meta = (ClampMin = "0.0", ForceUnits = "cm/s", DisplayName = "Sliding Min Required Exiting Speed"))
	float Slide_ExitMinRequiredSpeed;

	/** The braking deceleration applied to the character when sliding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Sliding", meta = (ClampMin = "0.0", ForceUnits = "cm/s^2", DisplayName = "Sliding Braking Deceleration"))
	float Slide_BrakingDeceleration;

	/** The maximum speed the character can move while sliding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Sliding", meta = (ClampMin = "0.0", ForceUnits = "cm/s", DisplayName = "Sliding Max Speed"))
	float Slide_MaxSpeed;

	/**
	 * When sliding, amount of lateral movement control available to the character.
	 * 0 = no control, 1 = full control at max speed of Sliding Max Speed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Sliding", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "Sliding Control"))
	float Slide_Control;

	/** The impulse force applied to the character when start sliding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Sliding", meta = (ClampMin = "0.0", ForceUnits = "cm/s^2", DisplayName = "Sliding Enter Impulse"))
	float Slide_EnterImpulse;

	/** Direction of force applied to the character when sliding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Sliding", meta = (ClampMin = "0.0", DisplayName = "Sliding Force Direction"))
	FVector Slide_ForceDirection;

	/** Gravity force applied to the character when sliding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Sliding", meta = (ClampMin = "0.0", Units = "x", DisplayName = "Sliding Gravity Scale"))
	float Slide_GravityScale;

	//////////////////////////////////////////////////////////////////////////
	/// Dashing
	
	/** @note Movement update functions */
	virtual bool CanDash() const;

	/** Returns the dash speed curve. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Movement: Dashing")
	UCurveFloat* GetDashSpeedCurve();

	/** The speed to apply when dashing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Dashing", meta = (ClampMin = "0.0", ForceUnits = "cm/s", DisplayName = "Dash Speed"))
	float Dash_Speed;

	/** The duration of the dash. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Dashing", meta = (ClampMin = "0.0", Units = "s", DisplayName = "Dash Duration"))
	float Dash_Duration;

	/**
	 * The dash speed over time curve.
	 * X = Time (0 to 1)
	 * Y = Percentage of the dash speed (0 to 1) with 0 meaning 0% and 1 meaning 100%.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Dashing", DisplayName = "Dash Speed [Time x Percentage]")
	FRuntimeFloatCurve Dash_SpeedCurve;

	/** Determines how the velocity is handled when dashing has ended. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Dashing", DisplayName = "Dash Finish Velocity Mode")
	ERootMotionFinishVelocityMode VelocityOnFinish;
	
	/** The velocity to clamp to when dashing has ended. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Dashing", meta = (ClampMin = "0.0", ForceUnits = "cm/s", DisplayName = "Dash End Velocity", EditCondition = "VelocityOnFinish != ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity"))
	float Dash_EndVelocity;

	/** The dash offset direction to apply to the final direction of the dash. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Dashing", DisplayName = "Dash Arcade Direction")
	FVector Dash_ArcadeDirection;

	/** Determines if the character should uncrouch when dashing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Dashing", DisplayName = "Dash Uncrouch")
	uint32 bDash_Uncrouch:1;

	/** Determines if the character should stop sliding when dashing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Dashing", DisplayName = "Dash Stop Sliding")
	uint32 bDash_StopSliding:1;

	/** Determines if the dash velocity should be additive to the current velocity. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Dashing", DisplayName = "Dash Additive Velocity")
	uint32 bDash_AdditiveVelocity:1;

	/** Determines if the character should receive gravity while dashing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Dashing", DisplayName = "Dash Uses Gravity")
	uint32 bDash_ApplyGravity:1;

	/** Determines if the character needs to be moving on ground to dash. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Dashing|Requirements", DisplayName = "Dash Requires Ground")
	uint32 bDash_RequiresGround:1;

	/** Tags that block dashing when present on the character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Dashing|Requirements")
	FGameplayTagContainer DashBlockedTags;

	//////////////////////////////////////////////////////////////////////////
	/// Grappling
	 

	/** @note Movement update functions */
	virtual void PhysGrapple(float deltaTime, int32 Iterations);
	virtual void ExitGrapple();

	/** Returns true if the character is currently grappling. */
	virtual bool IsGrappling() const;

	/** Returns true if the character can grapple. */
	virtual bool CanGrapple() const;

	/**
	 * Tries to start grappling.
	 *
	 * @returns true if the character started grappling, false otherwise.
	 */
	virtual bool TryGrapple();

	/** The current grapple hit result. */
	UPROPERTY(BlueprintReadOnly, Category = "Character Movement: Grappling")
	FHitResult GrappleHitResult;

	/** The current grapple rope length. */
	UPROPERTY(BlueprintReadOnly, Category = "Character Movement: Grappling")
	float GrappleRopeLength;

	/** Determines the maximum distance the character can grapple. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Grappling", meta = (UIMin = 0, ClampMin = 0, Units = "cm", DisplayName = "Grapple Max Distance"))
	float Grapple_MaxTraceDistance;

	/** Determines the strength of the pulling-in force when grappling. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Grappling", meta = (UIMin = 0, ClampMin = 0, Units = "cm/s^2", DisplayName = "Grapple Pull Force"))
	float Grapple_PullForce;

	/** The max speed the character can have while grappling. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Grappling", meta = (ClampMin = "0.0", ForceUnits = "cm/s", DisplayName = "Grapple Max Speed"))
	float Grapple_MaxSpeed;

	/** The friction factor applied to the ground friction while grappling. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Grappling", meta = (ClampMin = "0.0", Units = "x", DisplayName = "Grapple Friction Factor"))
	float Grapple_SurfaceFrictionFactor;

	/** The default rope configuration. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Grappling", meta = (DisplayName = "Default Rope Config"))
	FBotaniGrappleRopeConfig Grapple_RopeConfig;
	
private:
	float CapR() const;
	float CapHH() const;

private:
	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> DashSpeedCurve;
};
