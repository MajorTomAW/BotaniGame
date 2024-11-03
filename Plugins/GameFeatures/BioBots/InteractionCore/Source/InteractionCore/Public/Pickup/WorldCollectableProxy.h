// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/InteractableTarget.h"
#include "Interfaces/Pickupable.h"
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

	//~ Begin APickupProxyActor Interface
	virtual void OnPickupCollisionBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	//~ End APickupProxyActor Interface
	
protected:
	/** Determines the interaction option. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collectable")
	FBotaniInteractionOption InteractionOption;

	/** The pickupable component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collectable")
	TObjectPtr<class UPickupableComponent> PickupableComponent;
};
