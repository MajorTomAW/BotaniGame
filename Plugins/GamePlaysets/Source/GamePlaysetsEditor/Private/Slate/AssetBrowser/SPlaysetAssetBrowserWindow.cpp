// Copyright © 2024 MajorT. All rights reserved.

#include "SPlaysetAssetBrowserWindow.h"

#include "Widgets/SWindow.h"
#include "SlateOptMacros.h"
#include "SPlaysetAssetBrowser.h"
#include "ContentBrowserMenuContexts.h"
#include "EditorStyleSet.h"
#include "IContentBrowserSingleton.h"
#include "SlateOptMacros.h"
#include "SPlaysetAssetBrowserContent.h"
#include "SPlaysetSelectedAssetDetails.h"
#include "Filters/PlaysetMenuFilters.h"
#include "Filters/SBasicFilterBar.h"
#include "Styling/SlateIconFinder.h"
#include "Widgets/Views/STileView.h"

#define LOCTEXT_NAMESPACE "SPlaysetAssetBrowserWindow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPlaysetAssetBrowserWindow::Construct(const FArguments& InArgs)
{
	OnAssetsActivatedDelegate = InArgs._AssetBrowserArgs._OnAssetsActivated;
	
	SWindow::Construct(SWindow::FArguments()
		.Title(InArgs._WindowTitle.Get(LOCTEXT("PlaysetAssetBrowserWindowTitle", "Playset Asset Browser")))
		.SizingRule(ESizingRule::UserSized)
		.ClientSize(FVector2D(1400, 750))
		.SupportsMaximize(true)
		.SupportsMinimize(false)
	[
		SAssignNew(AssetBrowser, SPlaysetAssetBrowser)
		.AvailableClasses(InArgs._AssetBrowserArgs._AvailableClasses)
		.RecentAndFavoritesList(InArgs._AssetBrowserArgs._RecentAndFavoritesList)
		.SaveSettingsName(InArgs._AssetBrowserArgs._SaveSettingsName)
		.EmptySelectionMessage(InArgs._AssetBrowserArgs._EmptySelectionMessage)
		.AssetSelectionMode(InArgs._AssetBrowserArgs._AssetSelectionMode)
		.OnAssetSelected(InArgs._AssetBrowserArgs._OnAssetSelected)
		.OnAssetsActivated(this, &SPlaysetAssetBrowserWindow::OnAssetsActivated)
		.AdditionalWidget(InArgs._AssetBrowserArgs._AdditionalWidget)
	]);
}

bool SPlaysetAssetBrowserWindow::HasSelectedAssets() const
{
	return GetSelectedAssets().Num() > 0;
}

TArray<FAssetData> SPlaysetAssetBrowserWindow::GetSelectedAssets() const
{
	return AssetBrowser->GetSelectedAssets();
}

void SPlaysetAssetBrowserWindow::ClearAssetSelection()
{
	// Unselect all assets
}

void SPlaysetAssetBrowserWindow::OnAssetsActivated(const TArray<FAssetData>& AssetData, EAssetTypeActivationMethod::Type ActivationType)
{
	OnAssetsActivated(AssetData, ActivationType, false);
}

void SPlaysetAssetBrowserWindow::OnAssetsActivated(const TArray<FAssetData>& AssetData, EAssetTypeActivationMethod::Type ActivationType, bool bIsEmpty)
{
	bIsEmptySelection = bIsEmpty;
	
	if (OnAssetsActivatedDelegate.ExecuteIfBound(AssetData, ActivationType))
	{
		RequestDestroyWindow();
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE