// Copyright © 2024 MajorT. All rights reserved.


#include "GameplayInventoryTypes.h"

#include "Components/GameplayInventoryComponent.h"

FGameplayInventoryItemContext::FGameplayInventoryItemContext(AActor* InInventoryOwner)
{
	InventoryOwner = InInventoryOwner;
	Instigator = InInventoryOwner;
	InventoryComponent = InInventoryOwner->FindComponentByClass<UGameplayInventoryComponent>();
}

void FGameplayInventoryItemContext::FillContext()
{
}

bool FGameplayInventoryItemContext::HasInstigatorOrOwnerAuthority() const
{
	bool bHasAuthority = false;

	if (const AActor* Actor = Cast<AActor>(Instigator))
	{
		bHasAuthority = Actor->HasAuthority();
	}

	if (InventoryOwner && !bHasAuthority)
	{
		bHasAuthority = InventoryOwner->HasAuthority();
	}

	return bHasAuthority;
}

bool FGameplayInventoryItemContext::IsValid() const
{
	return (InventoryComponent != nullptr) &&
		(InventoryOwner != nullptr);
}
