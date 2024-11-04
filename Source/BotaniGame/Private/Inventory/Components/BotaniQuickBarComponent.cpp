// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Components/BotaniQuickBarComponent.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayTags/BotaniGameplayTags.h"
#include "Inventory/BotaniInventoryStatics.h"
#include "Inventory/Components/BotaniEquipmentManager.h"
#include "Inventory/Components/BotaniInventoryManager.h"
#include "Inventory/Definitions/BotaniEquipmentDefinition.h"
#include "Inventory/Instances/BotaniItemInstance.h"
#include "Library/InventorySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniQuickBarComponent)

UBotaniQuickBarComponent::UBotaniQuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UBotaniQuickBarComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_ReplayOrOwner;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Slots, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ActiveSlotIndex, Params);
}

void UBotaniQuickBarComponent::BeginPlay()
{
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Slots, this);
	
	Super::BeginPlay();
}

void UBotaniQuickBarComponent::CycleActiveSlotForward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIdx = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIdx = ActiveSlotIndex;

	do
	{
		NewIdx = (NewIdx + 1) % Slots.Num();

		if (Slots[NewIdx] != nullptr)
		{
			SetActiveSlotIndex(NewIdx);
			return;
		}
	}
	while (NewIdx != OldIdx);
}

void UBotaniQuickBarComponent::CycleActiveSlotBackward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIdx = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIdx = ActiveSlotIndex;

	do
	{
		NewIdx = (NewIdx - 1 + Slots.Num()) % Slots.Num();

		if (Slots[NewIdx] != nullptr)
		{
			SetActiveSlotIndex(NewIdx);
			return;
		}
	}
	while (NewIdx != OldIdx);
}

void UBotaniQuickBarComponent::SetActiveSlotIndex_Implementation(int32 NewIndex)
{
	if (ActiveSlotIndex != NewIndex)
	{
		UnequipItemInSlot();

		ActiveSlotIndex = NewIndex;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ActiveSlotIndex, this);

		if (Slots.IsValidIndex(NewIndex))
		{
			EquipItemInSlot();	
		}

		OnRep_ActiveSlotIndex();
	}
}

UBotaniEquipmentManager* UBotaniQuickBarComponent::FindEquipmentManager() const
{
	const AController* OwnerController = Cast<AController>(GetOwner());
	if (OwnerController == nullptr)
	{
		return nullptr;
	}

	const APawn* Pawn = OwnerController->GetPawn();
	if (Pawn == nullptr)
	{
		return nullptr;
	}

	return Cast<UBotaniEquipmentManager>(UInventorySystemBlueprintLibrary::FindEquipmentManager(Pawn));
}

UBotaniInventoryManager* UBotaniQuickBarComponent::FindInventoryManager() const
{
	const AController* OwnerController = Cast<AController>(GetOwner());
	if (OwnerController == nullptr)
	{
		return nullptr;
	}

	return Cast<UBotaniInventoryManager>(UInventorySystemBlueprintLibrary::FindInventoryManager(OwnerController));
}

AController* UBotaniQuickBarComponent::GetOwnerController() const
{
	if (GetOwner())
	{
		return Cast<AController>(GetOwner());
	}

	return nullptr;
}

void UBotaniQuickBarComponent::BroadcastQuickBarMessage(const FGameplayTag& MessageTag)
{
	FBotaniQuickBarChangeMessage Message;
	Message.Owner = GetOwnerController();
	Message.QuickBar = this;
	Message.Slots = Slots;
	Message.ActiveSlotIndex = ActiveSlotIndex;
	Message.EquippedItemHandle = EquippedItemHandle;

	UGameplayMessageSubsystem& MessageSub = UGameplayMessageSubsystem::Get(this);
	MessageSub.BroadcastMessage(MessageTag, Message);
}

int32 UBotaniQuickBarComponent::GetNextFreeItemSlot() const
{
	int32 SlotIdx = 0;

	for (TObjectPtr<UBotaniItemInstance> SlotItem : Slots)
	{
		if (SlotItem == nullptr)
		{
			return SlotIdx;
		}

		SlotIdx++;
	}

	return INDEX_NONE;
}

