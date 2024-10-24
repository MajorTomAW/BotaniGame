// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "AssetDefinitionDefault.h"
#include "GamePlaysetsEditor.h"
#include "Playset.h"
#include "PlaysetEditorIDs.h"
// #include "Style/PlaysetStyle.h"

#include "AssetDefinition_Playset.generated.h"

class UPlayset;

/**
 * The editor definitions for a UPlayset.
 */
UCLASS()
class GAMEPLAYSETSEDITOR_API UAssetDefinition_Playset : public UAssetDefinitionDefault
{
	GENERATED_BODY()

protected:
	//~ Begin UAssetDefinitionDefault Interface
	virtual FText GetAssetDisplayName() const override;
	virtual FText GetAssetDisplayName(const FAssetData& AssetData) const override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
	virtual FLinearColor GetAssetColor() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	
	virtual FAssetSupportResponse CanRename(const FAssetData& InAsset) const override;
	virtual FAssetSupportResponse CanDuplicate(const FAssetData& InAsset) const override;

	virtual UThumbnailInfo* LoadThumbnailInfo(const FAssetData& InAssetData) const override;
	//~ End UAssetDefinitionDefault Interface

private:
	static UPlayset* GetPlaysetFromAssetData(const FAssetData& AssetData);
};
