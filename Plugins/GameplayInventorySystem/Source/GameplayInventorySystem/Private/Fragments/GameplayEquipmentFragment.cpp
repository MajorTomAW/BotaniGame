// Copyright © 2024 MajorT. All rights reserved.


#include "Fragments/GameplayEquipmentFragment.h"

#include "GameplayInventoryTypes.h"
#include "Components/GameplayEquipmentManager.h"
#include "Instance/GameplayEquipmentInstance.h"
#include "Library/InventorySystemBlueprintLibrary.h"

UGameplayEquipmentFragment::UGameplayEquipmentFragment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UGameplayEquipmentFragment::CanApplyEquipmentFragment(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, FGameplayTagContainer* OutBlockingTags) const
{
	bool bResult = true;

	if (!K2_CanApplyEquipmentFragment(Context, EquipmentSpec, *OutBlockingTags))
	{
		bResult = false;
	}

	return bResult;
}
bool UGameplayEquipmentFragment::K2_CanApplyEquipmentFragment_Implementation(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, FGameplayTagContainer& OutBlockingTags) const
{
	return true;
}


void UGameplayEquipmentFragment::OnEquipmentInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, class UGameplayEquipmentInstance* EquipmentInstance) const
{
	K2_OnEquipmentInstanceCreated(Context, EquipmentSpec, EquipmentInstance);
}
void UGameplayEquipmentFragment::K2_OnEquipmentInstanceCreated_Implementation( const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& ItemSpec, class UGameplayEquipmentInstance* EquipmentInstance) const
{
}

void UGameplayEquipmentFragment::OnEquipmentInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, class UGameplayEquipmentInstance* EquipmentInstance) const
{
	K2_OnEquipmentInstanceDestroyed(Context, EquipmentSpec, EquipmentInstance);
}
void UGameplayEquipmentFragment::K2_OnEquipmentInstanceDestroyed_Implementation( const FGameplayInventoryItemContext& Context, const FGameplayEquipmentSpec& EquipmentSpec, class UGameplayEquipmentInstance* EquipmentInstance) const
{
}


bool UGameplayEquipmentFragment::CanApplyInventoryFragment(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, FGameplayTagContainer* OutBlockingTags) const
{
	if (!Super::CanApplyInventoryFragment(Context, Handle, OutBlockingTags))
	{
		return false;
	}

	const FGameplayEquipmentSpec& EquipmentSpec = GetEquipmentSpecFromHandle(Handle, Context);
	if (!EquipmentSpec.IsValid())
	{
		return false;
	}

	return CanApplyEquipmentFragment(Context, EquipmentSpec, OutBlockingTags);
}

void UGameplayEquipmentFragment::OnInstanceCreated(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const
{
	Super::OnInstanceCreated(Context, Handle, Instance);

	const FGameplayEquipmentSpec& EquipmentSpec = GetEquipmentSpecFromHandle(Handle, Context);
	if (!EquipmentSpec.IsValid())
	{
		return;
	}

	OnEquipmentInstanceCreated(Context, EquipmentSpec, Cast<UGameplayEquipmentInstance>(Instance));
}

void UGameplayEquipmentFragment::OnInstanceDestroyed(const FGameplayInventoryItemContext& Context, const FGameplayInventoryItemSpecHandle& Handle, class UObject* Instance) const
{
	Super::OnInstanceDestroyed(Context, Handle, Instance);

	const FGameplayEquipmentSpec& EquipmentSpec = GetEquipmentSpecFromHandle(Handle, Context);
	if (!EquipmentSpec.IsValid())
	{
		return;
	}

	OnEquipmentInstanceDestroyed(Context, EquipmentSpec, Cast<UGameplayEquipmentInstance>(Instance));
}

FGameplayEquipmentSpec& UGameplayEquipmentFragment::GetEquipmentSpecFromHandle(const FGameplayInventoryItemSpecHandle& Handle, const FGameplayInventoryItemContext& Context)
{
	const UGameplayEquipmentManager* EquipmentManager = UInventorySystemBlueprintLibrary::FindEquipmentManager(Context.InventoryOwner);
	if (!EquipmentManager)
	{
		static FGameplayEquipmentSpec EmptySpec;
		return EmptySpec;
	}

	return *EquipmentManager->FindEquipmentSpecFromHandle(Handle);
}
