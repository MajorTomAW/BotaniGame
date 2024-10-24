// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayInventorySystemFragment.h"
#include "GameplayInventoryTypes.h"
#include "Spec/GameplayInventoryItemSpecHandle.h"
#include "UObject/Object.h"
#include "GameplayInventoryItemFragment.generated.h"

struct FGameplayInventoryItemSpec;
struct FGameplayInventoryItemContext;

/**
 * UGameplayInventoryItemFragment
 *
 * A single fragment of an item in the gameplay inventory system.
 * Can be used to extend the functionality of an item definition.
 */
UCLASS(Abstract, DisplayName = "Inventory Item Fragment", ClassGroup = "Inventory System", CollapseCategories, MinimalAPI, meta = (ShortTooltip = "A single fragment of an item in the gameplay inventory system."))
class UGameplayInventoryItemFragment : public UGameplayInventorySystemFragment
{
	GENERATED_UCLASS_BODY()
	friend class UGameplayInventoryManager;
	friend class UGameplayInventoryItemDefinition;

public:
	/**
	 * Checks if this fragment can be applied to the specified item definition.
	 * @param Context		The context of the item.
	 * @param ItemSpec		The item spec of the item.
	 * @param OutBlockingTags	[OUT] Any tags that are blocking the fragment from being applied.
	 * @returns Whether the fragment can be applied.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Fragment|Item", meta = (DisplayName = "Can Apply Item Fragment"))
	GAMEPLAYINVENTORYSYSTEM_API bool K2_CanApplyItemFragment(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, FGameplayTagContainer& OutBlockingTags) const;

	/**
	 * Checks if this fragment can be applied to the specified item definition.
	 * @param Context		The context of the item.
	 * @param ItemSpec		The item spec of the item.
	 * @param OutBlockingTags	[OUT] Any tags that are blocking the fragment from being applied.
	 * @returns Whether the fragment can be applied.
	 */
	GAMEPLAYINVENTORYSYSTEM_API virtual bool CanApplyItemFragment(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, FGameplayTagContainer* OutBlockingTags) const;

	/**
	 * Called when the instance has been created and replicated.
	 * @param Context	The context of the item.
	 * @param ItemSpec	The item spec of the item.
	 * @param ItemInstance	The item instance that was created.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Fragment|Item", meta = (DisplayName = "On Item Instance Created"))
	GAMEPLAYINVENTORYSYSTEM_API void K2_OnItemInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, class UGameplayInventoryItemInstance* ItemInstance) const;

	/**
	 * Called when the instance has been created and replicated.
	 * @param Context	The context of the item.
	 * @param ItemSpec	The item spec of the item.
	 * @param ItemInstance	The item instance that was created.
	 */
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnItemInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, class UGameplayInventoryItemInstance* ItemInstance) const;

	/**
	 * Called when the instance has been destroyed.
	 * @param Context	The context of the item.
	 * @param ItemSpec	The item spec of the item.
	 * @param ItemInstance	The item instance that was destroyed (Is about to be destroyed).
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Fragment|Item", meta = (DisplayName = "On Item Instance Destroyed"))
	GAMEPLAYINVENTORYSYSTEM_API void K2_OnItemInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, class UGameplayInventoryItemInstance* ItemInstance) const;

	/**
	 * Called when the instance has been destroyed.
	 * @param Context	The context of the item.
	 * @param ItemSpec	The item spec of the item.
	 * @param ItemInstance	The item instance that was destroyed (Is about to be destroyed).
	 */
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnItemInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, class UGameplayInventoryItemInstance* ItemInstance) const;

public:
	//~ Begin UGameplayInventorySystemFragment Interface
	GAMEPLAYINVENTORYSYSTEM_API virtual bool CanApplyInventoryFragment(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, FGameplayTagContainer* OutBlockingTags) const override;
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const override;
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const override;
	//~ End UGameplayInventorySystemFragment Interface

protected:
	UFUNCTION(BlueprintCallable, Category = "Fragment|Equipment")
	static FGameplayInventoryItemSpec& GetItemSpecFromHandle(const FGameplayInventoryItemSpecHandle& Handle, const FGameplayInventoryItemContext& Context);
};
