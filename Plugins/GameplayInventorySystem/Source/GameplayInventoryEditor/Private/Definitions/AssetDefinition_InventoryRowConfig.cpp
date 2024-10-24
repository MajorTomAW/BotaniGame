// Copyright © 2024 MajorT. All rights reserved.


#include "AssetDefinition_InventoryRowConfig.h"

#include "Rows/GameplayInventoryRowConfig.h"

#define LOCTEXT_NAMESPACE "AssetDefinition_InventoryRowConfig"

FLinearColor UAssetDefinition_InventoryRowConfig::GetAssetColor() const
{
	return FLinearColor(FColor(201, 29, 85));
}

FText UAssetDefinition_InventoryRowConfig::GetAssetDisplayName() const
{
	return LOCTEXT("AssetDisplayName", "Inventory Row Config");
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_InventoryRowConfig::GetAssetCategories() const
{
	static const TArray Categories = {
		FAssetCategoryPath(LOCTEXT("InventoryCategory", "Inventory")) / LOCTEXT("RowConfigCategory", "Advanced")
	};

	return Categories;
}

TSoftClassPtr<UObject> UAssetDefinition_InventoryRowConfig::GetAssetClass() const
{
	return UGameplayInventoryRowConfig::StaticClass();
}

#undef LOCTEXT_NAMESPACE