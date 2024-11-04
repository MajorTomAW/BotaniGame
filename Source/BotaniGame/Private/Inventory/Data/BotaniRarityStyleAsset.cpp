// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inventory/Data/BotaniRarityStyleAsset.h"

FLinearColor UBotaniRarityStyleAsset::GetRarityStyleColor(const EBotaniItemRarity Rarity, const FName Param, bool& OutSuccess) const
{
	const auto& Params = GetRarityStyle(Rarity).ColorParameters;
	OutSuccess = Params.Contains(Param);
	if (OutSuccess)
	{
		return *Params.Find(Param);
	}
	
	return FLinearColor(); 
}

float UBotaniRarityStyleAsset::GetRarityStyleScalar(const EBotaniItemRarity Rarity, const FName Param, bool& OutSuccess) const
{
	const auto& Params = GetRarityStyle(Rarity).ScalarParameters;
	OutSuccess = Params.Contains(Param);
	if (OutSuccess)
	{
		return *Params.Find(Param);
	}

	return 0.0f;
}

UTexture2D* UBotaniRarityStyleAsset::GetRarityStyleTexture(const EBotaniItemRarity Rarity, const FName Param, bool& OutSuccess) const
{
	const auto& Params = GetRarityStyle(Rarity).TextureParameters;
	OutSuccess = Params.Contains(Param);
	if (OutSuccess)
	{
		return *Params.Find(Param);
	}

	return nullptr;
}
