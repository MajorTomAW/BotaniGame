// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AssetDefinition_BotDefinition.h"

#include "Bot/BotaniBotDefinition.h"

#define LOCTEXT_NAMESPACE "BotaniEditor"

FText UAssetDefinition_BotDefinition::GetAssetDisplayName() const
{
	return LOCTEXT("AssetDefinition_BotDefinition", "Bot Definition");
}

FLinearColor UAssetDefinition_BotDefinition::GetAssetColor() const
{
	return FLinearColor(FColor::FromHex("#0000ff"));
}

TSoftClassPtr<UObject> UAssetDefinition_BotDefinition::GetAssetClass() const
{
	return UBotaniBotDefinition::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_BotDefinition::GetAssetCategories() const
{
	static const auto Categories = { EAssetCategoryPaths::AI };
	return Categories;
}

#undef LOCTEXT_NAMESPACE
