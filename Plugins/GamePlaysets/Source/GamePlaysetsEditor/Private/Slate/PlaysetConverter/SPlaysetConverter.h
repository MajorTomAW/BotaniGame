// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "ContentBrowserDelegates.h"
#include "SPlaysetInfoPanel.h"
// #include "SPlaysetSelectedActorsList.h"
#include "Widgets/SCompoundWidget.h"


class SPlaysetSelectedActorsList;

class SPlaysetConverter : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPlaysetConverter)
		{
		}
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<AActor>>, SelectedActors)
		SLATE_EVENT(FOnAssetsActivated, OnAssetsActivated)
		SLATE_ARGUMENT(TSharedPtr<SWidget>, AdditionalWidget)
	SLATE_END_ARGS()

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	TArray<FAssetData> GetSelectedAssets() const;
	TArray<FAssetData> GetActiveAssets() const;
	void GetPlaysetInfo(FPlaysetDisplayInfo& OutDisplayInfo, FPlaysetDataList& OutData, EPlaysetOffsetType& OffsetType) const;

private:
	TArray<TWeakObjectPtr<AActor>> SelectedActors;
	// TSharedPtr<FPlaysetConverterOptions> ConvertOptions;

	// TSharedRef<STileView<TSharedRef<FSelectedPlaysetActor>>> SelectedActorsList;
	TSharedPtr<SPlaysetSelectedActorsList> SelectedActorsList;
	TSharedPtr<SPlaysetInfoPanel> PlaysetInfoPanel;

	SSplitter::FSlot* SelectedActorsSlot = nullptr;
	SSplitter::FSlot* PlaysetSlot = nullptr;
	SVerticalBox::FSlot* AdditionalWidgetSlot = nullptr;

private:
	void PopulatePlaysetSlot();
	void PopulateSelectedActorsSlot();
};
