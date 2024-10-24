// Copyright © 2024 MajorT. All rights reserved.


#include "Libraries/ProjectileFunctionLibrary.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/ConversionsFunctionLibrary.h"

UProjectileFunctionLibrary::UProjectileFunctionLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UProjectileFunctionLibrary::CalculateProjectileLaunchAngle(
	FVector StartLocation, FVector TargetLocation, float LaunchSpeed, float OverrideGravityZ, bool bSmallestAngle, bool& bIsReachable, float& OutAngle)
{
	const float Gravity = OverrideGravityZ * UConversionsFunctionLibrary::GetGravityConstantInCmS();
	const float Distance = FVector::DistXY(StartLocation, TargetLocation);
	float Height = FMath::Abs(TargetLocation.Z - StartLocation.Z);
	// if (bSmallestAngle) Height = FMath::Abs(TargetLocation.Z - 100.f - StartLocation.Z);

	// Quadratic equation
	const float a = (Gravity * FMath::Square(Distance)) / FMath::Square(LaunchSpeed) - Height;
	const float b_1 = FMath::Sqrt(FMath::Square(Height) + FMath::Square(Distance));
	const float b_2 = -1 * b_1;
	const float PotentialResult_1 = FMath::RadiansToDegrees(FMath::Acos(a / b_1));
	const float PotentialResult_2 = FMath::RadiansToDegrees(FMath::Acos(a / b_2));

	if (PotentialResult_1 == 0.f && PotentialResult_2 == 180.f)
	{
		bIsReachable = false;
		return;
	}

	// Calculate the angle
	const float d_1 = PotentialResult_1 + FMath::RadiansToDegrees(FMath::Atan(Distance / Height));
	const float d_2 = PotentialResult_2 + FMath::RadiansToDegrees(FMath::Atan(Distance / Height));
	const float result_1 = d_1 / 2.f;
	const float result_2 = FMath::Abs(90.f - (d_2 / 2.f));

	// Set the angle
	bIsReachable = true;
	bSmallestAngle ? OutAngle = FMath::Min(result_1, result_2) : OutAngle = FMath::Max(result_1, result_2);
}


void UProjectileFunctionLibrary::PredictProjectileTargetLocation(FVector StartLocation, FVector TargetLocation,
	FVector TargetVelocity, float LaunchSpeed, FVector& PredictedLocation)
{
	const float Distance = FVector::Distance(StartLocation, TargetLocation);
	PredictedLocation = Distance / LaunchSpeed * TargetVelocity + TargetLocation;
}

void UProjectileFunctionLibrary::SpawnCommonProjectile(UObject* WorldContextObject, TSubclassOf<AActor> ProjectileClass,
	FVector SpawnLocation, float YawRotation, float LaunchAngle, AActor* Owner, APawn* Instigator, bool bNoCollisionFail,
	AActor*& SpawnedProjectile)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	const FRotator SpawnRotation = FRotator(LaunchAngle, YawRotation, 0.f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = bNoCollisionFail ? ESpawnActorCollisionHandlingMethod::AlwaysSpawn : ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
	SpawnedProjectile = World->SpawnActor(ProjectileClass, &SpawnLocation, &SpawnRotation, SpawnParams);
}

AActor* UProjectileFunctionLibrary::SpawnCommonProjectileFromClass(UObject* WorldContextObject, TSubclassOf<AActor> ProjectileClass,
	FVector SpawnLocation, FVector TargetLocation, AActor* HomingTarget, AActor* Owner, APawn* Instigator)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	check(World);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* NewActor = World->SpawnActorDeferred<AActor>(ProjectileClass, FTransform::Identity, Owner, Instigator, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (NewActor == nullptr)
	{
		return nullptr;
	}

	UProjectileMovementComponent* PrjMoveComp = NewActor->FindComponentByClass<UProjectileMovementComponent>();
	if (PrjMoveComp == nullptr)
	{
		NewActor->Destroy();
		return nullptr;
	}

	float LaunchAngle;
	bool bIsReachable;
	CalculateProjectileLaunchAngle(
		SpawnLocation,
		TargetLocation,
		PrjMoveComp->InitialSpeed,
		PrjMoveComp->ProjectileGravityScale,
		true,
		bIsReachable,
		LaunchAngle);

	if (HomingTarget != nullptr)
	{
		PrjMoveComp->HomingTargetComponent = HomingTarget->GetRootComponent();	
	}

	LaunchAngle = LaunchAngle > 45.f ? 90.f - LaunchAngle : LaunchAngle;
	if (!bIsReachable)
	{
		LaunchAngle = 45.f;
	}

	FRotator AvatarRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetLocation);
	AvatarRotation.Pitch = LaunchAngle;

	const FTransform SpawnTransform = FTransform(AvatarRotation, SpawnLocation);

	NewActor->FinishSpawning(SpawnTransform, false);
	return NewActor;
}
