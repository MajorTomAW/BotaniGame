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
	/** List of modifiers that will customize the character that will be spawned. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Bot Modifiers", meta = (/*TitleProperty = "EditorFriendlyName",*/ ShowOnlyInnerProperties))
	TArray<TObjectPtr<class UBotaniCharacterModifier>> Modifiers;
};
