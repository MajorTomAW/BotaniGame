// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Messaging/BotaniVerbMessageReplication.h"

#include "GameFramework/GameplayMessageSubsystem.h"

//////////////////////////////////////////////////////////////////////////
/// FBotaniVerbMessageReplicationEntry
//////////////////////////////////////////////////////////////////////////

FString FBotaniVerMessageReplicationEntry::GetDebugString() const
{
	return Message.ToString();
}

//////////////////////////////////////////////////////////////////////////
/// FBotaniVerbMessageReplication
//////////////////////////////////////////////////////////////////////////

void FBotaniVerbMessageReplication::AddMessage(const FBotaniVerbMessage& Message)
{
	FBotaniVerMessageReplicationEntry& NewStack = CurrentMessages.Emplace_GetRef(Message);
	MarkItemDirty(NewStack);
}

void FBotaniVerbMessageReplication::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
}

void FBotaniVerbMessageReplication::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		const FBotaniVerMessageReplicationEntry& Entry = CurrentMessages[Index];
		BroadcastMessage(Entry.Message);
	}
}

void FBotaniVerbMessageReplication::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		const FBotaniVerMessageReplicationEntry& Entry = CurrentMessages[Index];
		BroadcastMessage(Entry.Message);
	}
}

void FBotaniVerbMessageReplication::BroadcastMessage(const FBotaniVerbMessage& Message) const
{
	check(Owner);
	UGameplayMessageSubsystem& MessageSub = UGameplayMessageSubsystem::Get(Owner);
	MessageSub.BroadcastMessage(Message.VerbTag, Message);
}
