// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BioDeviceSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FActorDeviceStateChangeParams
{
	GENERATED_BODY()

	FActorDeviceStateChangeParams()
		: OwningActor(nullptr)
		, DeviceName(NAME_None)
		, Implementer(nullptr)
	{
	}

	FActorDeviceStateChangeParams(AActor* InOwningActor, FName InDeviceName, UObject* Implementer, bool bEnabled, bool bActive)
		: OwningActor(InOwningActor)
		, DeviceName(InDeviceName)
		, Implementer(Implementer)
		, bDeviceEnabled(bEnabled)
		, bDeviceActive(bActive)
	{
	}

public:
	/** The actor owning the device that changed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Device")
	TObjectPtr<AActor> OwningActor;

	/** Name of the device that changed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Device")
	FName DeviceName;

	/** The object (often a component) that implements the device */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Device")
	TObjectPtr<UObject> Implementer;

	/** The new enabled state of the device */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Device")
	uint32 bDeviceEnabled : 1 = 0;

	/** The new active state of the device */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Device")
	uint32 bDeviceActive : 1 = 0;

public:
	FString ToString() const;
};

/** An internal delegate that gets called when the device is activated or deactivated */
DECLARE_DELEGATE_OneParam(FBioDeviceStateChangeDelegate, const FActorDeviceStateChangeParams&);

/** Blueprint delegate called when an actor device changes state */
DECLARE_DYNAMIC_DELEGATE_OneParam(FBioDeviceStateChangeBPDelegate, const FActorDeviceStateChangeParams&, Params);

/**
 * UBioDeviceSubsystem
 *
 * A subsystem for managing devices that can be tracked on actors.
 */
UCLASS(BlueprintType)
class BIODEVICES_API UBioDeviceSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UBioDeviceSubsystem();
	
	//~ Begin UWorldSubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PostInitialize() override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//~ End UWorldSubsystem Interface

public:
	/** Utility to get this device subsystem from an actor, will return null if actor is null or not in a world. */
	static UBioDeviceSubsystem* GetForActor(const AActor* Actor);

	/** Registers a device for tracking on an actor. */
	bool RegisterDevice(AActor* Actor, FName DeviceName, UObject* Implementer, bool bDefaultEnabledState = true, bool bDefaultActiveState = true);

	/** Registers a device for tracking on an actor. */
	FDelegateHandle RegisterDeviceCallback(AActor* Actor, FBioDeviceStateChangeDelegate Delegate, FName DeviceName = NAME_None, bool bCallImmediately = true);

	bool ChangeDeviceActiveState(AActor* Actor, FName DeviceName, UObject* Implementer, bool bActive);
	bool ChangeDeviceEnabledState(AActor* Actor, FName DeviceName, UObject* Implementer, bool bEnabled, bool bForce = false);
	bool IsDeviceActive(AActor* Actor, FName DeviceName) const;
	bool IsDeviceEnabled(AActor* Actor, FName DeviceName) const;

private:
	/** State for a specific device */
	struct FActorDeviceState
	{
		/** The device name that is tracking. */
		FName DeviceName;

		/** The enabled state of the device. */
		uint32 bDeviceEnabledState : 1;

		/** The active state of the device. */
		uint32 bDeviceActiveState : 1;

		/** The object implementing this device. Can be null. */
		TWeakObjectPtr<UObject> Implementer;
	};

	/** Holds the list of device delegates. */
	struct FActorDeviceStateDelegate
	{
		FActorDeviceStateDelegate(FBioDeviceStateChangeDelegate&& InDelegate, FName InDeviceName = NAME_None);
		FActorDeviceStateDelegate(FBioDeviceStateChangeBPDelegate&& InDelegate, FName InDeviceName = NAME_None);
		
		/** Call the appropriate native/bp delegate for the device state change. */
		void Execute(AActor* OwningActor, FName DeviceName, UObject* Implementer, bool bEnabled, bool bActive);

		/** Delegate that is called when the device state changes. */
		FBioDeviceStateChangeDelegate Delegate;

		/** BP Delegate that is called when the device state changes. */
		FBioDeviceStateChangeBPDelegate BPDelegate;

		/** A handle assigned to this delegate so it acts like a multicast delegate for removal. */
		FDelegateHandle DelegateHandle;

		/** If this is not null, will only activate for specific device names. */
		FName RequiredDeviceName;
	};
	
	/** Information for each registered device. */
	struct FActorDeviceData
	{
		/** Actor class for cross-referencing with the class callbacks. */
		TWeakObjectPtr<UClass> ActorClass;

		/** All active devices for this actor. */
		TArray<FActorDeviceState> RegisteredDeviceStates;

		/** All delegates bound to this actor. */
		TArray<FActorDeviceStateDelegate> DeviceStateDelegates;
	};
	
	/** Actors that were registered as tracking devices */
	TMap<FObjectKey, FActorDeviceData> ActorDeviceMap;

#if WITH_EDITORONLY_DATA
	/** Editor-only set to validate that all device requests are only being added for the actors that support it. */
	TSet<FObjectKey> AllDevices;

	void PostGarbageCollect();
#endif

	/** Gets or creates a new device data struct. */
	FActorDeviceData& FindOrAddDeviceData(AActor* Actor);

	/** Find an appropriate state struct if it exists. */
	const FActorDeviceState* FindDeviceStateStruct(const FActorDeviceData* DeviceData, FName DeviceName) const;

	/** Call all delegates for a specific device state change. */
	void CallDeviceStateDelegates(AActor* Actor, FActorDeviceState StateChange);

	/** Call all delegates for a specific device state change. */
	void CallDeviceStateDelegatesForMatchingDevices(AActor* Actor, FActorDeviceStateDelegate& Delegate);
};
