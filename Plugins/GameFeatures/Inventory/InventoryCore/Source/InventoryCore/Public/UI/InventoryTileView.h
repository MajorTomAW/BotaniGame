// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonTileView.h"
#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Instance/GameplayInventoryItemInstance.h"
#include "InventoryTileView.generated.h"

class UGameplayInventoryManager;


/**
 * UInventoryTileEntryObject
 *
 * Tile entry object for the inventory tile view.
 */
UCLASS(BlueprintType)
class UInventoryTileEntryObject : public UObject
{
	GENERATED_BODY()

public:
	/** Returns the item instance associated with this tile entry. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UGameplayInventoryItemInstance* GetItemInstance() const { return ItemInstance; }

	/** Sets the item instance associated with this tile entry. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetItemInstance(UGameplayInventoryItemInstance* InItemInstance) { ItemInstance = InItemInstance; }

	/** Returns the item definition of the given type. */
	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (DeterminesOutputType = "DefinitionClass"))
	UGameplayInventoryItemDefinition* GetItemDefinition(TSubclassOf<UGameplayInventoryItemDefinition> DefinitionClass) const;

public:
	/** Returns the item definition of the given type. */
	template <typename T>
	T* GetItemDefinition() const;

private:
	UPROPERTY()
	TObjectPtr<UGameplayInventoryItemInstance> ItemInstance;
};

template <typename T>
T* UInventoryTileEntryObject::GetItemDefinition() const
{
	return ItemInstance->GetItemDefinition<T>();
}

/**
 * UInventoryTileView
 *
 * Tile view widget to display a single inventory row
 */
UCLASS()
class INVENTORYCORE_API UInventoryTileView : public UCommonTileView
{
	GENERATED_UCLASS_BODY()

public:
	/** Initializes the tile view with the given inventory manager. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitializeWithInventoryManager(UGameplayInventoryManager* InInventoryManager);

protected:
	virtual void AddInitialEntries();
	
private:
	UPROPERTY()
	TObjectPtr<UGameplayInventoryManager> InventoryManager;
};
