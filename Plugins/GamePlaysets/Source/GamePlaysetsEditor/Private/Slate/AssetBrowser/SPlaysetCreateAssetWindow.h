// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SPlaysetAssetBrowserWindow.h"
#include "Widgets/SCompoundWidget.h"


class GAMEPLAYSETSEDITOR_API SPlaysetCreateAssetWindow : public SPlaysetAssetBrowserWindow
{
public:
	SLATE_BEGIN_ARGS(SPlaysetCreateAssetWindow)
		{
		}
		SLATE_ARGUMENT(SPlaysetAssetBrowserWindow::FArguments, AssetBrowserWindowArgs)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UClass& CreatedClass);

	bool ShouldProceedWithAction() const { return bProceedWithAction; }
	bool IsEmpty() const { return bIsEmpty; }
private:
	/** The function that will be called by our buttons or by the asset picker itself if double-clicking, hitting enter etc. */
	void OnAssetsActivatedInternal(const TArray<FAssetData>& AssetData, EAssetTypeActivationMethod::Type);

	FReply Proceed();
	FReply ProceedTemplate();
	FReply Cancel();

	FText GetCreateButtonTooltip() const;
	
private:
	TWeakObjectPtr<UClass> CreatedClass;
	bool bProceedWithAction = false;
	bool bIsEmpty = false;
};
