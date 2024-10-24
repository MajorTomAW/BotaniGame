// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GameplayInventorySystemFragment.generated.h"


struct FGameplayInventoryItemSpecHandle;
struct FGameplayInventoryItemSpecHandleHandle;
struct FGameplayInventoryItemContext;
/**
 * UGameplayInventorySystemFragment
 *
 * A single fragment of an item in the gameplay inventory system.
 * Can be applied to an item definition or an equipment definition to extend its functionality. 
 */
UCLASS(EditInlineNew, BlueprintType, Blueprintable, Abstract, DisplayName = "Inventory Item Fragment", ClassGroup = "Inventory System", CollapseCategories, MinimalAPI, meta = (ShortTooltip = "A single fragment of an item in the gameplay inventory system."))
class UGameplayInventorySystemFragment : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/**
	 * Checks if this fragment can be applied to the specified item definition.
	 * @param Context		The context of the item.
	 * @param Handle		The item spec handle of the item.
	 * @param OutBlockingTags	[OUT] Any tags that are blocking the fragment from being applied.
	 * @returns Whether the fragment can be applied.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Fragment", meta = (DisplayName = "Can Apply Fragment"))
	GAMEPLAYINVENTORYSYSTEM_API bool K2_CanApplyInventoryFragment(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, FGameplayTagContainer& OutBlockingTags) const;

	/**
	 * Checks if this fragment can be applied to the specified item definition.
	 * @param Context		The context of the item.
	 * @param Handle		The item spec handle of the item.
	 * @param OutBlockingTags	[OUT] Any tags that are blocking the fragment from being applied.
	 * @returns Whether the fragment can be applied.
	 */
	GAMEPLAYINVENTORYSYSTEM_API virtual bool CanApplyInventoryFragment(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, FGameplayTagContainer* OutBlockingTags) const;

	/**
	 * Called when the instance has been created and replicated.
	 * @param Context	The context of the item.
	 * @param Handle	The item spec of the item.
	 * @param Instance	The item instance that was created.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Fragment", meta = (DisplayName = "On Instance Created"))
	GAMEPLAYINVENTORYSYSTEM_API void K2_OnInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const;

	/**
	 * Called when the instance has been created and replicated.
	 * @param Context	The context of the item.
	 * @param Handle	The item spec of the item.
	 * @param Instance	The item instance that was created.
	 */
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const;

	/**
	 * Called when the instance has been destroyed.
	 * @param Context	The context of the item.
	 * @param Handle	The item spec of the item.
	 * @param Instance	The item instance that was destroyed (Is about to be destroyed).
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Fragment", meta = (DisplayName = "On Instance Destroyed"))
	GAMEPLAYINVENTORYSYSTEM_API void K2_OnInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const;

	/**
	 * Called when the instance has been destroyed.
	 * @param Context	The context of the item.
	 * @param Handle	The item spec of the item.
	 * @param Instance	The item instance that was destroyed (Is about to be destroyed).
	 */
	GAMEPLAYINVENTORYSYSTEM_API virtual void OnInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const;

protected:
	/** A list of additional requirements that must be met to apply this fragment. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Fragment", meta = (DisplayName = "Requirements", ShowOnlyInnerProperties))
	TArray<TObjectPtr<class UGameplayInventoryRequirement>> FragmentRequirements;

protected:
#if WITH_EDITORONLY_DATA
	/** For editor use only: The display name of the requirement */
	UPROPERTY(VisibleDefaultsOnly, Transient, Category = AlwaysHidden, meta = (EditCondition = "false", EditConditionHides))
	FString EditorFriendlyName;
#endif

#if WITH_EDITOR
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory", meta = (DisplayName = "Get Display Name"))
	GAMEPLAYINVENTORYSYSTEM_API FString GetEditorFriendlyName() const;

	//~ Begin UObject Interface
	virtual GAMEPLAYINVENTORYSYSTEM_API void PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext) override;
	virtual GAMEPLAYINVENTORYSYSTEM_API void PostCDOCompiled(const FPostCDOCompiledContext& Context) override;
	virtual GAMEPLAYINVENTORYSYSTEM_API EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	//~ End UObject Interface
#endif
};
