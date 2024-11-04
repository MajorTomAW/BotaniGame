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




UPickupIconModifier::UPickupIconModifier(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPickupIconModifier::ApplyModifier(
	UPickupItemIcon* Outer, USceneComponent* SpawnedIcon, APickupProxyActor* PickupProxy, const UGameplayInventoryItemDefinition* InItemDef)
{
	K2_ApplyModifier(Outer, SpawnedIcon, PickupProxy, InItemDef);
}

