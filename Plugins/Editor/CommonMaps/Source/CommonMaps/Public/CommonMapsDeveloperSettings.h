// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CommonMapsDeveloperSettings.generated.h"

/**
 * FCommonMapContainer
 *
 * A Container that holds the soft reference to the map.
 */
USTRUCT(BlueprintType)
struct FCommonMapContainer
{
	GENERATED_BODY()

public:
	FCommonMapContainer() {}

	explicit FCommonMapContainer(const TArray<FSoftObjectPath>& InPaths)
		: MapURL(InPaths)
	{
	}

	/** The soft reference to the map. */
	UPROPERTY(EditAnywhere, Category = "Common Maps", meta = (AllowedClasses="/Script/Engine.World"))
	TSet<FSoftObjectPath> MapURL;
};

/**
 * UCommonMapsDeveloperSettings
 *
 * Developer settings for the CommonMaps plugin.
 * Defines a list of maps to be used for the common map list.
 */
UCLASS(MinimalAPI, Config = EditorPerProjectUserSettings, meta = (DisplayName = "Common Maps"))
class UCommonMapsDeveloperSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

public:
	static UCommonMapsDeveloperSettings& Get();
	
	//~ Begin UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;
	//~ End UDeveloperSettings Interface

	/** The list of common maps to be used for the common map list. */
	UPROPERTY(EditAnywhere, Config, Category = "Common Maps", meta = (ForceInlineRow))
	TMap<FName, FCommonMapContainer> CommonMaps;

	/** Whether to prompt the user to save the map when opening a common map. */
	UPROPERTY(EditAnywhere, Config, Category = "Common Maps")
	bool bPromptUserToSaveMap;
};
