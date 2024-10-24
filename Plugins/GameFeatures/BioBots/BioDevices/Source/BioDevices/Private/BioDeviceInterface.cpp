// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BioDeviceInterface.h"

#include "BioDeviceManagerComponent.h"
#include "BioDeviceSubsystem.h"

UBioDeviceInterface::UBioDeviceInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

AActor* IBioDeviceInterface::GetOwningActor() const
{
	AActor* MutableActor = const_cast<AActor*>(Cast<AActor>(this));

	if (MutableActor == nullptr)
	{
		const UActorComponent* ActorComponent = Cast<UActorComponent>(this);
		if (ActorComponent)
		{
			MutableActor = ActorComponent->GetOwner();
		}
	}

	if (ensure(MutableActor))
	{
		return MutableActor;
	}

	return nullptr;
}

void IBioDeviceInterface::OnDeviceStateChanged(const FActorDeviceStateChangeParams& Params)
{
	Execute_OnDeviceStateChangedBP(GetOwningActor(), Params);
}


void IBioDeviceInterface::BindOnDeviceStateChanged(FName DeviceName)
{
	UObject* This = Cast<UObject>(this);
	AActor* MyActor = GetOwningActor();
	UBioDeviceSubsystem* DeviceSubsystem = UBioDeviceSubsystem::GetForActor(MyActor);

	if (ensure(MyActor && DeviceSubsystem))
	{
		// Bind as a weak lambda because this is not a UObject but is guaranteed to be valid as long as ThisObject is
		FBioDeviceStateChangeDelegate Delegate = FBioDeviceStateChangeDelegate::CreateWeakLambda(This,
			[this](const FActorDeviceStateChangeParams& _params)
			{
				this->OnDeviceStateChanged(_params);
			});

		OnDeviceStateChangedHandle = DeviceSubsystem->RegisterDeviceCallback(MyActor, MoveTemp(Delegate), DeviceName, true);
	}
}

void IBioDeviceInterface::BindOnDeviceStateChanged(FName DeviceName, FBioDeviceStateChangeDelegate& Delegate)
{
	UObject* This = Cast<UObject>(this);
	AActor* MyActor = GetOwningActor();
	UBioDeviceSubsystem* DeviceSubsystem = UBioDeviceSubsystem::GetForActor(MyActor);

	if (ensure(MyActor && DeviceSubsystem))
	{
		OnDeviceStateChangedHandle = DeviceSubsystem->RegisterDeviceCallback(MyActor, Delegate, DeviceName, true);
	}
}

bool IBioDeviceInterface::IsDeviceEnabled() const
{
	const UBioDeviceSubsystem* DeviceSubsystem = UBioDeviceSubsystem::GetForActor(GetOwningActor());
	return DeviceSubsystem ? DeviceSubsystem->IsDeviceEnabled(GetOwningActor(), GetDeviceName()) : false;
}

bool IBioDeviceInterface::IsDeviceActive() const
{
	const UBioDeviceSubsystem* DeviceSubsystem = UBioDeviceSubsystem::GetForActor(GetOwningActor());
	return DeviceSubsystem ? DeviceSubsystem->IsDeviceActive(GetOwningActor(), GetDeviceName()) : false;
}

EBioDeviceEnabledState IBioDeviceInterface::GetInitialDeviceEnabledState() const
{
	const UBioDeviceManagerComponent* DeviceManager = GetDeviceManager();
	return DeviceManager ? DeviceManager->InitialEnabledState : EBioDeviceEnabledState::Inherited;
}
