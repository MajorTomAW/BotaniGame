// Copyright © 2024 MajorT. All rights reserved.


#include "Spec/GameplayEquipmentSpec.h"

#include "Components/GameplayEquipmentManager.h"
#include "Definitions/GameplayEquipmentDefinition.h"

#include "Components/GameplayInventoryManager.h"
#include "Instance/GameplayEquipmentInstance.h"
#include "Instance/GameplayInventoryItemInstance.h"

//////////////////////////////////////////////////////////////////////////
/// FGameplayEquipmentSpec

FGameplayEquipmentSpec::FGameplayEquipmentSpec(const FGameplayInventoryItemSpec& Spec)
	: OwningItemSpec(Spec)
{
}

FString FGameplayEquipmentSpec::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetPathNameSafe(Instance), *GetNameSafe(EquipmentDefinition));
}

void FGameplayEquipmentSpec::PostReplicatedAdd(const struct FGameplayEquipmentSpecContainer& InArraySerializer)
{
	if (InArraySerializer.Owner)
	{
		InArraySerializer.Owner->OnEquipItem(*this, OwningItemSpec.GetInstance());
	}
}

void FGameplayEquipmentSpec::PostReplicatedRemove(const struct FGameplayEquipmentSpecContainer& InArraySerializer)
{
	if (InArraySerializer.Owner)
	{
		InArraySerializer.Owner->OnUnequipItem(*this, OwningItemSpec.GetInstance());
	}
}

void FGameplayEquipmentSpec::PostReplicatedChange(const struct FGameplayEquipmentSpecContainer& InArraySerializer)
{
}

//////////////////////////////////////////////////////////////////////////
/// FGameplayEquipmentSpecContainer

FGameplayEquipmentSpecContainer::FGameplayEquipmentSpecContainer()
	: Owner(nullptr)
{
}

void FGameplayEquipmentSpecContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		const FGameplayEquipmentSpec& Spec = Items[Index];
		UGameplayEquipmentInstance* EquipmentInstance = Spec.Instance;

		if (!IsValid(EquipmentInstance))
		{
			continue;
		}

		EquipmentInstance->OnUnequipped(Spec);
	}
}

void FGameplayEquipmentSpecContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		const FGameplayEquipmentSpec& Spec = Items[Index];
		UGameplayEquipmentInstance* EquipmentInstance = Spec.Instance;

		if (!IsValid(EquipmentInstance))
		{
			continue;
		}

		EquipmentInstance->OnEquipped(Spec, EquipmentInstance->GetCurrentItemContext());
	}
}

void FGameplayEquipmentSpecContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}
