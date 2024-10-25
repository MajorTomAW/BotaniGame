// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayInventoryComponent.h"
#include "GameFramework/HUD.h"
#include "GameplayInventoryManager.generated.h"

class UGameplayInventoryRowConfig;
DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryManagerChangedSignature,
                                    const FGameplayInventoryItemSpecHandle& ItemHandle);

/**
 * UGameplayInventoryManager
 *
 * An inventory manager that can be used to manage the inventory of a player or other entity.
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class GAMEPLAYINVENTORYSYSTEM_API UGameplayInventoryManager : public UGameplayInventoryComponent
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	virtual void InitializeComponent() override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;
	//~ End UObject Interface

	FInventoryManagerChangedSignature OnItemRemoved;

public:
	/**
	 * Returns whether the ItemDefinition can be added to the inventory, will be based on certain conditions like stack count, weight, etc.
	 * @param ItemSpec		FInventoryItemSpec of the item to give, including the item instance and stack count.
	 * @param Context		Context for the item.
	 * @return					True if the item can be added
	 */
	virtual bool CanAddItemDef(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context);
	virtual bool CanAddItemFullyToExisting(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context, FGameplayInventoryItemSpec& ExistingSpec);

	virtual bool CanAddItemToRow(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context, const UGameplayInventoryRowConfig* RowConfig);

	virtual UGameplayInventoryRowConfig* FindRowConfig(const FGameplayInventoryItemSpec& ItemSpec);

	/**
	 * Gives an item to the inventory.
	 * Will be ignored if the actor doesn't have authority, since the inventory is server-authoritative.
	 * 
	 * @param ItemSpec		FInventoryItemSpec of the item to give, including the item instance and stack count.
	 * @param Context		Context for the item.
	 * @return The item spec handle of the item that was given.
	 */
	virtual FGameplayInventoryItemSpecHandle GiveItem(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context);

	/**
	 * Removes an item by its handle from the inventory
	 * Will be ignored if the actor doesn't have authority, since the inventory is server-authoritative
	 * @note : As we're trying to remove a specific item by its handle, it will only remove the item if the handle matches exactly.
	 *		 Meaning that other items that may have the same item definition but a different handle (stack) will not be removed.
	 *
	 * @param ItemToRemoveHandle	The handle of the item to remove
	 * @param Context				Context for the item that stores stack count, etc.
	 */
	virtual void RemoveItem(const FGameplayInventoryItemSpecHandle& ItemToRemoveHandle, const FGameplayInventoryItemContext& Context);

	/**
	 * Removes an item by its definition from the inventory.
	 * Will be ignored if the actor doesn't have authority, since the inventory is server-authoritative.
	 * @param ItemToRemove			The item instance to remove.
	 * @param Context				Context for the item that stores stack count, etc.
	 */
	virtual void RemoveItem(UGameplayInventoryItemInstance* ItemToRemove, const FGameplayInventoryItemContext& Context);

	/**
	 * Removes an item by its definition from the inventory.
	 * Meaning that all items with the same definition will be removed. No matter the handle.
	 * Will be ignored if the actor doesn't have authority, since the inventory is server-authoritative.
	 * @param Context				Context for the item that stores stack count, etc.
	 */
	virtual void ConsumeItem(const FGameplayInventoryItemContext& Context);

	/** Removes an item that matches the predicate */
	void InternalRemoveItemByPredicate(const TFunctionRef<bool(const FGameplayInventoryItemSpec& Other)>& Predicate, const FGameplayInventoryItemContext& Context);

	/**
	 * Gives an item (from spec) to the inventory.
	 * Will be ignored if the actor doesn't have authority, since the inventory is server-authoritative.
	 * 
	 * @param ItemSpec		FInventoryItemSpec of the item to give, including the item instance and stack count.
	 * @param Context		Context for the item.
	 * @return The item spec handle of the item that was given.
	 */
	//UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory", meta = (DisplayName = "Give Item (From Spec)", AutoCreateRefTerm = "Context", Keywords = "add give item spec"))
	//FGameplayInventoryItemSpecHandle K2_GiveItemFromSpec(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context);

	/**
	 * Gives an item (from context) to the inventory.
	 * Will be ignored if the actor doesn't have authority, since the inventory is server-authoritative.
	 *
	 * @param Context	Context for the item, must contain item definition and stack count.
	 * @return The item spec handle of the item that was given.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory", meta = (DisplayName = "Give Item", AutoCreateRefTerm = "Context", Keywords = "add give item context"))
	FGameplayInventoryItemSpecHandle K2_GiveItemFromContext(const FGameplayInventoryItemContext& Context);

	/**
	 * Gives an item (from definition) to the inventory.
	 * Will be ignored if the actor doesn't have authority, since the inventory is server-authoritative.
	 *
	 * @param ItemDefinition The item definition to give.
	 * @param Context Context for the item.
	 * @param StackCount The number of items to give.
	 * @return The item spec handle of the item that was given.
	 */
	//UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory", meta = (DisplayName = "Give Item (From Definition)", AutoCreateRefTerm = "Context", Keywords = "add give item def definition"))
	//FGameplayInventoryItemSpecHandle K2_GiveItemFromDefinition(UGameplayInventoryItemDefinition* ItemDefinition, const FGameplayInventoryItemContext& Context, int32 StackCount = 1);

	//------- I dont think it is necessary to have different way on adding items if they all result in the same function

	/**
	 * Removes an item (from handle) from the inventory.
	 * Will be ignored if the actor doesn't have authority, since the inventory is server-authoritative.
	 *
	 * @param ItemToRemoveHandle The handle of the item to remove.
	 * @param Context Context for the item that stores stack count, etc.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory", meta = (DisplayName = "Remove Item (From Handle)", Keywords = "remove item handle"))
	void K2_RemoveItem(const FGameplayInventoryItemSpecHandle& ItemToRemoveHandle, const FGameplayInventoryItemContext& Context);

	/**
	 * Removes an item (from definition) from the inventory.
	 * Will be ignored if the actor doesn't have authority, since the inventory is server-authoritative.
	 *
	 * @param ItemToRemove The item instance to remove.
	 * @param Context Context for the item that stores stack count, etc.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory", meta = (DisplayName = "Remove Item (By Instance)", Keywords = "remove item instance"))
	void K2_RemoveItemByInstance(UGameplayInventoryItemInstance* ItemToRemove, const FGameplayInventoryItemContext& Context);

	/**
	 * Removes an item by its definition from the inventory.
	 * Meaning that all items with the same definition will be removed. No matter the handle.
	 * Will be ignored if the actor doesn't have authority, since the inventory is server-authoritative.
	 * @param Context				Context for the item that stores stack count, etc.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory", meta = (DisplayName = "Consume Item", Keywords = "consume item definition"))
	void K2_ConsumeItem(const FGameplayInventoryItemContext& Context);

	/**
	 * Returns all existing item handles in this inventory.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	TArray<FGameplayInventoryItemSpecHandle> GetInventoryList() const;

	/** Returns all existing item specs in this inventory. */
	TArray<FGameplayInventoryItemSpec>& GetItemSpecs();
	const TArray<FGameplayInventoryItemSpec>& GetItemSpecs() const;
	const TArray<FGameplayInventoryItemSpec> GetItemSpecsInRow(const FGameplayTag& RowTag) const;

	/** Returns the total count of all items in the inventory of the given type. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	int32 GetTotalItemCountByDefinition(UGameplayInventoryItemDefinition* InItemDef) const;

	/** Returns the list of row configs that this inventory manager uses */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	const TArray<UGameplayInventoryRowConfig*>& GetRowConfig() const;

	/** Creates and returns a new item context */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory", meta = (DefaultToSelf = "Instigator"))
	FGameplayInventoryItemContext MakeItemContext(UGameplayInventoryItemDefinition* ItemDefinition, int32 StackCount = -1, AActor* Instigator = nullptr, FGameplayTagContainer ContextTags = FGameplayTagContainer());

	/**
	 * Returns an inventory item spec from a handle.
	 * If the handle is invalid, returns an empty item spec.
	 */
	FGameplayInventoryItemSpec* FindItemSpecFromHandle(const FGameplayInventoryItemSpecHandle& ItemHandle) const;

	/**
	 * Returns an inventory item spec from a definition.
	 * If the definition is invalid, returns an empty item spec.
	 */
	FGameplayInventoryItemSpec* FindItemSpecFromDefinition(UGameplayInventoryItemDefinition* ItemDefinition) const;

	/** Constructs a default FGameplayInventoryItemSpec from a definition */
	virtual FGameplayInventoryItemSpec ConstructInventoryItemSpecFromDefinition(UGameplayInventoryItemDefinition* InItemDef, const FGameplayInventoryItemContext& InContext);

	/** Marks an inventory item spec as dirty */
	void MarkInventoryItemSpecDirty(FGameplayInventoryItemSpec& InSpec);

	/** Creates a new instance of an item, storing it in the item spec. */
	virtual UGameplayInventoryItemInstance* CreateNewInstanceOfItem(FGameplayInventoryItemSpec& InItemSpec, const FGameplayInventoryItemContext& InContext);

	/** Checks if we need to create a new item instance. Takes in count, stacking data, etc. */
	virtual bool ShouldCreateNewInstanceOfItem(const FGameplayInventoryItemSpec& InItemSpec);

	/** Called after an item has been given to the inventory */
	virtual void OnGiveItem(FGameplayInventoryItemSpec& InSpec);

	/** Called after an item has been removed from the inventory */
	virtual void OnRemoveItem(FGameplayInventoryItemSpec& InSpec);

	/** Called after an item has been changed in the inventory */
	virtual void OnChangeItem(FGameplayInventoryItemSpec& InSpec);

	static void OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos);
	static FOnShowDebugInfo OnShowDebugInfoDelegate;

	const TArray<FGameplayInventoryItemSpec>& GetInventoryListRef() const { return InventoryList.Items; }

protected:
	/** Returns the stack count of an item or the context. */ 
	virtual int32 GetStackCount(const int32& StackCount, const FGameplayInventoryItemContext& Context) const;

	/** Broadcasts an inventory change message */
	virtual void BroadcastInventoryChangeMessage(UGameplayInventoryItemInstance* ItemInstance, int32 OldStackCount, int32 NewStackCount);

protected:
	/** The initial inventory row config. */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TArray<TObjectPtr<class UGameplayInventoryRowConfig>> RowConfigs;
	
protected:
	/** Called when the inventory list is replicated */
	UFUNCTION()
	void OnRep_InventoryList();

	/** Timer handle to ensure a valid inventory list */
	FTimerHandle TimerHandle_InventoryList;
	
	/** The actual inventory list */
	UPROPERTY(ReplicatedUsing = OnRep_InventoryList, BlueprintReadOnly, Transient, Category = "Inventory")
	FGameplayInventoryItemContainer InventoryList;
};
