// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once
#include "BotaniVerbMessage.h"
#include "Net/Serialization/FastArraySerializer.h"


#include "BotaniVerbMessageReplication.generated.h"

struct FBotaniVerbMessageReplication;

/**
 * FBotaniVerbMessageReplicationEntry
 *
 * Represents a single verb message.
 */
USTRUCT(BlueprintType)
struct FBotaniVerMessageReplicationEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FBotaniVerMessageReplicationEntry()
	{
	}

	FBotaniVerMessageReplicationEntry(const FBotaniVerbMessage& InMessage)
		: Message(InMessage)
	{
	}

	FString GetDebugString() const;

private:
	friend FBotaniVerbMessageReplication;
	
	UPROPERTY()
	FBotaniVerbMessage Message;
};

/**
 * FBotaniVerbMessageReplication
 *
 * Container of verb messages to replicate
 */
USTRUCT(BlueprintType)
struct FBotaniVerbMessageReplication : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FBotaniVerbMessageReplication()
	{
	}

public:
	/** Sets the owner of this replication */
	void SetOwner(UObject* InOwner) { Owner = InOwner; }

	/** Broadcasts a message from the server to clients */
	void AddMessage(const FBotaniVerbMessage& Message);

	//~ Begin FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~ End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FBotaniVerMessageReplicationEntry, FBotaniVerbMessageReplication>(CurrentMessages, DeltaParms, *this);
	}

private:
	/** Rebroadcasts a message */
	void BroadcastMessage(const FBotaniVerbMessage& Message) const;

private:
	/** Replicated list of verb messages */
	UPROPERTY()
	TArray<FBotaniVerMessageReplicationEntry> CurrentMessages;

	/** Owner of this replication (for a route to a world) */
	UPROPERTY()
	TObjectPtr<UObject> Owner = nullptr;
};

template<>
struct TStructOpsTypeTraits<FBotaniVerbMessageReplication> : public TStructOpsTypeTraitsBase2<FBotaniVerbMessageReplication>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};