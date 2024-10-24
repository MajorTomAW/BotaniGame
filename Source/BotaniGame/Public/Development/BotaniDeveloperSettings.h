// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "BotaniDeveloperSettings.generated.h"

class ABotaniPlayerStart;
/**
 * Botani Developer Settings / Editor Cheats
 */
UCLASS(Config = EditorPerProjectUserSettings, MinimalAPI)
class UBotaniDeveloperSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	UBotaniDeveloperSettings();
	BOTANIGAME_API static UBotaniDeveloperSettings* Get();

	//~ Begin UDeveloperSettings interface
	virtual FName GetCategoryName() const override;
	virtual FName GetContainerName() const override;
	//~ End UDeveloperSettings interface

public:
	/** The experience override to use for Play in Editor (if not set, the default for the world settings of the open map will be used) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category = "Botani", meta = (AllowedTypes = "BotaniExperienceDefinition"))
	FPrimaryAssetId ExperienceOverride;
};

