// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "BotaniProjectile.generated.h"

class UNiagaraSystem;
/**
 * ABotaniProjectile
 *
 * The base class for all projectiles in BotaniGame.
 * Projectiles are spawned by weapons and are used to deal damage to enemies.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, meta = (ShortTooltip = "The base class for all projectiles in BotaniGame."), HideCategories = ("Replication", "ActorTick", "Actor Tick", "Rendering", "Actor", "Input", "Collision", "HLOD", "Events", "Lighting", "Cooking", "Physics", "DataLayers", "WorldPartition", "LevelInstance"))
class BOTANIGAME_API ABotaniProjectile : public AActor
{
	GENERATED_UCLASS_BODY()

public:
protected:
	//~ Begin AActor Interface
	virtual void PostInitializeComponents() override;
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;
	virtual void BeginPlay() override;
	//~ End AActor Interface

	/** Called when the projectile suddenly stops moving. (due to an impact) */
	UFUNCTION()
	virtual void OnProjectileImpact(const FHitResult& HitResult);

	/** Called when the collision component overlaps another actor. */
	UFUNCTION()
	virtual void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void ApplyDamageFromHit(const FHitResult& HitResult);

	/** The controller that spawned this projectile. */
	TWeakObjectPtr<AController> ControllerPtr;

protected:
	/** The projectile movement component for this projectile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBotaniProjectileMovementComponent> ProjectileMovementComponent;

	/** The collision component for this projectile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> CollisionComponent;
	static FName NAME_CollisionComponent;

	/** The particle system component for this projectile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNiagaraComponent> ParticleSystemComponent;

	/** The particle trail effect to spawn when this projectile is moving. */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|VFX")
	TSoftObjectPtr<UNiagaraSystem> ParticleTrailEffect;

	/** The socket name to attach the trail effect to. */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|VFX")
	FName ParticleTrailSocketName;


	/** The explosion template to spawn when this projectile impacts something. */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|VFX")
	TSubclassOf<class ABotaniExplosionTemplate> ExplosionTemplate;

	/** The default damage effect to apply when this projectile impacts something. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Damage")
	TSoftClassPtr<class UGameplayEffect> DefaultDamageEffect;

	/** The maximum life spawn of this projectile. */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (ClampMin = "0.0", UIMin = "0.0", Units = "seconds"))
	float ProjectileLifeSpan;

protected:
	/** Replicated status whether this projectile has exploded. */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Exploded)
	bool bExploded;

	UFUNCTION()
	virtual void OnRep_Exploded();

	/** Triggers the explosion of this projectile. */
	virtual void Explode(const FHitResult& Impact);

	/** Shut down the projectile and prepare for destruction. */
	virtual void DisableAndDestroy();

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext) override;
	virtual void PostCDOCompiled(const FPostCDOCompiledContext& Context) override;
#endif

private:
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TrailComponent;
};
