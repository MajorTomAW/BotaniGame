// Copyright © 2024 MajorT. All rights reserved.


#include "Library/InventorySystemBlueprintLibrary.h"

#include "GameplayInventoryLogChannels.h"
#include "Components/GameplayEquipmentManager.h"
#include "Components/GameplayInventoryManager.h"
#include "Fragments/Item/ItemFragment_PickupDefinition.h"
#include "GameFramework/PlayerState.h"
#include "Instance/GameplayInventoryItemInstance.h"
#include "Pickup/PickupProxyActor.h"

UInventorySystemBlueprintLibrary::UInventorySystemBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UGameplayInventoryManager* UInventorySystemBlueprintLibrary::FindInventoryManager(const AActor* Actor)
{
	if (!ensure(Actor))
	{
		return nullptr;
	}

	UGameplayInventoryManager* Result = Actor->FindComponentByClass<UGameplayInventoryManager>();

	const APawn* Pawn = Cast<APawn>(Actor);
	if (Pawn == nullptr)
	{
		const AController* Controller = Cast<AController>(Actor);
		if (Controller)
		{
			Result = Controller->FindComponentByClass<UGameplayInventoryManager>();
			Pawn = Controller->GetPawn();
		}
	}

	if (Pawn == nullptr)
	{
		return Result;
	}

	if (Result == nullptr)
	{
		Result = Pawn->Controller ? Pawn->Controller->FindComponentByClass<UGameplayInventoryManager>() : nullptr;
	}

	if (Result == nullptr)
	{
		Result = Pawn->GetPlayerState() ? Pawn->GetPlayerState()->FindComponentByClass<UGameplayInventoryManager>() : nullptr;
	}


	return Result;
}

UGameplayEquipmentManager* UInventorySystemBlueprintLibrary::FindEquipmentManager(const AActor* Actor)
{
	if (!ensure(Actor))
	{
		return nullptr;
	}

	UGameplayEquipmentManager* Result = Actor->FindComponentByClass<UGameplayEquipmentManager>();
	const APawn* OwnerPawn = Cast<APawn>(Actor);

	if (OwnerPawn == nullptr)
	{
		if (const AController* Controller = Cast<AController>(Actor))
		{
			OwnerPawn = Controller->GetPawn();
		}
	}

	if (OwnerPawn == nullptr)
	{
		if (const APlayerState* PlayerState = Cast<APlayerState>(Actor))
		{
			OwnerPawn = PlayerState->GetPawn();
		}
	}

	if (Result == nullptr)
	{
		Result = OwnerPawn->FindComponentByClass<UGameplayEquipmentManager>();
	}

	return Result;
}

UGameplayInventoryItemInstance* UInventorySystemBlueprintLibrary::GetInventoryItemInstanceFromHandle( UGameplayInventoryManager* InventoryManager, const FGameplayInventoryItemSpecHandle& ItemHandle)
{
	// Check if the inventory manager is valid
	if (!InventoryManager)
	{
		LOG_INVENTORY(Error, TEXT("[%hs]: Called with an invalid inventory manager."), __FUNCTION__);
		return nullptr;
	}

	// Get and validate the item spec
	const FGameplayInventoryItemSpec* ItemSpec = InventoryManager->FindItemSpecFromHandle(ItemHandle);
	if (!ItemSpec)
	{
		LOG_INVENTORY(Error, TEXT("[%hs]: Called with an invalid item spec handle."), __FUNCTION__);
		return nullptr;
	}

	return ItemSpec->GetInstance();
}

class UGameplayEquipmentInstance* UInventorySystemBlueprintLibrary::GetEquipmentInstanceFromHandle(UGameplayEquipmentManager* EquipmentManager, const FGameplayInventoryItemSpecHandle& ItemHandle)
{
	// Check if the equipment manager is valid
	if (!EquipmentManager)
	{
		LOG_INVENTORY(Error, TEXT("[%hs]: Called with an invalid equipment manager."), __FUNCTION__);
		return nullptr;
	}

	// Get and validate the equipment spec
	const FGameplayEquipmentSpec* EquipmentSpec = EquipmentManager->FindEquipmentSpecFromHandle(ItemHandle);
	if (!EquipmentSpec)
	{
		LOG_INVENTORY(Error, TEXT("[%hs]: Called with an invalid equipment spec handle."), __FUNCTION__);
		return nullptr;
	}

	return EquipmentSpec->GetInstance();
}

class APickupProxyActor* UInventorySystemBlueprintLibrary::SpawnPickupProxyActor(UObject* WorldContextObject, UGameplayInventoryItemDefinition* ItemDefinition, const FTransform& SpawnTransform, const int32 StackCount, AActor* Owner, APawn* Instigator)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		LOG_INVENTORY(Error, TEXT("[%hs]: Failed to get world from context object."), __FUNCTION__);
		return nullptr;
	}

	if (!ItemDefinition || !ItemDefinition->GetItemFragment<UItemFragment_PickupDefinition>())
	{
		LOG_INVENTORY(Error, TEXT("[%hs]: Item definition is invalid or does not have a pickup fragment."), __FUNCTION__);
		return nullptr;
	}

	UItemFragment_PickupDefinition* PickupFrag = ItemDefinition->GetItemFragment<UItemFragment_PickupDefinition>();
	UClass* PickupClass = PickupFrag->GetPickupProxyActor().LoadSynchronous();

	APickupProxyActor* PickupActor = World->SpawnActorDeferred<APickupProxyActor>(PickupClass, SpawnTransform, Owner, Instigator, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	check(PickupActor);
	
	PickupActor->ItemDefinition = ItemDefinition;

	PickupActor->FinishSpawning(SpawnTransform);
	return PickupActor;
}

