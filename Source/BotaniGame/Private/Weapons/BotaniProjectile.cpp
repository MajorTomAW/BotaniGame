// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Weapons/BotaniProjectile.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#if WITH_EDITOR
#include "UObject/ObjectSaveContext.h"
#endif

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BotaniCollisionChannels.h"
#include "BotaniLogChannels.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Teams/Subsystem/BotaniTeamSubsystem.h"
#include "Weapons/Components/BotaniProjectileMovementComponent.h"
#include "Weapons/Templates/BotaniExplosionTemplate.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniProjectile)

FName ABotaniProjectile::NAME_CollisionComponent(TEXT("ProjectileCollision"));

ABotaniProjectile::ABotaniProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Setting up the projectile collision component
	CollisionComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, NAME_CollisionComponent);
	CollisionComponent->InitSphereRadius(25.f);
	CollisionComponent->AlwaysLoadOnClient = true;
	CollisionComponent->AlwaysLoadOnServer = true;
	CollisionComponent->bTraceComplexOnMove = true;
	// CollisionComponent->SetCollisionProfileName();
	RootComponent = CollisionComponent;

	// Setting up the projectile movement component
	ParticleSystemComponent = ObjectInitializer.CreateDefaultSubobject<UNiagaraComponent>(this, TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(CollisionComponent);
	ParticleSystemComponent->bAutoActivate = true;

	// Setting up the projectile movement component
	ProjectileMovementComponent = ObjectInitializer.CreateDefaultSubobject<UBotaniProjectileMovementComponent>(this, TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
	ProjectileMovementComponent->InitialSpeed = 3000.f;
	ProjectileMovementComponent->MaxSpeed = 3000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->bInterpMovement = true;
	ProjectileMovementComponent->bInterpRotation = true;

	// Setting up the projectile
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);

	ProjectileLifeSpan = 16.0f;
	InitialLifeSpan = ProjectileLifeSpan;
	bNetLoadOnClient = false;
	bReplicates = true;
	SetReplicatingMovement(true);
}

void ABotaniProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &ThisClass::OnProjectileImpact);
	
	CollisionComponent->MoveIgnoreActors.Add(GetInstigator());
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnProjectileOverlap);

	SetLifeSpan(ProjectileLifeSpan);
	ControllerPtr = GetInstigatorController();
}

void ABotaniProjectile::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	Super::PostNetReceiveVelocity(NewVelocity);

	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->Velocity = NewVelocity;
	}
}

void ABotaniProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->MoveIgnoreActors.Add(GetInstigator());
	CollisionComponent->MoveIgnoreActors.Add(GetOwner());

	if (!ParticleTrailEffect.IsNull())
	{
		TrailComponent = UNiagaraFunctionLibrary::SpawnSystemAttached
			(
				ParticleTrailEffect.LoadSynchronous(),
				RootComponent,
				ParticleTrailSocketName,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				false,
				true
			);
	}
}

void ABotaniProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bExploded);
}

void ABotaniProjectile::OnProjectileImpact(const FHitResult& HitResult)
{
	// Only the server should handle the impact
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (bExploded)
	{
		return;
	}

	// Disable collision to make sure we don't hit anything twice
	SetActorEnableCollision(false);

	// Apply damage
	ApplyDamageFromHit(HitResult);
	
	Explode(HitResult);
	DisableAndDestroy();
}

void ABotaniProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Only the server should handle the impact
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	UBotaniTeamSubsystem* TeamSub = GetWorld()->GetSubsystem<UBotaniTeamSubsystem>();
	check(TeamSub);

	// Make sure we dont explode when hitting ourselves or a friendly target
	if (!TeamSub->CanCauseDamage(GetInstigator(), SweepResult.GetActor(), false))
	{
		return;
	}

	if (bExploded)
	{
		return;
	}

	// Disable collision to make sure we don't hit anything twice
	SetActorEnableCollision(false);

	// Apply damage
	ApplyDamageFromHit(SweepResult);

	Explode(SweepResult);
	DisableAndDestroy();
}

