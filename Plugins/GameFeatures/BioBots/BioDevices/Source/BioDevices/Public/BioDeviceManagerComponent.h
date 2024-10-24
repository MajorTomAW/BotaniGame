// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BioDeviceInterface.h"
#include "Components/GameFrameworkComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "BioDeviceManagerComponent.generated.h"


class IBioDeviceInterface;

/** An internal delegate that should mostly be used by blueprints */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInternalBioDeviceEnabledStateChangedDelegate, bool, bEnabled, bool, bForce);


/**
 * UBioDeviceManagerComponent
 *
 * A base class for a device manager component.
 */
UCLASS(Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume, Activation, Navigation, Tags, AssetUserData), ClassGroup = (BioDevices))
class BIODEVICES_API UBioDeviceManagerComponent : public UGameFrameworkComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_UCLASS_BODY()

public:
	/** Static getter for the device manager component. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Bio|Devices", meta = (DisplayName = "Get Device Manager"))
	static UBioDeviceManagerComponent* FindDeviceManagerComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UBioDeviceManagerComponent>() : nullptr); }

public:
	/** The initial state on how this device is enabled */
	UPROPERTY(EditAnywhere, Category = "Device")
	EBioDeviceEnabledState InitialEnabledState;

	/** The default enabled state of this device. Can only be overriden if the InitialEnabledState is not set to Inherited */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Device", meta = (EditCondition = "InitialEnabledState != EBioDeviceEnabledState::Inherited"))
	uint8 bDefaultEnabledState : 1;

	/** The default active state of this device */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Device", meta = (EditCondition = "InitialEnabledState != EBioDeviceEnabledState::Inherited && bDefaultEnabledState"))
	uint8 bDefaultActiveState : 1;

public:
	virtual void RegisterDevice(UObject* InDevice);

	/**
	 * Attempts to change the device-enabled state.
	 * @note The "initial device-enabled state" is used to determine if the change is allowed. However, the change can still be forced.
	 * 
	 * @param bNewEnabled	Whether the device should be enabled or disabled
	 * @param bForce	Whether to force the change (ignores the initial device-enabled state)
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Bio|Devices")
	bool AttemptToChangeDeviceEnabledState(const bool bNewEnabled, const bool bForce = false);

	/**
	 * Tries to activate the device.
	 * Will fail if the device is not enabled or already active.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Bio|Devices")
	bool AttemptToChangeDeviceActiveState(const bool bNewActive);

protected:
	//~ Begin UActorComponent Interface
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	//~ End UActorComponent Interface

	//~ Begin IGameFrameworkInitStateInterface Interface
	virtual FName GetFeatureName() const override;
	//~ End IGameFrameworkInitStateInterface Interface

#if WITH_EDITOR
	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface
#endif

protected:
	IBioDeviceInterface* GetDeviceInterfaceFromDevice() const;

private:
	/** The device that owns this manager */
	UPROPERTY()
	UObject* Device;
};
