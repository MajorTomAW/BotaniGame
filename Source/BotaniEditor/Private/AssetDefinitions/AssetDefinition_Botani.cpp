// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AssetDefinition_Botani.h"

#include "BotaniEditor.h"
#include "GameFeatures/Data/BotaniGameplayTagData.h"

#define LOCTEXT_NAMESPACE "BotaniAssetDefinition"

FText UAssetDefinition_Botani::GetAssetDisplayName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_Botani", "Botani Asset");
}

FLinearColor UAssetDefinition_Botani::GetAssetColor() const
{
	return FLinearColor::Green;
}

TSoftClassPtr<UObject> UAssetDefinition_Botani::GetAssetClass() const
{
	return UBotaniGameplayTagData::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_Botani::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath> Categories = {
		FAssetCategoryPath(LOCTEXT("GameplayTagData", "Experience"))
	};
	
	return Categories;
}

EAssetCommandResult UAssetDefinition_Botani::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	return Super::OpenAssets(OpenArgs);
}

#undef LOCTEXT_NAMESPACE