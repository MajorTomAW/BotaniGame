// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PickupItemIcon.generated.h"

/**
 * UPickupItemIcon
 *
 * A single icon that represents a pickup item in the world.
 * Your pickup can have multiple icons that represent it.
 * Icons can be static meshes, textures, materials, and so on.
 */
UCLASS(MinimalAPI, Blueprintable, BlueprintType, EditInlineNew, Config = Game, Abstract, meta = (DisplayName = "Pickup Icon"))
class UPickupItemIcon : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/**
	 * Called to apply this icon to the specified pickup proxy actor.
	 *
	 * Typically used to spawn a preview mesh or set a material on the pickup.
	 */
	GAMEPLAYINVENTORYSYSTEM_API virtual void ApplyPickupIcon(class APickupProxyActor* InProxy, const class UGameplayInventoryItemDefinition* InItemDef);
	

	/**
	 * Called to apply this icon to the specified pickup proxy actor.
	 *
	 * Typically used to spawn a preview mesh or set a material on the pickup.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup", meta = (DisplayName = "Apply Pickup Icon"))
	GAMEPLAYINVENTORYSYSTEM_API void K2_ApplyPickupIcon(class APickupProxyActor* InProxy, const class UGameplayInventoryItemDefinition* InItemDef);
};
