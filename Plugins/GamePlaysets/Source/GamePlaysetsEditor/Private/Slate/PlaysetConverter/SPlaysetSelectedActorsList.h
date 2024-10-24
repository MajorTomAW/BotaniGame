// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SelectedPlaysetActor.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STileView.h"

class GAMEPLAYSETSEDITOR_API SPlaysetSelectedActorsList : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPlaysetSelectedActorsList)
		{
		}
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<AActor>>, SelectedActors)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	TArray<FAssetData> GetSelectedAssets() const;
	TArray<FAssetData> GetActiveAssets() const;

private:
	TArray<TWeakObjectPtr<AActor>> SelectedWeakActors;
	TArray<TSharedRef<FSelectedPlaysetActor>> SelectedActors;
	TArray<TSharedRef<FSelectedPlaysetActor>> SelectedActiveActors;
	TArray<TSharedRef<FSelectedPlaysetActor>> GetSelectedActors() const;

	TSharedRef<ITableRow> GenerateTile(TSharedRef<FSelectedPlaysetActor> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	static TSharedRef< class SToolTip > CreateToolTip(const TSharedRef<FSelectedPlaysetActor>& InItem);

private:
	FText GetSelectedActorsText() const;

	void OnCheckStateChanged(ECheckBoxState NewState, TSharedRef<FSelectedPlaysetActor> InItem);
	bool IsItemActive(TSharedRef<FSelectedPlaysetActor> InItem) const;
	const FTextBlockStyle* GetTextStyle(TSharedRef<FSelectedPlaysetActor> InItem) const;
	const FSlateColor GetTextColor(TSharedRef<FSelectedPlaysetActor> InItem) const;
private:
	TSharedPtr<STextBlock> SelectedAssetsText;
	TSharedPtr<STileView<TSharedRef<FSelectedPlaysetActor>>> SelectionTileView;
};