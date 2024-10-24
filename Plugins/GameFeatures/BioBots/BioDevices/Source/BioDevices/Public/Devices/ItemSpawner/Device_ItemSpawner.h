// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BioDeviceInterface.h"
#include "GameFramework/Actor.h"
#include "Device_ItemSpawner.generated.h"

/**
 * ADevice_ItemSpawner
 *
 * A Bio Device used to spawn items.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (BioDevices))
class BIODEVICES_API ADevice_ItemSpawner : public AActor, public IBioDeviceInterface
{
	GENERATED_UCLASS_BODY()

public:
	static FName NAME_ItemSpawnerDevice;

	//~ Begin IBioDeviceInterface Interface
	virtual FName GetDeviceName() const override { return NAME_ItemSpawnerDevice; }
	virtual UBioDeviceManagerComponent* GetDeviceManager() const override;
	virtual EBioDeviceEnabledState GetInitialDeviceEnabledState() const override;
	//~ End IBioDeviceInterface Interface
};
