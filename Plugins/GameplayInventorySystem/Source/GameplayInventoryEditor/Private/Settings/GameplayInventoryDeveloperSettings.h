// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayInventoryDeveloperSettings.generated.h"

/**
 * UGameplayInventoryDeveloperSettings
 */
UCLASS(Config = DefaultEditorPerProjectUserSettings)
class GAMEPLAYINVENTORYEDITOR_API UGameplayInventoryDeveloperSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

public:
	static UGameplayInventoryDeveloperSettings* Get()
	{
		return GetMutableDefault<UGameplayInventoryDeveloperSettings>();
	}
	
	//~ Begin UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
	virtual FName GetContainerName() const override;
	//~ End UDeveloperSettings Interface

public:
	/** A list of item definitions that should be displayed in the "Common" section of the class picker. */
	UPROPERTY(EditDefaultsOnly, Category = "Editor", Config)
	TArray<TSoftClassPtr<class UGameplayInventoryItemDefinition>> FavouriteItemDefinitions;

	/** A list of equipment definitions that should be displayed in the "Common" section of the class picker. */
	UPROPERTY(EditDefaultsOnly, Category = "Editor", Config)
	TArray<TSoftClassPtr<class UGameplayEquipmentDefinition>> FavouriteEquipmentDefinitions;

	/** A keyword filter to apply to the class picker when selecting item definitions. */
	UPROPERTY(EditDefaultsOnly, Category = "Editor", Config)
	TArray<FName> KeywordFilter;

	/** An item class filter used to filter out classes that are not item definitions. */
	UPROPERTY(EditDefaultsOnly, Category = "Editor", Config)
	TSet<TSubclassOf<UGameplayInventoryItemDefinition>> ItemClassFilter;
};
