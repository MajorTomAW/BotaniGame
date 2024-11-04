// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pickupable.generated.h"

class UGameplayInventoryItemDefinition;

/**
 * FPickupTemplateData
 *
 * A struct that holds data for a pickup template.
 */
USTRUCT(BlueprintType)
struct FPickupTemplateData
{
	GENERATED_BODY()

public:
	/** The item definition to pick up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	TObjectPtr<UGameplayInventoryItemDefinition> PickupItem;

	/** The amount of the item to pick up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = 1, UIMin = 1))
	int32 PickupQuantity = 1;

	/** The instigator handle of the pickup */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	int32 PickupInstigatorHandle = INDEX_NONE;

	/** The item variant GUID */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (DisplayName = "Item Variang Guid"))
	FGuid ItemVariantGUID;
};

/**
 * UPickupable
 *
 * Common interface used for pickupable objects.
 */
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UPickupable : public UInterface
{
	GENERATED_BODY()
};

class IPickupable
{
	GENERATED_BODY()

public:
	/** Returns the list of items associated with this device/trap. */
	UFUNCTION(BlueprintCallable, Category = "Pickupable")
	virtual TArray<FPickupTemplateData> GetItemList() const = 0;
};

/**
 * UPickupableStatics
 *
 * A collection of functions to use in conjunction with pickupable objects.
 */
UCLASS()
class UPickupableStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Returns the first pickup template that lives on the potential pickupable object. */
	UFUNCTION(BlueprintPure, Category = "Pickupable")
	static TScriptInterface<IPickupable> GetFirstPickupable(AActor* Actor);
};
