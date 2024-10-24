// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniEditor.h"
#include "AssetTypeActions/AssetTypeActions_DataAsset.h"


class FBotaniInputConfig_Actions : public FAssetTypeActions_DataAsset
{
public:
	FBotaniInputConfig_Actions();
	
	//~ Begin IAssetTypeActions Interface
	virtual FText GetName() const override;
	virtual bool CanFilter() override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	virtual FText GetDisplayNameFromAssetData(const FAssetData& AssetData) const override;
	virtual uint32 GetCategories() override { return FBotaniEditorModule::GetInputCategory(); }
	//~ End IAssetTypeActions Interface
};
