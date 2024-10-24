// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BioDeviceSubsystem.h"
#include "UObject/Interface.h"
#include "BioDeviceInterface.generated.h"

struct FActorDeviceStateChangeParams;
class UBioDeviceManagerComponent;

/** Delegate for when the enabled state of a device changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBioDeviceEnabledStateChangedDelegate, const FName&, Device, bool, DesiredState, const bool, bForce);

/** The enabled state of a device */
UENUM(BlueprintType)
enum class EBioDeviceEnabledState : uint8
{
	/** Whether the device is enabled is defined by the device manager or a game feature */
	Inherited,

	/** The user can change the initial state of the device (However, it can still be forced) */
	Custom,

	/** The user can force the initial state of the device so that it can't be changed */
	Force,
};

/** Interface for devices that can be managed by a device manager */
UINTERFACE(MinimalAPI, meta = (BlueprintType))
class UBioDeviceInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/** Interface for devices that can be managed by a device manager */
class IBioDeviceInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	/** Returns the actor this object is bound to, might be this object. */
	BIODEVICES_API virtual AActor* GetOwningActor() const;
	
	/** Called when the device state changes */
	BIODEVICES_API virtual void OnDeviceStateChanged(const FActorDeviceStateChangeParams& Params);

	/** Called when the device state changes */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Bio|Devices", meta = (DisplayName = "On Device State Changed"))
	BIODEVICES_API void OnDeviceStateChangedBP(const FActorDeviceStateChangeParams& Params);

	/** Call to bind the OnDeviceStateChanged function to the appropriate delegate */
	BIODEVICES_API virtual void BindOnDeviceStateChanged(FName DeviceName);
	BIODEVICES_API virtual void BindOnDeviceStateChanged(FName DeviceName, FBioDeviceStateChangeDelegate& Delegate);
	
	/** Get the device manager that owns this device */
	BIODEVICES_API virtual UBioDeviceManagerComponent* GetDeviceManager() const = 0;

	/** Get the name of the device */
	BIODEVICES_API virtual FName GetDeviceName() const = 0;

	/** Check if the device is enabled */
	BIODEVICES_API virtual bool IsDeviceEnabled() const;

	/** Check if the device is active */
	BIODEVICES_API virtual bool IsDeviceActive() const;

	/** Gets the initial device-enabled state */
	BIODEVICES_API virtual EBioDeviceEnabledState GetInitialDeviceEnabledState() const;

protected:
	FDelegateHandle OnDeviceStateChangedHandle;
};
