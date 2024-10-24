// Copyright © 2024 MajorT. All rights reserved.

#include "PlaysetDeveloperSettings.h"

#include "Playset.h"
#include "PlaysetEditorIDs.h"

#define LOCTEXT_NAMESPACE "PlaysetDeveloperSettings"

UPlaysetDeveloperSettings::UPlaysetDeveloperSettings()
	: PlaysetColor(FColor::FromHex("26601EFF"))
{
	CategoryMapping.Add(FPlaysetCategoryWrapper("All", {UPlayset::StaticClass()}));
	bDestroyAttachedActors = true;

	// All
	{
		FPlaysetLevelBrowserCategoryInfo Info;
		Info.bIsBuiltIn = true;
		Info.SortOrder = TNumericLimits<int32>::Max();
		Info.DisplayName = LOCTEXT("AllPlaysets", "All Playsets");
		Info.ShortDisplayName = LOCTEXT("AllPlaysetsShort", "All");
		Info.BrushIcon = "PlacementBrowser.Icons.All";
		Info.UniqueHandle = FPlaysetBuiltInBrowserIDs::AllPlaysets();

		BrowserCategoryMapping.Add(Info);
	}

	// Recent
	{
		FPlaysetLevelBrowserCategoryInfo Info;
		Info.bIsBuiltIn = true;
		Info.SortOrder = TNumericLimits<int32>::Lowest();
		Info.DisplayName = LOCTEXT("RecentPlaysets", "Recent Playsets");
		Info.ShortDisplayName = LOCTEXT("RecentPlaysetsShort", "Recent");
		Info.BrushIcon = "PlacementBrowser.Icons.Recent";
		Info.UniqueHandle = FPlaysetBuiltInBrowserIDs::RecentPlaysets();

		BrowserCategoryMapping.Add(Info);
	}
}

UPlaysetDeveloperSettings* UPlaysetDeveloperSettings::Get()
{
	return GetMutableDefault<UPlaysetDeveloperSettings>();
}

void UPlaysetDeveloperSettings::SaveToConfig()
{
	UPlaysetDeveloperSettings* Settings = Get();

	FString DefaultEditorINi = FPaths::SourceConfigDir() / TEXT("DefaultEditor.ini");
	DefaultEditorINi = FConfigCacheIni::NormalizeConfigIniPath(DefaultEditorINi);
	Settings->SaveConfig(CPF_Config, *DefaultEditorINi);
}

FName UPlaysetDeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

FName UPlaysetDeveloperSettings::GetContainerName() const
{
	return "Editor";
}

FName UPlaysetDeveloperSettings::GetSectionName() const
{
	return "Playset Settings";
}

FName UPlaysetDeveloperSettings::GetCategoryForPlayset(const UClass* PlaysetClass) const
{
	FName Result = NAME_None;
	
	for (const auto& Wrapper : CategoryMapping)
	{
		// Check if the playset class is a child of the value class
		if (Result == NAME_None)
		{
			for (UClass* Class : Wrapper.Category.PlaysetClasses)
			{
				if (PlaysetClass->IsChildOf(Class))
				{
					Result = Wrapper.Category.Category;
					break;
				}
			}
		}

		for (UClass* Class : Wrapper.Category.PlaysetClasses)
		{
			if (PlaysetClass == Class)
			{
				Result = Wrapper.Category.Category;
				break;		
			}
		}

		for (const auto& Sub : Wrapper.SubCategories)
		{
			for (UClass* Class : Sub.PlaysetClasses)
			{
				if (PlaysetClass == Class || PlaysetClass->IsChildOf(Class))
				{
					Result = Sub.Category;
					break;
				}
			}
		}
	}

	return Result;
}

#undef LOCTEXT_NAMESPACE