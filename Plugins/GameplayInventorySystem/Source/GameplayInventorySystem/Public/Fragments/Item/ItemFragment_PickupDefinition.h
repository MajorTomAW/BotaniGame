// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Fragments/GameplayInventoryItemFragment.h"
#include "ItemFragment_PickupDefinition.generated.h"


class UNiagaraSystem;
class APickupProxyActor;

UENUM(BlueprintType)
enum class EInventoryItemAutoPickupBehavior : uint8
{
	/** The item will not be picked up automatically */
	DoNotAutoPickup = 0,

	/** The item will be picked up automatically if the target has enough space in their inventory */
	TryAutoPickup,

	/** The item will be picked up automatically and replaces the current stack in the Inventory */
	AutoPickupReplaceCurrent,
};

USTRUCT(BlueprintType)
struct FInventoryItemPickupData
{
	GENERATED_BODY()

	FInventoryItemPickupData()
		: AutoPickupBehavior(EInventoryItemAutoPickupBehavior::TryAutoPickup)
		, bAutoEquip(false)
		, bDestroyOnPickup(true)
		, PickupRadius(100.0f)
		, PickupEffect(nullptr)
		, PickupSound(nullptr)
	{
	}

public:
	/** How the item should behave when the player or another entity walks over it. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	EInventoryItemAutoPickupBehavior AutoPickupBehavior;
	
	/** Whether the item should be automatically equipped when picked up. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	uint8 bAutoEquip : 1;

	/**
	 * Whether the pickup proxy actor should be destroyed when picked up.
	 * Can be useful for item spawners that are meant to be picked up multiple times
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	uint8 bDestroyOnPickup:1;

	/** The radius around the item that will trigger the pickup. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = "0.0", UIMin = "0.0", Units = "cm"))
	float PickupRadius;

	/** The list of pickup icons to be shown when this item is spawned in the world. */
	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	TArray<TObjectPtr<class UPickupItemIcon>> PickupIcons;

	/** The effect to be played when the item is picked up. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	TSoftObjectPtr<UNiagaraSystem> PickupEffect;

	/** The sound to be played when the item is picked up. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	TSoftObjectPtr<USoundBase> PickupSound;

public:
	bool IsValid() const;
	bool Equals(const FInventoryItemPickupData& Other) const;
};

/**
 * EInventoryItemDropBehavior
 *
 * The drop behavior of an item.
 */
UENUM(BlueprintType)
enum class EInventoryItemDropBehavior : uint8
{
	/** The item will not be dropped */
	DoNotDrop = 0,

	/** The item will always be dropped */
	AlwaysDrop,

	/** The item will be dropped if the drop chance is met */
	ChanceDrop,
};

/**
 * FInventoryItemDropData
 *
 * Stores data about how an item should be dropped in the world.
 */
USTRUCT(BlueprintType)
struct FInventoryItemDropData
{
	GENERATED_BODY()

public:
	/** The drop behavior of the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	EInventoryItemDropBehavior DropBehavior = EInventoryItemDropBehavior::AlwaysDrop;

	/** The chance that the item will be dropped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop", meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "100.0", UIMax = "100.0", ForceUnits = "%", EditCondition = "DropBehavior == EInventoryItemDropBehavior::ChanceDrop", EditConditionHides))
	float DropChance = 100.f;
};


/**
 * UItemFragment_PickupDefinition
 *
 * Defines the pickup of an item such as the behavior and appearance of the item in the world.
 */
UCLASS(MinimalAPI, Config = Game, meta = (DisplayName = "Pickup Definition", ShortTooltip = "Defines the pickup of an item such as the behavior and appearance of the item in the world."))
class UItemFragment_PickupDefinition : public UGameplayInventoryItemFragment
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns true if the item should now be dropped in the world. */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool ShouldDropItem() const;

public:
	virtual TSoftClassPtr<APickupProxyActor> GetPickupProxyActor() const;
	
	/** The pickup proxy actor to be spawned in the world when the item is dropped. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (DisplayName = "Override Pickup Proxy Actor"))
	TSoftClassPtr<APickupProxyActor> PickupProxyActor;

	/** The data that defines how the item should behave when picked up. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	FInventoryItemPickupData PickupData;

	/** The data that defines how the item should behave when dropped. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	FInventoryItemDropData DropData;

protected:
#if WITH_EDITOR
	virtual FString GetEditorFriendlyName_Implementation() const override;
#endif

	UPROPERTY(Config)
	TSoftClassPtr<APickupProxyActor> DefaultPickupProxyActor;
};
