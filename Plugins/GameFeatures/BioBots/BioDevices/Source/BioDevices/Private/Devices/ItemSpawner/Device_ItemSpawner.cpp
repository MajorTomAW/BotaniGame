// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Devices/ItemSpawner/Device_ItemSpawner.h"

FName ADevice_ItemSpawner::NAME_ItemSpawnerDevice(TEXT("ItemSpawnerDevice"));

ADevice_ItemSpawner::ADevice_ItemSpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

UBioDeviceManagerComponent* ADevice_ItemSpawner::GetDeviceManager() const
{
	return nullptr; //@TODO: Add a device manager component to this device
}

EBioDeviceEnabledState ADevice_ItemSpawner::GetInitialDeviceEnabledState() const
{
	return EBioDeviceEnabledState::Inherited;
}
