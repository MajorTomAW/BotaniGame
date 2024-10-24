// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Script/AssetDefinition_Blueprint.h"
#include "AssetDefinition_InventoryFragment.generated.h"


UCLASS()
class GAMEPLAYINVENTORYEDITOR_API UAssetDefinition_EquipmentFragment : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:
	virtual FLinearColor GetAssetColor() const override;
	virtual FText GetAssetDisplayName() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};


UCLASS()
class GAMEPLAYINVENTORYEDITOR_API UAssetDefinition_ItemFragment : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:
	virtual FLinearColor GetAssetColor() const override;
	virtual FText GetAssetDisplayName() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};
