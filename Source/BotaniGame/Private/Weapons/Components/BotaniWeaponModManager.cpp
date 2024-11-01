// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Weapons/Components/BotaniWeaponModManager.h"

#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/Mods/Instances/BotaniWeaponModInstance.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniWeaponModManager)


void FBotaniWeaponModList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
}

void FBotaniWeaponModList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
}

void FBotaniWeaponModList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}




UBotaniWeaponModManager::UBotaniWeaponModManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, WeaponModifications(this)
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UBotaniWeaponModManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, WeaponModifications);
}

UBotaniWeaponModManager* UBotaniWeaponModManager::FindWeaponModManager(const AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return nullptr;
	}

	UBotaniWeaponModManager* WeaponModManager = Actor->FindComponentByClass<UBotaniWeaponModManager>();
	if (WeaponModManager == nullptr)
	{
		if (const AController* Controller = Cast<AController>(Actor))
		{
			WeaponModManager = Controller->GetPawn()->FindComponentByClass<UBotaniWeaponModManager>();
		}
	}

	if (WeaponModManager == nullptr)
	{
		if (const APlayerState* PlayerState = Cast<APlayerState>(Actor))
		{
			WeaponModManager = PlayerState->GetPawn()->FindComponentByClass<UBotaniWeaponModManager>();
		}
	}
	
	return WeaponModManager;
}

void UBotaniWeaponModManager::TickComponent(
	float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
