// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GameplayInventoryRowConfig.generated.h"

class UGameplayInventoryItemDefinition;

/**
 * EGameplayInventoryLimits
 * 
 * Enum that defines the different types of limits that can be placed on a row.
 * For example, you could have a row that can only hold a certain number of items.
 */
UENUM()
enum class EGameplayInventoryLimits : uint8
{
	/** No limit on the number of items that can be placed in the row. */
	NoLimit,

	/** The row can only hold a certain number of stacks. (for example, a stack limit of 3 could mean we have 64x Stone, 12x Wood, 1x Sword in a single row.) */
	ItemLimit,

	/** The row can only hold a certain number of items. (for example, an item limit of 10 could mean we have 4x Stone, 4x Wood, 2x Sword in a single row.) */
	SlotLimit,

	/** The row can only hold a certain amount of weight. */
	WeightLimit,
};

/**
 * UGameplayInventoryRowConfig
 *
 * Config class that defines a single row in the inventory.
 * Useful for defining both layouts in the inventory but also limitations on what can be placed in what row.
 * Besides cosmetic purposes, rows can also be used to define the max number of items that can be placed in a row,
 * or the max weight that a row can hold.
 * Maybe even a filter so that you can only add specific types of items to a row.
 * 
 * The most common example would be that you have two rows.
 * 1. Could be the QuickBar or HotBar that you can (always) see on your game HUD.
 * 2. Could be the rest of the Inventory that you can open and close.
 *
 * (Going further, you could have rows for different types of items, but that is a bit more advanced.
 * For example, in minecraft you have a row for the HotBar, a row for the main inventory, and a row for the armor slots.)
 *
 * However, if you don't change anything, the inventory will be a single row by default.
 * You can add more rows by adding more of these config objects and granting them to the inventory manager.
 */
UCLASS(BlueprintType, Config = Game, MinimalAPI, meta = (DisplayName = "Inventory Row Config", ShortToolTip = "Config class that defines a single row in the inventory."))
class UGameplayInventoryRowConfig : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns whether this row is meant for the spec. */
	virtual bool DoesSpecMatch(const struct FGameplayInventoryItemSpec& Spec) const;

#if WITH_EDITOR
	//~Begin UObject Interface
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	//~End UObject Interface
#endif

public:
	/** The display name that can be used to show the row in the UI. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Row")
	FText DisplayName;
	
	/** The tag that defines the row. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Row")
	FGameplayTag RowTag;

	/** The limits of this row. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Row")
	EGameplayInventoryLimits Limits;

	/** The maximum number of items that can be placed in this row. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Row", meta = (EditCondition = "Limits == EGameplayInventoryLimits::ItemLimit", EditConditionHides, ClampMin = 1, UIMin = 1))
	int32 MaxItemCount;

	/** The maximum number of stacks (slots) that can be placed in a row. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Row", meta = (EditCondition = "Limits == EGameplayInventoryLimits::SlotLimit", EditConditionHides, ClampMin = 1, UIMin = 1))
	int32 MaxSlotCount;

	/** The maximum weight that can be placed in this row. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Row", meta = (EditCondition = "Limits == EGameplayInventoryLimits::WeightLimit", Units = "kg", EditConditionHides))
	float MaxWeight;

	/** The gameplay tag filter that defines what items can be placed in this row. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Row|Filter")
	FGameplayTagContainer TagsFilter;

	/** The item class filter that defines what items can be placed in this row. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Row|Filter")
	TArray<TSubclassOf<class UGameplayInventoryItemDefinition>> ItemClassFilter;

	/** The negative item class filter that defines what items can't be placed in this row. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Row|Filter")
	TArray<TSubclassOf<UGameplayInventoryItemDefinition>> NegativeItemClassFilter;
};
