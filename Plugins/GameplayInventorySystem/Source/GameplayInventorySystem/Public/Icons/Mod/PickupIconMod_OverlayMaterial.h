// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Icons/PickupItemIcon.h"
#include "PickupIconMod_OverlayMaterial.generated.h"

/**
 * UPickupIconMod_OverlayMaterial
 *
 * Attempts to apply an overlay material to the icon.
 */
UCLASS(meta = (DisplayName = "Modifier: Overlay Material"), MinimalAPI)
class UPickupIconMod_OverlayMaterial : public UPickupIconModifier
{
	GENERATED_BODY()

public:
	//~ Begin UPickupIconModifier Interface
	GAMEPLAYINVENTORYSYSTEM_API virtual void ApplyModifier(UPickupItemIcon* Outer, USceneComponent* SpawnedIcon, APickupProxyActor* PickupProxy, const UGameplayInventoryItemDefinition* InItemDef) override;
	//~ End UPickupIconModifier Interface

protected:
	/** The overlay material to apply to the icon. */
	UPROPERTY(EditAnywhere, Category = "Modifier")
	TSoftObjectPtr<UMaterialInterface> OverlayMaterial;

	/** Additional scalar parameters to set on the overlay material. */
	UPROPERTY(EditAnywhere, Category = "Modifier")
	TMap<FName, float> ScalarParameters;

	/** Additional vector parameters to set on the overlay material. */
	UPROPERTY(EditAnywhere, Category = "Modifier")
	TMap<FName, FLinearColor> VectorParameters;
};
