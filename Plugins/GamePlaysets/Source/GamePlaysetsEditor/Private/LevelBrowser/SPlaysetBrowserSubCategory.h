// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlaysetEditorIDs.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STileView.h"


class SPlaysetBrowserSubCategory : public SExpandableArea
{
public:
	SLATE_BEGIN_ARGS(SPlaysetBrowserSubCategory)
		{
		}
		SLATE_ARGUMENT(SExpandableArea::FArguments, ExpandableAreaArgs)
		SLATE_ARGUMENT(float, ThumbnailSize)
		SLATE_ARGUMENT(float, EntryWidth)
		SLATE_ARGUMENT(float, EntryHeight)
		SLATE_ARGUMENT(TArray<TSharedPtr<FPlaceablePlayset>>, InItems)
	SLATE_END_ARGS()

	friend class SPlaysetLevelBrowser;
	void Construct(const FArguments& InArgs);

protected:
	FName CategoryName;
	float ThumbnailSize;
	
private:
	using SSelf = SPlaysetBrowserSubCategory;
	
	TArray<TSharedPtr<FPlaceablePlayset>> Items;
	TSharedPtr<STileView<TSharedPtr<FPlaceablePlayset>>> TileView;

	TSharedRef<ITableRow> OnGenerateWidgetForItem(TSharedPtr<FPlaceablePlayset> InItem, const TSharedRef<STableViewBase>& OwnerTable);
};
