// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayInventoryTypes.h"
#include "Components/ActorComponent.h"
#include "Spec/GameplayInventoryItemSpec.h"
#include "GameplayInventoryComponent.generated.h"

/**
 * UGameplayInventoryComponent
 *
 * The base inventory component for all actors in the gameplay inventory system.
 */
UCLASS(Abstract, HideCategories = (Object))
class GAMEPLAYINVENTORYSYSTEM_API UGameplayInventoryComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

protected:
	/** Returns true if the owning actor has authority */
	virtual bool IsOwnerActorAuthoritative() const;

	/** Returns the owner as a pawn or tries to get the pawn whatever the owner is. */
	virtual APawn* GetOwnerPawn() const;

	/** Returns the owner as a pawn or tries to get the pawn whatever the owner is. */
	template <typename T>
	T* GetOwnerPawn() const
	{
		return Cast<T>(GetOwnerPawn());
	}
};
