// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Devices/PlayerStart/BioPlayerSpawningManager.h"

#include "BotaniLogChannels.h"
#include "EngineUtils.h"
#include "Devices/PlayerStart/Device_BioPlayerStart.h"
#include "Game/BotaniGameModeBase.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BioPlayerSpawningManager)

UBioPlayerSpawningManager::UBioPlayerSpawningManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(false);
	bAutoActivate = true;
	bAutoRegister = true;
	bWantsInitializeComponent = true;

	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBioPlayerSpawningManager::InitializeComponent()
{
	Super::InitializeComponent();

	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &ThisClass::OnLevelAdded);


	if (ABotaniGameModeBase* BotaniGameMode = GetWorld()->GetAuthGameMode<ABotaniGameModeBase>())
	{
		BotaniGameMode->DeterminePlayerStartSpot.BindUObject(this, &ThisClass::DeterminePlayerStartSpot);
	}

	UWorld* World = GetWorld();
	World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::HandleOnActorSpawned));

	for (TActorIterator<ADevice_BioPlayerStart> It(World); It; ++It)
	{
		ADevice_BioPlayerStart* PlayerStart = *It;
		if (PlayerStart == nullptr)
		{
			continue;
		}

		PlayerStartDevices.Add(PlayerStart);
	}
}

void UBioPlayerSpawningManager::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBioPlayerSpawningManager::OnLevelAdded(ULevel* InLevel, UWorld* InWorld)
{
	if (InWorld != GetWorld())
	{
		return;
	}

	for (AActor* Actor : InLevel->Actors)
	{
		if (ADevice_BioPlayerStart* PlayerStart = Cast<ADevice_BioPlayerStart>(Actor))
		{
			PlayerStartDevices.Add(PlayerStart);
		}
	}
}

void UBioPlayerSpawningManager::HandleOnActorSpawned(AActor* Actor)
{
	if (ADevice_BioPlayerStart* PlayerStart = Cast<ADevice_BioPlayerStart>(Actor))
	{
		PlayerStartDevices.Add(PlayerStart);
	}
}

AActor* UBioPlayerSpawningManager::DeterminePlayerStartSpot(AController* Player)
{
	if (Player == nullptr)
	{
		return nullptr;
	}

	TArray<ADevice_BioPlayerStart*> ValidPlayerStarts;
	for (auto StartIt = PlayerStartDevices.CreateIterator(); StartIt; ++StartIt)
	{
		ADevice_BioPlayerStart* StartDevice = StartIt->Get();
		if (StartDevice == nullptr)
		{
			StartIt.RemoveCurrent();
			continue;
		}

		if (!StartDevice->IsDeviceEnabled())
		{
			StartIt.RemoveCurrent();
			continue;
		}

		ValidPlayerStarts.Add(StartDevice);
	}

	BOTANI_GFP_LOG(Warning, TEXT("ValidPlayerStarts.Num() = %d"), ValidPlayerStarts.Num());

	auto SortByPriority = [](const ADevice_BioPlayerStart& A, const ADevice_BioPlayerStart& B)
	{
		return A.GetPriority() > B.GetPriority();
	};

	ValidPlayerStarts.Sort(SortByPriority);
	return ValidPlayerStarts.Last();
}
