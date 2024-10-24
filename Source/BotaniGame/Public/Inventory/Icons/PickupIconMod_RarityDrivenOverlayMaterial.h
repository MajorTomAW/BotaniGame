// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Icons/Mod/PickupIconMod_OverlayMaterial.h"
#include "PickupIconMod_RarityDrivenOverlayMaterial.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Overlay Material (Rarity Driven)"))
class UPickupIconMod_RarityDrivenOverlayMaterial : public UPickupIconMod_OverlayMaterial
{
	GENERATED_BODY()

protected:
	//~ Begin UPickupIconModifier Interface
	virtual void ApplyModifier(UPickupItemIcon* Outer, USceneComponent* SpawnedIcon, APickupProxyActor* PickupProxy, const UGameplayInventoryItemDefinition* InItemDef) override;
	//~ End UPickupIconModifier Interface
};
