// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Spec/GameplayInventoryItemSpecHandle.h"
#include "GameFeatureAction_AddItems.generated.h"

USTRUCT(BlueprintType)
struct FBotaniItemGrant
{
	GENERATED_BODY()

	/** The item to grant */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	TObjectPtr<UGameplayInventoryItemDefinition> ItemDefinition;

	/** The quantity of the item to grant */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (ClampMin = 1, UIMin = 1))
	int32 Quantity = 1;

	/** Whether to equip the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	uint8 bEquipItem : 1 = false;

	/** Whether to add the item to the quick bar */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	uint8 bAddToQuickBar : 1 = false;

	/**
	 * Whether we want to override the quick bar slot index.
	 * (Might result in the item not being added to the quick bar if the slot is already occupied)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (EditCondition = "bAddToQuickBar", InlineEditConditionToggle))
	uint8 bOverrideQuickBarSlotIndex : 1 = false;

	/** The quick bar slot index to add the item to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (EditCondition = "bOverrideQuickBarSlotIndex", DisplayName = "Quick Bar Slot Override Index"))
	int32 QuickBarSlotIndex = 0;
};

USTRUCT()
struct FGameFeatureItemsEntry
{
	GENERATED_BODY()

	/** The base actor class to add to */
	UPROPERTY(EditAnywhere, Category = "Items")
	TSoftClassPtr<AActor> ActorClass;

	/** List of items to grant to actors of the specified class */
	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<FBotaniItemGrant> GrantedItems;
};

/**
 * GameFeatureAction to add items to the player's inventory
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Inventory Items"))
class UGameFeatureAction_AddItems : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	/** List of items to grant to actors of the specified class */
	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<FGameFeatureItemsEntry> ItemsToGrant;

public:
	//~ Begin UGameFeatureAction Interface
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction Interface

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject Interface

protected:
	//~ Begin UGameFeatureAction_WorldActionBase Interface
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
	//~ End UGameFeatureAction_WorldActionBase Interface

private:
	struct FActorExtensions
	{
		TArray<FGameplayInventoryItemSpecHandle> Items;
	};

	struct FPerContextData
	{
		TMap<AActor*, FActorExtensions> ActorExtensions;
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
	};

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

private:
	void Reset(FPerContextData& ActiveData);
	void HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex, FGameFeatureStateChangeContext ChangeContext);
	void AddActorItems(AActor* Actor, const FGameFeatureItemsEntry& ItemsEntry, FPerContextData& ActiveData);
	void RemoveActorItems(AActor* Actor, FPerContextData& ActiveData);

	template <class ComponentType>
	ComponentType* FindOrAddComponentForActor(AActor* Actor, const FGameFeatureItemsEntry& ItemsEntry, FPerContextData& ActiveData)
	{
		return Cast<ComponentType>(FindOrAddComponentForActor(ComponentType::StaticClass(), Actor, ItemsEntry, ActiveData));
	}

	UActorComponent* FindOrAddComponentForActor(UClass* ComponentType, AActor* Actor, const FGameFeatureItemsEntry& ItemsEntry, FPerContextData& ActiveData);
};
