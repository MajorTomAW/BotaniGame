// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Components/PickupableComponent.h"

#include "Pickup/PickupProxyActor.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(PickupableComponent)

UPickupableComponent::UPickupableComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TArray<FPickupTemplateData> UPickupableComponent::GetItemList() const
{
	TArray<FPickupTemplateData> ItemListCopy = ItemList;
	
	//@TODO: Ist bullshit hier, aber momentan zu faul das zu fixen
	if (APickupProxyActor* ProxyActor = GetOwner<APickupProxyActor>())
	{
		FPickupTemplateData ProxyData;
		ProxyData.PickupItem = ProxyActor->GetItemDefinition();
		ProxyData.PickupQuantity = ProxyActor->PickupQuantity;
		
		ItemListCopy.Add(ProxyData);		
	}

	return ItemListCopy;
}
