// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Requirements/InventoryRequirement_HasAbilitySystem.h"

UInventoryRequirement_HasAbilitySystem::UInventoryRequirement_HasAbilitySystem()
{
}

bool UInventoryRequirement_HasAbilitySystem::CanInventoryItemPerformAction(const FGameplayInventoryItemSpec& ItemSpec, const FGameplayInventoryItemContext& Context) const
{
	if (!Super::CanInventoryItemPerformAction(ItemSpec, Context))
	{
		return false;
	}

	// Check if the ability system is valid
	return true; //@TODO: Implement this
}

/*
#if WITH_EDITOR
FString UInventoryRequirement_HasAbilitySystem::GetEditorFriendlyName_Implementation() const
{
	return TEXT("Has Ability System");
}
#endif*/