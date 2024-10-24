// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Icons/PickupIconMod_RarityDrivenOverlayMaterial.h"

#include "Inventory/Data/BotaniRarityStyleAsset.h"
#include "Inventory/Definitions/BotaniInventoryItemDefinition.h"
#include "System/BotaniAssetManager.h"

void UPickupIconMod_RarityDrivenOverlayMaterial::ApplyModifier(UPickupItemIcon* Outer, USceneComponent* SpawnedIcon,
                                                               APickupProxyActor* PickupProxy, const UGameplayInventoryItemDefinition* InItemDef)
{
	Super::ApplyModifier(Outer, SpawnedIcon, PickupProxy, InItemDef);

	const UMeshComponent* MeshComp = Cast<UMeshComponent>(SpawnedIcon);
	UMaterialInstanceDynamic* DynMat = CastChecked<UMaterialInstanceDynamic>(MeshComp->GetOverlayMaterial());

	if (ensure(DynMat))
	{
		const UBotaniInventoryItemDefinition* ItemDef = Cast<UBotaniInventoryItemDefinition>(InItemDef);
		if (ItemDef == nullptr)
		{
			return;
		}
		
		const EBotaniItemRarity Rarity = ItemDef->Rarity;
		const UBotaniGameData& GameDate =  UBotaniAssetManager::Get().GetGameData();
		const FBotaniRarityStyleInfo Info = GameDate.BotaniRarityData.LoadSynchronous()->GetRarityStyle(Rarity);

		for (const auto& KVP : Info.ColorParameters)
		{
			DynMat->SetVectorParameterValue(KVP.Key, KVP.Value);
		}

		for (const auto& KVP : Info.ScalarParameters)
		{
			DynMat->SetScalarParameterValue(KVP.Key, KVP.Value);
		}

		for (const auto& KVP : Info.TextureParameters)
		{
			DynMat->SetTextureParameterValue(KVP.Key, KVP.Value);
		}
	}
}
