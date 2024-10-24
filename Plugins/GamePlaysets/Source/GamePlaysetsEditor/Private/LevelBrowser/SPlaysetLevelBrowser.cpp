// Copyright © 2024 Botanibots Team. All rights reserved.


#include "SPlaysetLevelBrowser.h"

#include "GamePlaysetsEditor.h"
#include "Playset.h"
#include "PlaysetEditorIDs.h"
#include "SlateOptMacros.h"
#include "SPlaysetBrowserSubCategory.h"
#include "Runtime/Slate/Private/Widgets/Views/SListPanel.h"
#include "Widgets/Input/SSearchBox.h"

#define LOCTEXT_NAMESPACE "SPlaysetLevelBrowser"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPlaysetLevelBrowser::Construct(const FArguments& InArgs, TSharedRef<SDockTab> ParentTab)
{
	bRefreshAllPlaysets = false;
	bRefreshRecentlyPlaced = false;
	bUpdateShownItems = true;

	ActiveTabName = FPlaysetBuiltInBrowserIDs::RecentPlaysets();

	ThumbnailSize = InArgs._ThumbnailSize;
	EntryHeight = InArgs._EntryHeight;
	EntryWidth = InArgs._EntryWidth;

	ParentTab->SetOnTabDrawerOpened(FSimpleDelegate::CreateSP(this, &SSelf::OnTabDrawerOpened));
	
	UpdateItemSubCategories();

	// Category filter panel
	SAssignNew(CategoryFilterPtr, SListPanel)
		.ItemHeight(64.f)
		.ItemWidth(64.f)
		.ListOrientation(Orient_Vertical);

	UpdateBrowserCategories();
	
	CategoryBox =
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			CategoryFilterPtr.ToSharedRef()	
		];

	// Widget container panel
	SAssignNew(WidgetListView, SListView<TSharedPtr<FName>>)
	.ListItemsSource(&VirtualPaths)
	.Orientation(Orient_Vertical)
	.OnGenerateRow(this, &SSelf::OnGenerateCategoryForItem);


	// Layout
	TSharedPtr<SWidget> MainLayout =
		SNew(SSplitter)
		.PhysicalSplitterHandleSize(2.f)

		// Category panel
		+ SSplitter::Slot()
		.Resizable(false)
		.SizeRule(SSplitter::SizeToContent)
		[
			CategoryBox.ToSharedRef()
		]

		// Content panel
		+ SSplitter::Slot()
		.SizeRule(SSplitter::FractionOfParent)
		[
			WidgetListView.ToSharedRef()
		];
	
	// Body
	ChildSlot
	.Padding(8.f)
	[
		SNew(SVerticalBox)

		// Category slot
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			// Refresh button
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(FMargin(4.f, 0.f))
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "SecondaryButton")
				.Text(LOCTEXT("ButtonRefreshAllPlaysets", "Refresh"))
				.OnClicked(FOnClicked::CreateLambda([&]()
				{
					bRefreshAllPlaysets = true;
					bUpdateShownItems = true;
					bUpdateBrowserCategories = true;
					bUpdateItemSubCategories = true;
					
					return FReply::Handled();
				}))
			]

			// Search box
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			.HAlign(HAlign_Fill)
			.Padding(FMargin(4.f, 0.f))
			[
				SNew(SBorder)
				.HAlign(HAlign_Fill)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(8.f)
				[
					SNew(SSearchBox)
					.HintText(LOCTEXT("SearchBoxHint", "Search Playsets"))
				]
			]
		]

		// Main Layout
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(0.f)
			[
				MainLayout.ToSharedRef()
			]
		]
	];

	IGamePlaysetsEditorModule& PlaysetEdModule = IGamePlaysetsEditorModule::Get();
	PlaysetEdModule.OnPlaysetBrowserCategoriesChanged().AddSP(this, &SPlaysetLevelBrowser::UpdateBrowserCategories);
	PlaysetEdModule.OnPlaysetBrowserCategoryRefreshed().AddSP(this, &SPlaysetLevelBrowser::CategoryRefresh);
	PlaysetEdModule.OnPlaysetItemFilteringChanged().AddSP(this, &SPlaysetLevelBrowser::UpdateShownItems);
}

void SPlaysetLevelBrowser::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (bUpdateShownItems)
	{
		UpdateShownItems();
	}
}

void SPlaysetLevelBrowser::UpdateShownItems()
{
	bUpdateShownItems = false;
	IGamePlaysetsEditorModule& EdModule = IGamePlaysetsEditorModule::Get();
	
	FilteredItems.Reset();
	VirtualPaths.Reset();
	
	EdModule.GetItemsForCategory(ActiveTabName, FilteredItems);
	UpdateItemSubCategories();
	
	WidgetListView->RequestListRefresh();
}

