// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagStackContainer.h"
#include "GameplayTagStackInterface.h"
#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Fragments/Item/ItemFragment_PickupDefinition.h"
#include "GameFramework/Actor.h"
#include "PickupProxyActor.generated.h"

class USphereComponent;

/**
 * APickupProxyActor
 *
 * An actor class used to function as a proxy between the item definition and the world it was dropped in.
 */
UCLASS(MinimalAPI, Config = Game, HideCategories = ("Rendering", "Replication", "Networking", "HLOD", "Input", "Actor", "Tick", "Cooking", "Collision", "Physics", "LevelInstance", "DataLayers"))
class APickupProxyActor : public AActor, public IGameplayTagStackInterface
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin AActor Interface
	GAMEPLAYINVENTORYSYSTEM_API virtual void BeginPlay() override;
	GAMEPLAYINVENTORYSYSTEM_API virtual void Destroyed() override;
	//~ End AActor Interface

	//~ Begin IGameplayTagStackInterface Interface
	GAMEPLAYINVENTORYSYSTEM_API virtual const FGameplayTagStackContainer& GetOwnedGameplayTagStacks() const override;
	//~ End IGameplayTagStackInterface Interface

	/** Called after the pickup has been dropped and initialized. */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnDropped(AActor* InInstigator, UGameplayInventoryItemDefinition* InItemDefinition, const FInventoryItemPickupData& InPickupData);

	/** Called right before the pickup is picked up. */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnPickedUp(AActor* InInstigator);

	/** Returns the item definition for this pickup. */
	UPROPERTY(ReplicatedUsing = OnRep_ItemDefinition, EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (ExposeOnSpawn = true))
	TSoftObjectPtr<UGameplayInventoryItemDefinition> ItemDefinition;

	/** Determines the authority-only pickup quantity. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Pickup", meta = (ExposeOnSpawn = true, UIMin = 0, ClampMin = 0))
	int32 PickupQuantity = 1;
	
	/** Gets the item definition for this pickup. */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	GAMEPLAYINVENTORYSYSTEM_API UGameplayInventoryItemDefinition* GetItemDefinition() const;

protected:
	/** Called when a player interacts with the pickup. */
	UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	GAMEPLAYINVENTORYSYSTEM_API void OnPickupCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Called after the pickup has been dropped and initialized. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup", meta = (DisplayName = "On Dropped"))
	GAMEPLAYINVENTORYSYSTEM_API void K2_OnDropped(AActor* InInstigator, UGameplayInventoryItemDefinition* InItemDefinition, const FInventoryItemPickupData& InPickupData);

	/** Called right before the pickup is picked up. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup", meta = (DisplayName = "On Picked Up"))
	GAMEPLAYINVENTORYSYSTEM_API void K2_OnPickedUp(AActor* InInstigator);

protected:
	/** The collision component used for interaction and world collision. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	GAMEPLAYINVENTORYSYSTEM_API void ApplyPickupFragment(const UItemFragment_PickupDefinition* InPickupFragment);

public:
	/** The scene component used to attach any preview meshes or effects. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USceneComponent> RotationSceneComponent;

#if WITH_EDITOR
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnConstruction(const FTransform& Transform) override;
#endif

private:
	/** The item definition path for this pickup. */
	FSoftObjectPath ItemDefinitionPath;

	UFUNCTION()
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnRep_ItemDefinition();

	GAMEPLAYINVENTORYSYSTEM_API FInventoryItemPickupData GetPickupData() const;
	
protected:
	/** The stored tag stacks of the item instance. */
	UPROPERTY(Transient, Replicated)
	FGameplayTagStackContainer TagStacks;
	
	GAMEPLAYINVENTORYSYSTEM_API void RequestUnloadItem();
	
	/** The delay before unloading the item definition. Prevents unloaded items from being used. */
	UPROPERTY(Config)
	float UnloadItemDelay = 5.0f;

	/** The timer handle for unloading the item definition. */
	FTimerHandle UnloadItemTimerHandle;

	/** Whether the item pickup effects have been played. */
	uint8 bItemPickupEffectsPlayed : 1;
};
