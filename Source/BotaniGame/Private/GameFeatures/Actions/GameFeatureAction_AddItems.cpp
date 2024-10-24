// Copyright © 2024 Botanibots Team. All rights reserved.


#include "GameFeatures/Actions/GameFeatureAction_AddItems.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include "Components/GameplayInventoryManager.h"
#include "Definitions/GameplayEquipmentDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddItems)

#define LOCTEXT_NAMESPACE "GameFeatures"

void UGameFeatureAction_AddItems::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);

	if (!ensureAlways(ActiveData.ActorExtensions.IsEmpty()) ||
		!ensureAlways(ActiveData.ComponentRequests.IsEmpty()))
	{
		Reset(ActiveData);
	}
	
	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddItems::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	FPerContextData* ActiveData = ContextData.Find(Context);
	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddItems::IsDataValid(FDataValidationContext& Context) const
{
	return Super::IsDataValid(Context);
}
#endif

void UGameFeatureAction_AddItems::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	const UWorld* World = WorldContext.World();
	const UGameInstance* GameInstance = World->GetGameInstance();
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if ((GameInstance != nullptr) &&
		(World != nullptr) &&
		World->IsGameWorld())
	{
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			int32 EntryIndex = 0;

			for (const auto& Entry : ItemsToGrant)
			{
				if (Entry.ActorClass.IsNull())
				{
					continue;
				}

				UGameFrameworkComponentManager::FExtensionHandlerDelegate AddItemsDelegate = UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
					this, &ThisClass::HandleActorExtension, EntryIndex, ChangeContext);

				TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(Entry.ActorClass, AddItemsDelegate);
				EntryIndex++;
			}
		}
	}
}

void UGameFeatureAction_AddItems::Reset(FPerContextData& ActiveData)
{
	while (!ActiveData.ActorExtensions.IsEmpty())
	{
		auto ExtensionIt = ActiveData.ActorExtensions.CreateIterator();
		RemoveActorItems(ExtensionIt->Key, ActiveData);
	}

	ActiveData.ComponentRequests.Empty();
}

void UGameFeatureAction_AddItems::HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex, FGameFeatureStateChangeContext ChangeContext)
{
	FPerContextData* ActiveData = ContextData.Find(ChangeContext);

	if (ItemsToGrant.IsValidIndex(EntryIndex) && ActiveData)
	{
		const FGameFeatureItemsEntry& Entry = ItemsToGrant[EntryIndex];

		if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) ||
			(EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
		{
			RemoveActorItems(Actor, *ActiveData);
		}
		else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) ||
			(EventName == UGameFrameworkComponentManager::NAME_ReceiverAdded))
		{
			AddActorItems(Actor, Entry, *ActiveData);
		}
	}
}

void UGameFeatureAction_AddItems::AddActorItems(AActor* Actor, const FGameFeatureItemsEntry& ItemsEntry, FPerContextData& ActiveData)
{
	check(Actor);
	if (!Actor->HasAuthority())
	{
		return;
	}

	// early out if we already have an entry for this actor
	if (ActiveData.ActorExtensions.Find(Actor) != nullptr)
	{
		return;
	}

	if (UGameplayInventoryManager* InventoryManager = FindOrAddComponentForActor<UGameplayInventoryManager>(Actor, ItemsEntry, ActiveData))
	{
		FActorExtensions AddedExtensions;
		AddedExtensions.Items.Reserve(ItemsEntry.GrantedItems.Num());

		for (const FBotaniItemGrant& Item : ItemsEntry.GrantedItems)
		{
			if (Item.ItemDefinition == nullptr)
			{
				continue;
			}

			FGameplayInventoryItemSpec ItemSpec(Item.ItemDefinition, Item.Quantity, Actor);
			FGameplayInventoryItemSpecHandle ItemHandle = InventoryManager->GiveItem(ItemSpec, FGameplayInventoryItemContext(Actor));

			AddedExtensions.Items.Add(ItemHandle);
		}
	}
}

void UGameFeatureAction_AddItems::RemoveActorItems(AActor* Actor, FPerContextData& ActiveData)
{
	if (FActorExtensions* ActorExtensions = ActiveData.ActorExtensions.Find(Actor))
	{
		if (UGameplayInventoryManager* InventoryManager = Actor->FindComponentByClass<UGameplayInventoryManager>())
		{
			for (const FGameplayInventoryItemSpecHandle& Handle : ActorExtensions->Items)
			{
				InventoryManager->RemoveItem(Handle, FGameplayInventoryItemContext(Actor));
			}
		}
	}
}

UActorComponent* UGameFeatureAction_AddItems::FindOrAddComponentForActor(UClass* ComponentType, AActor* Actor, const FGameFeatureItemsEntry& ItemsEntry, FPerContextData& ActiveData)
{
	UActorComponent* Component = Actor->FindComponentByClass(ComponentType);

	bool bMakeComponentRequest = (Component == nullptr);
	if (Component)
	{
		if (Component->CreationMethod == EComponentCreationMethod::Native)
		{
			UObject* ComponentArchetype = Component->GetArchetype();
			bMakeComponentRequest = ComponentArchetype->HasAnyFlags(RF_ClassDefaultObject);
		}
	}

	if (bMakeComponentRequest)
	{
		const UWorld* World = Actor->GetWorld();
		const UGameInstance* GameInstance = World->GetGameInstance();

		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			TSharedPtr<FComponentRequestHandle> RequestHandle = ComponentManager->AddComponentRequest(ItemsEntry.ActorClass, ComponentType);
			ActiveData.ComponentRequests.Add(RequestHandle);
		}

		if (!Component)
		{
			Component = Actor->FindComponentByClass(ComponentType);
			ensureAlways(Component);
		}
	}

	return Component;
}

#undef LOCTEXT_NAMESPACE