void SPlaysetLevelBrowser::SetActiveTab(const FName& InNewTab)
{
	if (InNewTab != ActiveTabName)
	{
		ActiveTabName = InNewTab;

		IGamePlaysetsEditorModule::Get().RegenerateItemsForCategory(ActiveTabName);
	}
}

void SPlaysetLevelBrowser::UpdateBrowserCategories()
{
	bUpdateBrowserCategories = false;
	
	bool bAllTabExists = false;
	FName TabToActivate;
	CategoryFilterPtr->ClearItems();

	TArray<FPlaysetCategoryInfo> CategoryInfos;
	IGamePlaysetsEditorModule::Get().GetSortedCategories(CategoryInfos);

	for (const auto& Category : CategoryInfos)
	{
		if (Category.UniqueHandle == FPlaysetBuiltInBrowserIDs::AllPlaysets())
		{
			bAllTabExists = true;
		}

		if (Category.UniqueHandle.IsEqual(ActiveTabName))
		{
			TabToActivate = ActiveTabName;
		}

		CategoryFilterPtr->AddSlot()
		[
			SNew(SBox)
			.HeightOverride(64.f)
			.WidthOverride(64.f)
			.MaxDesiredHeight(64.f)
			.MaxDesiredWidth(64.f)
			.Padding(1.5f)
			.MaxAspectRatio(1.f)
			[
				SNew(SCheckBox)
				.Padding(FMargin(8.f))
				.Style(FAppStyle::Get(), "PaletteToolBar.Tab")
				.ToolTipText(Category.DisplayName)
				.OnCheckStateChanged(this, &SSelf::OnCategoryChanged, Category.UniqueHandle)
				.IsChecked(this, &SSelf::GetPlaysetTabCheckedState, Category.UniqueHandle)
				[
					SNew(SVerticalBox)

					// Icon
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Bottom)
					.Padding(FMargin(0.f, 4.f, 0.f , 0.f))
					[
						SNew(SImage)
						.ColorAndOpacity(FSlateColor::UseForeground())
						.Image(Category.Icon.GetIcon())
						.DesiredSizeOverride(FVector2d(28.f))
					]

					// Display name
					+ SVerticalBox::Slot()
					.Padding(FMargin(0.f))
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Bottom)
					[
						SNew(STextBlock)
						.Font(FAppStyle::GetFontStyle("SmallFontBold"))
						.Text(Category.ShortName)
					]
				]
			]
		];
	}

	if (TabToActivate.IsNone())
	{
		if (bAllTabExists)
		{
			TabToActivate = FPlaysetBuiltInBrowserIDs::AllPlaysets();
		}
		else if (CategoryInfos.Num() > 0)
		{
			TabToActivate = CategoryInfos[0].UniqueHandle;
		}
	}

	SetActiveTab(TabToActivate);
}

void SPlaysetLevelBrowser::UpdateItemSubCategories()
{
	for (auto Item : FilteredItems)
	{
		const UPlayset* Playset = Cast<UPlayset>(Item->AssetData.GetAsset());
		
		bool bFound = false;
		for (auto VirtualCategory : VirtualPaths)
		{
			if (!VirtualCategory.Get()->IsEqual(Playset->GetDefaultSubCategoryID()))
			{
				continue;
			}

			bFound = true;
		}

		if (!bFound)
		{
			VirtualPaths.Add(MakeShared<FName>(Playset->GetDefaultSubCategoryID()));
		}
	}
}

void SPlaysetLevelBrowser::CategoryRefresh(FName CategoryName)
{
	if (ActiveTabName == CategoryName)
	{
		UpdateShownItems();
	}
}

void SPlaysetLevelBrowser::OnTabDrawerOpened()
{
}

void SPlaysetLevelBrowser::OnCategoryChanged(const ECheckBoxState NewState, FName InCategory)
{
	if (NewState == ECheckBoxState::Checked)
	{
		SetActiveTab(InCategory);
	}
}

ECheckBoxState SPlaysetLevelBrowser::GetPlaysetTabCheckedState(FName CategoryName) const
{
	return ActiveTabName == CategoryName ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

TSharedRef<ITableRow> SPlaysetLevelBrowser::OnGenerateCategoryForItem(TSharedPtr<FName> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	TArray<TSharedPtr<FPlaceablePlayset>> Items;

	for (auto Item : FilteredItems)
	{
		if (Item.Get()->VirtualPath == InItem->ToString())
		{
			Items.Add(Item);
		}
	}
	
	return SNew(STableRow<TSharedPtr<FPlaceablePlayset>>, OwnerTable)
		[
			SNew(SPlaysetBrowserSubCategory)
			.EntryHeight(EntryHeight)
			.EntryWidth(EntryWidth)
			.ThumbnailSize(ThumbnailSize)
			.InItems(Items)
		];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE