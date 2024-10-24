// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Icons/PickupItemIcon.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PickupItemIcon)

UPickupItemIcon::UPickupItemIcon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPickupItemIcon::ApplyPickupIcon(class APickupProxyActor* InProxy, const class UGameplayInventoryItemDefinition* InItemDef)
{
	K2_ApplyPickupIcon(InProxy, InItemDef);
}
