// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BotaniEditorSettings.generated.h"

class UGameplayEffect;
class UBotaniGameplayAbility;
/**
 * Settings for the Botani Editor environment.
 */
UCLASS(DisplayName = "Botani Editor Settings", Config = DefaultEditorPerProjectUserSettings, MinimalAPI)
class UBotaniEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UBotaniEditorSettings();
	BOTANIEDITOR_API static UBotaniEditorSettings* Get();
	
	//~ Begin UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;
	virtual FName GetContainerName() const override;
	virtual FText GetSectionDescription() const override;
	virtual FText GetSectionText() const override;
	//~ End UDeveloperSettings Interface

public:
	BOTANIEDITOR_API TArray<UClass*> GetFavouriteAbilities() const;
	BOTANIEDITOR_API TArray<UClass*> GetFavouriteEffects() const;
	BOTANIEDITOR_API bool ShouldRenderRarityBackground() const { return bRenderRarityBackground; }

protected:
	/** List of favourite abilities which will be used to fill the "Common" section of the ability class picker. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category = "Ability System")
	TArray<TSoftClassPtr<UBotaniGameplayAbility>> FavouriteAbilities;

	/** List of favourite effects which will be used to fill the "Common" section of the effect class picker. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category = "Ability System")
	TArray<TSoftClassPtr<UGameplayEffect>> FavouriteEffects;

	/** Whether to render the item rarity as a thumbnail background. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category = "Item System", meta = (DisplayName = "Render Rarity Background"))
	uint8 bRenderRarityBackground : 1;

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext) override;
#endif

	void ApplySettings();
};
