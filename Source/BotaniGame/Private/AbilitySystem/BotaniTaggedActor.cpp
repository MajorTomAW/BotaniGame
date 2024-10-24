// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/BotaniTaggedActor.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniTaggedActor)

ABotaniTaggedActor::ABotaniTaggedActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ABotaniTaggedActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(StaticGameplayTags);
}

#if WITH_EDITOR
bool ABotaniTaggedActor::CanEditChange(const FProperty* InProperty) const
{
	// Prevent editing of the other tags property
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AActor, Tags))
	{
		return false;
	}

	return Super::CanEditChange(InProperty);
}
#endif