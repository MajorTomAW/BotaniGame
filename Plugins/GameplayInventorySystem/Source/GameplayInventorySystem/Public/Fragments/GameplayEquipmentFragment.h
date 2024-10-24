// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayInventorySystemFragment.h"
#include "Spec/GameplayEquipmentSpec.h"
#include "GameplayEquipmentFragment.generated.h"

struct FGameplayEquipmentSpec;

/**
 * UGameplayEquipmentFragment
 *
 * A single fragment of equipment that gets activated when equipped.
 */
UCLASS(Abstract, CollapseCategories, MinimalAPI, meta = (ShortToolTip = "A single fragment of equipment that gets activated when equipped."))
class UGameplayEquipmentFragment : public UGameplayInventorySystemFragment
{
	GENERATED_UCLASS_BODY()
	friend class UGameplayEquipmentManager;
	friend class UGameplayEquipmentDefinition;

public:
	/**
	 * Checks if this fragment can be applied to the specified item definition.
	 * @param Context			The context of the item.
	 * @param EquipmentSpec			The item spec of the item.
	 * @param OutBlockingTags	[OUT] Any tags that are blocking the fragment from being applied.
	 * @return			Whether the fragment can be applied.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Fragment|Equipment", meta = (DisplayName = "Can Apply Equipment Fragment"))
	GAMEPLAYINVENTORYSYSTEM_API bool K2_CanApplyEquipmentFragment(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, FGameplayTagContainer& OutBlockingTags) const;

	/**
	 * Checks if this fragment can be applied to the specified item definition.
	 * @param Context			The context of the item.
	 * @param EquipmentSpec			The item spec of the item.
	 * @param OutBlockingTags	[OUT] Any tags that are blocking the fragment from being applied.
	 * @return			Whether the fragment can be applied.
	 */
	GAMEPLAYINVENTORYSYSTEM_API virtual bool CanApplyEquipmentFragment(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, FGameplayTagContainer* OutBlockingTags) const;

	/**
	 * Called when the instance has been created and replicated.
	 * @param Context			The context of the item.
	 * @param EquipmentSpec			The item spec of the item.
	 * @param EquipmentInstance	The equipment instance that was created.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Fragment|Equipment", meta = (DisplayName = "On Equipment Instance Created"))
	GAMEPLAYINVENTORYSYSTEM_API void K2_OnEquipmentInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, class UGameplayEquipmentInstance* EquipmentInstance) const;

	/**
	 * Called when the instance has been created and replicated.
	 * @param Context			The context of the item.
	 * @param EquipmentSpec			The item spec of the item.
	 * @param EquipmentInstance	The equipment instance that was created.
	 */
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnEquipmentInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, class UGameplayEquipmentInstance* EquipmentInstance) const;

	/**
	 * Called when the instance has been destroyed.
	 * @param Context			The context of the item.
	 * @param EquipmentSpec			The item spec of the item.
	 * @param EquipmentInstance	The equipment instance that was destroyed.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Fragment|Equipment", meta = (DisplayName = "On Equipment Instance Destroyed"))
	GAMEPLAYINVENTORYSYSTEM_API void K2_OnEquipmentInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, class UGameplayEquipmentInstance* EquipmentInstance) const;

	/**
	 * Called when the instance has been destroyed.
	 * @param Context			The context of the item.
	 * @param EquipmentSpec			The item spec of the item.
	 * @param EquipmentInstance	The equipment instance that was destroyed.
	 */
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnEquipmentInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, class UGameplayEquipmentInstance* EquipmentInstance) const;

public:
	//~ Begin UGameplayInventorySystemFragment Interface
	GAMEPLAYINVENTORYSYSTEM_API virtual bool CanApplyInventoryFragment(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, FGameplayTagContainer* OutBlockingTags) const override;
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const override;
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const override;
	//~ End UGameplayInventorySystemFragment Interface

protected:
	UFUNCTION(BlueprintCallable, Category = "Fragment|Equipment")
	static FGameplayEquipmentSpec& GetEquipmentSpecFromHandle(const FGameplayInventoryItemSpecHandle& Handle, const FGameplayInventoryItemContext& Context);
};
