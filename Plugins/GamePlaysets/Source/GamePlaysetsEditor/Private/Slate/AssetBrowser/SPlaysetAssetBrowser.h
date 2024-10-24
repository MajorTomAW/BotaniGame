// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "ContentBrowserDelegates.h"
#include "Filters/PlaysetMenuFilters.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Views/STileView.h"


class FFrontendFilter;
class SPlaysetAssetBrowserContent;

class GAMEPLAYSETSEDITOR_API SPlaysetAssetBrowser : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPlaysetAssetBrowser)
		: _RecentAndFavoritesList(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->GetRecentlyOpenedAssets())
		, _SaveSettingsName("PlaysetAssetBrowser")
		, _AssetSelectionMode(ESelectionMode::Single)
		{
		}
		SLATE_ARGUMENT(TArray<UClass*>, AvailableClasses)
		SLATE_ATTRIBUTE(const FMainMRUFavoritesList*, RecentAndFavoritesList)
		SLATE_ARGUMENT(TOptional<FName>, SaveSettingsName)
		SLATE_ARGUMENT(TOptional<FText>, EmptySelectionMessage)
		SLATE_ARGUMENT(ESelectionMode::Type, AssetSelectionMode)
		SLATE_EVENT(FOnAssetSelected, OnAssetSelected)
		SLATE_EVENT(FOnAssetsActivated, OnAssetsActivated)
		SLATE_ARGUMENT(TSharedPtr<SWidget>, AdditionalWidget)
	SLATE_END_ARGS()
	
public:
	typedef SPlaysetAssetBrowser SThis;

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	virtual ~SPlaysetAssetBrowser() override;

	void RefreshBackendFilter() const;

	TArray<FAssetData> GetSelectedAssets() const;
	
	TArray<UClass*> GetDisplayedAssetTypes() const;

	void InitContextMenu();

private:
	/** This function should take into account all the different widgets states that could affect the FARFilter. */
	FARFilter GetCurrentBackendFilter() const;
	
	bool ShouldFilterAsset(const FAssetData& AssetData) const;
	bool ShouldFilterTemplateAsset(const FAssetData& AssetData) const;
	
	void PopulateFiltersSlot();
	void PopulateAssetBrowserContentSlot();
	void PopulateAssetBrowserDetailsSlot();

	TArray<TSharedRef<FPlaysetAssetBrowserMainFilter>> GetMainFilters() const;

private:
	/** This has to be called whenever any kind of filtering changes and refreshes the actual asset view */
	void OnFilterChanged() const;
	
	void OnGetChildFiltersForFilter(TSharedRef<FPlaysetAssetBrowserMainFilter> PlaysetAssetBrowserMainFilter, TArray<TSharedRef<FPlaysetAssetBrowserMainFilter>>& OutChildren) const;
	bool OnCompareMainFiltersForEquality(const FPlaysetAssetBrowserMainFilter& PlaysetAssetBrowserMainFilter, const FPlaysetAssetBrowserMainFilter& PlaysetAssetBrowserMainFilter1) const;
	
	TSharedRef<ITableRow> GenerateWidgetRowForMainFilter(TSharedRef<FPlaysetAssetBrowserMainFilter> MainFilter, const TSharedRef<STableViewBase>& OwningTable) const;
	
	void OnAssetSelected(const FAssetData& AssetData);
	void OnAssetsActivated(const TArray<FAssetData>& AssetData, EAssetTypeActivationMethod::Type ActivationType) const;
	TSharedPtr<SWidget> OnGetAssetContextMenu(const TArray<FAssetData>& AssetData) const;
	TSharedRef<SToolTip> OnGetCustomAssetTooltip(FAssetData& AssetData);
	void OnMainFilterSelected(TSharedPtr<FPlaysetAssetBrowserMainFilter> MainFilter, ESelectInfo::Type Arg);

	TArray<TSharedRef<FFrontendFilter>> OnGetExtraFrontendFilters() const;
	void OnExtendAddFilterMenu(UToolMenu* ToolMenu) const;
    
	bool OnIsAssetRecent(const FAssetData& AssetCandidate) const;
    
	void SaveSettings() const;
	void LoadSettings();

private:
	TArray<UClass*> AvailableClasses;
	TAttribute<const FMainMRUFavoritesList*> RecentAndFavoritesList;
	TOptional<FName> SaveSettingsName;
	TOptional<FText> EmptySelectionMessage;
	FOnAssetSelected OnAssetSelectedDelegate;
	FOnAssetsActivated OnAssetsActivatedDelegate;
	ESelectionMode::Type AssetSelectionMode = ESelectionMode::Single;
	TArray<TSharedRef<FPlaysetAssetBrowserMainFilter>> AssetBrowserMainFilters;

private:
	TSharedPtr<STileView<TSharedRef<FPlaysetAssetBrowserMainFilter>>> MainFilterSelector;
	TSharedPtr<SPlaysetAssetBrowserContent> AssetBrowserContent;
	TSharedPtr<SWidget> DetailsContainer;
	TSharedPtr<SWidgetSwitcher> DetailsSwitcher;

	SSplitter::FSlot* FiltersSlot = nullptr;
	SSplitter::FSlot* AssetBrowserContentSlot = nullptr;
	SSplitter::FSlot* AssetBrowserDetailsAreaSlot = nullptr;
	SWidgetSwitcher::FSlot* AssetBrowserDetailsSlot = nullptr;
	SVerticalBox::FSlot* AdditionalWidgetSlot = nullptr;

	/** Used to temporarily stop saving & loading config state.
	 * This is used for initialization where setting the default state should not be saved into config.*/
	bool bSuppressSaveAndLoad = false;
	
	/** We use this fallback as a means to save our last selected filter, in case we have no valid active selections anymore. */
	mutable FName LastSelectedMainFilterIdentifierFallback = NAME_None;
};
