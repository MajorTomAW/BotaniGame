// Copyright © 2024 MajorT. All rights reserved.


#include "Fragments/Item/ItemFragment_PickupDefinition.h"

#include "Pickup/PickupProxyActor.h"

UItemFragment_PickupDefinition::UItemFragment_PickupDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PickupProxyActor = nullptr;
}

bool FInventoryItemPickupData::IsValid() const
{
	return !Equals(FInventoryItemPickupData());
}

bool FInventoryItemPickupData::Equals(const FInventoryItemPickupData& Other) const
{
	bool bEqual = true;
	bEqual &= bDestroyOnPickup == Other.bDestroyOnPickup;
	bEqual &= bAutoEquip == Other.bAutoEquip;
	bEqual &= AutoPickupBehavior == Other.AutoPickupBehavior;
	bEqual &= PickupRadius == Other.PickupRadius;
	return bEqual;
}

bool UItemFragment_PickupDefinition::ShouldDropItem() const
{
	switch (DropData.DropBehavior) {
	case EInventoryItemDropBehavior::DoNotDrop:
		{
			return false;		
		}
	
	case EInventoryItemDropBehavior::AlwaysDrop:
		{
			return true;
		}
		
	case EInventoryItemDropBehavior::ChanceDrop:
		{
			const float RandomValue = FMath::FRandRange(0.0f, 100.0f);
			return RandomValue <= DropData.DropChance;
		}
	}

	return false;
}

TSoftClassPtr<APickupProxyActor> UItemFragment_PickupDefinition::GetPickupProxyActor() const
{
	if (!DefaultPickupProxyActor.IsNull())
	{
		return DefaultPickupProxyActor;
	}
	
	return DefaultPickupProxyActor;
}

#if WITH_EDITOR
FString UItemFragment_PickupDefinition::GetEditorFriendlyName_Implementation() const
{
	return TEXT("Pickup Definition");
}
#endif
