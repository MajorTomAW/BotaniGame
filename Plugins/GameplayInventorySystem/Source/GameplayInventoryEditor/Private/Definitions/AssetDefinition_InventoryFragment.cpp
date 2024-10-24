// Copyright © 2024 MajorT. All rights reserved.


#include "AssetDefinition_InventoryFragment.h"

#include "Fragments/GameplayEquipmentFragment.h"
#include "Fragments/GameplayInventoryItemFragment.h"
#include "Fragments/GameplayInventorySystemFragment.h"

#define LOCTEXT_NAMESPACE "AssetDefinition_InventoryFragment"

//////////////////////////////////////////////////////////////////////////
/// UAssetDefinition_EquipmentFragment

FLinearColor UAssetDefinition_EquipmentFragment::GetAssetColor() const
{
	return FLinearColor(FColor( 63, 126, 255 ));
}

FText UAssetDefinition_EquipmentFragment::GetAssetDisplayName() const
{
	return LOCTEXT("AssetDefinition_EquipmentFragment", "Equipment Fragment");
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_EquipmentFragment::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath> Categories = {
		FAssetCategoryPath(LOCTEXT("InventoryCategory", "Inventory"))  / LOCTEXT("EquipmentCategory", "Fragments")
	};
	
	return Categories;
}

TSoftClassPtr<UObject> UAssetDefinition_EquipmentFragment::GetAssetClass() const
{
	return UGameplayEquipmentFragment::StaticClass();
}


//////////////////////////////////////////////////////////////////////////
/// UAssetDefinition_ItemFragment

FText UAssetDefinition_ItemFragment::GetAssetDisplayName() const
{
	return LOCTEXT("AssetDefinition_ItemFragment", "Item Fragment");
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_ItemFragment::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath> Categories = {
		FAssetCategoryPath(LOCTEXT("InventoryCategory", "Inventory"))  / LOCTEXT("EquipmentCategory", "Fragments")
	};

	return Categories;
}

TSoftClassPtr<UObject> UAssetDefinition_ItemFragment::GetAssetClass() const
{
	return UGameplayInventoryItemFragment::StaticClass();
}

FLinearColor UAssetDefinition_ItemFragment::GetAssetColor() const
{
	return FLinearColor(FColor( 63, 126, 255 ));
}

#undef LOCTEXT_NAMESPACE
