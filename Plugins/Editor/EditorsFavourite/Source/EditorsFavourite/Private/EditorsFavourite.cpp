// Copyright © 2024 MajorT. All rights reserved.

#include "EditorsFavourite.h"

#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "ContentBrowserDataUtils.h"
#include "ContentBrowserMenuContexts.h"
#include "ContentBrowserModule.h"
#include "EditorFavouritesSettings.h"
#include "EditorFolderUtils.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "SAssetView.h"
#include "Selection.h"
#include "Framework/Commands/GenericCommands.h"
#include "Subsystems/EditorAssetSubsystem.h"

#define LOCTEXT_NAMESPACE "FEditorsFavouriteModule"

class FEditorsFavouriteModule final : public IEditorsFavouriteModule
{
public:
	typedef FEditorsFavouriteModule Self;

	//~ Begin IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface

	virtual void RegenerateFolderColors() override;

protected:
	/** Called after the engine has been initialized. */
	void OnPostEngineInit();
};
IMPLEMENT_MODULE(FEditorsFavouriteModule, EditorsFavourite)


/******************************* Implementation *******************************/

namespace EditorsFavouriteUtils
{
	static TArray<FString> GetAllSubFolders(const FString& BaseDir)
	{
		TArray<FString> Results;

		IFileManager& FileManager = IFileManager::Get();

		// Get all folders that are in the base directory
		FileManager.FindFilesRecursive(Results, *BaseDir, TEXT("*"), false, true, false);

		return Results;
	}

	static FString GetGamePath(const FString& LongPath)
	{
		FString NormalizedPath = LongPath;
		int32 Idx = NormalizedPath.Find(FName("Content").ToString());
		if (Idx != INDEX_NONE)
		{
			NormalizedPath = NormalizedPath.RightChop(Idx);
		}

		// Replace "Content" with "Game"
		NormalizedPath = NormalizedPath.Replace(*FName("Content").ToString(), *FName("/Game").ToString());
		return NormalizedPath;
	}

	static FString GetPluginPath(const FString& LongPath)
	{
		FString NormalizedPath = LongPath;
		int32 Idx = NormalizedPath.Find(FName("Content").ToString());
		if (Idx != INDEX_NONE)
		{
			NormalizedPath = NormalizedPath.RightChop(Idx);
		}
		return NormalizedPath;
	}
}

void FEditorsFavouriteModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FEditorsFavouriteModule::OnPostEngineInit);
}

void FEditorsFavouriteModule::OnPostEngineInit()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	RegenerateFolderColors();
}

void FEditorsFavouriteModule::RegenerateFolderColors()
{
	// Get the settings
	UEditorFavouritesSettings* Settings = UEditorFavouritesSettings::Get();
	TMap<FString, FName> FolderNameMap;
	
	// Get all folders in the content browser
	{
		const FString ProjectPath = FPaths::ProjectContentDir();
		TArray<FString> FolderPaths = EditorsFavouriteUtils::GetAllSubFolders(ProjectPath);
	
		for (const FString& Dir : FolderPaths)
		{
			// Get the folder name
			FName FolderName = FEditorFolderUtils::GetLeafName(FName(*Dir));
			FolderNameMap.Add(Dir, FolderName);
		}

		
		for (const auto& KVP : FolderNameMap)
		{
			FLinearColor FolderColor;
			if (Settings->GetFolderColor(KVP.Value.ToString(), FolderColor))
			{
				// Set the folder color
				FString NormalizedPath = EditorsFavouriteUtils::GetGamePath(KVP.Key);
				AssetViewUtils::SetPathColor(NormalizedPath, FolderColor);
			}
		}
	}

	FolderNameMap.Empty();

	// Get all folders in the plugins
	{
		const FString PluginsPath = FPaths::ProjectPluginsDir();
		TArray<FString> FolderPaths = EditorsFavouriteUtils::GetAllSubFolders(PluginsPath);

		for (const FString& Dir : FolderPaths)
		{
			// Get the folder name
			FName FolderName = FEditorFolderUtils::GetLeafName(FName(*Dir));
			FolderNameMap.Add(Dir, FolderName);
		}

		for (const auto& KVP : FolderNameMap)
		{
			FLinearColor FolderColor;
			if (Settings->GetFolderColor(KVP.Value.ToString(), FolderColor))
			{
				// Set the folder color
				FString Path = KVP.Key;
				FString PathCopy = Path;
				FString FullPath = Path;
				
				int32 Idx = Path.Find(FName(KVP.Value).ToString());
				if (Idx != INDEX_NONE)
				{
					Path = Path.RightChop(Idx);
				}

				Idx = FullPath.Find(FName("Content").ToString());
				if (Idx != INDEX_NONE)
				{
					FullPath = FullPath.RightChop(Idx);
				}

				// Remove everything in the PathCopy variable from the right until the first slash
				Idx = PathCopy.Find(FName("/Content").ToString());
				if (Idx != INDEX_NONE)
				{
					PathCopy = PathCopy.Left(Idx);
				}

				// Loop through PathCopy from right to left and store every character until we reach the first slash
				FString PluginName;
				for (int32 i = PathCopy.Len() - 1; i >= 0; --i)
				{
					if (PathCopy[i] == '/')
					{
						break;
					}

					TCHAR Found = PathCopy[i];
					FString FoundString = FString(1, &Found);
					
					PluginName = FoundString + PluginName;
				}
				
				
				FullPath = "/" + FullPath.Replace(*FName("Content").ToString(), *PluginName);
				AssetViewUtils::SetPathColor(FullPath, FolderColor);
				AssetViewUtils::SetPathColor(("/" + Path), FolderColor);
			}
		}
	}
}

void FEditorsFavouriteModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
