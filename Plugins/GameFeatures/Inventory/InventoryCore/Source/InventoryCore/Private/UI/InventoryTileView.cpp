// Copyright © 2024 Botanibots Team. All rights reserved.


#include "UI/InventoryTileView.h"

#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Inventory/Components/BotaniInventoryManager.h"
#include "Inventory/Instances/BotaniItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryTileView)

UInventoryTileView::UInventoryTileView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UGameplayInventoryItemDefinition* UInventoryTileEntryObject::GetItemDefinition(TSubclassOf<UGameplayInventoryItemDefinition> DefinitionClass) const
{
	if (ItemInstance == nullptr)
	{
		return nullptr;
	}
	
	if (ItemInstance->GetItemDefinition()->IsA(DefinitionClass))
	{
		return ItemInstance->GetItemDefinition();
	}

	return nullptr;
}

void UInventoryTileView::InitializeWithInventoryManager(UGameplayInventoryManager* InInventoryManager)
{
	check(InInventoryManager);

	if (InventoryManager == InInventoryManager)
	{
		return;
	}

	InventoryManager = InInventoryManager;
	AddInitialEntries();
}

void UInventoryTileView::AddInitialEntries()
{
	check(InventoryManager);

	constexpr int32 NumEntries = 5;
	
	for (int i = 0; i < NumEntries; i++)
	{
		UInventoryTileEntryObject* Entry = NewObject<UInventoryTileEntryObject>(this);
		AddItem(Entry);
	}
}
