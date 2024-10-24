// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BioDeviceManagerComponent.h"

#include "BioDeviceInterface.h"
#include "BioDeviceSubsystem.h"

UBioDeviceManagerComponent::UBioDeviceManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UActorComponent::SetAutoActivate(true);
	
	InitialEnabledState = EBioDeviceEnabledState::Inherited;
	bDefaultEnabledState = true;
	bDefaultActiveState = true;
	Device = nullptr;
}

void UBioDeviceManagerComponent::RegisterDevice(UObject* InDevice)
{
	check(InDevice);
	if (Device == InDevice)
	{
		return;
	}

	Device = InDevice;

	// Get the device interface from the device
	const IBioDeviceInterface* DeviceInterface = GetDeviceInterfaceFromDevice();
	if (DeviceInterface == nullptr)
	{
		return;
	}

	AActor* MyActor = Cast<AActor>(Device);
	const FName DeviceName = DeviceInterface->GetDeviceName();

	// Register the device with the device subsystem
	if (UBioDeviceSubsystem* DeviceSubsystem = UBioDeviceSubsystem::GetForActor(MyActor))
	{
		DeviceSubsystem->RegisterDevice(MyActor, DeviceName, this, bDefaultEnabledState, bDefaultActiveState);
	}
}

bool UBioDeviceManagerComponent::AttemptToChangeDeviceEnabledState(const bool bEnabled, const bool bForce)
{
	if (!HasAuthority())
	{
		return false;
	}
	
	if (Device == nullptr)
	{
		return false;
	}

	// Check if the device is already in the desired state
	if (const IBioDeviceInterface* DeviceInterface = GetDeviceInterfaceFromDevice())
	{
		if (DeviceInterface->IsDeviceEnabled() == bEnabled)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	// Force enabled rejects the change (However, it can still be forced)
	if (InitialEnabledState == EBioDeviceEnabledState::Force && !bForce)
	{
		return false;
	}

	FName DeviceName = NAME_None;
	if (const IBioDeviceInterface* DeviceInterface = GetDeviceInterfaceFromDevice())
	{
		DeviceName = DeviceInterface->GetDeviceName();
	}

	if (UBioDeviceSubsystem* DeviceSubsystem = UBioDeviceSubsystem::GetForActor(Cast<AActor>(Device)))
	{
		AActor* MyActor = Cast<AActor>(Device);
		DeviceSubsystem->ChangeDeviceEnabledState(MyActor, DeviceName, this, bEnabled, bForce);
	}

	return true;
}

bool UBioDeviceManagerComponent::AttemptToChangeDeviceActiveState(const bool bNewActive)
{
	if (!HasAuthority())
	{
		return false;
	}
	
	if (Device == nullptr)
	{
		return false;
	}

	if (const IBioDeviceInterface* DeviceInterface = GetDeviceInterfaceFromDevice())
	{
		if (!DeviceInterface->IsDeviceEnabled())
		{
			UE_LOG(LogTemp, Warning, TEXT("Attempting to activate device: %s, but it is not enabled."), *DeviceInterface->GetDeviceName().ToString());
			return false;
		}

		if (DeviceInterface->IsDeviceActive())
		{
			UE_LOG(LogTemp, Warning, TEXT("Attempting to activate device: %s, but it is already active."), *DeviceInterface->GetDeviceName().ToString());
			return false;
		}
	}

	FName DeviceName = NAME_None;
	if (const IBioDeviceInterface* DeviceInterface = GetDeviceInterfaceFromDevice())
	{
		DeviceName = DeviceInterface->GetDeviceName();
	}

	if (UBioDeviceSubsystem* DeviceSubsystem = UBioDeviceSubsystem::GetForActor(Cast<AActor>(Device)))
	{
		AActor* MyActor = Cast<AActor>(Device);
		DeviceSubsystem->ChangeDeviceActiveState(MyActor, DeviceName, this, bNewActive);
	}

	return true;
}

void UBioDeviceManagerComponent::OnRegister()
{
	Super::OnRegister();
}

void UBioDeviceManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// If the device is not set, assume the owner is the device
	if (Device == nullptr)
	{
		Device = GetOwner();

		if (Device == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("BioDeviceManagerComponent: Device is not set and owner is nullptr. Cannot register device."));
			return;
		}
	}

	RegisterDevice(Device);
	
	if (IBioDeviceInterface* DeviceInterface = GetDeviceInterfaceFromDevice())
	{
		DeviceInterface->BindOnDeviceStateChanged(GetFeatureName());
	}
}

FName UBioDeviceManagerComponent::GetFeatureName() const
{
	if (const IBioDeviceInterface* DeviceInterface = GetDeviceInterfaceFromDevice())
	{
		return DeviceInterface->GetDeviceName();
	}

	return NAME_None;
}

#if WITH_EDITOR
void UBioDeviceManagerComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	// Make sure the device is updated
	if (Device != nullptr)
	{
		Device->PostEditChangeProperty(PropertyChangedEvent);
	}

	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_CHECKED(ThisClass, InitialEnabledState))
	{
		if (InitialEnabledState == EBioDeviceEnabledState::Inherited)
		{
			bDefaultEnabledState = true;
			bDefaultActiveState = true;
		}
	}

	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_CHECKED(ThisClass, bDefaultEnabledState))
	{
		if (!bDefaultEnabledState)
		{
			bDefaultActiveState = false;
		}
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

IBioDeviceInterface* UBioDeviceManagerComponent::GetDeviceInterfaceFromDevice() const
{
	if (Device == nullptr)
	{
		return nullptr;
	}

	// Works only in C++
	if (IBioDeviceInterface* DeviceInterface = Cast<IBioDeviceInterface>(Device))
	{
		return DeviceInterface;
	}

	return nullptr;
}

