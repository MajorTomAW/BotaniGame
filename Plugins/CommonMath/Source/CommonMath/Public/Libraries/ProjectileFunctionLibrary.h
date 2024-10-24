// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProjectileFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class COMMONMATH_API UProjectileFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Projectile Path */
	UFUNCTION(BlueprintCallable, Category = "Common Projectile|Math", meta = (DisplayName = "Calculate Projectile Launch Angle"))
	static void CalculateProjectileLaunchAngle(FVector StartLocation, FVector TargetLocation, float LaunchSpeed, float OverrideGravityZ, bool bSmallestAngle, bool& bIsReachable, float &OutAngle);

	UFUNCTION(BlueprintCallable, Category = "Common Projectile|Math", meta = (DisplayName = "Predict Projectile Target Location"))
	static void PredictProjectileTargetLocation(FVector StartLocation, FVector TargetLocation, FVector TargetVelocity, float LaunchSpeed, FVector& PredictedLocation);

	UFUNCTION(BlueprintCallable, Category = "Common Projectile|Actions", meta = (DisplayName = "Spawn Common Projectile", WorldContext = "WorldContextObject"))
	static void SpawnCommonProjectile(UObject* WorldContextObject, TSubclassOf<AActor> ProjectileClass, FVector SpawnLocation, float YawRotation, float LaunchAngle, AActor* Owner, APawn* Instigator, bool bNoCollisionFail, AActor*& SpawnedProjectile);
	
	UFUNCTION(BlueprintCallable, Category = "Common Projectile|Actions", meta = (DisplayName = "Spawn Common Projectile (from Class)", WorldContext = "WorldContextObject", DeterminesOutputType = "ProjectileClass"))
	static AActor* SpawnCommonProjectileFromClass(UObject* WorldContextObject, TSubclassOf<AActor> ProjectileClass, FVector SpawnLocation, FVector TargetLocation, AActor* HomingTarget, AActor* Owner, APawn* Instigator);
};
