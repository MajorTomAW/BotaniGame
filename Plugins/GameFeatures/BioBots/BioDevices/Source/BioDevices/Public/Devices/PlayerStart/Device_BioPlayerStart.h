// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BioDeviceInterface.h"
#include "Player/BotaniPlayerStart.h"

#include "Device_BioPlayerStart.generated.h"

/**
 * ADevice_BioPlayerStart
 *
 * A Bio Device used to spawn players or bots.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BIODEVICES_API ADevice_BioPlayerStart : public ABotaniPlayerStart, public IBioDeviceInterface
{
	GENERATED_UCLASS_BODY()

public:
	const static FName NAME_PlayerStartDevice;

	//~ Begin IBioDeviceInterface Interface
	virtual FName GetDeviceName() const override { return NAME_PlayerStartDevice; }
	virtual UBioDeviceManagerComponent* GetDeviceManager() const override;
	//~ End IBioDeviceInterface Interface

	//~ Begin AActor Interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	/** Whether to show the indicator in game */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style")
	uint8 bShowIndicatorInGame : 1;
	
	/** Color to use when the player start is enabled */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style")
	FLinearColor EnabledColor;

	/** Color to use when the player start is disabled */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style")
	FLinearColor DisabledColor;

protected:
	/** The device manager that owns this device */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device")
	TObjectPtr<class UBioDeviceManagerComponent> DeviceManager;

	/** The Gameplay Message Receiver component that manages spawning players or bots */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Device")
	TObjectPtr<class UBotaniGameplayReceiverMessageComponent> GameplayMessageReceiver;
};
