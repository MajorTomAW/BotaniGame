// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "IAssetTypeActions.h"
#include "SPlaysetAssetBrowser.h"
#include "Widgets/SWindow.h"

class GAMEPLAYSETSEDITOR_API SPlaysetAssetBrowserWindow : public SWindow
{
public:
	SLATE_BEGIN_ARGS(SPlaysetAssetBrowserWindow)
		{
		}
		SLATE_ARGUMENT(SPlaysetAssetBrowser::FArguments, AssetBrowserArgs)
		SLATE_ARGUMENT(TOptional<FText>, WindowTitle)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	bool HasSelectedAssets() const;
	TArray<FAssetData> GetSelectedAssets() const;
	void ClearAssetSelection();

	bool bIsEmptySelection = false;

protected:
	void OnAssetsActivated(const TArray<FAssetData>& AssetData, EAssetTypeActivationMethod::Type ActivationType);
	void OnAssetsActivated(const TArray<FAssetData>& AssetData, EAssetTypeActivationMethod::Type ActivationType, bool bIsEmpty = false);
	
	TSharedPtr<SPlaysetAssetBrowser> AssetBrowser;

private:
	FOnAssetsActivated OnAssetsActivatedDelegate;
};
