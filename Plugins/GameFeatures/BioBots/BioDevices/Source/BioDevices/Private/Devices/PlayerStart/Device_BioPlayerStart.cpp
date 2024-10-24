// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Devices/PlayerStart/Device_BioPlayerStart.h"
#include "Game/Components/BotaniGameplayReceiverMessageComponent.h"
#include "BioDeviceManagerComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Device_BioPlayerStart)

const FName ADevice_BioPlayerStart::NAME_PlayerStartDevice("PlayerStartDevice");

ADevice_BioPlayerStart::ADevice_BioPlayerStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DeviceManager = ObjectInitializer.CreateDefaultSubobject<UBioDeviceManagerComponent>(this, TEXT("DeviceManager"));
	GameplayMessageReceiver = ObjectInitializer.CreateDefaultSubobject<UBotaniGameplayReceiverMessageComponent>(this, TEXT("GameplayMessageReceiver"));
	
	EnabledColor = FColor::FromHex("8FBFF5FF");
	DisabledColor = FColor::FromHex("F58F8FFF");

#if WITH_EDITOR
	if (GetArrowComponent() && GEngine->IsEditor())
	{
		GetArrowComponent()->ArrowColor = DeviceManager->bDefaultEnabledState ? EnabledColor.ToFColor(true) : DisabledColor.ToFColor(true);
	}
#endif
	
	GetCapsuleComponent()->SetCapsuleRadius(50.f);
}

UBioDeviceManagerComponent* ADevice_BioPlayerStart::GetDeviceManager() const
{
	return DeviceManager;
}

void ADevice_BioPlayerStart::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void ADevice_BioPlayerStart::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ensure(DeviceManager))
	{
		DeviceManager->RegisterDevice(this);
	}
}

void ADevice_BioPlayerStart::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	
	Super::BeginPlay();
}

void ADevice_BioPlayerStart::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void ADevice_BioPlayerStart::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	GetArrowComponent()->SetArrowColor(DeviceManager->bDefaultEnabledState ? EnabledColor : DisabledColor);
}
#endif
