// Copyright © 2024 MajorT. All rights reserved.

#include "SPlaysetAssetBrowser.h"

#include "ContentBrowserMenuContexts.h"
#include "GamePlaysetsEditor.h"
#include "IContentBrowserSingleton.h"
#include "Playset.h"
#include "SlateOptMacros.h"
#include "SPlaysetAssetBrowserContent.h"
#include "SPlaysetSelectedAssetDetails.h"
#include "Filters/SBasicFilterBar.h"
#include "Settings/PlaysetDeveloperSettings.h"
#include "Style/PlaysetStyle.h"

#define LOCTEXT_NAMESPACE "SPlaysetAssetBrowser"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPlaysetAssetBrowser::Construct(const FArguments& InArgs)
{
	AvailableClasses = InArgs._AvailableClasses;
	RecentAndFavoritesList = InArgs._RecentAndFavoritesList;
	SaveSettingsName = InArgs._SaveSettingsName;
	EmptySelectionMessage = InArgs._EmptySelectionMessage;
	AssetSelectionMode = InArgs._AssetSelectionMode;
	OnAssetSelectedDelegate = InArgs._OnAssetSelected;
	OnAssetsActivatedDelegate = InArgs._OnAssetsActivated;

	ChildSlot
	[
		SNew(SBorder)
		.Padding(0.f)
		.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(0.f)
			[
				SNew(SSplitter)
				.Orientation(Orient_Horizontal)
				.PhysicalSplitterHandleSize(2.f)
				+ SSplitter::Slot().Expose(FiltersSlot)
					.Value(0.2f)
					.MinSize(100.f)
					.SizeRule(SSplitter::SizeToContent)
				+ SSplitter::Slot().Expose(AssetBrowserContentSlot)
					.Value(0.6f)
				+ SSplitter::Slot().Expose(AssetBrowserDetailsAreaSlot)
					.Value(0.25f)
					.MinSize(100.f)
					.SizeRule(SSplitter::SizeToContent)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSeparator)
				.Orientation(Orient_Horizontal)
				.Thickness(2.f)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Expose(AdditionalWidgetSlot)
		]
	];

	bSuppressSaveAndLoad = true;
	PopulateAssetBrowserContentSlot();
	PopulateFiltersSlot();
	PopulateAssetBrowserDetailsSlot();

	if(InArgs._AdditionalWidget.IsValid())
	{
		AdditionalWidgetSlot->AttachWidget(InArgs._AdditionalWidget.ToSharedRef());
	}

	bSuppressSaveAndLoad = false;
	LoadSettings();

	InitContextMenu();
}

SPlaysetAssetBrowser::~SPlaysetAssetBrowser()
{
	SaveSettings();
}

void SPlaysetAssetBrowser::RefreshBackendFilter() const
{
	AssetBrowserContent->SetARFilter(GetCurrentBackendFilter());
}

TArray<FAssetData> SPlaysetAssetBrowser::GetSelectedAssets() const
{
	return AssetBrowserContent->GetCurrentSelection();
}

TArray<UClass*> SPlaysetAssetBrowser::GetDisplayedAssetTypes() const
{
	TArray<UClass*> AllClasses;
	for (UClass* Class : AvailableClasses)
	{
		AllClasses.AddUnique(Class);

		// Find all subclasses of the class
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(Class, ChildClasses, true);
		for (UClass* ChildClass : ChildClasses)
		{
			AllClasses.AddUnique(ChildClass);
		}
	}
	
	return AllClasses;
}

void SPlaysetAssetBrowser::InitContextMenu()
{
	static FName MenuName("PlaysetAssetBrowser.ContextMenu");
	if (UToolMenus::Get()->IsMenuRegistered(MenuName))
	{
		return;
	}

	UToolMenu* Menu = UToolMenus::Get()->RegisterMenu(MenuName);
	FToolMenuSection& DefaultSection = Menu->AddSection("Default");

	auto ToolMenuAction = FToolMenuExecuteAction::CreateLambda([] (const FToolMenuContext& Context)
	{
		UContentBrowserAssetContextMenuContext* CBContext = Context.FindContext<UContentBrowserAssetContextMenuContext>();
		if (CBContext == nullptr)
		{
			return;
		}

		if (GEditor)
		{
			GEditor->SyncBrowserToObjects(CBContext->SelectedAssets);
		}
	});

	FToolUIActionChoice ActionChoice(ToolMenuAction);
	DefaultSection.AddEntry(FToolMenuEntry::InitMenuEntry(
		"NavigateToAsset",
		LOCTEXT("NavigateToAsset", "Find in Content Browser"),
		FText::GetEmpty(),
		FSlateIcon(FAppStyle::Get().GetStyleSetName(), "Common.AssetBrowser.FindInContentBrowser"),
		ActionChoice));
}