void UBotaniQuickBarComponent::AddItemToSlot(const int32 SlotIndex, const FGameplayInventoryItemSpecHandle& ItemHandle)
{
	if (!Slots.IsValidIndex(SlotIndex) || (!ItemHandle.IsValid()))
	{
		return;
	}

	// Check if an item is already in the slot
	if (Slots[SlotIndex] != nullptr)
	{
		return;
	}

	UBotaniInventoryManager* InventoryManager = FindInventoryManager();
	if (InventoryManager == nullptr)
	{
		return;
	}

	UObject* MutableInstance = InventoryManager->FindItemSpecFromHandle(ItemHandle)->GetInstance();
	if (MutableInstance == nullptr)
	{
		return;
	}

	UBotaniItemInstance* ItemInstance = CastChecked<UBotaniItemInstance>(MutableInstance);

	Slots[SlotIndex] = ItemInstance;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Slots, this);
	
	OnRep_Slots();
}

UBotaniItemInstance* UBotaniQuickBarComponent::RemoveItemFromSlot(const int32 SlotIndex)
{
	UBotaniItemInstance* Result = nullptr;

	// Unequip the item if it's equipped
	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		
		ActiveSlotIndex = -1;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ActiveSlotIndex, this);
	}

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];

		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Slots, this);
			
			OnRep_Slots();
		}
	}
	
	return Result;
}

UBotaniItemInstance* UBotaniQuickBarComponent::RemoveItemByHandle(const FGameplayInventoryItemSpecHandle& ItemHandle)
{
	UBotaniItemInstance* Result = nullptr;

	// Unequip the item if it is equipped
	if (EquippedItemHandle == ItemHandle)
	{
		UnequipItemInSlot();

		ActiveSlotIndex = -1;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ActiveSlotIndex, this);
	}

	// Try to find the item in the slots
	for (int32 SlotIndex = 0; SlotIndex < Slots.Num(); SlotIndex++)
	{
		if (Slots[SlotIndex] != nullptr && Slots[SlotIndex]->GetItemSpecHandle() == ItemHandle)
		{
			Result = Slots[SlotIndex];
			Slots[SlotIndex] = nullptr;
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Slots, this);
			
			OnRep_Slots();
			break;
		}
	}

	return Result;
}

void UBotaniQuickBarComponent::OnRep_Slots()
{
	BroadcastQuickBarMessage(BotaniGameplayTags::Inventory::Message::TAG_InventoryMessage_QuickBar_SlotsChanged);
}

void UBotaniQuickBarComponent::OnRep_ActiveSlotIndex()
{
	BroadcastQuickBarMessage(BotaniGameplayTags::Inventory::Message::TAG_InventoryMessage_QuickBar_ActiveIndexChanged);
}

void UBotaniQuickBarComponent::UnequipItemInSlot()
{
	if (UBotaniEquipmentManager* EquipmentManager = FindEquipmentManager())
	{
		if (!EquippedItemHandle.IsValid())
		{
			return;
		}

		EquipmentManager->UnequipItem(EquippedItemHandle);
		EquippedItemHandle.Invalidate();
	}
}

void UBotaniQuickBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItemHandle.IsValid() == false);

	UBotaniItemInstance* SlotItem = Slots[ActiveSlotIndex];
	if (SlotItem == nullptr)
	{
		return;
	}

	const UBotaniEquipmentDefinition* EquipmentDefinition = CastChecked<UBotaniEquipmentDefinition>(SlotItem->GetItemDefinition()->EquipmentDefinition);
	if (EquipmentDefinition == nullptr)
	{
		return;
	}

	if (UBotaniEquipmentManager* EquipmentManager = FindEquipmentManager())
	{
		EquippedItemHandle = SlotItem->GetItemSpecHandle();
		//const FGameplayInventoryItemContext Context = UBotaniInventoryStatics::CreateItemContext(GetOwnerController(), SlotItem, EquippedItemHandle, SlotItem->GetCurrentStackCount());
		EquipmentManager->EquipItemByHandle(EquippedItemHandle, SlotItem->GetItemContext());
	}
}

