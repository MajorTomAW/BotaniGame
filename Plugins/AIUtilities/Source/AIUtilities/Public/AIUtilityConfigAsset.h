// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/AIUtilityManager.h"
#include "Engine/DataAsset.h"
#include "AIUtilityConfigAsset.generated.h"

/**
 * UAIUtilityConfigAsset 
 */
UCLASS(meta = (DisplayName = "AI Utility Config"))
class AIUTILITIES_API UAIUtilityConfigAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** The default value for the AIUtilityConfig */
	UPROPERTY(EditAnywhere, Category = "AI Utility Config")
	FAIUtilityConfig DefaultConfig;
};
