// Copyright © 2024 Botanibots Team. All rights reserved.


#include "SPlaysetBrowserSubCategory.h"

#include "Playset.h"
#include "SlateOptMacros.h"
#include "SPlaysetBrowserEntry.h"

#define LOCTEXT_NAMESPACE "SPlaysetBrowserSubCategory"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPlaysetBrowserSubCategory::Construct(const FArguments& InArgs)
{
	Items = InArgs._InItems;
	if (!Items.IsValidIndex(0) || Items[0] == nullptr)
	{
		return;
	}
	
	UPlayset* Playset = Cast<UPlayset>(Items.Last()->AssetData.GetAsset());
	CategoryName = Playset ? Playset->GetDefaultSubCategoryID() : NAME_None;
	ThumbnailSize = InArgs._ThumbnailSize;

	FText Title = FText::FromName(CategoryName);
	if (Title.IsEmpty())
	{
		Title = LOCTEXT("NoCategory", "No Category");
	}
	
	// External scrollbar
	TSharedRef<SScrollBar> ScrollBar =
		SNew(SScrollBar)
			.Padding(FMargin(2.f))
			.Thickness(FVector2d(9.f));

	SExpandableArea::FArguments ExpArgs = InArgs._ExpandableAreaArgs;
	ExpArgs.AreaTitleFont(FAppStyle::GetFontStyle("NormalFontBold"));
	ExpArgs.AreaTitle(Title);
	ExpArgs.InitiallyCollapsed(Items.Num() <= 0);
	ExpArgs.BodyContent()
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		[
			SAssignNew(TileView, STileView<TSharedPtr<FPlaceablePlayset>>)
			.ListItemsSource(&Items)
			.Orientation(Orient_Vertical)
			.ExternalScrollbar(ScrollBar)
			.ItemAlignment(EListItemAlignment::LeftAligned)
			.ItemHeight(InArgs._EntryHeight)
			.ItemWidth(InArgs._EntryWidth)
			.SelectionMode(ESelectionMode::Single)
			.OnGenerateTile(this, &SSelf::OnGenerateWidgetForItem)
		]
	];

	SExpandableArea::Construct(ExpArgs);
}

TSharedRef<ITableRow> SPlaysetBrowserSubCategory::OnGenerateWidgetForItem(TSharedPtr<FPlaceablePlayset> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedRef<FPlaceablePlayset>>, OwnerTable)
		.Padding(4.f)
		.Style(&FAppStyle::Get(), "PlacementBrowser.PlaceableItemRow")
		[
			SNew(SPlaysetBrowserEntry, InItem)
			.ThumbnailSize(ThumbnailSize)
		];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE