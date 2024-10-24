// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Device/BioDevice_AISpawner.h"

#include "BioDeviceManagerComponent.h"
#include "BioCharacterSpawnerComponent.h"
#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BioDevice_AISpawner)

const FName ABioDevice_AISpawner::NAME_AISpawnerDevice("AISpawnerDevice");

ABioDevice_AISpawner::ABioDevice_AISpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SpawnRadiusOrigin = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Spawn Radius Origin"));
	SetRootComponent(SpawnRadiusOrigin);
	
	DeviceManager = ObjectInitializer.CreateDefaultSubobject<UBioDeviceManagerComponent>(this, TEXT("Device Manager"));
	CharacterSpawnerComponent = ObjectInitializer.CreateDefaultSubobject<UBioCharacterSpawnerComponent>(this, TEXT("CharacterSpawnerComponent"));

	bShowSpawnRadius = true;
}

UBioDeviceManagerComponent* ABioDevice_AISpawner::GetDeviceManager() const
{
	return DeviceManager;
}

void ABioDevice_AISpawner::OnDeviceStateChanged(const FActorDeviceStateChangeParams& Params)
{
	IBioDeviceInterface::OnDeviceStateChanged(Params);

	// Only the server should be able to start/stop spawning
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	// Start or stop spawning based on the device state
	if (Params.bDeviceEnabled && Params.bDeviceActive)
	{
		CharacterSpawnerComponent->StartSpawning();
	}
	else
	{
		CharacterSpawnerComponent->StopSpawning();
	}
}

void ABioDevice_AISpawner::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ensure(DeviceManager))
	{
		DeviceManager->RegisterDevice(this);
	}

	if (ensure(CharacterSpawnerComponent))
	{
		CharacterSpawnerComponent->OnGetSpawnLocation.BindUObject(this, &ThisClass::GetSpawnLocation);
	}
}

FVector ABioDevice_AISpawner::GetSpawnLocation() const
{
	// Get a random reach-able location within the spawn radius
	FNavLocation NavLocation;
	const FVector Origin = GetActorLocation();

	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if ( NavSystem->GetRandomReachablePointInRadius(Origin, SpawnRadius, NavLocation) )
	{
		return NavLocation.Location;
	}

	return GetActorLocation();
}
