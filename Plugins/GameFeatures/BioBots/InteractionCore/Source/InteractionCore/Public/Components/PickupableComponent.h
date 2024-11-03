// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagStackContainer.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Pickupable.h"
#include "PickupableComponent.generated.h"

struct FPickupTemplateData;

/**
 * UPickupableComponent
 *
 * A component that holds data about a pickupable object.
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent), HideCategories = ("Activation", "Tags", "AssetUserData", "Navigation"))
class INTERACTIONCORE_API UPickupableComponent : public UActorComponent, public IPickupable
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin IPickupable Interface
	virtual TArray<FPickupTemplateData> GetItemList() const override;
	//~ End IPickupable Interface

protected:
	/** The list of items associated with this device/trap. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	TArray<FPickupTemplateData> ItemList;

private:
	/** Authority-only list of tag stacks for each item. */
	UPROPERTY()
	TMap<int32, FGameplayTagStackContainer> ItemInstigatorTagStacks;
};
