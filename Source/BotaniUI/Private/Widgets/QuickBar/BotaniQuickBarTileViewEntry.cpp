// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Widgets/QuickBar/BotaniQuickBarTileViewEntry.h"

#include "Inventory/Definitions/BotaniInventoryItemDefinition.h"
#include "Inventory/BotaniInventoryStatics.h"
#include "Inventory/Components/BotaniEquipmentManager.h"
#include "Inventory/Components/BotaniInventoryManager.h"
#include "Inventory/Instances/BotaniEquipmentInstance.h"
#include "Inventory/Instances/BotaniItemInstance.h"
#include "Widgets/QuickBar/BotaniQuickBarTileView.h"

UBotaniQuickBarTileViewEntry::UBotaniQuickBarTileViewEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBotaniQuickBarTileViewEntry::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBotaniQuickBarTileViewEntry::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UBotaniQuickBarComponent* PotQuickBar = UBotaniInventoryStatics::FindQuickBarComponent(GetOwningPlayer());
	if (PotQuickBar == nullptr)
	{
		return;
	}

	if (this->QuickBar == PotQuickBar)
	{
		return;
	}

	UBotaniQuickBarComponent* OldQuickBar = QuickBar;
	QuickBar = PotQuickBar;

	OnQuickBarChanged(QuickBar, OldQuickBar);
}

void UBotaniQuickBarTileViewEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	EntryData = CastChecked<UBotaniQuickBarEntryData>(ListItemObject);
	OnEntryDataSet(EntryData);
}

int32 UBotaniQuickBarTileViewEntry::GetQuickBarSlotID() const
{
	if (EntryData == nullptr)
	{
		return -1;
	}

	return EntryData->SlotID;
}

UBotaniInventoryItemDefinition* UBotaniQuickBarTileViewEntry::GetItemDefinition() const
{
	if (EntryData == nullptr)
	{
		return nullptr;
	}


	UBotaniInventoryManager* InventoryManager = UBotaniInventoryManager::GetInventoryManager(GetOwningPlayer());
	if (InventoryManager == nullptr)
	{
		return nullptr;
	}

	const FGameplayInventoryItemSpec& Spec = *InventoryManager->FindItemSpecFromHandle(EntryData->Handle);
	if (Spec.IsValid())
	{
		return Spec.GetItemDefinition<UBotaniInventoryItemDefinition>();
	}

	return nullptr;
}

class UBotaniItemInstance* UBotaniQuickBarTileViewEntry::GetItemInstance() const
{
	if (EntryData == nullptr)
	{
		return nullptr;
	}

	UBotaniInventoryManager* InventoryManager = UBotaniInventoryManager::GetInventoryManager(GetOwningPlayer());
	if (InventoryManager == nullptr)
	{
		return nullptr;
	}

	const FGameplayInventoryItemSpec& Spec = *InventoryManager->FindItemSpecFromHandle(EntryData->Handle);
	if (Spec.IsValid())
	{
		return Spec.GetInstance<UBotaniItemInstance>();
	}

	return nullptr;
}

class UBotaniEquipmentInstance* UBotaniQuickBarTileViewEntry::GetEquipmentInstance() const
{
	if (EntryData == nullptr)
	{
		return nullptr;
	}

	UGameplayEquipmentManager* EquipmentManager = GetOwningPlayer()->FindComponentByClass<UGameplayEquipmentManager>();
	if (EquipmentManager == nullptr)
	{
		return nullptr;
	}

	const FGameplayEquipmentSpec& Spec = *EquipmentManager->FindEquipmentSpecFromHandle(EntryData->Handle);
	if (Spec.IsValid())
	{
		return Spec.GetInstance<UBotaniEquipmentInstance>();
	}

	return nullptr;
}
