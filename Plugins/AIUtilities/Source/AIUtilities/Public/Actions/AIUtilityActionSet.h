// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AIUtilityActionSet.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "AI Utility Action Set"))
class AIUTILITIES_API UAIUtilityActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Returns the list of actions in this set. */
	TArray<class UAIUtilityActionDefinition*> GetActions() const { return Actions; }

protected:
	/** List of actions in this set. */
	UPROPERTY(EditAnywhere, Category = "AI Utility Action Set", Instanced)
	TArray<TObjectPtr<class UAIUtilityActionDefinition>> Actions;
};
