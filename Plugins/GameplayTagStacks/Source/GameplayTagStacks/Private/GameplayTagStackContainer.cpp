// Copyright © 2024 MajorT. All rights reserved.

#include "GameplayTagStackContainer.h"

#include "NativeGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "UObject/Stack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTagStackContainer)

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_StackChangeMessage, "Gameplay.Message.TagStackChanged", "A generic message that is sent when a tag stack changes");

//////////////////////////////////////////////////////////////////////
// FGameplayTagStack

FString FGameplayTagStack::	GetDebugString() const
{
	return FString::Printf(TEXT("%sx%d"), *Tag.ToString(), StackCount);
}

//////////////////////////////////////////////////////////////////////
// FGameplayTagStackContainer

void FGameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to AddStack"), ELogVerbosity::Warning);
		return;
	}

	if (StackCount > 0)
	{
		for (FGameplayTagStack& Stack : Stacks)
		{
			if (Stack.Tag == Tag)
			{
				const int32 NewCount = Stack.StackCount + StackCount;
				Stack.StackCount = NewCount;
				TagToCountMap[Tag] = NewCount;
				MarkItemDirty(Stack);
				BroadcastChangeMessage(Stack.Tag, NewCount, NewCount - StackCount);
				
				return;
			}
		}

		FGameplayTagStack& NewStack = Stacks.Emplace_GetRef(Tag, StackCount);
		MarkItemDirty(NewStack);
		TagToCountMap.Add(Tag, StackCount);
		BroadcastChangeMessage(Tag, StackCount, 0);
	}
}

void FGameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to RemoveStack"), ELogVerbosity::Warning);
		return;
	}

	//@TODO: Should we error if you try to remove a stack that doesn't exist or has a smaller count?
	if (StackCount > 0)
	{
		for (auto It = Stacks.CreateIterator(); It; ++It)
		{
			FGameplayTagStack& Stack = *It;
			if (Stack.Tag == Tag)
			{
				if (Stack.StackCount <= StackCount)
				{
					It.RemoveCurrent();
					TagToCountMap.Remove(Tag);
					MarkArrayDirty();
					BroadcastChangeMessage(Tag, 0, Stack.StackCount);
				}
				else
				{
					const int32 NewCount = Stack.StackCount - StackCount;
					Stack.StackCount = NewCount;
					TagToCountMap[Tag] = NewCount;
					MarkItemDirty(Stack);
					BroadcastChangeMessage(Tag, NewCount, Stack.StackCount);
				}
				return;
			}
		}
	}
}

void FGameplayTagStackContainer::SetStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to SetStack"), ELogVerbosity::Warning);
		return;
	}

	if (StackCount > 0)
	{
		for (FGameplayTagStack& Stack : Stacks)
		{
			if (Stack.Tag == Tag)
			{
				const int32 OldCount = Stack.StackCount;
				Stack.StackCount = StackCount;
				TagToCountMap[Tag] = StackCount;
				MarkItemDirty(Stack);
				BroadcastChangeMessage(Tag, StackCount, OldCount);
				return;
			}
		}

		FGameplayTagStack& NewStack = Stacks.Emplace_GetRef(Tag, StackCount);
		MarkItemDirty(NewStack);
		TagToCountMap.Add(Tag, StackCount);
		BroadcastChangeMessage(Tag, StackCount, 0);
	}
	else
	{
		for (auto It = Stacks.CreateIterator(); It; ++It)
		{
			FGameplayTagStack& Stack = *It;
			if (Stack.Tag == Tag)
			{
				It.RemoveCurrent();
				TagToCountMap.Remove(Tag);
				MarkArrayDirty();
				BroadcastChangeMessage(Tag, 0, Stack.StackCount);
				return;
			}
		}
	}
}

void FGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FGameplayTag Tag = Stacks[Index].Tag;
		BroadcastChangeMessage(Tag, FinalSize, TagToCountMap[Tag]);
		
		TagToCountMap.Remove(Tag);
	}
}

void FGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FGameplayTagStack& Stack = Stacks[Index];
		BroadcastChangeMessage(Stack.Tag, Stack.StackCount, 0);
		
		TagToCountMap.Add(Stack.Tag, Stack.StackCount);
	}
}

void FGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FGameplayTagStack& Stack = Stacks[Index];
		BroadcastChangeMessage(Stack.Tag, Stack.StackCount, TagToCountMap[Stack.Tag]);
		
		TagToCountMap[Stack.Tag] = Stack.StackCount;
	}
}

void FGameplayTagStackContainer::BroadcastChangeMessage(const FGameplayTag& Tag, int32 NewCount, int32 OldCount) const
{
	FGameplayTagStackChangeMessage Message;
	Message.Owner = Owner;
	Message.Tag = Tag;
	Message.NewCount = NewCount;
	Message.OldCount = OldCount;
	Message.Delta = NewCount - OldCount;

	if (Owner)
	{
		UGameplayMessageSubsystem& MessageSub = UGameplayMessageSubsystem::Get(Owner);
		MessageSub.BroadcastMessage(TAG_StackChangeMessage, Message);	
	}
}

