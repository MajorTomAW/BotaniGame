// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Icons/Mod/PickupIconMod_OverlayMaterial.h"

void UPickupIconMod_OverlayMaterial::ApplyModifier(
	UPickupItemIcon* Outer, USceneComponent* SpawnedIcon, APickupProxyActor* PickupProxy, const UGameplayInventoryItemDefinition* InItemDef)
{
	Super::ApplyModifier(Outer, SpawnedIcon, PickupProxy, InItemDef);

	if (UMeshComponent* MeshComp = Cast<UMeshComponent>(SpawnedIcon))
	{
		UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(OverlayMaterial.LoadSynchronous(), PickupProxy);
		MeshComp->SetOverlayMaterial(DynMaterial);

		for (const auto& KVP : ScalarParameters)
		{
			DynMaterial->SetScalarParameterValue(KVP.Key, KVP.Value);
		}

		for (const auto& KVP : VectorParameters)
		{
			DynMaterial->SetVectorParameterValue(KVP.Key, KVP.Value);
		}
	}
}
