// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SPlaysetConverterWindow.h"
#include "SPlaysetSelectedActorsList.h"
#include "Widgets/SCompoundWidget.h"



class GAMEPLAYSETSEDITOR_API SPlaysetConvertAssetWindow : public SPlaysetConverterWindow
{
public:
	SLATE_BEGIN_ARGS(SPlaysetConvertAssetWindow)
		{
		}
		SLATE_ARGUMENT(SPlaysetConverterWindow::FArguments, PlaysetConverterArgs)
		SLATE_ARGUMENT(TSharedPtr<SPlaysetSelectedActorsList>, SelectedActorsList)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	bool ShouldProceedWithAction() const { return bProceedWithAction; }
	bool HasSelectedAssets() const;
	bool HasValidPlaysetName() const;

protected:
	TSharedPtr<SPlaysetSelectedActorsList> SelectedActorsList;

	void OnAssetsActivatedInternal(const TArray<FAssetData>& AssetData, EAssetTypeActivationMethod::Type Method);

private:
	FReply Proceed();
	FReply Cancel();

	bool CanCreatePlayset() const;

private:
	bool bProceedWithAction = false;
};
