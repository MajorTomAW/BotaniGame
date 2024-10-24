// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BioDeviceInterface.h"
#include "AbilitySystem/BotaniTaggedActor.h"

#include "BioDevice_AISpawner.generated.h"

/**
 * ABioDevice_AISpawner
 *
 * An Bio Device used to spawn Character AI's.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, meta = (DisplayName = "BioDevice_AISpawner"), HideCategories = (Object, Rendering, Replication, Actor, Input, HLOD, Physics, Networking, LevelInstance, Cooking, DataLayers, Collision, Gameplay))
class ABioDevice_AISpawner : public ABotaniTaggedActor, public IBioDeviceInterface
{
	GENERATED_UCLASS_BODY()

public:
	static const FName NAME_AISpawnerDevice;

	//~ Begin IBioDeviceInterface Interface
	virtual FName GetDeviceName() const override { return NAME_AISpawnerDevice; }
	virtual UBioDeviceManagerComponent* GetDeviceManager() const override;
	virtual void OnDeviceStateChanged(const FActorDeviceStateChangeParams& Params) override;
	//~ End IBioDeviceInterface Interface

	//~ Begin AActor Interface
	virtual void PostInitializeComponents() override;
	//~ End AActor Interface
	
protected:
	/** Empty scene component used for the origin of the spawn radius */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Spawner")
	TObjectPtr<class USceneComponent> SpawnRadiusOrigin;
	
	/** The device manager that owns this device */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device")
	TObjectPtr<class UBioDeviceManagerComponent> DeviceManager;

	/** The Gameplay Message Receiver component that manages spawning ai's and bots */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device")
	TObjectPtr<class UBioCharacterSpawnerComponent> CharacterSpawnerComponent;
	
protected:
	/** Determines if we need to show the spawn radius or not. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", AdvancedDisplay)
	uint32 bShowSpawnRadius : 1;

	/** The maximum distance from the device that an AI can spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", AdvancedDisplay, meta = (Units = "m", ClampMin = 1, ClampMax = 50, UIMin = 1, UIMax = 50))
	float SpawnRadius;

private:
	/** Gets a random spawn location within the spawn radius */
	FVector GetSpawnLocation() const;
};
