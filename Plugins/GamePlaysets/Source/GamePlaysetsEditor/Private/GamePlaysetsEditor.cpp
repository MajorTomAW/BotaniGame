// Copyright © 2024 MajorT. All rights reserved.

#include "GamePlaysetsEditor.h"

#include "AssetSelection.h"
#include "ContentBrowserMenuContexts.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "LevelEditor.h"
#include "Selection.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "ActorFactories/ActorFactoryPawn.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Definitions/AssetDefinition_Playset.h"
#include "Details/PlaysetDetailsCustomization.h"
#include "Engine/AssetManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "LevelBrowser/SPlaysetLevelBrowser.h"
#include "Settings/PlaysetDeveloperSettings.h"
#include "Slate/PlaysetConverter/SPlaysetConvertAssetWindow.h"
#include "Slate/PlaysetConverter/SPlaysetConverter.h"
#include "Slate/PlaysetConverter/SPlaysetConverterWindow.h"
#include "Style/PlaysetStyle.h"
#include "Styling/SlateIconFinder.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "Thumbnails/PlaysetThumbnailRenderer.h"

#define LOCTEXT_NAMESPACE "FGamePlaysetsEditorModule"

namespace LevelEditorExtension_Playset
{
	static TSharedRef<SDockTab> HandleTabManagerSpawnTab(const FSpawnTabArgs& Args)
	{
		// Dock Tab
		TSharedRef<SDockTab> DockTab = SNew(SDockTab)
			.Label(LOCTEXT("PlaysetLevelBrowserLabel", "Playset Browser"))
			.ContentPadding(FMargin(0.f))
			.TabRole(PanelTab);

		// Content Tab
		TSharedRef<SPlaysetLevelBrowser> LevelBrowser =
			SNew(SPlaysetLevelBrowser, DockTab)
			.EntryHeight(148.f)
			.EntryWidth(128.f)
			.ThumbnailSize(96.f);

		// Attach content to the dock tab
		DockTab->SetContent(LevelBrowser);
		return DockTab;
	}
	
	static void AddLevelEditorExtension()
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
		LevelEditorModule.OnTabManagerChanged().AddLambda([&LevelEditorModule]()
		{
			const FSlateIcon AssetIcon = FSlateIconFinder::FindIconForClass(UPlayset::StaticClass());
			
			LevelEditorModule.GetLevelEditorTabManager()->RegisterTabSpawner(FPlaysetEditorIDs::PlaysetAssetCategory(), FOnSpawnTab::CreateStatic(HandleTabManagerSpawnTab))
			.SetDisplayName(NSLOCTEXT("PlaysetLevelEditorTab", "PlaysetLevelEditorTab", "Playset Browser"))
			.SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorCategory())
			.SetIcon(AssetIcon);
		});
	}
}

namespace AssetMenuExtension_Playset
{
	static void Execute_ExposeToLevelBrowser(const FName InHandle)
	{
		UPlaysetDeveloperSettings* Settings = UPlaysetDeveloperSettings::Get();
		if (!Settings)
		{
			return;
		}

		TArray<FAssetData> SelectedAssets;
		AssetSelectionUtils::GetSelectedAssets(SelectedAssets);

		for (auto& Wrapper : Settings->BrowserCategoryMapping)
		{
			if (Wrapper.UniqueHandle == InHandle)
			{
				for (const FAssetData& Asset : SelectedAssets)
				{
					/*UPlayset* Playset = Cast<UPlayset>(Asset.GetAsset());
					if (!Playset)
                    {
                        continue;
                    }

					const FName Sub = Playset->GetDefaultSubCategoryID();
					Wrapper.FindOrAddToSub(Sub, Asset.GetSoftObjectPath());*/
					Wrapper.Playsets.Add(Asset.GetSoftObjectPath());
				}
			}
		}

		UPlaysetDeveloperSettings::SaveToConfig();

		// Request a refresh of the level browser
		IGamePlaysetsEditorModule& PlaysetEdModule = IGamePlaysetsEditorModule::Get();
		PlaysetEdModule.RegenerateItemsForCategory(InHandle);
		PlaysetEdModule.RegenerateItemsForCategory(FPlaysetBuiltInBrowserIDs::AllPlaysets());
	}
	
