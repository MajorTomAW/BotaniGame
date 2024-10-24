// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/InteractableTarget.h"
#include "Inventory/Pickup/BotaniPickupProxy.h"
#include "WorldCollectableProxy.generated.h"

/**
 * AWorldCollectableProxy
 *
 * A proxy actor that can be used to represent a collectable item in the world.
 */
UCLASS(Abstract, Blueprintable, MinimalAPI)
class AWorldCollectableProxy : public ABotaniPickupProxy, public IInteractableTarget
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin IInteractableTarget Interface
	INTERACTIONCORE_API virtual void GatherInteractionOptions(const FBotaniInteractionQuery& InteractionQuery, FBotaniInteractionOptionBuilder& OptionBuilder) override;
	//~ End IInteractableTarget Interface
	
protected:
	/** Determines the interaction option. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collectable")
	FBotaniInteractionOption InteractionOption;
};