FARFilter SPlaysetAssetBrowser::GetCurrentBackendFilter() const
{
	FARFilter Filter;

	// if the filter list hasn't specified any classes of the available ones, we explicitly set the filter to the available classes.
	// this is because if no classes are specified, we could potentially receive assets outside the available classes
	if(Filter.ClassPaths.IsEmpty())
	{
		for(UClass* AvailableClass : AvailableClasses)
		{
			Filter.ClassPaths.Add(AvailableClass->GetClassPathName());
		}
	}

	return Filter;
}

bool SPlaysetAssetBrowser::ShouldFilterAsset(const FAssetData& AssetData) const
{
	const UPlayset* Playset = Cast<UPlayset>(AssetData.GetAsset());
	if(Playset && Playset->IsTemplate())
	{
		return true;
	}
	
	for (const TSharedRef<FPlaysetAssetBrowserMainFilter>& Filter : MainFilterSelector->GetSelectedItems())
	{
		if (Filter->FilterMode == "All")
		{
			return false;
		}
		
		if (Filter->FilterMode == UPlaysetDeveloperSettings::Get()->GetCategoryForPlayset(Playset->GetClass()))
		{
			return false;
		}
	}
	
	return true;
}

bool SPlaysetAssetBrowser::ShouldFilterTemplateAsset(const FAssetData& AssetData) const
{
	const UPlayset* Playset = Cast<UPlayset>(AssetData.GetAsset());
	if(Playset && !Playset->IsTemplate())
	{
		return true;
	}
	
	for (const TSharedRef<FPlaysetAssetBrowserMainFilter>& Filter : MainFilterSelector->GetSelectedItems())
	{
		if (Filter->FilterMode == "All")
		{
			return false;
		}
	}

	return true;
}

void SPlaysetAssetBrowser::PopulateFiltersSlot()
{
	AssetBrowserMainFilters = GetMainFilters();

	SAssignNew(MainFilterSelector, STileView<TSharedRef<FPlaysetAssetBrowserMainFilter>>)
	.ListItemsSource(&AssetBrowserMainFilters)
	.Orientation(Orient_Vertical)
	.ItemWidth(300.f)
	.ItemHeight(100.f)
	.ItemAlignment(EListItemAlignment::Fill)
	.OnSelectionChanged(this, &SThis::OnMainFilterSelected)
	.OnGenerateTile(this, &SThis::GenerateWidgetRowForMainFilter)
	.SelectionMode(ESelectionMode::Type::Single);

	TSharedRef<FPlaysetAssetBrowserMainFilter>* AllFilter =AssetBrowserMainFilters.FindByPredicate([](const TSharedRef<FPlaysetAssetBrowserMainFilter>& MainFilterCandidate)
	{
		return MainFilterCandidate->FilterMode == "All";
	});

	if (AllFilter)
	{
		MainFilterSelector->SetSelection(*AllFilter);
	}

	FiltersSlot->AttachWidget(MainFilterSelector.ToSharedRef());
}

