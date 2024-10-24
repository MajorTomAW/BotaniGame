// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Library/BioDeviceFunctionLibrary.h"

#include "BioDeviceInterface.h"
#include "BioDeviceSubsystem.h"

FString UBioDeviceFunctionLibrary::Conv_DeviceStateParamsToString(const FActorDeviceStateChangeParams& Params)
{
	return Params.ToString();
}

bool UBioDeviceFunctionLibrary::IsDeviceEnabled(UObject* Device)
{
	if (const IBioDeviceInterface* DeviceInterface = GetDeviceInterface(Device))
	{
		return DeviceInterface->IsDeviceEnabled();
	}

	return false;
}

bool UBioDeviceFunctionLibrary::IsDeviceActive(UObject* Device)
{
	if (const IBioDeviceInterface* DeviceInterface = GetDeviceInterface(Device))
	{
		return DeviceInterface->IsDeviceActive();
	}

	return false;
}

FName UBioDeviceFunctionLibrary::GetDeviceName(UObject* Device)
{
	if (const IBioDeviceInterface* DeviceInterface = GetDeviceInterface(Device))
	{
		return DeviceInterface->GetDeviceName();
	}

	return NAME_None;
}

IBioDeviceInterface* UBioDeviceFunctionLibrary::GetDeviceInterface(UObject* Device)
{
	if (Device == nullptr)
	{
		return nullptr;
	}

	return Cast<IBioDeviceInterface>(Device);
}
