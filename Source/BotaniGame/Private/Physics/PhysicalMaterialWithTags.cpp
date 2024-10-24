// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Physics/PhysicalMaterialWithTags.h"

UPhysicalMaterialWithTags::UPhysicalMaterialWithTags(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Tags = FGameplayTagContainer::EmptyContainer;
}