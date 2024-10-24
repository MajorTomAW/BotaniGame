// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BotaniProjectileMovementComponent.generated.h"

/**
 * UBotaniProjectileMovementComponent
 *
 * The movement component for a projectile
 */
UCLASS(Config = Game, MinimalAPI)
class UBotaniProjectileMovementComponent : public UProjectileMovementComponent
{
	GENERATED_UCLASS_BODY()

public:
protected:
	/** The maximum number of bounces the projectile can make */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Bounces", meta = (UIMin = "0"))
	int32 MaxBounces;

protected:
	//~ Begin UProjectileMovementComponent Interface
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) override;
	//~ End UProjectileMovementComponent Interface

	/** The current number of bounces the projectile has made */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Projectile Bounces")
	int32 CurrentBounces;

protected:
	virtual bool CanBounce() const;
};
