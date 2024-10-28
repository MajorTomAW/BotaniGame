// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Playsets/Cosmetics/BotaniCharacterCosmeticsManager.h"

#include "Net/UnrealNetwork.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniCharacterCosmeticsManager)

void FBotaniCharacterCosmeticList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
}

void FBotaniCharacterCosmeticList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}

void FBotaniCharacterCosmeticList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
}

FBotaniCharacterCosmeticHandle FBotaniCharacterCosmeticList::AddEntry(UBotaniCharacterDefinition* NewCosmetic)
{
}

void FBotaniCharacterCosmeticList::RemoveEntry(FBotaniCharacterCosmeticHandle Handle)
{
}

void FBotaniCharacterCosmeticList::ClearAllEntries(bool bBroadcastChangeDelegate)
{
}

bool FBotaniCharacterCosmeticList::SpawnCosmeticForEntry(FBotaniAppliedCharacterCosmeticEntry& Entry)
{
}

bool FBotaniCharacterCosmeticList::DestroyCosmeticForEntry(FBotaniAppliedCharacterCosmeticEntry& Entry)
{
}


UBotaniCharacterCosmeticsManager::UBotaniCharacterCosmeticsManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UBotaniCharacterCosmeticsManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CharacterCosmeticList);
}

void UBotaniCharacterCosmeticsManager::BeginPlay()
{
	Super::BeginPlay();
}

void UBotaniCharacterCosmeticsManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CharacterCosmeticList.ClearAllEntries(false);
	
	Super::EndPlay(EndPlayReason);
}

void UBotaniCharacterCosmeticsManager::OnRegister()
{
	Super::OnRegister();

	if (!IsTemplate())
	{
		CharacterCosmeticList.SetCosmeticsSpawner(this);
	}
}


