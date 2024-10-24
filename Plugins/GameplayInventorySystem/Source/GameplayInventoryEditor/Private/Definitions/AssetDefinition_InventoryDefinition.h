// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GamePlaysetsEditor/Private/Definitions/AssetDefinition_Playset.h"
#include "AssetDefinition_InventoryDefinition.generated.h"


UCLASS()
class GAMEPLAYINVENTORYEDITOR_API UAssetDefinition_InventoryDefinition : public UAssetDefinition_Playset
{
	GENERATED_BODY()

public:
	//~ Begin UAssetDefinitionDefault Interface
	virtual FText GetAssetDisplayName() const override;
	virtual FText GetAssetDisplayName(const FAssetData& AssetData) const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	virtual FAssetSupportResponse CanDuplicate(const FAssetData& InAsset) const override;
	virtual EAssetCommandResult OpenAssets(const FAssetOpenArgs& OpenArgs) const override;
	//~ End UAssetDefinitionDefault Interface
};
