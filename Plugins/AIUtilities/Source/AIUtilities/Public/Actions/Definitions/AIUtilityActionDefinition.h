// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIUtilityQueryContext.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AIUtilityActionDefinition.generated.h"

/**
 * 
 */
UCLASS(EditInlineNew, meta = (DisplayName = "AI Utility Action Definition"))
class AIUTILITIES_API UAIUtilityActionDefinition : public UObject
{
	GENERATED_BODY()

public:
	/** Optional description of this action. (For debugging purposes) */
	UPROPERTY(EditAnywhere, Category = "Action Definition")
	FString Description;

	/** Gameplay Tag used to identify this action. */
	UPROPERTY(EditAnywhere, Category = "Action Definition", meta = (Categories = "AI.Utility.Action"))
	FGameplayTag ActionTag;

	/**
	 * Optional parameters to pass to the action.
	 * Usually actions get their data from the query context, but this allows for more specific data to be passed.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Action Definition")
	TMap<FName, FAIUtilityParameter> OptionalParameters;

	/** The re-weighting factor for this action to be applied to the final score. */
	UPROPERTY(EditAnywhere, Category = "Action Definition", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float Weight = 1.0f;

	/** Gameplay Tags required for this action to be considered. */
	UPROPERTY(EditAnywhere, Category = "Action Definition")
	FGameplayTagContainer RequiredTags;

	/** Gameplay Tags that will prevent this action from being considered. */
	UPROPERTY(EditAnywhere, Category = "Action Definition")
	FGameplayTagContainer BlockingTags;
};