FString UInventorySystemBlueprintLibrary::Conv_ItemHandleToString(FGameplayInventoryItemSpecHandle Handle)
{
	return Handle.ToString();
}

FGameplayInventoryItemContext UInventorySystemBlueprintLibrary::CreateItemContext(UGameplayInventoryComponent* OwnerComponent, UObject* Instigator, FGameplayTagContainer Tags)
{
	FGameplayInventoryItemContext NewContext;
	NewContext.InventoryComponent = OwnerComponent;

	if (Instigator)
	{
		AActor* Actor = Cast<AActor>(Instigator);
		NewContext.Instigator = Actor;
	}

	if (NewContext.Instigator == nullptr)
	{
		NewContext.Instigator = OwnerComponent->GetOwner();
	}
	
	NewContext.ContextTags = Tags;
	NewContext.ItemInstance = Instigator ? Cast<UGameplayInventoryItemInstance>(Instigator) : nullptr;

	return NewContext;
}

FGameplayInventoryItemContext UInventorySystemBlueprintLibrary::CreateItemContextAdvanced(UGameplayInventoryComponent* OwnerComponent, UObject* Instigator, FGameplayTagContainer Tags, UGameplayInventoryItemDefinition* ItemDefinition, const int32 StackCount)
{
	FGameplayInventoryItemContext NewContext;
	NewContext.InventoryComponent = OwnerComponent;

	if (Instigator)
	{
		AActor* Actor = Cast<AActor>(Instigator);
		NewContext.Instigator = Actor;
	}

	if (NewContext.Instigator == nullptr)
	{
		NewContext.Instigator = OwnerComponent->GetOwner();
	}

	NewContext.ContextTags = Tags;
	NewContext.ItemDefinition = ItemDefinition;
	NewContext.StackCount = StackCount;

	return NewContext;
}

void UInventorySystemBlueprintLibrary::DropItemByHandle(UGameplayInventoryManager* InventoryManager, const FGameplayInventoryItemSpecHandle& ItemHandle, const int32 DropCount, const FTransform& DropTransform)
{
	check(InventoryManager);
	check(ItemHandle.IsValid());
	check(InventoryManager->GetOwner()->HasAuthority());

	if (UGameplayEquipmentManager* EquipmentManager = FindEquipmentManager(InventoryManager->GetOwner()))
	{
		EquipmentManager->UnequipItem(ItemHandle);
	}
	else
	{
		LOG_INVENTORY(Fatal, TEXT("Failed to unequip item '%s' before dropping."), *ItemHandle.ToString());
	}

	FGameplayInventoryItemContext Context(InventoryManager->GetOwner());
	Context.InventoryComponent = InventoryManager;
	Context.StackCount = DropCount;
	Context.Instigator = InventoryManager->GetOwner();

	const FGameplayInventoryItemSpec& Spec = *InventoryManager->FindItemSpecFromHandle(ItemHandle);
	if (!Spec.IsValid())
	{
		return;
	}

	const UGameplayInventoryItemDefinition* ItemDef = Spec.GetInstance()->GetItemDefinition();
	check(ItemDef);

	InventoryManager->RemoveItem(ItemHandle, Context);

	const UItemFragment_PickupDefinition* PickupFrag = ItemDef->GetItemFragment<UItemFragment_PickupDefinition>();
	if (PickupFrag == nullptr)
	{
		LOG_INVENTORY(Warning, TEXT("Item '%s' does not have a pickup fragment."), *ItemDef->GetName());
		return;
	}

	UClass* PickupClass = PickupFrag->GetPickupProxyActor().LoadSynchronous();

	APickupProxyActor* PickupActor = InventoryManager->GetOwner()->GetWorld()->SpawnActorDeferred<APickupProxyActor>(PickupClass, DropTransform);
	PickupActor->ItemDefinition = ItemDef;
	PickupActor->FinishSpawning(DropTransform);
}

FGameplayInventoryItemSpecHandle UInventorySystemBlueprintLibrary::FindItemHandleByTag(UGameplayInventoryManager* InventoryManager, const FGameplayTag Tag, const bool bExactMatch)
{
	check(InventoryManager);
	check(Tag.IsValid());

	for (const auto& Handle : InventoryManager->GetInventoryList())
	{
		FGameplayInventoryItemSpec& Spec = *InventoryManager->FindItemSpecFromHandle(Handle);
		if (!Spec.IsValid())
		{
			continue;
		}

		const UGameplayInventoryItemDefinition* ItemDef = Spec.GetInstance()->GetItemDefinition();
		check(ItemDef);

		if (bExactMatch)
		{
			if (ItemDef->GetGameplayTags().HasTagExact(Tag))
			{
				return Handle;
			}
		}
		else
		{
			if (ItemDef->GetGameplayTags().HasTag(Tag))
			{
				return Handle;
			}
		}
	}

	return FGameplayInventoryItemSpecHandle();
}