void SPlaysetAssetBrowser::PopulateAssetBrowserContentSlot()
{
	FAssetPickerConfig Config;
	Config.Filter = GetCurrentBackendFilter();
	Config.bCanShowFolders = false;
	Config.bCanShowClasses = true;
	Config.bAddFilterUI = true;
	// Config.DefaultFilterMenuExpansion = IGamePlaysetsEditorModule::Get().GetAdvancedPlaysetItemCategory(); @TODO: Get the filter category bit
	Config.ExtraFrontendFilters = OnGetExtraFrontendFilters();
	Config.OnExtendAddFilterMenu = FOnExtendAddFilterMenu::CreateSP(this, &SThis::OnExtendAddFilterMenu);
	Config.bUseSectionsForCustomFilterCategories = true;
	Config.OnShouldFilterAsset = FOnShouldFilterAsset::CreateSP(this, &SThis::ShouldFilterAsset);
	Config.OnAssetSelected = FOnAssetSelected::CreateSP(this, &SThis::OnAssetSelected);
	Config.OnAssetsActivated = FOnAssetsActivated::CreateSP(this, &SThis::OnAssetsActivated);
	Config.OnGetCustomAssetToolTip = FOnGetCustomAssetToolTip::CreateSP(this, &SThis::OnGetCustomAssetTooltip);
	Config.OnIsAssetValidForCustomToolTip = FOnIsAssetValidForCustomToolTip::CreateLambda([](FAssetData& AssetData)
	{
		return true;
	});
	Config.bForceShowEngineContent = false;
	Config.bForceShowPluginContent = true;
	Config.SelectionMode = AssetSelectionMode;
	// Config.CustomColumns.Add(ParentColumn);
	Config.bShowBottomToolbar = true;
	Config.bShowPathInColumnView = true;
	Config.bAllowDragging = false;
	Config.OnGetAssetContextMenu = FOnGetAssetContextMenu::CreateSP(this, &SThis::OnGetAssetContextMenu);


	FAssetPickerConfig TemplateConfig;
	TemplateConfig.Filter = GetCurrentBackendFilter();
	TemplateConfig.bCanShowFolders = false;
	TemplateConfig.bCanShowClasses = false;
	TemplateConfig.bAddFilterUI = false;
	
	TemplateConfig.ExtraFrontendFilters = OnGetExtraFrontendFilters();
	TemplateConfig.OnExtendAddFilterMenu = FOnExtendAddFilterMenu::CreateSP(this, &SThis::OnExtendAddFilterMenu);
	TemplateConfig.bUseSectionsForCustomFilterCategories = true;
	TemplateConfig.OnShouldFilterAsset = FOnShouldFilterAsset::CreateSP(this, &SThis::ShouldFilterTemplateAsset);
	TemplateConfig.OnAssetSelected = FOnAssetSelected::CreateSP(this, &SThis::OnAssetSelected);
	TemplateConfig.OnAssetsActivated = FOnAssetsActivated::CreateSP(this, &SThis::OnAssetsActivated);
	TemplateConfig.OnGetCustomAssetToolTip = FOnGetCustomAssetToolTip::CreateSP(this, &SThis::OnGetCustomAssetTooltip);
	TemplateConfig.OnIsAssetValidForCustomToolTip = FOnIsAssetValidForCustomToolTip::CreateLambda([](FAssetData& AssetData)
	{
		return true;
	});

	if(SaveSettingsName.IsSet())
	{
		Config.SaveSettingsName = SaveSettingsName.GetValue().ToString();
	}

	SAssignNew(AssetBrowserContent, SPlaysetAssetBrowserContent)
	.InitialConfig(Config)
	.TemplateConfig(TemplateConfig);

	AssetBrowserContentSlot->AttachWidget(AssetBrowserContent.ToSharedRef());
}

void SPlaysetAssetBrowser::PopulateAssetBrowserDetailsSlot()
{
	DetailsContainer = SNew(SBox)
	.Padding(5.f)
	[
		SAssignNew(DetailsSwitcher, SWidgetSwitcher)
		+ SWidgetSwitcher::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		.Padding(30.f, 60.f)
		[
			SNew(STextBlock)
			.Text(EmptySelectionMessage.Get(FText::GetEmpty()))
			.AutoWrapText(true)
			.TextStyle(&FAppStyle::GetWidgetStyle<FTextBlockStyle>("NormalText.Subdued"))
		]
		+ SWidgetSwitcher::Slot()
		.Expose(AssetBrowserDetailsSlot)
	];
	
	AssetBrowserDetailsAreaSlot->AttachWidget(DetailsContainer.ToSharedRef());
}

