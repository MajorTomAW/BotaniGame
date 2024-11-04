// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Interfaces/Pickupable.h"


// Add default functionality here for any IPickupable functions that are not pure virtual.
TScriptInterface<IPickupable> UPickupableStatics::GetFirstPickupable(AActor* Actor)
{
	// If the actor is directly pickupable, return it
	TScriptInterface<IPickupable> PickupableActor(Actor);
	if (PickupableActor)
	{
		return PickupableActor;
	}

	// Otherwise, try finding a pickupable component
	TArray<UActorComponent*> PickupableComponents = Actor ? Actor->GetComponentsByInterface(UPickupable::StaticClass()) : TArray<UActorComponent*>();
	if (PickupableComponents.Num() > 0)
	{
		return TScriptInterface<IPickupable>(PickupableComponents[0]);
	}

	return nullptr;
}
