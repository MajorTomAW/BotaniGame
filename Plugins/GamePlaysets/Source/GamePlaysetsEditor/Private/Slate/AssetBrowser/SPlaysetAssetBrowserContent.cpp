// Copyright © 2024 MajorT. All rights reserved.

#include "SPlaysetAssetBrowserContent.h"

#include "ContentBrowserModule.h"
#include "EditorStyleSet.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPlaysetAssetBrowserContent::Construct(const FArguments& InArgs)
{
	FAssetPickerConfig Config = InArgs._InitialConfig;
	Config.GetCurrentSelectionDelegates.Add(&GetCurrentSelectionDelegate);
	Config.RefreshAssetViewDelegates.Add(&RefreshAssetViewDelegate);
	/*Config.SyncToAssetsDelegates.Add(&SyncToAssetsDelegate);*/
	Config.SetFilterDelegates.Add(&SetNewFilterDelegate);
	
	Config.bCanShowRealTimeThumbnails = false;
	Config.ThumbnailScale = 1.f;
	Config.bCanShowFolders = true;
	Config.bShowBottomToolbar = false;
	Config.bAllowDragging = false;
	Config.bAllowRename = false;
	Config.bAllowNullSelection = false;
	Config.bCanShowClasses = true;
	Config.bAutohideSearchBar = true;
	Config.ThumbnailLabel = EThumbnailLabel::Type::NoLabel;
	Config.bShowPathInColumnView = false;
	Config.bUseSectionsForCustomFilterCategories = true;
	Config.bForceShowPluginContent = true;
	// Config.Collections.Add(FCollectionNameType("Playsets", ECollectionShareType::CST_All));

	FAssetPickerConfig TemplateConfig = InArgs._TemplateConfig;
	TemplateConfig.GetCurrentSelectionDelegates.Add(&GetCurrentTemplateSelectionDelegate);
	TemplateConfig.RefreshAssetViewDelegates.Add(&RefreshAssetViewDelegate);
	TemplateConfig.SetFilterDelegates.Add(&SetNewFilterDelegate);
	
	TemplateConfig.bCanShowRealTimeThumbnails = false;
	TemplateConfig.ThumbnailScale = 1.f;
	TemplateConfig.bCanShowFolders = false;
	TemplateConfig.bShowBottomToolbar = false;
	TemplateConfig.bAllowDragging = false;
	TemplateConfig.bAllowRename = false;
	TemplateConfig.bAllowNullSelection = false;
	TemplateConfig.bCanShowClasses = true;
	TemplateConfig.bAutohideSearchBar = true;
	TemplateConfig.ThumbnailLabel = EThumbnailLabel::Type::NoLabel;
	TemplateConfig.bShowPathInColumnView = false;
	TemplateConfig.bUseSectionsForCustomFilterCategories = true;
	TemplateConfig.bForceShowPluginContent = true;
	
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	
	ChildSlot
	[
		SNew(SSplitter)
		.Orientation(Orient_Vertical)
		.PhysicalSplitterHandleSize(2.f)
		
		+ SSplitter::Slot()
		.SizeRule(SSplitter::SizeToContent)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Padding(16.f, 8.f, 0.f, 0.f)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("PlaysetAssetBrowser", "PlaysetAssetBrowser", "Templates"))
				.TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("PrimaryButtonText"))
				.Font(FSlateFontInfo(FAppStyle::Get().GetFontStyle("NotificationList.FontBold")))
			]
			+ SVerticalBox::Slot()
			[
				ContentBrowserModule.Get().CreateAssetPicker(TemplateConfig)
			]
		]

		+ SSplitter::Slot()
		.SizeRule(SSplitter::SizeToContent)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Padding(16.f, 8.f, 0.f, 8.f)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("PlaysetAssetBrowser", "PlaysetAssetBrowser", "Copy Existing"))
				.TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("PrimaryButtonText"))
				.Font(FSlateFontInfo(FAppStyle::Get().GetFontStyle("NotificationList.FontBold")))
			]
			+ SVerticalBox::Slot()
			[
				ContentBrowserModule.Get().CreateAssetPicker(Config)
			]
		]
	];
}

void SPlaysetAssetBrowserContent::SetARFilter(FARFilter InFilter)
{
	SetNewFilterDelegate.Execute(InFilter);
}

TArray<FAssetData> SPlaysetAssetBrowserContent::GetCurrentSelection() const
{
	TArray<FAssetData> SelectedAssets = GetCurrentSelectionDelegate.Execute();
	SelectedAssets.Append(GetCurrentTemplateSelectionDelegate.Execute());

	return SelectedAssets;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