TArray<TSharedRef<FPlaysetAssetBrowserMainFilter>> SPlaysetAssetBrowser::GetMainFilters() const
{
	TArray<TSharedRef<FPlaysetAssetBrowserMainFilter>> MainFilters;

	// Add the filter for all playsets
	for (const auto& Wrapper : UPlaysetDeveloperSettings::Get()->CategoryMapping)
	{
		TSharedRef<FPlaysetAssetBrowserMainFilter> Filter = MakeShared<FPlaysetAssetBrowserMainFilter>(Wrapper.Category.Category);
		MainFilters.Add(Filter);
	}
	
	return MainFilters;
}

void SPlaysetAssetBrowser::OnFilterChanged() const
{
	RefreshBackendFilter();
}

void SPlaysetAssetBrowser::OnGetChildFiltersForFilter(
	TSharedRef<FPlaysetAssetBrowserMainFilter> PlaysetAssetBrowserMainFilter, TArray<TSharedRef<FPlaysetAssetBrowserMainFilter>>& OutChildren) const
{
	//@TODO: Get the children filters for the given filter
}

bool SPlaysetAssetBrowser::OnCompareMainFiltersForEquality(
	const FPlaysetAssetBrowserMainFilter& PlaysetAssetBrowserMainFilter, const FPlaysetAssetBrowserMainFilter& PlaysetAssetBrowserMainFilter1) const
{
	return PlaysetAssetBrowserMainFilter == PlaysetAssetBrowserMainFilter1;
}

TSharedRef<ITableRow> SPlaysetAssetBrowser::GenerateWidgetRowForMainFilter( TSharedRef<FPlaysetAssetBrowserMainFilter> MainFilter, const TSharedRef<STableViewBase>& OwningTable) const
{
	FLinearColor FolderColor = FAppStyle::Get().GetSlateColor("ContentBrowser.DefaultFolderColor").GetSpecifiedColor();

	
	TSharedRef<SWidget> Widget = SNew(SOverlay)
	+ SOverlay::Slot()
	.Padding(2.f)
	[
		SNew(SImage)
		.Image_Lambda([this, MainFilter]()
		{
			if (MainFilter->FilterMode == "All")
			{
				return FPlaysetStyle::Get()->GetBrush("PlaysetsBrowser.AllPlaysets");	
			}

			return FPlaysetStyle::Get()->GetBrush("PlaysetsBrowser.CustomPlayset");
		})
		/*.ColorAndOpacity(FColor::FromHex("26601EFF"))*/
	]
	+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Bottom)
		.Padding(16.f, 0.f, 0.f, 16.f)
	[
		SNew(STextBlock)
		.Text(MainFilter->GetDisplayName())
		.TransformPolicy(ETextTransformPolicy::ToUpper)
		.TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("PrimaryButtonText"))
		.Font(FSlateFontInfo(FAppStyle::Get().GetFontStyle("NotificationList.FontBold")))
	];
	
	/*if(MainFilter->FilterMode == FPlaysetAssetBrowserMainFilter::EFilterMode::PlaysetAssetTag)
	{
		Widget->SetToolTipText(MainFilter->AssetTagDefinition.Description);	
	}

	if(MainFilter->FilterMode == FPlaysetAssetBrowserMainFilter::EFilterMode::PlaysetAssetTagDefinitionsAsset)
	{
		Widget->SetToolTipText(MainFilter->AssetTagDefinitionsAsset->GetDescription());	
	}*/

	/*FCommonPlaysetsEditorModule& PlaysetsEditorModule = FModuleManager::GetModuleChecked<FCommonPlaysetsEditorModule>("CommonPlaysetsEditor");

	FTableRowStyle* Style = new FTableRowStyle();
	Style->TextColor = FLinearColor::White;
	Style->SelectedTextColor = FLinearColor::Blue;
	
	Style->SetActiveBrush(PlaysetsEditorModule.AssetBrowserActiveBrush->Brush);
	Style->SetActiveHighlightedBrush(PlaysetsEditorModule.AssetBrowserActiveBrush->Brush);
	Style->SetActiveHoveredBrush(PlaysetsEditorModule.AssetBrowserActiveBrush->Brush);

	Style->SetInactiveBrush(PlaysetsEditorModule.AssetBrowserNormalBrush->Brush);
	Style->SetInactiveHighlightedBrush(PlaysetsEditorModule.AssetBrowserNormalBrush->Brush);
	Style->SetInactiveHoveredBrush(PlaysetsEditorModule.AssetBrowserNormalBrush->Brush);

	Style->SetSelectorFocusedBrush(PlaysetsEditorModule.AssetBrowserActiveBrush->Brush);
	Style->SetUseParentRowBrush(false);*/
	
	return SNew(STableRow<TSharedRef<FPlaysetAssetBrowserMainFilter>>, OwningTable)
		[
			Widget
		];
}

