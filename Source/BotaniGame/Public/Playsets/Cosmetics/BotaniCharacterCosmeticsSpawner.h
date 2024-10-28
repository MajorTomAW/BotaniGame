// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "BotaniCharacterCosmeticsManager.generated.h"

class UBotaniCharacterDefinition;

/** A handle created by adding a character cosmetic entry, can be used to remove it later */
USTRUCT(BlueprintType)
struct FBotaniCharacterCosmeticHandle
{
	GENERATED_BODY()

	void Reset()
	{
		CosmeticHandle = INDEX_NONE;
	}

	bool IsValid() const
	{
		return CosmeticHandle != INDEX_NONE;
	}

private:
	friend struct FBotaniCharacterCosmeticList;

	/** The handle index */
	UPROPERTY()
	int32 CosmeticHandle = INDEX_NONE;
};

/** A single applied character cosmetic part */
USTRUCT()
struct FBotaniAppliedCharacterCosmeticEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FBotaniAppliedCharacterCosmeticEntry()
	{
	}

private:
	friend struct FBotaniCharacterCosmeticList;

	/** The character definition of the part */
	UPROPERTY()
	TObjectPtr<UBotaniCharacterDefinition> Cosmetic;

	/** Handle index returned to the user (server only) */
	UPROPERTY(NotReplicated)
	int32 CosmeticHandle = INDEX_NONE;

	/** The spawned instances of the cosmetic part (client only) */
	UPROPERTY(NotReplicated)
	TArray<TObjectPtr<UChildActorComponent>> SpawnedComponents;
};

/** Replicated list of applied character cosmetic parts */
USTRUCT(BlueprintType)
struct FBotaniCharacterCosmeticList : public FFastArraySerializer
{
	GENERATED_BODY()

	FBotaniCharacterCosmeticList()
		: CosmeticsSpawner(nullptr)
	{
	}

	FBotaniCharacterCosmeticList(const FBotaniCharacterCosmeticList& Other)
		: Entries(Other.Entries)
		, CosmeticsSpawner(nullptr)
	{
	}

	FBotaniCharacterCosmeticList(UBotaniCharacterCosmeticsSpawner* InCosmeticsSpawner)
		: CosmeticsSpawner(InCosmeticsSpawner)
	{
	}

public:
	//~ Begin FFastArraySerializer interface
	virtual void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	virtual void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	virtual void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FBotaniAppliedCharacterCosmeticEntry, FBotaniCharacterCosmeticList>(Entries, DeltaParms, *this);
	}
	//~ End FFastArraySerializer interface

	/** Called to add a new cosmetic part to the list and equip it on the owning pawn. */
	FBotaniCharacterCosmeticHandle AddEntry(UBotaniCharacterDefinition* NewCosmetic);

	/** Called to remove a previously added cosmetic part from the list and unequip it from the owning pawn. */
	void RemoveEntry(FBotaniCharacterCosmeticHandle Handle);

	/** Called to remove all added cosmetic parts from the list and unequip them from the owning pawn. */
	void ClearAllEntries(bool bBroadcastChangeDelegate = true);

	/** Called to initialize the list with a new spawner */
	void SetCosmeticsSpawner(UBotaniCharacterCosmeticsSpawner* InCosmeticsSpawner)
	{
		CosmeticsSpawner = InCosmeticsSpawner;
	}

protected:
	friend UBotaniCharacterCosmeticsSpawner;

	bool SpawnCosmeticForEntry(FBotaniAppliedCharacterCosmeticEntry& Entry);
	bool DestroyCosmeticForEntry(FBotaniAppliedCharacterCosmeticEntry& Entry);
	

private:
	/** Replicated list of applied character cosmetic parts */
	UPROPERTY()
	TArray<FBotaniAppliedCharacterCosmeticEntry> Entries;

	/** The component that is used to spawn the cosmetic parts */
	UPROPERTY(NotReplicated)
	TObjectPtr<UBotaniCharacterCosmeticsSpawner> CosmeticsSpawner;

	/** Up counter for handles */
	int32 CosmeticHandleCounter = 0;
};

template<>
struct TStructOpsTypeTraits<FBotaniCharacterCosmeticList> : public TStructOpsTypeTraitsBase2<FBotaniCharacterCosmeticList>
{
	enum { WithNetDeltaSerializer = true };
};

/**
 * UBotaniCharacterCosmeticsManager
 *
 * A pawn component that handles spawning cosmetic elements to the owner pawn and all clients.
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class UBotaniCharacterCosmeticsManager : public UPawnComponent
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRegister() override;
	//~ End UActorComponent interface
protected:
private:
	/** Replicated list of applied character cosmetic parts */
	UPROPERTY(Replicated, Transient)
	FBotaniCharacterCosmeticList CharacterCosmeticList;
};
