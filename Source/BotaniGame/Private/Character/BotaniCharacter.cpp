// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Character/BotaniCharacter.h"

#include "BotaniLogChannels.h"
#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/Component/BotaniCameraComponent.h"
#include "Character/Components/BotaniHealthComponent.h"
#include "Character/Components/BotaniPawnExtensionComponent.h"
#include "Character/Components/Movement/BotaniMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTags/BotaniGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "Player/BotaniPlayerController.h"
#include "Player/BotaniPlayerState.h"
#include "System/Components/BotaniSignificanceManager.h"

static FName NAME_BotaniCharacterCollisionProfile_Capsule(TEXT("BotaniPawnCapsule"));
static FName NAME_BotaniCharacterCollisionProfile_Mesh(TEXT("BotaniPawnMesh"));

ABotaniCharacter::ABotaniCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBotaniMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	/** General initialization */
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NetCullDistanceSquared = 900000000.0f;
	NetUpdateFrequency = 100.f;
	MinNetUpdateFrequency = 33.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.f;
	CrouchedEyeHeight = 50.f;

	bWantsToSprint = false;
	bWasSprinting = false;

	/** Capsule component initialization */
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.f, 90.f);
	CapsuleComp->SetCollisionProfileName(NAME_BotaniCharacterCollisionProfile_Capsule);

	/** Mesh component initialization */
	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	MeshComp->SetCollisionProfileName(NAME_BotaniCharacterCollisionProfile_Mesh);
	MeshComp->SetOnlyOwnerSee(false);
	MeshComp->SetOwnerNoSee(true);
	MeshComp->ComponentTags.Add("ThirdPersonMesh");
	MeshComp->bOnlyAllowAutonomousTickPose = false;
	MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

	/** Camera component initialization */
	CameraComponent = ObjectInitializer.CreateDefaultSubobject<UBotaniCameraComponent>(this, TEXT("Camera"));
	CameraComponent->SetupAttachment(CapsuleComp);
	CameraComponent->bUsePawnControlRotation = true;
	CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	

	/** First person mesh component initialization */
	FirstPersonArmsMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("FirstPersonMesh"));
	FirstPersonArmsMesh->SetupAttachment(CameraComponent);
	FirstPersonArmsMesh->SetOnlyOwnerSee(true);
	FirstPersonArmsMesh->SetOwnerNoSee(false);
	FirstPersonArmsMesh->bCastDynamicShadow = false;
	FirstPersonArmsMesh->CastShadow = false;
	FirstPersonArmsMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	FirstPersonArmsMesh->ComponentTags.Add("FirstPersonMesh");
	FirstPersonArmsMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

	/** Character movement component initialization */
	BotaniMoveComp = CastChecked<UBotaniMovementComponent>(GetCharacterMovement());
	BotaniMoveComp->GravityScale = 2.3f;
	BotaniMoveComp->MaxAcceleration = 2400.f;
	BotaniMoveComp->BrakingFrictionFactor = 1.f;
	BotaniMoveComp->BrakingFriction = 6.f;
	BotaniMoveComp->GroundFriction = 8.f;
	BotaniMoveComp->BrakingDecelerationWalking = 1400.f;
	BotaniMoveComp->bUseControllerDesiredRotation = false;
	BotaniMoveComp->bOrientRotationToMovement = false;
	BotaniMoveComp->RotationRate = FRotator(0.f, 720.f, 0.f);
	BotaniMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	BotaniMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	BotaniMoveComp->GetNavAgentPropertiesRef().bCanJump = true;
	BotaniMoveComp->bCanWalkOffLedgesWhenCrouching = true;

	/** Pawn extension component initialization */
	PawnExtensionComponent = ObjectInitializer.CreateDefaultSubobject<UBotaniPawnExtensionComponent>(this, TEXT("PawnExtComponent"));
	PawnExtensionComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));

	/** Health component initialization */
	HealthComponent = ObjectInitializer.CreateDefaultSubobject<UBotaniHealthComponent>(this, TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
}

void ABotaniCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MyTeamID);

	// Movement props
	DOREPLIFETIME_CONDITION(ThisClass, bWantsToSprint, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ThisClass, bWantsToWallRun, COND_SkipOwner);
}

void ABotaniCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ABotaniCharacter::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();

	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		if (UBotaniSignificanceManager* SignificanceManager = USignificanceManager::Get<UBotaniSignificanceManager>(World))
		{
			// SignificanceManager->RegisterObject(this, SignificanceType)
		}
	}
}

void ABotaniCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UWorld* World = GetWorld();

	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		if (UBotaniSignificanceManager* SignificanceManager = USignificanceManager::Get<UBotaniSignificanceManager>(World))
		{
			SignificanceManager->UnregisterObject(this);
		}
	}
}

void ABotaniCharacter::Reset()
{
	Super::Reset();
}

void ABotaniCharacter::NotifyControllerChanged()
{
	const FGenericTeamId OldTeamID = MyTeamID;
	
	Super::NotifyControllerChanged();

	// Update our team ID based on the new controller
	if (HasAuthority() && (Controller != nullptr))
	{
		if (IBotaniTeamAgentInterface* TeamAgent = Cast<IBotaniTeamAgentInterface>(Controller))
		{
			MyTeamID = TeamAgent->GetGenericTeamId();
			ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
		}
	}
}

void ABotaniCharacter::Restart()
{
	Super::Restart();

	bWantsToSprint = false;
}

UAbilitySystemComponent* ABotaniCharacter::GetAbilitySystemComponent() const
{
	if (PawnExtensionComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtensionComponent->GetBotaniAbilitySystemComponent();
}

void ABotaniCharacter::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	if (GetController() == nullptr)
	{
		if (HasAuthority())
		{
			const FGenericTeamId OldTeamID = MyTeamID;
			MyTeamID = TeamID;
			ConditionalBroadcastTeamChanged(this, OldTeamID, TeamID);
		}
		else
		{
			BOTANI_LOG(Error, TEXT("Cannot set team for %s on non-authority"), *GetPathName(this));
		}
	}
	else
	{
		BOTANI_LOG(Error, TEXT("Cannot set team for %s when possessed by a controller. (%s)"), *GetPathName(this), *GetController()->GetPathName());
	}
}

FGenericTeamId ABotaniCharacter::GetGenericTeamId() const
{
	return MyTeamID;
}

FOnBotaniTeamIndexChangedDelegate* ABotaniCharacter::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamIndexChangedDelegate;
}

void ABotaniCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	UBotaniAbilitySystemComponent* BotaniASC = GetBotaniAbilitySystemComponent();
	if (BotaniASC == nullptr)
	{
		return;
	}

	const FGameplayTag* MovementModeTag;
	if (MovementMode == MOVE_Custom)
	{
		MovementModeTag = BotaniGameplayTags::Movement::MovementModeTagMap.Find(MovementMode);
	}
	else
	{
		MovementModeTag = BotaniGameplayTags::Movement::Custom::CustomMovementModeTagMap.Find(CustomMovementMode);
	}

	if (MovementModeTag && MovementModeTag->IsValid())
	{
		BotaniASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
	}
}

FCollisionQueryParams ABotaniCharacter::GetIgnoreCharacterQueryParams() const
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	TArray<AActor*> ChildActors;
	GetAllChildActors(ChildActors);
	QueryParams.AddIgnoredActors(ChildActors);
	
	return QueryParams;
}

#pragma region blueprint_helpers
ABotaniPlayerController* ABotaniCharacter::GetBotaniPlayerController() const
{
	return CastChecked<ABotaniPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

ABotaniPlayerState* ABotaniCharacter::GetBotaniPlayerState() const
{
	return CastChecked<ABotaniPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UBotaniAbilitySystemComponent* ABotaniCharacter::GetBotaniAbilitySystemComponent() const
{
	return Cast<UBotaniAbilitySystemComponent>(GetAbilitySystemComponent());
}

UBotaniMovementComponent* ABotaniCharacter::GetBotaniMovementComponent() const
{
	return BotaniMoveComp;
}

bool ABotaniCharacter::CanJumpInternal_Implementation() const
{
	if (Super::CanJumpInternal_Implementation())
	{
		return true;
	}

	if (IsWallRunning())
	{
		return true;
	}

	return false;
}
#pragma endregion


/************************************************************************
 * Movement Logic
 ************************************************************************/

void ABotaniCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	/*
	if (!bWantsToSprint || !GetCharacterMovement()->IsFalling())
	{
		ResetSprintState();
	}
	*/
	
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}

void ABotaniCharacter::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;
	
	Super::PossessedBy(NewController);

	PawnExtensionComponent->HandleControllerChanged();

	// Grab the current team ID and listen for future changes
	if (IBotaniTeamAgentInterface* TeamAgent = Cast<IBotaniTeamAgentInterface>(NewController))
	{
		MyTeamID = TeamAgent->GetGenericTeamId();
		TeamAgent->GetOnTeamIndexChangedDelegate()->AddDynamic(this, &ThisClass::OnControllerChangedTeam);
	}

	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ABotaniCharacter::UnPossessed()
{
	AController* const OldController = GetController();

	// Stop listening for team changes
	const FGenericTeamId OldTeamID = MyTeamID;
	if (IBotaniTeamAgentInterface* TeamAgent = Cast<IBotaniTeamAgentInterface>(OldController))
	{
		TeamAgent->GetOnTeamIndexChangedDelegate()->RemoveAll(this);
	}
	
	Super::UnPossessed();

	PawnExtensionComponent->HandleControllerChanged();

	// Determine what the new team ID should be after possession
	MyTeamID = FGenericTeamId::NoTeam;
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ABotaniCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtensionComponent->HandleControllerChanged();
}

void ABotaniCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtensionComponent->HandlePlayerStateReplicated();
}

void ABotaniCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtensionComponent->SetupPlayerInputComponent();
}

void ABotaniCharacter::Sprint()
{
	bWantsToSprint = true;

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_SetSprinting(true);		
	}
}

void ABotaniCharacter::StopSprinting()
{
	ResetSprintState();

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_SetSprinting(false);
	}
}

void ABotaniCharacter::Server_SetSprinting_Implementation(bool bNewSprinting)
{
	if (bNewSprinting)
	{
		Sprint();
	}
	else
	{
		StopSprinting();
	}
}

bool ABotaniCharacter::Server_SetSprinting_Validate(bool bNewSprinting)
{
	return true;
}

bool ABotaniCharacter::CanSprint() const
{
	return CanSprintInternal();
}

bool ABotaniCharacter::IsSprinting() const
{
	if (BotaniMoveComp == nullptr)
	{
		return false;
	}

	if (!bWantsToSprint)
	{
		return false;
	}

	if (!CanSprint())
	{
		return false;
	}

	// Don't sprint if the player is not moving
	const FVector Velocity = GetVelocity();
	if (Velocity.IsZero())
	{
		return false;
	}

	// Determine the velocity angle [1, -1] on XY plane
	const FVector VelocityDirXY = Velocity.GetSafeNormal2D();
	const FVector Forward = GetActorForwardVector();
	const float Dot = (VelocityDirXY | Forward);

	// Sprint if the angle difference from forward is less than ~95 degrees
	constexpr float Tolerance = -0.1f;
	if (Dot <= Tolerance)
	{
		return false;
	}

	return true;
}

bool ABotaniCharacter::CanSprintInternal_Implementation() const
{
	return SprintIsAllowedInternal();
}

bool ABotaniCharacter::SprintIsAllowedInternal() const
{
	// Ensure that the BotaniMovComp state is valid
	bool bSprintIsAllowed = GetBotaniMoveComp()->CanAttemptSprint();

	if (bSprintIsAllowed && !GetBotaniMoveComp()->bAllowSprintingWhenCrouched)
	{
		bSprintIsAllowed = !bIsCrouched;
	}

	return bSprintIsAllowed;
}

void ABotaniCharacter::ResetSprintState()
{
	bWantsToSprint = false;
}

void ABotaniCharacter::SetCanWallRun(bool bNewCanWallRun)
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!IsValid(PlayerController))
	{
		return;
	}

	bWantsToWallRun = bNewCanWallRun;

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_SetCanWallRun(bWantsToWallRun);
	}
}

void ABotaniCharacter::Server_SetCanWallRun_Implementation(bool bNewCanWallRun)
{
	SetCanWallRun(bNewCanWallRun);
}

bool ABotaniCharacter::Server_SetCanWallRun_Validate(bool bNewCanWallRun)
{
	return true;
}

bool ABotaniCharacter::CanWallRun(FHitResult* OutHit) const
{
	// Do we have any tags that block wall running?
	if (!IsAllowedToWallRun())
	{
		return false;
	}

	if (!IsValid(BotaniMoveComp))
	{
		return false;
	}

	// Do we have a wall to run on?
	FHitResult HitResult;
	const bool bResult = TraceWallsToRunOn(HitResult, Wall_Both, BotaniMoveComp->bDrawWallRunDebug);

	if (bResult && OutHit)
	{
		*OutHit = HitResult;
	}

	return bResult;
}

bool ABotaniCharacter::IsAllowedToWallRun() const
{
	if (!IsValid(BotaniMoveComp))
	{
		return false;
	}
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC))
	{
		return true; // soll ich das so machen?
	}

	return !ASC->HasAnyMatchingGameplayTags(BotaniMoveComp->WallRunBlockedTags);
}

bool ABotaniCharacter::IsWallRunning() const
{
	if (!IsValid(BotaniMoveComp))
	{
		return false;
	}

	return BotaniMoveComp->IsCustomMovementMode(CMOVE_WallRun);
}

bool ABotaniCharacter::TraceWallsToRunOn(FHitResult& OutHit, EBotaniWallRunSide WallSide, bool bShowDebug) const
{
	// Check for errors
	if (WallSide == Wall_Error)
	{
		return false;
	}

	// Trace params
	const UWorld* World = GetWorld();
	check(World);

	FCollisionQueryParams QueryParams = GetIgnoreCharacterQueryParams();

	FHitResult Hit;

	// Trace direction
	const FVector Velocity = GetActorForwardVector();
	const FVector VelocityForward = Velocity.GetSafeNormal2D();
	const FVector VelocityRight = (VelocityForward ^ FVector::DownVector);
	const FVector ActorLocation = GetActorLocation();

	auto DoTrace = [&] (const FVector& InTraceStart, const FVector& InTraceEnd)
	{
		auto Result = World->LineTraceSingleByChannel(Hit, InTraceStart, InTraceEnd, ECC_Camera, QueryParams);

		if (bShowDebug)
		{
			DrawDebugLine(World, InTraceStart, InTraceEnd, Result ? FColor::Blue : FColor::Red, false, 0.1f, 0, 1.f);
		}

		return Result;
	};

	auto DoDoubleTrace = [&, this] (EBotaniWallRunSide InWallSide)
	{
		ensure(((!!(InWallSide & Wall_Left)) != (!!(InWallSide & Wall_Right))));

		// Expr
		const float Sign = (InWallSide == Wall_Left) ? -1.f : 1.f;
		const FVector SideDelta = (VelocityRight * BotaniMoveComp->WallTraceVectorsHeadDelta) * Sign;
		const FVector ForwardDelta = (VelocityForward * BotaniMoveComp->WallTraceVectorsTailDelta);
		const FVector BackDelta = -ForwardDelta;

		// Compute point-vectors
		const FVector TraceStart = ActorLocation;
		const FVector TraceEndFront = (TraceStart + ForwardDelta + SideDelta);
		const FVector TraceEndBack = (TraceStart + BackDelta + SideDelta);

		return (DoTrace(TraceStart, TraceEndFront) ||
			DoTrace(TraceStart, TraceEndBack));
	};

	// Do left or/and right traces
	if (((WallSide & Wall_Left) && (DoDoubleTrace(Wall_Left)) ||
		((WallSide & Wall_Right) && (DoDoubleTrace(Wall_Right)))))
	{
		// Save trace result if a wall has been found
		OutHit = Hit;
		return true;
	}

	return false;
}

void ABotaniCharacter::Slide()
{
	if (!IsValid(BotaniMoveComp))
	{
		return;
	}

	bWantsToSlide = true;
}

void ABotaniCharacter::StopSliding()
{
	if (!IsValid(BotaniMoveComp))
	{
		return;
	}

	bWantsToSlide = false;
}

bool ABotaniCharacter::CanSlide() const
{
	if (!IsAllowedToSlide())
	{
		return false;
	}

	// Check if we're on the ground
	if (!BotaniMoveComp->IsMovingOnGround())
	{
		return false;
	}


	return true;
}

bool ABotaniCharacter::IsAllowedToSlide() const
{
	if (!IsValid(BotaniMoveComp))
	{
		return false;
	}
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC))
	{
		return true;
	}

	return !ASC->HasAnyMatchingGameplayTags(BotaniMoveComp->SlideBlockedTags);
}

bool ABotaniCharacter::IsSliding() const
{
	if (!IsValid(BotaniMoveComp))
	{
		return false;
	}

	return BotaniMoveComp->IsCustomMovementMode(CMOVE_Slide);
}

bool ABotaniCharacter::TraceSurfaceToSlideOn(FHitResult& OutHit, bool bShowDebug) const
{
	// Trace params
	const UWorld* World = GetWorld();
	check(World);

	FCollisionQueryParams QueryParams = GetIgnoreCharacterQueryParams();
	FHitResult Hit;

	// Trace direction
	const FVector Start = GetCapsuleComponent()->GetComponentLocation();
	const FVector End = Start + FVector::DownVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f;

	auto DoTrace = [&] (const FVector& InTraceStart, const FVector& InTraceEnd)
	{
		auto Result = World->LineTraceSingleByChannel(Hit, InTraceStart, InTraceEnd, ECC_Camera, QueryParams);

		if (bShowDebug)
		{
			DrawDebugLine(World, InTraceStart, InTraceEnd, Result ? FColor::Green : FColor::Orange, false, 0.1f, 0, 1.f);
		}

		return Result;
	};

	if (DoTrace(Start, End))
	{
		OutHit = Hit;
		return true;
	}

	return false;
}

bool ABotaniCharacter::IsAllowedToGrapple() const
{
	if (!IsValid(BotaniMoveComp))
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC))
	{
		return true;
	}

	return true; //@TODO: Check for tags
}

bool ABotaniCharacter::IsGrappling() const
{
	if (!IsValid(BotaniMoveComp))
	{
		return false;
	}

	return BotaniMoveComp->IsCustomMovementMode(CMOVE_Grapple);
}

bool ABotaniCharacter::CanGrapple(FHitResult& OutHit) const
{
	if (!IsValid(BotaniMoveComp))
	{
		return false;
	}

	if (!IsAllowedToGrapple())
	{
		return false;
	}
	
	if (TraceForGrappleTarget(OutHit, true))
	{
		return OutHit.Distance	<= BotaniMoveComp->Grapple_MaxTraceDistance;
	}

	return false;
}

bool ABotaniCharacter::TraceForGrappleTarget(FHitResult& OutHit, bool bShowDebug) const
{
	const UWorld* World = GetWorld();
	check(World);

	FCollisionQueryParams QueryParams = GetIgnoreCharacterQueryParams();
	FHitResult Hit;

	// Trace direction
	FVector Start;
	FRotator Rotation;
	GetActorEyesViewPoint(Start, Rotation);

	const FVector End = Start + (Rotation.Vector() * BotaniMoveComp->Grapple_MaxTraceDistance);

	auto DoTrace = [&] (const FVector& InTraceStart, const FVector& InTraceEnd)
	{
		auto Result = World->LineTraceSingleByChannel(Hit, InTraceStart, InTraceEnd, ECC_Camera, QueryParams);

		if (bShowDebug)
		{
			DrawDebugLine(World, InTraceStart, InTraceEnd, Result ? FColor::Green : FColor::Purple, false, 0.1f, 0, 1.f);
		}

		return Result;
	};

	if (DoTrace(Start, End))
	{
		OutHit = Hit;
		return true;
	}

	return false;
}

bool ABotaniCharacter::Grapple()
{
	if (!IsValid(BotaniMoveComp))
	{
		return false;
	}

	bWantsToGrapple = true;
	return true;
}

void ABotaniCharacter::StopGrapple()
{
	if (!IsValid(BotaniMoveComp))
	{
		return;
	}

	bWantsToGrapple = false;
}


void ABotaniCharacter::OnAbilitySystemInitialized()
{
	UBotaniAbilitySystemComponent* BotaniASC = GetBotaniAbilitySystemComponent();
	check(BotaniASC);

	HealthComponent->InitializeWithAbilitySystem(BotaniASC);

	InitializeGameplayTags();
}

void ABotaniCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void ABotaniCharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from a previous character
	if (UBotaniAbilitySystemComponent* BotaniASC = GetBotaniAbilitySystemComponent())
	{
		// Clear all movement mode tags
		for (const auto& TagMapping : BotaniGameplayTags::Movement::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				BotaniASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		// Clear all custom movement mode tags
		for (const auto& TagMapping : BotaniGameplayTags::Movement::Custom::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				BotaniASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		// Initialize the default movement mode tag
		//UBotaniCharacterMovementComponent* BotaniMovement = CastChecked<UBotaniCharacterMovementComponent>(GetCharacterMovement());
		//SetMovementModeTag(BotaniMovement->Movement, BotaniMovement->CustomMovementMode, true);
	}
}

void ABotaniCharacter::OnDeathStarted(AActor* OwningActor)
{
}

void ABotaniCharacter::OnDeathFinished(AActor* OwningActor)
{
}

void ABotaniCharacter::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ABotaniCharacter::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	const FGenericTeamId OldTeamID = MyTeamID;
	MyTeamID = IntegerToGenericTeamId(NewTeam);
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}
