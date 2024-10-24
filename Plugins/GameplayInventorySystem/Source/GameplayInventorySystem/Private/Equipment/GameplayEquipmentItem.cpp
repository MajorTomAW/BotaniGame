// Copyright © 2024 MajorT. All rights reserved.


#include "Equipment/GameplayEquipmentItem.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayEquipmentItem)

AGameplayEquipmentItem::AGameplayEquipmentItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = true;
	SetCanBeDamaged(false);
	
	ItemMeshComponent = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("ItemMeshComponent"));
	ItemMeshComponent->SetGenerateOverlapEvents(false);
	ItemMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	ItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(ItemMeshComponent);
}

void AGameplayEquipmentItem::OnEquipped(APawn* OwningPawn, const FGameplayInventoryItemContext& Context)
{
	K2_OnEquipped(OwningPawn, Context);
}

void AGameplayEquipmentItem::OnUnequipped(APawn* OwningPawn, const FGameplayInventoryItemContext& Context)
{
	K2_OnUnequipped(OwningPawn, Context);
}

APawn* AGameplayEquipmentItem::GetOwningPawn() const
{
	return Cast<APawn>(GetOwner());
}

APawn* AGameplayEquipmentItem::GetTypedOwningPawn(const TSubclassOf<APawn> PawnClass) const
{
	APawn* OwningPawn = GetOwningPawn();
	
	if (OwningPawn && OwningPawn->IsA(PawnClass))
	{
		return OwningPawn;
	}

	return nullptr;
}
