// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Widgets/QuickBar/BotaniQuickBarTileView.h"

#include "Inventory/Components/BotaniInventoryManager.h"
#include "Inventory/Components/BotaniQuickBarComponent.h"
#include "Spec/GameplayInventoryItemSpec.h"
#include "Spec/GameplayInventoryItemSpecHandle.h"

UBotaniQuickBarEntryData::UBotaniQuickBarEntryData()
{
}

void UBotaniQuickBarEntryData::Initialize(const FGameplayInventoryItemSpec& InItemSpec)
{
	ItemSpec = InItemSpec;
	Handle = InItemSpec.IsValid() ? InItemSpec.GetHandle() : FGameplayInventoryItemSpecHandle();
}


UBotaniQuickBarTileView::UBotaniQuickBarTileView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TileAlignment = EListItemAlignment::EvenlyDistributed;
	Orientation = Orient_Vertical;
	SetScrollbarVisibility(ESlateVisibility::Collapsed);
	
	EntryHeight = 52.0f;
	EntryWidth = 52.0f;

#if WITH_EDITORONLY_DATA
	EntrySpacing = 12.0f;
#endif
}


void UBotaniQuickBarTileView::InitializeWithInventoryManager(UBotaniInventoryManager* InInventoryManager, UBotaniQuickBarComponent* InQuickBar)
{
	check(InInventoryManager);

	if (InventoryManager == InInventoryManager)
	{
		return;
	}

	InventoryManager = InInventoryManager;
	QuickBar = InQuickBar;

	AddInitialEntries();
}

UBotaniQuickBarTileViewEntry* UBotaniQuickBarTileView::FindEntryWidgetByHandle(const FGameplayInventoryItemSpecHandle& Handle) const
{
	UBotaniQuickBarEntryData* EntryData = FindEntryDataByHandle(Handle);
	if (EntryData == nullptr)
	{
		return nullptr;
	}

	UBotaniQuickBarTileViewEntry* EntryWidget = GetEntryWidgetFromItem<UBotaniQuickBarTileViewEntry>(EntryData);
	return EntryWidget;
}

UBotaniQuickBarEntryData* UBotaniQuickBarTileView::FindEntryDataByHandle(const FGameplayInventoryItemSpecHandle& Handle) const
{
	check(Handle.IsValid());

	for (UObject* Entry : GetListItems())
	{
		auto* EntryData = Cast<UBotaniQuickBarEntryData>(Entry);
		if (EntryData && EntryData->Handle == Handle)
		{
			return EntryData;
		}
	}

	return nullptr;
}

void UBotaniQuickBarTileView::AddInitialEntries()
{
	check(QuickBar);

	for (int32 SlotIdx = 0; SlotIdx < QuickBar->GetNumSlots(); ++SlotIdx)
	{
		UBotaniQuickBarEntryData* EntryData = NewObject<UBotaniQuickBarEntryData>(this);

		if (TileDirection >= 0)
		{
			EntryData->SlotID = SlotIdx;	
		}
		else
		{
			EntryData->SlotID = QuickBar->GetNumSlots() - SlotIdx -1;
		}
		
		AddItem(EntryData);
	}
}
