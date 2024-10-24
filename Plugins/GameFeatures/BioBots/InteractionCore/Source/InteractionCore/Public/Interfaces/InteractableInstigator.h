// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInstigator.generated.h"


struct FBotaniInteractionOption;
struct FBotaniInteractionQuery;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UInteractableInstigator : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for interactable instigators.
 * Implementing this interface allows you to add an arbitrator to the interaction process.
 * For example, we may want to present the user with a menu to pick which interaction they want to perform.
 * This will allow us to take the multiple matches (Assuming your IInteractableTarget subclass generates more than one option).
 */
class IInteractableInstigator
{
	GENERATED_BODY()

public:
	/** Will be called if there are more than one InteractOptions that need to be decided on. */
	virtual FBotaniInteractionOption ChooseBestInteractionOption(const FBotaniInteractionQuery& InteractQuery, const TArray<FBotaniInteractionOption>& InteractOptions) = 0;
};
