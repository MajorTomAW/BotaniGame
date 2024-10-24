// Copyright © 2024 Botanibots Team. All rights reserved.


#include "GameFeatures/Data/BotaniAbilityTagRelationshipMapping.h"

UBotaniAbilityTagRelationshipMapping::UBotaniAbilityTagRelationshipMapping(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBotaniAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FBotaniAbilityTagRelationShip& Tags = AbilityTagRelationships[i];
		if (AbilityTags.HasTag(Tags.AbilityTag))
		{
			if (OutTagsToBlock)
			{
				OutTagsToBlock->AppendTags(Tags.AbilityTagsToBlock);
			}

			if (OutTagsToCancel)
			{
				OutTagsToCancel->AppendTags(Tags.AbilityTagsToCancel);
			}
		}
	}
}

void UBotaniAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FBotaniAbilityTagRelationShip& Tags = AbilityTagRelationships[i];
		if (AbilityTags.HasTag(Tags.AbilityTag))
		{
			if (OutActivationRequired)
			{
				OutActivationRequired->AppendTags(Tags.ActivationRequiredTags);
			}

			if (OutActivationBlocked)
			{
				OutActivationBlocked->AppendTags(Tags.ActivationBlockedTags);
			}
		}
	}
}

bool UBotaniAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const
{
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FBotaniAbilityTagRelationShip& Tags = AbilityTagRelationships[i];

		if (Tags.AbilityTag == ActionTag && Tags.AbilityTagsToCancel.HasAny(AbilityTags))
		{
			return true;
		}
	}

	return false;
}
