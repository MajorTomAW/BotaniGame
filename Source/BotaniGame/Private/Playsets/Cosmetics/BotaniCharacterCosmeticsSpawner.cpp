// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Playsets/Cosmetics/BotaniCharacterCosmeticsSpawner.h"

#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Playsets/BotaniCharacterDefinition.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniCharacterCosmeticsSpawner)

void FBotaniCharacterCosmeticList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	bool bCreatedAnyActors = false;
	for (const int32 Idx : AddedIndices)
	{
		FBotaniAppliedCharacterCosmeticEntry& Entry = Entries[Idx];
		bCreatedAnyActors |= SpawnCosmeticForEntry(Entry);
	}

	if (bCreatedAnyActors && ensure(CosmeticsSpawner))
	{
		// CosmeticsSpawner->BroadcastChanged();
	}
}

void FBotaniCharacterCosmeticList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	bool bChangedAnyActors = false;

	// We need to destroy and recreate the actors for the changed entries
	for (const int32 Idx : ChangedIndices)
	{
		FBotaniAppliedCharacterCosmeticEntry& Entry = Entries[Idx];
		bChangedAnyActors |= DestroyCosmeticForEntry(Entry);
		bChangedAnyActors |= SpawnCosmeticForEntry(Entry);
	}

	if (bChangedAnyActors && ensure(CosmeticsSpawner))
	{
		// CosmeticsSpawner->BroadcastChanged();
	}
}

void FBotaniCharacterCosmeticList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	bool bDestroyedAnyActors = false;

	for (const int32 Idx : RemovedIndices)
	{
		FBotaniAppliedCharacterCosmeticEntry& Entry = Entries[Idx];
		bDestroyedAnyActors |= DestroyCosmeticForEntry(Entry);
	}

	if (bDestroyedAnyActors && ensure(CosmeticsSpawner))
	{
		// CosmeticsSpawner->BroadcastChanged();
	}
}

FBotaniCharacterCosmeticHandle FBotaniCharacterCosmeticList::AddEntry(const UBotaniCharacterDefinition* NewCosmetic)
{
	FBotaniCharacterCosmeticHandle NewHandle;
	NewHandle.Handle = CosmeticHandleCounter++;

	if (ensure(CosmeticsSpawner &&
		CosmeticsSpawner->GetOwner() &&
		CosmeticsSpawner->GetOwner()->HasAuthority()))
	{
		FBotaniAppliedCharacterCosmeticEntry& NewCosmeticEntry = Entries.AddDefaulted_GetRef();
		NewCosmeticEntry.Cosmetic = (UBotaniCharacterDefinition*)NewCosmetic;
		NewCosmeticEntry.CosmeticHandle = NewHandle.Handle;

		if (SpawnCosmeticForEntry(NewCosmeticEntry))
		{
			// CosmeticsSpawner->BroadcastChanged();
		}

		MarkItemDirty(NewCosmeticEntry);
	}

	return NewHandle;
}

void FBotaniCharacterCosmeticList::RemoveEntry(FBotaniCharacterCosmeticHandle Handle)
{
}

void FBotaniCharacterCosmeticList::ClearAllEntries(bool bBroadcastChangeDelegate)
{
}

bool FBotaniCharacterCosmeticList::SpawnCosmeticForEntry(FBotaniAppliedCharacterCosmeticEntry& Entry)
{
	bool bCreatedAnyActors = false;

	if (!ensure(CosmeticsSpawner) || CosmeticsSpawner->IsNetMode(NM_DedicatedServer))
	{
		return bCreatedAnyActors;
	}

	if (Entry.Cosmetic == nullptr)
	{
		return bCreatedAnyActors;
	}

	UWorld* World = CosmeticsSpawner->GetWorld();
	USceneComponent* BaseAttachTarget = CosmeticsSpawner->GetPawn<ACharacter>()->GetMesh();
	
	if (Entry.Cosmetic->BaseMesh && BaseAttachTarget)
	{
		USkeletalMeshComponent* NewCosmetic = NewObject<USkeletalMeshComponent>(CosmeticsSpawner->GetOwner());
		NewCosmetic->SetSkeletalMesh(Entry.Cosmetic->BaseMesh.LoadSynchronous());
		NewCosmetic->SetupAttachment(BaseAttachTarget);
		NewCosmetic->RegisterComponent();
		NewCosmetic->SetAnimInstanceClass(CosmeticsSpawner->DefaultAnimationInstance.LoadSynchronous());
		NewCosmetic->SetOwnerNoSee(true);
		NewCosmetic->SetCastHiddenShadow(true);

		for (const auto& KVP : Entry.Cosmetic->ColorParameters)
		{
			NewCosmetic->SetVectorParameterValueOnMaterials(KVP.Key, FVector(KVP.Value));
		}
		
		Entry.SpawnedComponents.Add(NewCosmetic);
		bCreatedAnyActors = true;
	}

	return bCreatedAnyActors;
}

bool FBotaniCharacterCosmeticList::DestroyCosmeticForEntry(FBotaniAppliedCharacterCosmeticEntry& Entry)
{
	bool bDestroyedAnyActors = false;
	return bDestroyedAnyActors;
}


UBotaniCharacterCosmeticsSpawner::UBotaniCharacterCosmeticsSpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UBotaniCharacterCosmeticsSpawner::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CharacterCosmeticList);
}

void UBotaniCharacterCosmeticsSpawner::BeginPlay()
{
	Super::BeginPlay();

	for (UBotaniCharacterDefinition* Def : DefaultCharacterDefinitions)
	{
		if (Def == nullptr)
		{
			continue;
		}

		AddCharacterDefinition(Def);
	}
}

void UBotaniCharacterCosmeticsSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CharacterCosmeticList.ClearAllEntries(false);
	
	Super::EndPlay(EndPlayReason);
}

void UBotaniCharacterCosmeticsSpawner::OnRegister()
{
	Super::OnRegister();

	if (!IsTemplate())
	{
		CharacterCosmeticList.SetCosmeticsSpawner(this);
	}
}

FBotaniCharacterCosmeticHandle UBotaniCharacterCosmeticsSpawner::AddCharacterDefinition(
	const UBotaniCharacterDefinition* NewCosmetic)
{
	if (!GetOwner()->HasAuthority())
	{
		return FBotaniCharacterCosmeticHandle();
	}
	
	return CharacterCosmeticList.AddEntry(NewCosmetic);
}

void UBotaniCharacterCosmeticsSpawner::RemoveCharacterDefinition(FBotaniCharacterCosmeticHandle Handle)
{
	CharacterCosmeticList.RemoveEntry(Handle);
}

void UBotaniCharacterCosmeticsSpawner::RemoveAllCharacterDefinitions()
{
	CharacterCosmeticList.ClearAllEntries(true);
}


