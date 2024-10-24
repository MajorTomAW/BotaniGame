// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BotaniPawnData.generated.h"

/**
 * UBotaniPawnData
 *
 * Non-mutable data asset that holds data for a pawn.
 */
UCLASS(BlueprintType, Const, meta = (DisplayName = "Botani Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class BOTANIGAME_API UBotaniPawnData : public UPrimaryDataAsset
{
	GENERATED_UCLASS_BODY()

public:
	/** Class to instantiate for this pawn (should usually derive from ABotaniCharacter). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	TSubclassOf<APawn> PawnClass;

	/** Mapping of ability tags to use for actions taken by this pawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<class UBotaniAbilityTagRelationshipMapping> TagRelationshipMapping;

	/** List of ability sets to grant to this pawn's ability system. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TObjectPtr<class UBotaniAbilitySet>> AbilitySets;

	/** Input configuration used by player-controlled pawns to create input mappings and bind input actions. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UBotaniInputConfig> InputConfig;

	/** Default camera mode used by player-controlled pawns. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<class UBotaniCameraMode> DefaultCameraMode;
};
