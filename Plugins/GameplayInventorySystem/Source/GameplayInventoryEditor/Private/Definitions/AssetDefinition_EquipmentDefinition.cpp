// Copyright © 2024 MajorT. All rights reserved.


#include "AssetDefinition_EquipmentDefinition.h"

#include "GameplayInventoryEditor.h"
#include "Definitions/GameplayEquipmentDefinition.h"

#define LOCTEXT_NAMESPACE "AssetDefinition_EquipmentDefinition"

FText UAssetDefinition_EquipmentDefinition::GetAssetDisplayName() const
{
	return LOCTEXT("AssetDefinition_EquipmentDefinition", "Equipment Definition");
}

TSoftClassPtr<UObject> UAssetDefinition_EquipmentDefinition::GetAssetClass() const
{
	return UGameplayEquipmentDefinition::StaticClass();
}

FLinearColor UAssetDefinition_EquipmentDefinition::GetAssetColor() const
{
	return FLinearColor(FColor::FromHex("26601EFF"));
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_EquipmentDefinition::GetAssetCategories() const
{
	static const FAssetCategoryPath CategoryPaths[] = {
		FAssetCategoryPath(LOCTEXT("AssetCategory_Equipment", "Inventory"))
	};

	return CategoryPaths;
}

EAssetCommandResult UAssetDefinition_EquipmentDefinition::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	const EToolkitMode::Type Mode = OpenArgs.GetToolkitMode();
	IGameplayInventoryEditorModule& InventoryEdModule = IGameplayInventoryEditorModule::Get();

	for (UGameplayEquipmentDefinition* ItemDef : OpenArgs.LoadObjects<UGameplayEquipmentDefinition>())
	{
		InventoryEdModule.CreateItemEditorApp(Mode, OpenArgs.ToolkitHost, ItemDef);
	}

	return EAssetCommandResult::Handled;
}

#undef LOCTEXT_NAMESPACE