void SPlaysetAssetBrowser::OnAssetSelected(const FAssetData& AssetData)
{
	if (!AssetData.IsValid())
	{
		AssetBrowserDetailsSlot->AttachWidget(SNullWidget::NullWidget);
		DetailsSwitcher->SetActiveWidgetIndex(0);
	}

	TSharedRef<SWidget> SelectedAssetDetails = SNew(SPlaysetSelectedAssetDetails, AssetData)
		.OnAssetTagActivatedTooltip(LOCTEXT("SecondaryAssetTagButtonTooltip", "\n\nClicking this tag will activate/deactivate its corresponding filter."));
		
	TSharedRef<SWidget> Details = SNew(SVerticalBox)
	+ SVerticalBox::Slot()
	.AutoHeight()
	[
		SelectedAssetDetails
	];

	// We don't attach the preview currently until some of the issues are resolved
	AssetBrowserDetailsSlot->AttachWidget(SelectedAssetDetails);

	DetailsSwitcher->SetActiveWidgetIndex(1);

	OnAssetSelectedDelegate.ExecuteIfBound(AssetData);
}

void SPlaysetAssetBrowser::OnAssetsActivated(const TArray<FAssetData>& AssetData, EAssetTypeActivationMethod::Type ActivationType) const
{
	OnAssetsActivatedDelegate.ExecuteIfBound(AssetData, ActivationType);
}

TSharedPtr<SWidget> SPlaysetAssetBrowser::OnGetAssetContextMenu(const TArray<FAssetData>& AssetData) const
{
	if(AssetData.Num() == 1)
	{
		FName MenuName("PlaysetAssetBrowser.ContextMenu");
		UContentBrowserAssetContextMenuContext* MenuContext = NewObject<UContentBrowserAssetContextMenuContext>();
		MenuContext->SelectedAssets = AssetData;
		MenuContext->bCanBeModified = false;
		FToolMenuContext Context(MenuContext);
		
		return UToolMenus::Get()->GenerateWidget(MenuName, Context);
	}

	return nullptr;
}

TSharedRef<SToolTip> SPlaysetAssetBrowser::OnGetCustomAssetTooltip(FAssetData& AssetData)
{
	return SNew(SToolTip)
	[
		SNew(SPlaysetSelectedAssetDetails, AssetData)
	];
}

void SPlaysetAssetBrowser::OnMainFilterSelected(TSharedPtr<FPlaysetAssetBrowserMainFilter> MainFilter, ESelectInfo::Type Arg)
{
	if(MainFilter.IsValid())
	{
		OnFilterChanged();

		/*LastSelectedMainFilterIdentifierFallback = MainFilter->GetIdentifier();*/
		SaveSettings();
	}
}

TArray<TSharedRef<FFrontendFilter>> SPlaysetAssetBrowser::OnGetExtraFrontendFilters() const
{
	TArray<TSharedRef<FFrontendFilter>> Result;

	return Result;
}

void SPlaysetAssetBrowser::OnExtendAddFilterMenu(UToolMenu* ToolMenu) const
{
	for (FToolMenuSection& Section : ToolMenu->Sections)
	{
		Section.Blocks.RemoveAll([](FToolMenuEntry& ToolMenuEntry)
		{
			return ToolMenuEntry.Name == FName("Common");
		});
	}
}

bool SPlaysetAssetBrowser::OnIsAssetRecent(const FAssetData& AssetCandidate) const
{
	return RecentAndFavoritesList.Get()->FindMRUItemIdx(AssetCandidate.PackageName.ToString()) != INDEX_NONE;
}

void SPlaysetAssetBrowser::SaveSettings() const
{
}

void SPlaysetAssetBrowser::LoadSettings()
{
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE