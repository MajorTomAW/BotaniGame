// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/BotaniInventoryStatics.h"

#include "Components/GameplayEquipmentManager.h"
#include "Components/GameplayInventoryManager.h"
#include "GameFramework/PlayerState.h"
#include "Instance/GameplayInventoryItemInstance.h"
#include "Inventory/Components/BotaniQuickBarComponent.h"
#include "Library/InventorySystemBlueprintLibrary.h"

FGameplayInventoryItemContext UBotaniInventoryStatics::CreateItemContext(AController* InventoryOwner, UObject* Instigator, const FGameplayInventoryItemSpecHandle& ItemSpecHandle, const int32 Quantity)
{
	FGameplayInventoryItemContext ItemContext;
	ItemContext.FillContext();
	
	ItemContext.InventoryOwner = InventoryOwner;
	ItemContext.Instigator = Instigator;
	ItemContext.InventoryComponent = UInventorySystemBlueprintLibrary::FindInventoryManager(InventoryOwner);
	ItemContext.StackCount = Quantity;

	UGameplayInventoryManager* InventoryManager = Cast<UGameplayInventoryManager>(ItemContext.InventoryComponent);
	
	ItemContext.ItemInstance = InventoryManager ? InventoryManager->FindItemSpecFromHandle(ItemSpecHandle)->GetInstance() : nullptr;
	ItemContext.ItemDefinition = ItemContext.ItemInstance ? ItemContext.ItemInstance->GetItemDefinition() : nullptr;

	return ItemContext;
}

UBotaniQuickBarComponent* UBotaniInventoryStatics::FindQuickBarComponent(const AActor* Actor)
{
	if (Actor == nullptr)
	{
		return nullptr;
	}

	UBotaniQuickBarComponent* QuickBarComponent = Actor->FindComponentByClass<UBotaniQuickBarComponent>();
	

	if (QuickBarComponent == nullptr)
	{
		if (const AController* Controller = Cast<AController>(Actor))
		{
			QuickBarComponent = Controller->FindComponentByClass<UBotaniQuickBarComponent>();
		}	
	}

	if (QuickBarComponent == nullptr)
	{
		if (const APlayerState* PlayerState = Cast<APlayerState>(Actor))
		{
			QuickBarComponent = PlayerState->GetOwningController()->FindComponentByClass<UBotaniQuickBarComponent>();
		}
	}




	return QuickBarComponent;
}
