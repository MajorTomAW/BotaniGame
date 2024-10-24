// Copyright © 2024 MajorT. All rights reserved.

#pragma once
#include "GameplayTagContainer.h"

#include "GameplayInventoryTypes.generated.h"

/**
 * FGameplayInventoryItemContext
 *
 * Context struct used to pass around information about an item.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYSYSTEM_API FGameplayInventoryItemContext
{
	GENERATED_BODY()

	FGameplayInventoryItemContext()
	{
	}

	explicit FGameplayInventoryItemContext(AActor* InInventoryOwner);

	bool operator==(const FGameplayInventoryItemContext& Other) const
	{
		return (Instigator == Other.Instigator && InventoryOwner == Other.InventoryOwner && ItemDefinition == Other.ItemDefinition);
	}

	void FillContext();

	bool HasInstigatorOrOwnerAuthority() const;

public:
	/** The object that instigated the action */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UObject> Instigator = nullptr;

	/** The actor that owns the inventory associated with the item */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<AActor> InventoryOwner = nullptr;

	/** The inventory component associated with the owner */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<class UGameplayInventoryComponent> InventoryComponent;

	/** Additional context gameplay tags that can be used to give more context to the item */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FGameplayTagContainer ContextTags;

	/** The item definition of this item */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<class UGameplayInventoryItemDefinition> ItemDefinition = nullptr;

	/** The item stack count associated with this item */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 StackCount = 0;

	/** The item instance associated with this item. (May often be null) */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<class UGameplayInventoryItemInstance> ItemInstance = nullptr;

public:
	bool IsValid() const;
};

/**
 * FGameplayInventoryItemStackingData
 *
 * Data struct used to store information about how an item stacks.
 */
USTRUCT(BlueprintType)
struct FGameplayInventoryItemStackingData
{
	GENERATED_BODY()

	FGameplayInventoryItemStackingData()
		: bCanStack(false)
		, MaxStackSize(1)
	{
	}

public:
	/** Whether this item supports stacking. If false, the item will be treated as a unique item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	uint8 bCanStack : 1;

	/** The max stack size of the item describes how many items can be stacked together. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (EditCondition = "bCanStack", ClampMin = 1, UIMin = 1))
	int32 MaxStackSize;
};