	static void CreateCategorySelectionSubmenu(FMenuBuilder& MenuBuilder)
	{
		const UPlaysetDeveloperSettings* Settings = UPlaysetDeveloperSettings::Get();
		if (!Settings)
		{
			return;
		}

		TArray<FPlaysetLevelBrowserCategoryInfo> Categories = Settings->BrowserCategoryMapping;
		for (const auto& Wrapper : Categories)
		{
			MenuBuilder.AddMenuEntry
			(
				FText::Format(LOCTEXT("CategoryLabel", "{0}"), Wrapper.DisplayName),
				FText::FromString(LOCTEXT("CategoryTooltip", "Expose this asset to the {0} category in the Playset Level Browser").ToString()),
				FSlateIcon(FAppStyle::GetAppStyleSetName(), Wrapper.BrushIcon),
				FUIAction
				(
					FExecuteAction::CreateStatic(&Execute_ExposeToLevelBrowser, Wrapper.UniqueHandle)
				)
			);
		}
	}
	
	static void AddAssetMenuExtension()
	{
		UToolMenu* Menu = UE::ContentBrowser::ExtendToolMenu_AssetContextMenu(UPlayset::StaticClass());
		FToolMenuSection& Section = Menu->FindOrAddSection("Playset");
		Section.AddSubMenu
		(
			"Playset",
			LOCTEXT("ExposeToBrowser", "Expose to Level Browser"),
			LOCTEXT("ExposeToBrowserTooltip", "Expose this asset to an existing category in the Playset Level Browser"),
			FNewMenuDelegate::CreateStatic(CreateCategorySelectionSubmenu),
			FToolUIActionChoice(),
			EUserInterfaceActionType::None,
			false,
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Star")
		);
	}
}

/******************************* IGamePlaysetsEditorModule *******************************/

class FGamePlaysetsEditorModule final : public IGamePlaysetsEditorModule
{
public:
	using Self = FGamePlaysetsEditorModule;

	DECLARE_DERIVED_EVENT(FGamePlaysetsEditorModule, IGamePlaysetsEditorModule::FOnPlaysetBrowserCategoriesChanged, FOnPlaysetBrowserCategoriesChanged)
	virtual FOnPlaysetBrowserCategoriesChanged& OnPlaysetBrowserCategoriesChanged() override { return PlaysetBrowserCategoriesChanged; }

	DECLARE_DERIVED_EVENT(FGamePlaysetsEditorModule, IGamePlaysetsEditorModule::FOnPlaysetItemFilteringChanged, FOnPlaysetItemFilteringChanged)
	virtual FOnPlaysetItemFilteringChanged& OnPlaysetItemFilteringChanged() override { return PlaysetItemFilteringChanged; }

	DECLARE_DERIVED_EVENT(FGamePlaysetsEditorModule, IGamePlaysetsEditorModule::FOnPlaysetBrowserCategoryRefreshed, FOnPlaysetBrowserCategoryRefreshed)
	virtual FOnPlaysetBrowserCategoryRefreshed& OnPlaysetBrowserCategoryRefreshed() override { return PlaysetBrowserCategoryRefreshed; }
	
	//~ Begin IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface

protected:
	/** Called after the engine has been initialized. */
	void OnPostEngineInit();

	/** Called to add the level editor menu. */
	static void AddLevelEditorMenu();
	void AddBuiltInCategories();

private:
	/** The playset developer settings */
	UPlaysetDeveloperSettings* PlaysetDeveloperSettings = nullptr;

	static void ExecuteConvertToPlayset();

private:
	static FGuid CreateID() {return FGuid::NewGuid(); }

	FOnPlaysetBrowserCategoriesChanged PlaysetBrowserCategoriesChanged;
	FOnPlaysetItemFilteringChanged PlaysetItemFilteringChanged;
	FOnPlaysetBrowserCategoryRefreshed PlaysetBrowserCategoryRefreshed;

public:
	virtual void GetSortedCategories(TArray<FPlaysetCategoryInfo>& OutCategories) override;

	virtual bool RegisterPlaysetBrowserCategory(const FPlaysetCategoryInfo& Info) override;

	virtual void GetItemsForCategory(FName Category, TArray<TSharedPtr<FPlaceablePlayset>>& OutItems) const override;

	virtual void RegenerateItemsForCategory(FName Category) override;
};

IMPLEMENT_MODULE(FGamePlaysetsEditorModule, GamePlaysetsEditor)


/******************************* Implementation *******************************/

void FGamePlaysetsEditorModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FGamePlaysetsEditorModule::OnPostEngineInit);
	FCoreDelegates::OnPreExit.AddLambda([]()
	{
		UPlaysetDeveloperSettings::SaveToConfig();
	});
}

