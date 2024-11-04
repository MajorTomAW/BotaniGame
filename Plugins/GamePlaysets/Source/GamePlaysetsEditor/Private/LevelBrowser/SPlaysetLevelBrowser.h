// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlaysetEditorIDs.h"
#include "Widgets/SCompoundWidget.h"


class SPlaysetLevelBrowser : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPlaysetLevelBrowser)                                  
		{
		}
		SLATE_ARGUMENT(float, ThumbnailSize)
		SLATE_ARGUMENT(float , EntryWidth)
		SLATE_ARGUMENT(float, EntryHeight)
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs, TSharedRef<SDockTab> ParentTab);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	using SSelf = SPlaysetLevelBrowser;
	
	/** Flags */
	uint32 bRefreshRecentlyPlaced	:	1	= 0;
	uint32 bRefreshAllPlaysets		:	1	= 0;
	uint32 bUpdateShownItems		:	1	= 0;
	uint32 bUpdateBrowserCategories	:	1	= 0;
	uint32 bUpdateItemSubCategories	:	1	= 0;

	/** Entry settings. */
	float ThumbnailSize				=	0.f;
	float EntryWidth				=	0.f;
	float EntryHeight				=	0.f;

	/** The name of the currently active tab */
	FName ActiveTabName;

	/** List panel containing all the category filters. */
	TSharedPtr<SListPanel> CategoryFilterPtr;

	//@TODO: Use FCategoryDrivenContentBuilder in UE_DEPRECATED(5.5) to build the category filter panel.
	TSharedPtr<SVerticalBox> CategoryBox;
	TSharedPtr<SListView<TSharedPtr<FName>>> WidgetListView;

	/** List of items to be shown in the widget */
	TArray<TSharedPtr<FPlaceablePlayset>> FilteredItems;
	TArray<TSharedPtr<FName>> VirtualPaths;


private:
	void UpdateShownItems();
	void SetActiveTab(const FName& InNewTab);
	void CategoryRefresh(FName CategoryName);
	void OnTabDrawerOpened();

	void UpdateBrowserCategories();
	void UpdateItemSubCategories();

	void InitContextMenu();

	void OnCategoryChanged(const ECheckBoxState NewState, FName InCategory);
	ECheckBoxState GetPlaysetTabCheckedState(FName CategoryName) const;

	TSharedRef<ITableRow> OnGenerateCategoryForItem(TSharedPtr<FName> InCategory, const TSharedRef<STableViewBase>& OwnerTable);
};
