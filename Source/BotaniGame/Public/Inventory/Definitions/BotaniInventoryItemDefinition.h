// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Definitions/GameplayInventoryItemDefinition.h"
#include "BotaniInventoryItemDefinition.generated.h"

UENUM(BlueprintType)
enum class EBotaniItemRarity : uint8
{
	/** Rarity is not set */
	Unset,
	
	/** Dropped from normal enemies */
	Common,

	/** Dropped from normal enemies */
	Epic,

	/** Dropped from bosses */
	Legendary,

	/** Non-droppable items */
	Divine
};

USTRUCT(BlueprintType)
struct FBotaniItemDurabilityStat
{
	GENERATED_BODY()

public:
	/** The durability of the item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Durability", meta = (Categories = "Item.Stat.Durability"))
	FGameplayTag DurabilityTag;
	
	/** The maximum durability of the item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Durability")
	int32 MaxDurability = 100;

	/** How much durability the item starts with */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Durability")
	int32 StartupDurability = 100;

	/** Multiplies the decay rate of the item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Durability", meta = (ForceUnits = "x"))
	float DurabilityDecayMultiplier = 1.0f;
};

/**
 * UBotaniInventoryItemDefinition
 */
UCLASS(Abstract, meta = (DisplayName = "Botani Item Definition"))
class BOTANIGAME_API UBotaniInventoryItemDefinition : public UGameplayInventoryItemDefinition
{
	GENERATED_UCLASS_BODY()

public:
	/** Rarity of the item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Properties")
	EBotaniItemRarity Rarity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Properties|Durability")
	uint8 bHasDurability : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Properties|Durability", meta = (EditCondition = "bHasDurability", TitleProperty = "DurabilityTag"))
	TArray<FBotaniItemDurabilityStat> DurabilityStats;
};