void FGamePlaysetsEditorModule::OnPostEngineInit()
{
	// Register the details customizer
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("Playset", FOnGetDetailCustomizationInstance::CreateStatic(&FPlaysetDetailsCustomization::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();
	
	// Register the slate style set
	FPlaysetStyle::Initialize();

	AddBuiltInCategories();
	LevelEditorExtension_Playset::AddLevelEditorExtension();
	AssetMenuExtension_Playset::AddAssetMenuExtension();

	if (GIsEditor)
	{
		UThumbnailManager::Get().RegisterCustomRenderer(UPlayset::StaticClass(), UPlaysetThumbnailRenderer::StaticClass());
	}

	// Get the playset developer settings
	/*PlaysetDeveloperSettings = GetMutableDefault<UPlaysetDeveloperSettings>();
	PlaysetDeveloperSettings->OnSettingChanged().AddRaw(this, &FGamePlaysetsEditorModule::ReloadConfig);*/

	// Register level editor menu
	AddLevelEditorMenu();
}

void FGamePlaysetsEditorModule::AddLevelEditorMenu()
{
	// Asset type icons
	const FSlateIcon AssetIcon = FSlateIconFinder::FindIconForClass(UPlayset::StaticClass());
	
	// Actor context menu
	UToolMenu* Menu = UToolMenus::Get()->FindMenu("LevelEditor.ActorContextMenu");
	if (!Menu)
	{
		return;
	}
	
	FToolMenuSection& Section = Menu->FindOrAddSection("PlaysetOptions", LOCTEXT("AssetOptionsHeading", "Playset Options"));
	Section.InsertPosition.Position = EToolMenuInsertType::First;
	Section.AddMenuEntry(
		"Playset_ConvertToPlayset",
		LOCTEXT("Playset_ConvertToPlayset", "Convert to Playset"),
		LOCTEXT("Playset_ConvertToPlaysetTooltip", "Convert the selected actor(s) to a Playset."),
		AssetIcon,
		FUIAction(
			FExecuteAction::CreateStatic(&FGamePlaysetsEditorModule::ExecuteConvertToPlayset)
			)
		);
}

void FGamePlaysetsEditorModule::AddBuiltInCategories()
{
	// Recent
	/*RegisterPlaysetBrowserCategory
	(
		FPlaysetCategoryInfo
		(
			LOCTEXT("PlaysetCategory_RecentlyPlaced", "Recently Placed"),
			LOCTEXT("PlaysetCategory_RecentlyPlaced_Short", "Recent"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "PlacementBrowser.Icons.Recent"),
			FPlaysetBuiltInBrowserIDs::RecentPlaysets(),
			TNumericLimits<int32>::Lowest()
		)
	);*/

	// All
	{
		const FName CategoryHandle = FPlaysetBuiltInBrowserIDs::AllPlaysets();
		/*RegisterPlaysetBrowserCategory
		(
			FPlaysetCategoryInfo
			(
				LOCTEXT("PlaysetCategory_All", "All Playsets"),
				LOCTEXT("PlaysetCategory_All_Short", "All"),
				FSlateIcon(FAppStyle::GetAppStyleSetName(), "PlacementBrowser.Icons.All"),
				CategoryHandle,
				TNumericLimits<int32>::Max()
			)
		);*/

		if (FPlaysetCategory* Category = Categories.Find(CategoryHandle))
		{
			FSoftObjectPath Path = FSoftObjectPath(TEXT("/BioDevice_PlayerStart/PID_CBD_PlayerStart.PID_CBD_PlayerStart"));
			UActorFactory* Factory = GEditor->FindActorFactoryByClass(Path.TryLoad()->GetClass());
			FAssetData AssetData = FAssetData(Path.TryLoad());

			PlaysetCache.Add(AssetData);
			Category->Items.Add(CreateID(), MakeShareable(new FPlaceablePlayset(Factory, AssetData, TEXT("Test"))));
			Category->Items.Add(CreateID(), MakeShareable(new FPlaceablePlayset(Factory, AssetData, TEXT("Test"))));
			Category->Items.Add(CreateID(), MakeShareable(new FPlaceablePlayset(Factory, AssetData, TEXT("Test No"))));
		}
	}

	// Custom
	if (UPlaysetDeveloperSettings* Settings = UPlaysetDeveloperSettings::Get())
	{
		for (auto Wrapper : Settings->BrowserCategoryMapping)
		{
			RegisterPlaysetBrowserCategory
			(
				FPlaysetCategoryInfo
				(
					Wrapper.DisplayName,
					Wrapper.ShortDisplayName,
					FSlateIcon(FAppStyle::GetAppStyleSetName(), Wrapper.BrushIcon),
					Wrapper.UniqueHandle,
					Wrapper.SortOrder
				)
			);
		}
	}
}


void FGamePlaysetsEditorModule::ExecuteConvertToPlayset()
{
	TArray<TWeakObjectPtr<AActor>> SelectedActors;
	USelection* Selection = GEditor->GetSelectedActors();
	for (FSelectionIterator It(*Selection); It; ++It)
	{
		AActor* Actor = Cast<AActor>(*It);
		if (Actor)
		{
			SelectedActors.Add(Actor);
		}
	}
	
	const IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	const TSharedPtr<SWindow> ParentWindow = MainFrameModule.GetParentWindow();

	SPlaysetConverter::FArguments ConverterArgs;
	ConverterArgs.SelectedActors(SelectedActors);

	SPlaysetConverterWindow::FArguments WindowArgs;
	WindowArgs.PlaysetConverterArgs(ConverterArgs)
	.WindowTitle(LOCTEXT("PlaysetConverterWindowTitle", "Convert to Playset"));

	TSharedRef<SPlaysetConvertAssetWindow> ConverterWindow = SNew(SPlaysetConvertAssetWindow).PlaysetConverterArgs(WindowArgs);
	FSlateApplication::Get().AddModalWindow(ConverterWindow, ParentWindow);

	if (!ConverterWindow->ShouldProceedWithAction())
	{
		return;
	}

	TArray<FAssetData> ActiveAssets = ConverterWindow->GetActiveAssets();

	// Get the playset info
	FPlaysetDisplayInfo DisplayInfo;
	FPlaysetDataList DataList;
	EPlaysetOffsetType OffsetType;
	ConverterWindow->GetPlaysetInfo(DisplayInfo, DataList, OffsetType);

	const FString PlaysetName = DisplayInfo.ItemName.IsEmpty() ? TEXT("NewPlayset") : DisplayInfo.ItemName.ToString();
	const FString PrefixedPlaysetName = TEXT("PID_") + PlaysetName;
	const FString PathName = FPaths::GetPath(ActiveAssets.Top().GetPackage()->GetPathName());

	FSaveAssetDialogConfig SaveConfig;
	SaveConfig.DialogTitleOverride = LOCTEXT("SavePlaysetDialogTitle", "Save Playset");
	SaveConfig.DefaultPath = FPaths::Combine(UPlaysetDeveloperSettings::Get()->DefaultPlaysetDirectory.Path, PrefixedPlaysetName);
	SaveConfig.DefaultAssetName = PrefixedPlaysetName;
	SaveConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::Disallow;

	const FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	const FString SaveObjectPath = ContentBrowserModule.Get().CreateModalSaveAssetDialog(SaveConfig);
	if (SaveObjectPath.IsEmpty())
	{
		return;
	}

	const FString SavePackageName = FPackageName::ObjectPathToPackageName(SaveObjectPath);
	const FString SavePackagePath = FPaths::GetPath(SavePackageName);
	const FString SaveAssetName = FPaths::GetBaseFilename(SavePackageName);

	UPackage* Package = CreatePackage(*SavePackageName);
	if (ensure(Package))
	{
		// Create the playset
		if (UPlayset* Playset = NewObject<UPlayset>(Package, *SaveAssetName, RF_Public | RF_Standalone))
		{
			Playset->InitializeDisplayInfo(DisplayInfo);
			Playset->InitializeDataList(DataList);
			Playset->InitializeOffsetType(OffsetType);
			
			Playset->InitializeSavedActors(ActiveAssets);

			// Save actors into the playset
			UEditorAssetSubsystem* EdSub = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>();
			UAssetEditorSubsystem* AssetSub = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

			// Save the selected assets into the playset (serialize the actors)

			// Get all properties of the playset

			
			
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Playset);

			// Notify the asset registry
			FAssetRegistryModule::AssetCreated(Playset);

			// Mark the package dirty
			Package->MarkPackageDirty();
		}
	}

	FSlateApplication::Get().DismissAllMenus();
}

void FGamePlaysetsEditorModule::GetSortedCategories(TArray<FPlaysetCategoryInfo>& OutCategories)
{
	TArray<FName> SortedIDs;
	Categories.GenerateKeyArray(SortedIDs);

	SortedIDs.Sort([&](const FName& A, const FName& B)
	{
		return Categories[A].SortOrder < Categories[B].SortOrder;
	});

	OutCategories.Reset();

	for (const auto& ID : SortedIDs)
	{
		OutCategories.Add(Categories[ID]);
	}
}

bool FGamePlaysetsEditorModule::RegisterPlaysetBrowserCategory(const FPlaysetCategoryInfo& Info)
{
	if (Categories.Contains(Info.UniqueHandle))
	{
		return false;
	}

	Categories.Add(Info.UniqueHandle, Info);
	PlaysetBrowserCategoriesChanged.Broadcast();
	return true;
}

void FGamePlaysetsEditorModule::GetItemsForCategory(FName CategoryName, TArray<TSharedPtr<FPlaceablePlayset>>& OutItems) const
{
	UE_LOG(LogTemp, Display, TEXT("GetItemsForCategory: %s"), *CategoryName.ToString());
	
	OutItems.Reset();

	if (CategoryName == FPlaysetBuiltInBrowserIDs::AllPlaysets())
	{
		for (const auto& Pair : Categories)
        {
            for (const auto& ItemPair : Pair.Value.Items)
            {
                OutItems.Add(ItemPair.Value);
            }
        }

		for (TObjectIterator<UPlayset> PlaysetIt; PlaysetIt; ++PlaysetIt)
		{
			TSharedPtr<FPlaceablePlayset> Item =
				MakeShareable(new FPlaceablePlayset(
					GEditor->FindActorFactoryByClass(PlaysetIt->GetClass()),
					FAssetData(*PlaysetIt),
					TEXT("")));
			
			OutItems.Add(Item);
		}

		TArray<FAssetData> Playsets;
		{
			static const FPrimaryAssetType PlaysetAssetType = TEXT("Playset");
			UAssetManager::Get().GetPrimaryAssetDataList(PlaysetAssetType, Playsets);

			for (const FAssetData& Asset : Playsets)
			{
				TSharedPtr<FPlaceablePlayset> Item =
					MakeShareable(new FPlaceablePlayset(
						GEditor->FindActorFactoryByClass(Asset.GetClass()),
						Asset,
						TEXT("")));

				OutItems.Add(Item);
			}
		}

		return;
	}
	
	if (const FPlaysetCategory* Category = Categories.Find(CategoryName))
	{
		// UE_LOG(LogTemp, Display, TEXT("Found Category: %s. Num Items: %d"), *CategoryName.ToString(), Category->Items.Num());
		
		for (const auto& Pair : Category->Items)
		{
			OutItems.Add(Pair.Value);
		}
	}
}

void FGamePlaysetsEditorModule::RegenerateItemsForCategory(FName CategoryName)
{
	if (FPlaysetCategory* Category = Categories.Find(CategoryName))
	{
		Category->Items.Reset();

		UPlaysetDeveloperSettings* Settings = UPlaysetDeveloperSettings::Get();
		if (!Settings)
		{
			return;
		}

		for (auto& Mapping : Settings->BrowserCategoryMapping)
		{
			if (!Mapping.UniqueHandle.IsEqual(CategoryName))
			{
				continue;
			}

			for (auto& Asset : Mapping.Playsets)
			{
				UObject* AssetObject = Asset.TryLoad();
				if (!AssetObject)
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to load asset: %s"), *Asset.ToString());
					continue;
				}
				
				UActorFactory* Factory = GEditor->FindActorFactoryByClass(Asset.TryLoad()->GetClass());
				FAssetData AssetData = FAssetData(Asset.TryLoad());
				
				const UPlayset* Playset = Cast<UPlayset>(Asset.TryLoad());
				TSharedPtr<FPlaceablePlayset> Item = MakeShareable(new FPlaceablePlayset
				(
					Factory,
					AssetData,
					Playset ? Playset->GetDefaultSubCategoryID().ToString() : TEXT("")
				));

				Category->Items.Add(CreateID(), Item);
			}
		}
	}

	PlaysetBrowserCategoryRefreshed.Broadcast(CategoryName);
}

void FGamePlaysetsEditorModule::ShutdownModule()
{
	FCoreDelegates::OnPostEngineInit.RemoveAll(this);

	// Unregister the slate style set
	FPlaysetStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE