// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SPlaysetConverter.h"
#include "SPlaysetInfoPanel.h"
#include "Widgets/SWindow.h"

class GAMEPLAYSETSEDITOR_API SPlaysetConverterWindow : public SWindow
{
public:
	SLATE_BEGIN_ARGS(SPlaysetConverterWindow)
	{
	}
	SLATE_ARGUMENT(SPlaysetConverter::FArguments, PlaysetConverterArgs)
	SLATE_ARGUMENT(TOptional<FText>, WindowTitle)
SLATE_END_ARGS()

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	TArray<FAssetData> GetActiveAssets() const;
	void GetPlaysetInfo(FPlaysetDisplayInfo& OutDisplayInfo, FPlaysetDataList& OutData, EPlaysetOffsetType& OffsetType) const;
	

protected:
	void OnAssetsActivated(const TArray<FAssetData>& AssetData, EAssetTypeActivationMethod::Type ActivationType);
	TSharedPtr<SPlaysetConverter> PlaysetConverter;

private:
	FOnAssetsActivated OnAssetsActivatedDelegate;
};