// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEquipmentItem.generated.h"

struct FGameplayInventoryItemContext;
/**
 * AGameplayEquipmentItem
 *
 * An equipment actor that gets spawned attached to the owner when equipped.
 */
UCLASS(MinimalAPI, Abstract, HideCategories = ("Replication", "Actor", "Input", "Events", "Collision", "HLOD", "Physics", "LevelInstance", "Cooking", "WorldPartition", "DataLayers", "Rendering", "ActorTick", "Actor Tick"))
class AGameplayEquipmentItem : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns the skeletal mesh component for the item. */
	USkeletalMeshComponent* GetItemMeshComponent() const { return ItemMeshComponent.Get(); }

	/**
	 * Called when this equipment actor has been spawned and equipped.
	 * @param OwningPawn		The pawn that owns this equipment.
	 * @param Context			The context in which this item was equipped.
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	virtual void OnEquipped(APawn* OwningPawn, const FGameplayInventoryItemContext& Context);

	/**
	 * Called when this equipment actor has been unequipped.
	 * @param OwningPawn		The pawn that owns this equipment.
	 * @param Context			The context in which this item was unequipped.
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	virtual void OnUnequipped(APawn* OwningPawn, const FGameplayInventoryItemContext& Context);


	/**
     * Called when this equipment actor has been spawned and equipped.
	 * @param OwningPawn		The pawn that owns this equipment.
	 * @param Context			The context in which this item was equipped.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "On Equipped"))
	void K2_OnEquipped(APawn* OwningPawn, const FGameplayInventoryItemContext& Context);

	/**
	 * Called when this equipment actor has been unequipped.
	 * @param OwningPawn		The pawn that owns this equipment.
	 * @param Context			The context in which this item was unequipped.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "On Unequipped"))
	void K2_OnUnequipped(APawn* OwningPawn, const FGameplayInventoryItemContext& Context);
	
protected:
	/** Returns the owning pawn of this equipment item. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	GAMEPLAYINVENTORYSYSTEM_API APawn* GetOwningPawn() const;

	/** Returns the owning pawn of this equipment item, cast to the specified class. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment", meta = (DeterminesOutputType = "PawnClass"))
	GAMEPLAYINVENTORYSYSTEM_API APawn* GetTypedOwningPawn(TSubclassOf<APawn> PawnClass) const;

private:
	/** The skeletal mesh component for the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ItemMeshComponent;
};
