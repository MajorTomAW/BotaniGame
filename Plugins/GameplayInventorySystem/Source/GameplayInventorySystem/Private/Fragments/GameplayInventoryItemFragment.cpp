// Copyright © 2024 MajorT. All rights reserved.


#include "Fragments/GameplayInventoryItemFragment.h"

#include "Components/GameplayInventoryManager.h"
#include "Instance/GameplayInventoryItemInstance.h"
#include "Library/InventorySystemBlueprintLibrary.h"
#include "Requirements/GameplayInventoryRequirement.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#include "UObject/ObjectSaveContext.h"
#endif

UGameplayInventoryItemFragment::UGameplayInventoryItemFragment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


FGameplayInventoryItemSpec& UGameplayInventoryItemFragment::GetItemSpecFromHandle(const FGameplayInventoryItemSpecHandle& Handle, const FGameplayInventoryItemContext& Context)
{
	const UGameplayInventoryManager* InventoryManager = UInventorySystemBlueprintLibrary::FindInventoryManager(Context.InventoryOwner);
	if (!InventoryManager)
	{
		static FGameplayInventoryItemSpec EmptySpec;
		return EmptySpec;
	}

	return *InventoryManager->FindItemSpecFromHandle(Handle);
}

bool UGameplayInventoryItemFragment::CanApplyInventoryFragment(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, FGameplayTagContainer* OutBlockingTags) const
{
	if (!Super::CanApplyInventoryFragment(Context, Handle, OutBlockingTags))
	{
		return false;
	}

	const FGameplayInventoryItemSpec& ItemSpec = GetItemSpecFromHandle(Handle, Context);
	if (!ItemSpec.IsValid())
	{
		return false;
	}

	return CanApplyItemFragment(Context, ItemSpec, OutBlockingTags);
}

void UGameplayInventoryItemFragment::OnInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const
{
	Super::OnInstanceCreated(Context, Handle, Instance);

	const FGameplayInventoryItemSpec& ItemSpec = GetItemSpecFromHandle(Handle, Context);
	if (!ItemSpec.IsValid())
	{
		return;
	}

	OnItemInstanceCreated(Context, ItemSpec, Cast<UGameplayInventoryItemInstance>(Instance));
}

void UGameplayInventoryItemFragment::OnInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const
{
	Super::OnInstanceDestroyed(Context, Handle, Instance);

	const FGameplayInventoryItemSpec& ItemSpec = GetItemSpecFromHandle(Handle, Context);
	if (!ItemSpec.IsValid())
	{
		return;
	}

	OnItemInstanceDestroyed(Context, ItemSpec, Cast<UGameplayInventoryItemInstance>(Instance));
}


bool UGameplayInventoryItemFragment::CanApplyItemFragment(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, FGameplayTagContainer* OutBlockingTags) const
{
	bool bResult = true;

	if (!K2_CanApplyItemFragment(Context, ItemSpec, *OutBlockingTags))
	{
		bResult = false;
	}

	for (const UGameplayInventoryRequirement* Requirement : FragmentRequirements)
	{
		if (Requirement == nullptr)
		{
			continue;
		}

		if (!Requirement->CanInventoryItemPerformAction(ItemSpec, Context))
		{
			bResult = false;
		}
	}

	return bResult;
}

bool UGameplayInventoryItemFragment::K2_CanApplyItemFragment_Implementation( const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, FGameplayTagContainer& OutBlockingTags) const
{
	return true; // Override this function in a blueprint to implement custom logic
}


void UGameplayInventoryItemFragment::OnItemInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, class UGameplayInventoryItemInstance* ItemInstance) const
{
	K2_OnItemInstanceCreated(Context, ItemSpec, ItemInstance);
}

void UGameplayInventoryItemFragment::K2_OnItemInstanceCreated_Implementation(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, class UGameplayInventoryItemInstance* ItemInstance) const
{
}


void UGameplayInventoryItemFragment::OnItemInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, class UGameplayInventoryItemInstance* ItemInstance) const
{
	K2_OnItemInstanceDestroyed(Context, ItemSpec, ItemInstance);
}
void UGameplayInventoryItemFragment::K2_OnItemInstanceDestroyed_Implementation(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpec& ItemSpec, class UGameplayInventoryItemInstance* ItemInstance) const
{
}
