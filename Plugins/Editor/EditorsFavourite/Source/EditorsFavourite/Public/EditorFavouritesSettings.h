// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EditorFavouritesSettings.generated.h"

USTRUCT()
struct FFavouriteFoldersToColor
{
	GENERATED_BODY()

public:
	/** A list of default folder names that will be colored in the content browser */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Content Browser")
	TArray<FString> FolderNames;

	/** The color to use for the folders */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Content Browser")
	FLinearColor FolderColor = FLinearColor::White;

public:
	void GeneratePreviewName();

protected:
	FName PreviewName;
};

UCLASS(Config = Editor, meta = (DisplayName = "Editor's Favourite"))
class EDITORSFAVOURITE_API UEditorFavouritesSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UEditorFavouritesSettings();
	static UEditorFavouritesSettings* Get();

	//~ Begin UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;
	virtual FName GetContainerName() const override;
	virtual FName GetSectionName() const override;
	virtual FText GetSectionDescription() const override;
	//~ End UDeveloperSettings Interface

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

public:
	/** Get the color for a folder */
	bool GetFolderColor(const FString& FolderName, FLinearColor& OutColor) const;

	TArray<FFavouriteFoldersToColor> GetFolderColorMappings() const { return FolderColorMappings; }
	

protected:
	/** The list of folders to color in the content browser */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Global|Content Browser")
	TArray<FFavouriteFoldersToColor> FolderColorMappings;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Global|Content Browser")
	bool bAllowPartialMatch = true;

	/** The current developer name */
	UPROPERTY(VisibleDefaultsOnly, Category = "Developer")
	FString DeveloperName;

	/** The list of folders to color in the content browser (developer-/user specific) */
	UPROPERTY(VisibleDefaultsOnly, Category = "Developer|Content Browser")
	TArray<FFavouriteFoldersToColor> DeveloperFolderColorMappings;

	/** Set to true if you want to refresh the content browser after changing the settings */
	//UPROPERTY(Config, EditDefaultsOnly, Category = "Content Browser")
	bool bRefreshContentBrowser = false;
};
