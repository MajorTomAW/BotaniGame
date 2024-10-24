// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BotaniExperienceActionSet.generated.h"

/**
 * UBotaniExperienceActionSet
 *
 * Definition of a set of actions to perform as a part of entering an experience.
 * Used to quickly map out a set of actions to be shared across multiple experiences.
 */
UCLASS(BlueprintType, NotBlueprintable)
class BOTANIGAME_API UBotaniExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UBotaniExperienceActionSet();

	//~ Begin UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~ End UObject interface

	//~ Begin UPrimaryDataAsset interface
#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	//~ End UPrimaryDataAsset interface

public:
	/** List of Game Feature Plugin URL's this experience wants to have active. Should be relative to the project's directory. */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TArray<FString> GameFeaturesToEnable;
	
	/** List of additional actions to perform as this experience is loaded/activated/deactivated/unloaded. */
	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	TArray<TObjectPtr<class UGameFeatureAction>> FeatureActions;
};
