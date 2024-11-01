// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventorySystemBlueprintLibrary.generated.h"

class UGameplayInventoryItemDefinition;
class UGameplayEquipmentManager;
class UGameplayInventoryComponent;
struct FGameplayInventoryItemContext;
struct FGameplayInventoryItemSpecHandle;
class UGameplayInventoryItemInstance;
class UGameplayInventoryManager;

/**
 * Inventory System library for Blueprint functions.
 * Many of these functions are useful to call from native as well.
 */
UCLASS(meta=(ScriptName = "InventorySystemLibrary"))
class GAMEPLAYINVENTORYSYSTEM_API UInventorySystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/**
	 * Tries to find an inventory manager component on the actor.
	 * @param Actor The actor to find the inventory manager on.
	 * @return The inventory manager component, or nullptr if not found.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory", meta = (DefaultToSelf = "Actor"))
	static UGameplayInventoryManager* FindInventoryManager(const AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory", meta = (DefaultToSelf = "Actor"))
	static UGameplayEquipmentManager* FindEquipmentManager(const AActor* Actor);

	/**
	 * Provides the inventory item instance associated with the given item spec handle.
	 *
	 * @param InventoryManager The inventory manager to get the item instance from.
	 * @param ItemHandle The item spec handle to get the item instance from.
	 * @return The item instance associated with the item spec handle. Will be nullptr if the item spec handle is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	static UGameplayInventoryItemInstance* GetInventoryItemInstanceFromHandle(UGameplayInventoryManager* InventoryManager, const FGameplayInventoryItemSpecHandle& ItemHandle);

	/**
	 * Provides the equipment instance associated with the given item spec handle:
	 *
	 * @param EquipmentManager The equipment manager to get the equipment instance from.
	 * @param ItemHandle The item spec handle to get the equipment instance from.
	 * @return The equipment instance associated with the item spec handle. Will be nullptr if the item spec handle is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	static class UGameplayEquipmentInstance* GetEquipmentInstanceFromHandle(UGameplayEquipmentManager* EquipmentManager, const FGameplayInventoryItemSpecHandle& ItemHandle);

	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (WorldContext = "WorldContextObject"))
	static class APickupProxyActor* SpawnPickupProxyActor(UObject* WorldContextObject, UGameplayInventoryItemDefinition* ItemDefinition, const FTransform& SpawnTransform, const int32 StackCount, AActor* Owner, APawn* Instigator);

	/** Converts an item handle to a string */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "To String (Item Handle)", CompactNodeTitle = "->", BlueprintAutocast), Category="Utilities|String")
	static FString Conv_ItemHandleToString(FGameplayInventoryItemSpecHandle Handle);

	/*UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory System", meta = (DisplayName = "Create Item Context (Simple)", DefaultToSelf = "Instigator"))
	static FGameplayInventoryItemContext CreateItemContext(UGameplayInventoryComponent* OwnerComponent, UObject* Instigator, FGameplayTagContainer Tags);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory System", meta = (DisplayName = "Create Item Context (Advanced)", DefaultToSelf = "Instigator"))
	static FGameplayInventoryItemContext CreateItemContextAdvanced(UGameplayInventoryComponent* OwnerComponent, UObject* Instigator, FGameplayTagContainer Tags, UGameplayInventoryItemDefinition* ItemDefinition, const int32 StackCount);*/

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory", meta = (DisplayName = "Drop Item (By Handle)"))
	static void DropItemByHandle(UGameplayInventoryManager* InventoryManager, const FGameplayInventoryItemSpecHandle& ItemHandle, const int32 DropCount, const FTransform& DropTransform);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	static FGameplayInventoryItemSpecHandle FindItemHandleByTag(UGameplayInventoryManager* InventoryManager, const FGameplayTag Tag, bool bExactMatch = false);
};
