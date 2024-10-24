// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "BotaniAbilityTagRelationshipMapping.generated.h"

/**
 * FBotaniAbilityTagRelationship
 *
 * Struct that defines the relationship between different ability tags
 */
USTRUCT(BlueprintType)
struct FBotaniAbilityTagRelationShip
{
	GENERATED_BODY()

	FBotaniAbilityTagRelationShip()
		: bExactAbilityTagMatchOnly(false)
	{
	}

public:
	/** The tag that this container relationship is about. Single tag, but abilities can have multiple of these */
	UPROPERTY(EditDefaultsOnly, Category = "Relationship", meta = (Categories = "Gameplay.Action"))
	FGameplayTag AbilityTag;

	/** Whether the ability tag must match exactly */
	UPROPERTY(EditDefaultsOnly, Category = "Relationship")
	uint8 bExactAbilityTagMatchOnly:1;

	/** The other ability tags that will be blocked by any ability using this tag */
	UPROPERTY(EditDefaultsOnly, Category = "Relationship")
	FGameplayTagContainer AbilityTagsToBlock;

	/** The other ability tags that will be canceled by any ability using this tag */
	UPROPERTY(EditDefaultsOnly, Category = "Relationship")
	FGameplayTagContainer AbilityTagsToCancel;

	/** If an ability has the tag, this is implicitly added to the activation-required tags of the ability */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationRequiredTags;

	/** If an ability has the tag, this is implicitly added to the activation blocked tags of the ability */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationBlockedTags;
};

/**
 * UBotaniAbilityTagRelationshipMapping
 */
UCLASS()
class BOTANIGAME_API UBotaniAbilityTagRelationshipMapping : public UDataAsset
{
	GENERATED_UCLASS_BODY()

public:
	/** Given a set of ability tags, parse the tag relationship and fill out tags to block and cancel */
	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const;

	/** Given a set of ability tags, add additional required and blocking tags */
	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const;

	/** Returns true if the specified ability tags are canceled by the passed in action tag */
	bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;

private:
	/** The list of relationships between different gameplay tags (which ones block or cancel others) */
	UPROPERTY(EditDefaultsOnly, Category = "Relationships", meta = (TitleProperty = "AbilityTag"))
	TArray<FBotaniAbilityTagRelationShip> AbilityTagRelationships;
};
