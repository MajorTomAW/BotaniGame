// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayInventoryTypes.h"
#include "Playset.h"
#include "GameplayInventoryItemDefinition.generated.h"

class UGameplayInventoryItemFragment;
/**
 * UGameplayInventoryItemDefinition
 *
 * The base class for all items in the gameplay inventory system.
 */
UCLASS(Abstract, BlueprintType, meta = (PrioritizeCategories = "Item", DisplayName = "Inventory Item Definition"), Config = Game, HideCategories = ("Placement", "Spawning", "Tags"))
class GAMEPLAYINVENTORYSYSTEM_API UGameplayInventoryItemDefinition : public UPlayset
{
	GENERATED_UCLASS_BODY()

public:
	/** Destructor */
	virtual ~UGameplayInventoryItemDefinition() override;

	//~ Begin UPlayset Interface
	virtual const FGameplayTagContainer& GetGameplayTags() const override;
	//~ End UPlayset Interface

	/** Tries to find a fragment of the specified class. */
	UFUNCTION(BlueprintCallable, Category = "Item|Properties", meta = (DeterminesOutputType = "FragmentClass"))
	UGameplayInventoryItemFragment* GetItemFragment(TSubclassOf<UGameplayInventoryItemFragment> FragmentClass) const;

	template <typename T>
	T* GetItemFragment() const
	{
		return Cast<T>(GetItemFragment(T::StaticClass()));
	}


public:
	/******************************************************************************************************
	 * ITEM PROPERTIES
	 ******************************************************************************************************/

	/** The stacking data for this item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Properties")
	FGameplayInventoryItemStackingData StackingData;

	/** A list of additional requirements that must be met to use the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Item|Properties", meta = (DisplayName = "Requirements", ShowOnlyInnerProperties))
	TArray<TObjectPtr<class UGameplayInventoryRequirement>> ItemRequirements;

	/** The item tags that are associated with this item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Properties", meta = (DisplayName = "Item Tags"))
	FGameplayTagContainer ItemTags;

	/** The class of the item instance that is created when added to an inventory. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Instancing", meta = (DisplayName = "Override Item Instance"))
	TSubclassOf<UGameplayInventoryItemInstance> ItemInstanceClass;

	/** The equipment definition of this item (if equippable). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment")
	TObjectPtr<class UGameplayEquipmentDefinition> EquipmentDefinition;

	/** List of additional fragments that can be applied to the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Fragments", meta = (DisplayName = "Item Fragments", TitleProperty = EditorFriendlyName, ShowOnlyInnerProperties))
	TArray<TObjectPtr<class UGameplayInventoryItemFragment>> ItemFragments;

public:
	/** Returns the item instance class, if set to null, the default item instance class is used. */
	virtual UClass* GetInstanceClass() const;

protected:
#if WITH_EDITOR
	//~ Begin UObject Interface
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	//~ End UObject Interface
#endif

protected:
	UPROPERTY(Config)
	TSubclassOf<UGameplayInventoryItemInstance> DefaultInstanceClass;
};
