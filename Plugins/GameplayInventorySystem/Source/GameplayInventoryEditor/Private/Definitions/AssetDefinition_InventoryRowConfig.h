// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetDefinitionDefault.h"
#include "AssetDefinition_InventoryRowConfig.generated.h"

UCLASS()
class UAssetDefinition_InventoryRowConfig : public UAssetDefinitionDefault
{
	GENERATED_BODY()
	
public:
	virtual FLinearColor GetAssetColor() const override;
	virtual FText GetAssetDisplayName() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};
