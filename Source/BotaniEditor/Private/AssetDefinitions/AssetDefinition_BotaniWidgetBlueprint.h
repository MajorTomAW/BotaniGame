// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Editor/UMGEditor/Private/AssetDefinition_WidgetBlueprint.h"
#include "AssetDefinition_BotaniWidgetBlueprint.generated.h"

/**
 * 
 */
UCLASS()
class UAssetDefinition_BotaniWidgetBlueprint : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:
	UAssetDefinition_BotaniWidgetBlueprint();

	//~ Begin UAssetDefinition Interface
	virtual FText GetAssetDisplayName() const override;
	virtual FLinearColor GetAssetColor() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	virtual EAssetCommandResult OpenAssets(const FAssetOpenArgs& OpenArgs) const override;
	virtual EAssetCommandResult PerformAssetDiff(const FAssetDiffArgs& DiffArgs) const override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
	//~ End UAssetDefinition Interface
};
