// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Abilities/BotaniInteractionCollectAbility.h"

#include "GameplayTagStackInterface.h"
#include "Components/GameplayInventoryManager.h"
#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Fragments/Item/ItemFragment_PickupDefinition.h"
#include "Interfaces/Pickupable.h"
#include "Inventory/BotaniInventoryStatics.h"
#include "Inventory/Components/BotaniQuickBarComponent.h"
#include "Library/InventorySystemBlueprintLibrary.h"
#include "Instance/GameplayInventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniInteractionCollectAbility)

UBotaniInteractionCollectAbility::UBotaniInteractionCollectAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EBotaniAbilityActivationPolicy::OnInputTriggered;
	ActivationGroup = EBotaniAbilityActivationGroup::Independent;
}

void UBotaniInteractionCollectAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		return;
	}

	
}

void UBotaniInteractionCollectAbility::AddItemToInventory(TScriptInterface<IPickupable> Pickupable)
{
	UGameplayInventoryManager* InventoryManager = UInventorySystemBlueprintLibrary::FindInventoryManager(GetControllerFromActorInfo());
	if (InventoryManager == nullptr)
	{
		return;
	}

	for (const auto& TemplateData : Pickupable->GetItemList())
	{
		const FGameplayInventoryItemContext Context = InventoryManager->MakeItemContext(TemplateData.PickupItem, TemplateData.PickupQuantity, Pickupable.GetObject());
		const FGameplayInventoryItemSpec ItemSpec = FGameplayInventoryItemSpec::FromContext(Context);
		UItemFragment_PickupDefinition* PickupFrag = ItemSpec.GetItemDefinition()->GetItemFragment<UItemFragment_PickupDefinition>();
		
		if (PickupFrag == nullptr)
		{
			continue;
		}

		if (!InventoryManager->CanAddItemDef(ItemSpec, Context))
		{
			continue;
		}

		FGameplayInventoryItemSpecHandle Handle = InventoryManager->GiveItem(ItemSpec, Context);
		if (!Handle.IsValid())
		{
			continue;
		}

		// Equip the item if its set to auto equip
		if (PickupFrag->PickupData.bAutoEquip)
		{
			if (UBotaniQuickBarComponent* QuickBar = UBotaniInventoryStatics::FindQuickBarComponent(GetControllerFromActorInfo()))
			{
				const int32 SlotIdx = QuickBar->GetNextFreeItemSlot();
				if (SlotIdx != INDEX_NONE)
				{
					QuickBar->AddItemToSlot(SlotIdx, Handle);
					QuickBar->SetActiveSlotIndex(SlotIdx);
				}
			}
		}

		// Apply stored tag stacks
		const IGameplayTagStackInterface* TagStackSource = Cast<IGameplayTagStackInterface>(CurrentEventData.Target);
		IGameplayTagStackInterface* TagStackTarget = Cast<IGameplayTagStackInterface>(InventoryManager->FindItemSpecFromHandle(Handle)->GetInstance());
		for (const auto& Stack : TagStackSource->GetOwnedGameplayTagStacks().GetStacks())
		{
			TagStackTarget->SetGameplayTagStack(Stack.GetTag(), Stack.GetStackCount());
		}
	}
}
