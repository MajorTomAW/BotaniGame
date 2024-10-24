// Copyright © 2024 MajorT. All rights reserved.


#include "Spec/GameplayInventoryItemSpec.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messaging/GameplayInventoryMessages.h"
#include "Spec/GameplayInventoryItemSpecHandle.h"
#include "NativeGameplayTags.h"
#include "Components/GameplayInventoryManager.h"
#include "Definitions/GameplayInventoryItemDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayInventoryItemSpec)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Inventory_Message_StackChanged, "Inventory.Message.StackChanged");

//////////////////////////////////////////////////////////////////////////
/// FGameplayInventoryItemSpecHandle

void FGameplayInventoryItemSpecHandle::GenerateNewHandle()
{
	// Must be in C++ to avoid duplicate statics across execution units
	static int32 GHandle = 1;
	Handle = GHandle++;
}

//////////////////////////////////////////////////////////////////////////
/// FGameplayInventoryItemSpec

FGameplayInventoryItemSpec::FGameplayInventoryItemSpec()
	: Instance(nullptr)
	, Item(nullptr)
	, SourceObject(nullptr)
{
	Handle.GenerateNewHandle();
}

FGameplayInventoryItemSpec::FGameplayInventoryItemSpec(UGameplayInventoryItemDefinition* InItemDef, int32 InStackCount, UObject* InSourceObject)
	: Instance(nullptr)
	, Item(InItemDef)
	, SourceObject(InSourceObject)
	, StackCount(InStackCount)
{
	Handle.GenerateNewHandle();
}

void FGameplayInventoryItemSpec::PostReplicatedAdd(const FGameplayInventoryItemContainer& InArraySerializer)
{
	if (InArraySerializer.OwnerComponent)
	{
		InArraySerializer.OwnerComponent->OnGiveItem(*this);
	}
}

void FGameplayInventoryItemSpec::PostReplicatedRemove(const FGameplayInventoryItemContainer& InArraySerializer)
{
	if (InArraySerializer.OwnerComponent)
	{
		InArraySerializer.OwnerComponent->OnRemoveItem(*this);
	}
}

void FGameplayInventoryItemSpec::PostReplicatedChange(const FGameplayInventoryItemContainer& InArraySerializer)
{
	if (InArraySerializer.OwnerComponent)
	{
		InArraySerializer.OwnerComponent->OnChangeItem(*this);
	}
}

bool FGameplayInventoryItemSpec::IsStackFull() const
{
	return StackCount >= Item->StackingData.MaxStackSize;
}

int32 FGameplayInventoryItemSpec::GetMaxStackCount() const
{
	return Item->StackingData.MaxStackSize;
}

//////////////////////////////////////////////////////////////////////////
/// FGameplayInventoryItemContainer

FGameplayInventoryItemContainer::FGameplayInventoryItemContainer()
	: OwnerComponent(nullptr)
{
}

FGameplayInventoryItemContainer::FGameplayInventoryItemContainer(UGameplayInventoryManager* InOwnerComponent)
	: OwnerComponent(InOwnerComponent)
{
}

void FGameplayInventoryItemContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		FGameplayInventoryItemSpec& Spec = Items[Index];
		BroadcastInventoryChangeMessage(Spec.Instance, Spec.StackCount, 0);
		Spec.LastObservedStackCount = 0;
	}
}

void FGameplayInventoryItemContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		FGameplayInventoryItemSpec& Spec = Items[Index];
		BroadcastInventoryChangeMessage(Spec.Instance, 0, Spec.StackCount);
		Spec.LastObservedStackCount = Spec.StackCount;
	}
}

void FGameplayInventoryItemContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		FGameplayInventoryItemSpec& Spec = Items[Index];
		check(Spec.LastObservedStackCount != INDEX_NONE);
		BroadcastInventoryChangeMessage(Spec.Instance, Spec.LastObservedStackCount, Spec.StackCount);
		Spec.LastObservedStackCount = Spec.StackCount;
	}
}

TArray<UGameplayInventoryItemInstance*> FGameplayInventoryItemContainer::GetAllInstances() const
{
	TArray<UGameplayInventoryItemInstance*> Instances;
	Instances.Reserve(Items.Num());
	for (const FGameplayInventoryItemSpec& Spec : Items)
	{
		if (Spec.Instance)
		{
			Instances.Add(Spec.Instance);
		}
	}
	return Instances;
}

void FGameplayInventoryItemContainer::BroadcastInventoryChangeMessage(UGameplayInventoryItemInstance* InItemInstance, int32 InOldCount, int32 InNewCount) const
{
	// Construct the change message
	const FGameplayInventoryChangeMessage Message = FGameplayInventoryChangeMessage(InItemInstance, InOldCount, InNewCount);

	UGameplayMessageSubsystem& MessageSub = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSub.BroadcastMessage(TAG_Inventory_Message_StackChanged, Message);
}
