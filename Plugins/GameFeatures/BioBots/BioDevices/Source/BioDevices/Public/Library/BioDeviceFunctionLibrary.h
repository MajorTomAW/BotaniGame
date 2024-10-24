// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BioDeviceFunctionLibrary.generated.h"

class IBioDeviceInterface;
struct FActorDeviceStateChangeParams;
/**
 * 
 */
UCLASS()
class BIODEVICES_API UBioDeviceFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Converts a device state change params to a string */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "To String (Device State Params)", CompactNodeTitle = "->", BlueprintAutocast), Category="Utilities|String")
	static FString Conv_DeviceStateParamsToString(const FActorDeviceStateChangeParams& Params);

public:
	/** Check if the device is enabled */
	UFUNCTION(BlueprintPure, Category = "Bio|Devices", meta = (DefaultToSelf = "Device"))
	static bool IsDeviceEnabled(UObject* Device);

	/** Check if the device is active */
	UFUNCTION(BlueprintPure, Category = "Bio|Devices", meta = (DefaultToSelf = "Device"))
	static bool IsDeviceActive(UObject* Device);

	/** Get the device name */
	UFUNCTION(BlueprintPure, Category = "Bio|Devices", meta = (DefaultToSelf = "Device"))
	static FName GetDeviceName(UObject* Device);

private:
	static IBioDeviceInterface* GetDeviceInterface(UObject* Device);
};
