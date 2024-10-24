// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PlaysetDeveloperSettings.generated.h"


class USlateBrushAsset;

USTRUCT()
struct FPlaysetCategoryDetails
{
	GENERATED_BODY()

	FPlaysetCategoryDetails()
	{
	}

	FPlaysetCategoryDetails(FName InCategory, const TArray<TSubclassOf<class UPlayset>>& InPlaysetClasses)
		: Category(InCategory)
		, PlaysetClasses(InPlaysetClasses)
	{
	}

	UPROPERTY(EditDefaultsOnly, Category = "General")
	FName Category = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category = "General")
	TArray<TSubclassOf<class UPlayset>> PlaysetClasses;
};

USTRUCT()
struct FPlaysetCategoryWrapper
{
	GENERATED_BODY()

	FPlaysetCategoryWrapper()
	{
	}

	FPlaysetCategoryWrapper(FName InCategory, const TArray<TSubclassOf<class UPlayset>>& InPlaysetClasses)
		: Category(InCategory, InPlaysetClasses)
	{
	}

	UPROPERTY(EditDefaultsOnly, Category = "General")
	FPlaysetCategoryDetails Category;

	UPROPERTY(EditDefaultsOnly, Category = "Style")
	TArray<FPlaysetCategoryDetails> SubCategories;
};

USTRUCT()
struct FPlaysetLevelBrowserCategoryInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "false", EditConditionHides))
	bool bIsBuiltIn = false;

	UPROPERTY(EditDefaultsOnly, Category = "General", meta = (EditCondition = "!bIsBuiltIn"))
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, Category = "General", meta = (EditCondition = "!bIsBuiltIn", EditConditionHides))
	FText ShortDisplayName;

	UPROPERTY(EditDefaultsOnly, Category = "General", meta = (EditCondition = "!bIsBuiltIn", EditConditionHides))
	FName UniqueHandle;

	UPROPERTY(EditDefaultsOnly, Category = "General", meta = (EditCondition = "!bIsBuiltIn", EditConditionHides))
	FName BrushIcon = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category = "General")
	int32 SortOrder = 0;

	UPROPERTY(EditAnywhere, Category = "Browser", meta = (AllowedClasses="/Script/GamePlaysets.Playset"))
	TSet<FSoftObjectPath> Playsets;
};

/**
 * Implements the settings for the Playset plugin.
 */
UCLASS(Config = Editor)
class GAMEPLAYSETSEDITOR_API UPlaysetDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPlaysetDeveloperSettings();
	static UPlaysetDeveloperSettings* Get();
	static void SaveToConfig();

	//~ Begin UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;
	virtual FName GetContainerName() const override;
	virtual FName GetSectionName() const override;
	//~ End UDeveloperSettings Interface

	FName GetCategoryForPlayset(const UClass* PlaysetClass) const;

public:
	/******************************* General *******************************/

	/** The category mapping for the playset. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "General")
	TArray<FPlaysetCategoryWrapper> CategoryMapping;

	/** The default directory for playsets. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "General", meta = (RelativeToGameContentDir, LongPackageName))
	FDirectoryPath DefaultPlaysetDirectory;

	/******************************* Browser *******************************/

	/** The category mapping for the playset browser. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Browser")
	TArray<FPlaysetLevelBrowserCategoryInfo> BrowserCategoryMapping;
	
	/******************************* Style *******************************/

	/** The color of the playset. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Style", meta = (ColorPicker))
	FLinearColor PlaysetColor;

	/** The default background slate brush for the playset. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Style")
	TSoftObjectPtr<USlateBrushAsset> DefaultBackgroundBrush;

	/******************************* Advanced *******************************/

	/** Whether to destroy all attached actors when the playset is destroyed. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Advanced")
	uint8 bDestroyAttachedActors : 1;
};