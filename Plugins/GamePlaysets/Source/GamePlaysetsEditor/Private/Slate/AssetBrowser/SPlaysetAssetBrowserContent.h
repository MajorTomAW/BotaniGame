// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "IContentBrowserSingleton.h"
#include "Widgets/SCompoundWidget.h"


class GAMEPLAYSETSEDITOR_API SPlaysetAssetBrowserContent : public SCompoundWidget
{
	DECLARE_DELEGATE_RetVal_OneParam(bool, FOnShouldFilterAsset, const FAssetData&)
	DECLARE_DELEGATE_RetVal(TArray<TSharedRef<FFrontendFilter>>, FOnGetExtraFrontendFilters)
	SLATE_BEGIN_ARGS(SPlaysetAssetBrowserContent)
		{
		}
		SLATE_ARGUMENT(FAssetPickerConfig, InitialConfig)
		SLATE_ARGUMENT(FAssetPickerConfig, TemplateConfig)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void SetARFilter(FARFilter InFilter);

	TArray<FAssetData> GetCurrentSelection() const;
	
private:	
	FRefreshAssetViewDelegate RefreshAssetViewDelegate;
	FSyncToAssetsDelegate SyncToAssetsDelegate;
	FSetARFilterDelegate SetNewFilterDelegate;
	FGetCurrentSelectionDelegate GetCurrentSelectionDelegate;
	FGetCurrentSelectionDelegate GetCurrentTemplateSelectionDelegate;

	FAssetPickerConfig InitialAssetPickerConfig;
};
