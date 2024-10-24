// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PickupItemIcon.h"
#include "PickupIcon_StaticMesh.generated.h"

/**
 * UPickupIcon_StaticMesh
 *
 * An icon representing a static mesh pickup item.
 */
UCLASS(meta = (DisplayName = "Icon: Static Mesh"))
class UPickupIcon_StaticMesh : public UPickupItemIcon
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UPickupItemIcon Interface
	virtual void ApplyPickupIcon(APickupProxyActor* InProxy, const UGameplayInventoryItemDefinition* InItemDef) override;
	//~ End UPickupItemIcon Interface

protected:
	/** The static mesh to display in the icon. */
	UPROPERTY(EditAnywhere, Category = "Icon")
	TSoftObjectPtr<UStaticMesh> StaticMesh;

	/** The relative transform of the static mesh in the icon. */
	UPROPERTY(EditAnywhere, Category = "Icon")
	FTransform RelativeTransform;

	/** List of modifiers to apply to the icon. */
	UPROPERTY(EditAnywhere, Instanced, Category = "Icon|Modifiers", meta = (TitleProperty = "GetName", ShowOnlyInnerProperties))
	TArray<TObjectPtr<class UPickupIconModifier>> Modifiers;
};
