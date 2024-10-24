// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Icons/PickupIcon_StaticMesh.h"

UPickupIcon_StaticMesh::UPickupIcon_StaticMesh(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	EditorFriendlyName = TEXT("Static Mesh");
#endif
}

void UPickupIcon_StaticMesh::ApplyPickupIcon(
	APickupProxyActor* InProxy, const UGameplayInventoryItemDefinition* InItemDef)
{
	Super::ApplyPickupIcon(InProxy, InItemDef);


	UActorComponent* SceneIcon = InProxy->AddComponentByClass(UStaticMeshComponent::StaticClass(), true, RelativeTransform, false);
	UStaticMeshComponent* StaticMeshComponent = CastChecked<UStaticMeshComponent>(SceneIcon);
	{
		StaticMeshComponent->SetStaticMesh(StaticMesh.LoadSynchronous());
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StaticMeshComponent->SetCastShadow(false);
		StaticMeshComponent->AttachToComponent(InProxy->RotationSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	for (UPickupIconModifier* Modifier : Modifiers)
	{
		if (Modifier == nullptr)
		{
			continue;
		}

		Modifier->ApplyModifier(this, StaticMeshComponent, InProxy, InItemDef);
	}
}
