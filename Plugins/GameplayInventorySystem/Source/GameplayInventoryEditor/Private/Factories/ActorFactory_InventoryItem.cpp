// Copyright © 2024 Botanibots Team. All rights reserved.


#include "ActorFactory_InventoryItem.h"

#include "EngineUtils.h"
#include "Pickup/PickupProxyActor.h"

UActorFactory_InventoryItem::UActorFactory_InventoryItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = NSLOCTEXT("GameplayInventory", "ActorFactory_InventoryItem", "Inventory Item");
	NewActorClass = APickupProxyActor::StaticClass();

	bShowInEditorQuickMenu = true;
	bUseSurfaceOrientation = true;
	bUsePlacementExtent = true;
}

AActor* UActorFactory_InventoryItem::GetDefaultActor(const FAssetData& AssetData)
{
	UGameplayInventoryItemDefinition* ItemDef = Cast<UGameplayInventoryItemDefinition>(AssetData.GetAsset());
	if (ItemDef == nullptr)
	{
		return nullptr;
	}

	if (UItemFragment_PickupDefinition* Frag = ItemDef->GetItemFragment<UItemFragment_PickupDefinition>())
	{
		return Frag->GetPickupProxyActor().LoadSynchronous()->GetDefaultObject<APickupProxyActor>();
	}

	return APickupProxyActor::StaticClass()->GetDefaultObject<AActor>();
}

bool UActorFactory_InventoryItem::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	const UGameplayInventoryItemDefinition* ItemDef = Cast<UGameplayInventoryItemDefinition>(AssetData.GetAsset());
	if (!ItemDef)
	{
		OutErrorMsg = NSLOCTEXT("GameplayInventory", "CanCreateActorFrom_NoItemDef", "No Inventory Item Definition was specified.");
		return false;
	}

	if (!ItemDef->GetItemFragment<UItemFragment_PickupDefinition>())
	{
		OutErrorMsg = NSLOCTEXT("GameplayInventory", "CanCreateActorFrom_NoPickupDef", "The Inventory Item Definition does not have a Pickup Definition.");
		return false;
	}

	return true;
}

void UActorFactory_InventoryItem::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

	UGameplayInventoryItemDefinition* ItemDef = Cast<UGameplayInventoryItemDefinition>(Asset);
	if (!ensure(ItemDef))
	{
		return;
	}

	FString ActorLabel = ItemDef->DisplayInfo.ItemName.ToString();
	// Find the number of actors that are already spawned in the level
	UWorld* World = NewActor->GetWorld();
	int32 NumActors = 0;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (It->GetClass() == NewActor->GetClass())
		{
			NumActors++;
		}
	}
	// Append the number of actors to the label
	ActorLabel.AppendInt(NumActors);
	
	NewActor->SetActorLabel(ActorLabel);
	NewActor->SetActorRotation(ItemDef->DefaultRotation);

	if (ItemDef->bUseLocationOffset)
	{
		NewActor->SetActorLocation(NewActor->GetActorLocation() + ItemDef->LocationOffset);
	}

	if (APickupProxyActor* PickupProxy = Cast<APickupProxyActor>(NewActor))
	{
		PickupProxy->ItemDefinition = Cast<UGameplayInventoryItemDefinition>(Asset);
	}
}

UObject* UActorFactory_InventoryItem::GetAssetFromActorInstance(AActor* ActorInstance)
{
	check(ActorInstance->IsA(NewActorClass));
	const APickupProxyActor* PickupProxy = Cast<APickupProxyActor>(ActorInstance);
	if (!PickupProxy->ItemDefinition.IsNull())
	{
		return PickupProxy->ItemDefinition.Get();
	}

	return nullptr;
}
