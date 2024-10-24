// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BotaniExperienceDefinition.generated.h"

class UBotaniPawnData;
class UGameFeatureAction;

/**
 * UBotaniExperienceDefinition
 *
 * The very base definition of an experience used by this project.
 */
UCLASS(BlueprintType, Const)
class BOTANIGAME_API UBotaniExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UBotaniExperienceDefinition();

	
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

	/** Default pawn data to use for player-controlled pawns. */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TObjectPtr<const UBotaniPawnData> DefaultPawnData;

	/** List of additional actions to perform as this experience is loaded/activated/deactivated/unloaded. */
	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	TArray<TObjectPtr<UGameFeatureAction>> FeatureActions;

	/** List of additional action sets to compose into this experience. */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TArray<TObjectPtr<class UBotaniExperienceActionSet>> FeatureActionSets;
};
