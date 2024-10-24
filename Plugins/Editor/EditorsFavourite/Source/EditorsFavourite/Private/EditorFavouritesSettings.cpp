// Copyright © 2024 Botanibots Team. All rights reserved.


#include "EditorFavouritesSettings.h"

#include "EditorsFavourite.h"

void FFavouriteFoldersToColor::GeneratePreviewName()
{
	FString AllFoldersName;
	for (const FString& FolderName : FolderNames)
	{
		AllFoldersName += FolderName + TEXT(", ");
	}

	PreviewName = FName(*AllFoldersName);
}

UEditorFavouritesSettings::UEditorFavouritesSettings()
{
	DeveloperName = FApp::GetSessionOwner();
}

UEditorFavouritesSettings* UEditorFavouritesSettings::Get()
{
	return GetMutableDefault<UEditorFavouritesSettings>();
}

FName UEditorFavouritesSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

FName UEditorFavouritesSettings::GetContainerName() const
{
	return "Editor";
}

FName UEditorFavouritesSettings::GetSectionName() const
{
	return "Editor's Favourite";
}

FText UEditorFavouritesSettings::GetSectionDescription() const
{
	return FText::FromString("Settings for the Editor's Favourite plugin.");
}

void UEditorFavouritesSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for (FFavouriteFoldersToColor& FolderColor : FolderColorMappings)
	{
		FolderColor.GeneratePreviewName();
	}

	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_CHECKED(UEditorFavouritesSettings, bRefreshContentBrowser))
	{
		if (bRefreshContentBrowser)
		{
			IEditorsFavouriteModule::Get().RegenerateFolderColors();
			bRefreshContentBrowser = false;
		}
	}
}

bool UEditorFavouritesSettings::GetFolderColor(const FString& FolderName, FLinearColor& OutColor) const
{
	for (auto& KVP : FolderColorMappings)
	{
		if (KVP.FolderNames.Contains(FolderName))
		{
			OutColor = KVP.FolderColor;
			return true;
		}
	}

	if (!bAllowPartialMatch)
	{
		return false;
	}

	for (auto& KVP : FolderColorMappings)
	{
		for (const auto& Folder : KVP.FolderNames)
		{
			if (FolderName.Contains(Folder))
			{
				OutColor = KVP.FolderColor;
				return true;
			}
		}
	}

	return false;
}
