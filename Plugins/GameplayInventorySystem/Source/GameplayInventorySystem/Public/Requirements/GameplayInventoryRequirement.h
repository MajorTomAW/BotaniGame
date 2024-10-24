// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayInventoryRequirement.generated.h"

class UGameplayInventoryItemDefinition;
struct FGameplayInventoryItemSpec;
struct FGameplayInventoryItemContext;

/**
 * UGameplayInventoryRequirement
 *
 * Requirements are used to define the conditions that must be met to perform an inventory-related action.
 * Useful for defining requirements for crafting, looting, and using items.
 *
 * Inventory Requirements live within an inventory or equipment item (which is typically a data-only asset).
 */
UCLASS(Abstract, Const, EditInlineNew, CollapseCategories, BlueprintType, Blueprintable)
class GAMEPLAYINVENTORYSYSTEM_API UGameplayInventoryRequirement : public UObject
{
	GENERATED_BODY()

public:
	UGameplayInventoryRequirement();

	/**
	 * Checks if the inventory item can perform the action.
	 * Could be used to determine whether an item can be crafted, collected, or used.
	 *
	 * @param ItemSpec		The item spec of the inventory item.
	 * @param Context		The context of the inventory item.
	 * @return If the inventory item can perform the action
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual bool CanInventoryItemPerformAction(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context) const;

protected:
	/** Returns the inventory item that owns this requirement (the outermost object) */
	template <typename T>
	T* GetOwningItem() const
	{
		return Cast<T>(GetOuter());
	}

	/** Returns the inventory item that owns this requirement (the outermost object) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory", meta = (DeterminesOutputType = "Class"))
	UGameplayInventoryItemDefinition* GetOwningItemDef(UGameplayInventoryItemDefinition* Class) const;

	/**
	 * Checks if the inventory item can perform the action.
	 * Could be used to determine whether an item can be crafted, collected, or used.
	 *
	 * @param ItemSpec		The item spec of the inventory item.
	 * @param Context		The context of the inventory item.
	 * @return If the inventory item can perform the action
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory", meta = (DisplayName = "Can Inventory Item Perform Action"))
	bool CanInventoryItemPerformActionBP(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context) const;

/*
#if WITH_EDITORONLY_DATA
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory", meta = (DisplayName = "Get Display Name"))
	FString GetEditorFriendlyName() const;
#endif

#if WITH_EDITOR
	//~ Begin UObject Interface
	virtual void PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext) override;
	//~ End UObject Interface
#endif

protected:
#if WITH_EDITORONLY_DATA
	/** For editor use only: The display name of the requirement #1#
	UPROPERTY(VisibleDefaultsOnly, Transient, Category = AlwaysHidden, meta = (EditCondition = "false", EditConditionHides))
	FString EditorFriendlyName;
#endif*/
};
