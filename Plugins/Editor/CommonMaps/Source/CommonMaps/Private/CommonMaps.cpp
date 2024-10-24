// Copyright © 2024 MajorT. All rights reserved.

#include "CommonMaps.h"

#include "AssetSelection.h"
#include "CommonMapsDeveloperSettings.h"
#include "FileHelpers.h"

#define LOCTEXT_NAMESPACE "FCommonMapsModule"

namespace CommonMapsFunctionLibrary
{
	static bool HasPlayWorld()
	{
		return GEditor->PlayWorld != nullptr;
	}

	static bool HasNoPlayWorld()
	{
		return !HasPlayWorld();
	}

	static bool CanShowCommonMaps()
	{
		return !HasPlayWorld();
	}

	static void OpenCommonMap_Clicked(const FString MapPath)
	{
		const bool bPromptUserToSaveMap = UCommonMapsDeveloperSettings::Get().bPromptUserToSaveMap;
		constexpr bool bSaveMapPackages = true;
		constexpr bool bSaveContentPackages = true;
		constexpr bool bFastSave = false;
		constexpr bool bNotifyNoPackagesSaved = false;
		constexpr bool bCanBeDeclined = true;

		if (FEditorFileUtils::SaveDirtyPackages(bPromptUserToSaveMap, bSaveMapPackages, bSaveContentPackages, bFastSave, bNotifyNoPackagesSaved, bCanBeDeclined))
		{
			if (ensure(MapPath.Len()))
			{
				GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(MapPath);
			}
		}
	}

	static TSharedRef<SWidget> GetCommonMapsDropdown()
	{
		FMenuBuilder MenuBuilder(true, nullptr);

		for (const auto& Pair : UCommonMapsDeveloperSettings::Get().CommonMaps)
		{
			TAttribute<FText> SectionText;
			SectionText.Set(FText::FromName(Pair.Key));
			MenuBuilder.BeginSection(NAME_None, SectionText);

			for (const auto& Map : Pair.Value.MapURL)
			{
				if (!Map.IsValid() || Map.IsNull())
				{
					continue;
				}

				const FText DisplayName = FText::FromString(Map.GetAssetName());
				MenuBuilder.AddMenuEntry(
					DisplayName,
					FText::Format(LOCTEXT("CommonPathDesciptioin", "{0}"), FText::FromString(Map.ToString())),
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Level"),
					FUIAction(
						FExecuteAction::CreateStatic(&OpenCommonMap_Clicked, Map.ToString()),
						FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
						FIsActionChecked(),
						FIsActionButtonVisible::CreateStatic(&CanShowCommonMaps)
					));
			}

			MenuBuilder.EndSection();
		}

		return MenuBuilder.MakeWidget();
	}

	static void RegisterGameEditorMenus()
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		FToolMenuSection& Section = Menu->FindOrAddSection("PlayGameExtensions", TAttribute<FText>(),FToolMenuInsert("Play", EToolMenuInsertType::Default));

		FToolMenuEntry CommonMapEntry = FToolMenuEntry::InitComboButton(
			"CommonMapOptions",
			FUIAction(
				FExecuteAction(),
				FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
				FIsActionChecked(),
				FIsActionButtonVisible::CreateStatic(&CanShowCommonMaps)),
				FOnGetContent::CreateStatic(&GetCommonMapsDropdown),
				LOCTEXT("CommonMaps_Label", "Common Maps"),
				LOCTEXT("CommonMaps_Tooltip", "Open a common map"),
				FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Level")
			);

		CommonMapEntry.StyleNameOverride = "CalloutToolbar";
		Section.AddEntry(CommonMapEntry);
	}
}

void FCommonMapsModule::StartupModule()
{
	ExtendContextMenu();

	if (IsRunningGame())
	{
		return;
	}

	if (FSlateApplication::IsInitialized())
	{
		ToolsMenuHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&CommonMapsFunctionLibrary::RegisterGameEditorMenus));
	}
}

void FCommonMapsModule::ShutdownModule()
{
}

void FCommonMapsModule::ExtendContextMenu()
{
	UToolMenu* WorldAssetMenu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.World");
	if (!WorldAssetMenu) return;

	FToolMenuSection& Section = WorldAssetMenu->AddSection("AssetContextCommonMaps",
														   LOCTEXT("CommonMapsOptionsMenuHeading", "Common Maps"));
	
	Section.AddSubMenu(FName("Common Maps"),
		LOCTEXT("AddToCommonMapsFromMenu", "Add to Common Maps"),
		LOCTEXT("AddToCommonMapsFromMenuTooltip", "Add this map into Common Maps list."),
		FNewMenuDelegate::CreateRaw(this, &FCommonMapsModule::CreateCategorySelectionSubmenu),
		FToolUIActionChoice(),
		EUserInterfaceActionType::None,
		false,
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Star"));
}

void FCommonMapsModule::AddToCommonMapsMenu(FName CategoryName)
{
	TArray<FAssetData> SelectedAssets;
	AssetSelectionUtils::GetSelectedAssets(SelectedAssets);

	UCommonMapsDeveloperSettings* Settings = &UCommonMapsDeveloperSettings::Get();
	if (Settings == nullptr)
	{
		FMessageDialog::Open(EAppMsgCategory::Error, EAppMsgType::Ok, FText::FromString("Failed to get CommonMapsDeveloperSettings."), FText::FromString("Error"));
	}

	for (const auto& AssetData : SelectedAssets)
	{
		if (FCommonMapContainer* CurrentMaps = Settings->CommonMaps.Find(CategoryName))
		{
			CurrentMaps->MapURL.Add(AssetData.GetSoftObjectPath());
		}
		else
		{
			Settings->CommonMaps.Add(CategoryName, FCommonMapContainer({ AssetData.GetSoftObjectPath() }));
		}
	}

	Settings->SaveConfig(CPF_Config);
}

void FCommonMapsModule::CreateCategorySelectionSubmenu(FMenuBuilder& MenuBuilder)
{
	const UCommonMapsDeveloperSettings* DevSettings = &UCommonMapsDeveloperSettings::Get();
	if (!DevSettings) return;

	TArray<FName> Categories;
	DevSettings->CommonMaps.GetKeys(Categories);
	for (const FName& CategoryName : Categories)
	{
		MenuBuilder.AddMenuEntry(
			FText::Format(LOCTEXT("CategoryLabel", "{0}"), FText::FromName(CategoryName)),
			FText::Format(LOCTEXT("CategoryTooltip", "Add this map to \"{0}\" category."), FText::FromName(CategoryName)),
			FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &FCommonMapsModule::AddToCommonMapsMenu, CategoryName)));
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCommonMapsModule, CommonMaps)