void ABotaniProjectile::ApplyDamageFromHit(const FHitResult& HitResult)
{
	if (AActor* TargetActor = HitResult.GetActor())
	{
		// Check for friendly fire
		UBotaniTeamSubsystem* TeamSub = GetWorld()->GetSubsystem<UBotaniTeamSubsystem>();
		check(TeamSub);

		if (TeamSub->CanCauseDamage(GetInstigator(), TargetActor, false))
		{
			APlayerState* PlayerState = ControllerPtr.Get()->PlayerState;
			UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
			UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState);

			if (TargetASC && SourceASC && PlayerState)
			{
				FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
				ContextHandle.AddSourceObject(PlayerState);
				ContextHandle.AddHitResult(HitResult);
				ContextHandle.AddInstigator(PlayerState, this);

				const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DefaultDamageEffect.LoadSynchronous(), 1.f, ContextHandle);

				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}
			else
			{
				BOTANI_LOG(Error, TEXT("Failed to apply damage to %s. Source %s or target %s ability system component is null."), *GetNameSafe(TargetActor), *GetNameSafe(GetInstigator()), *GetNameSafe(TargetActor));
			}
		}
		else
		{
			BOTANI_LOG(Log, TEXT("Friendly fire prevented by %s"), *GetNameSafe(this));
		}
	}
}

void ABotaniProjectile::OnRep_Exploded()
{
	FVector PrjDirection = GetActorForwardVector();

	const FVector Start = GetActorLocation() - PrjDirection * 200.f;
	const FVector End = GetActorLocation() + PrjDirection * 200.f;

	FHitResult Impact;
	if (!GetWorld()->LineTraceSingleByChannel(Impact, Start, End, BOTANI_TRACE_CHANNEL_PROJECTILE, FCollisionQueryParams(SCENE_QUERY_STAT(ProjClient), true, GetInstigator())))
	{
		// If we didn't hit anything, just use actor location
		Impact.ImpactPoint = GetActorLocation();
		Impact.ImpactNormal = -PrjDirection;
	}

	Explode(Impact);
}

void ABotaniProjectile::Explode(const FHitResult& Impact)
{
	if (ParticleSystemComponent)
	{
		ParticleSystemComponent->Deactivate();
	}

	if (TrailComponent)
	{
		TrailComponent->Deactivate();
	}

	// Effects and damage origin shouldn't be placed inside the mesh at impact point
	const FVector NudgedImpactLocation = Impact.ImpactPoint + Impact.ImpactNormal * 10.f;

	if (ExplosionTemplate != nullptr)
	{
		FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), NudgedImpactLocation);
		ABotaniExplosionTemplate* const Template = GetWorld()->SpawnActorDeferred<ABotaniExplosionTemplate>(ExplosionTemplate, SpawnTransform);
		if (Template)
		{
			//Template->SurfaceHit = Impact; @TODO: Implement this
			Template->FinishSpawning(SpawnTransform);
		}
	}

	bExploded = true;
}

void ABotaniProjectile::DisableAndDestroy()
{
	UAudioComponent* AudioComponent = FindComponentByClass<UAudioComponent>();
	if (AudioComponent && AudioComponent->IsPlaying())
	{
		AudioComponent->FadeOut(0.1f, 0.f);
	}

	ProjectileMovementComponent->StopMovementImmediately();

	// Give clients some time to show the explosion
	SetLifeSpan(2.f);
}

#if WITH_EDITOR
void ABotaniProjectile::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	InitialLifeSpan = ProjectileLifeSpan;
}

void ABotaniProjectile::PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext)
{
	Super::PostSaveRoot(ObjectSaveContext);
	InitialLifeSpan = ProjectileLifeSpan;
}

void ABotaniProjectile::PostCDOCompiled(const FPostCDOCompiledContext& Context)
{
	InitialLifeSpan = ProjectileLifeSpan;
	Super::PostCDOCompiled(Context);
}
#endif
