// Copyright © 2024 MajorT. All rights reserved.


#include "Components/GameplayInventoryComponent.h"

#include "GameplayInventoryTypes.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayInventoryComponent)

UGameplayInventoryComponent::UGameplayInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
	SetIsReplicatedByDefault(true);
}

bool UGameplayInventoryComponent::IsOwnerActorAuthoritative() const
{
	const AActor* Owner = GetOwner();
	if (!ensureMsgf(Owner, TEXT("IsOwnerActorAuthoritative called on a component with no owner.")))
	{
		return false;
	}

	return Owner->HasAuthority();
}