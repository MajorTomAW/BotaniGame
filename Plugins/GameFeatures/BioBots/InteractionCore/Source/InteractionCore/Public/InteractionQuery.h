// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "InteractionQuery.generated.h"

/** A query request that gets send to an interactable target. */
USTRUCT(BlueprintType)
struct FBotaniInteractionQuery
{
	GENERATED_BODY()

public:
	/** The requesting pawn. */
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TWeakObjectPtr<AActor> RequestingAvatar;

	/** Allow us to specify a controller - does not need to match the owner of the requesting avatar. */
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TWeakObjectPtr<AController> RequestingController;

	/** A generic UObject to shove in extra data required for the interaction */
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TWeakObjectPtr<UObject> OptionalObjectData;
};
