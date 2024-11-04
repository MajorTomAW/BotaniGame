// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BotaniBotDefinition.generated.h"

/**
 * @class UBotaniBotDefinition
 *
 * A bot character definition describes an AI character to be spawned.
 * It contains a base Type which can be modified by Bot Modifiers to customize the spawned AI Character.
 */
UCLASS(MinimalAPI, BlueprintType, meta = (DisplayName = "Bot Definition"))
class UBotaniBotDefinition : public UPrimaryDataAsset
{
	GENERATED_UCLASS_BODY()

public:
	/** Determines whether this bot should override the default pawn data. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bot Data")
	uint32 bOverrideDefaultPawnData : 1;

	/** The override pawn data to use for this bot if bOverrideDefaultPawnData is true. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bot Data", meta = (EditCondition = "bOverrideDefaultPawnData"))
	TObjectPtr<class UBotaniPawnData> OverridePawnData;

	/** Determines whether this bot should inherit the default abilities from the default pawn data. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bot Data", meta = (EditCondition = "!bOverrideDefaultPawnData"))
	uint32 bInheritDefaultAbilities : 1;

	/** Determines whether this bot should inherit the default tag relationship mappings from the default pawn data. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bot Data", meta = (EditCondition = "!bOverrideDefaultPawnData"))
	uint32 bInheritDefaultTagRelationshipMappings : 1;
	
	
	/** List of modifiers that will customize the character that will be spawned. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Bot Modifiers", meta = (/*TitleProperty = "EditorFriendlyName",*/ ShowOnlyInnerProperties))
	TArray<TObjectPtr<class UBotaniCharacterModifier>> Modifiers;
};
