// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Settings/BotaniEditorSettings.h"

#include "AbilitySystem/Abilities/BotaniGameplayAbility.h"

#if WITH_EDITOR
#include "UObject/ObjectSaveContext.h"
#endif

UBotaniEditorSettings::UBotaniEditorSettings()
{
	FavouriteAbilities.Add(UBotaniGameplayAbility::StaticClass());
	FavouriteEffects.Add(UGameplayEffect::StaticClass());
	bRenderRarityBackground = true;
}

UBotaniEditorSettings* UBotaniEditorSettings::Get()
{
	return GetMutableDefault<UBotaniEditorSettings>();
}

FName UBotaniEditorSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

FName UBotaniEditorSettings::GetContainerName() const
{
	return "Editor";
}

FText UBotaniEditorSettings::GetSectionDescription() const
{
	return NSLOCTEXT("BotaniEditorSettings", "Description", "Settings for the Botani Editor environment.");
}

FText UBotaniEditorSettings::GetSectionText() const
{
	return NSLOCTEXT("BotaniEditorSettings", "Section", "Botani Editor Settings");
}

TArray<UClass*> UBotaniEditorSettings::GetFavouriteAbilities() const
{
	TArray<UClass*> Results;
	for (TSoftClassPtr<UBotaniGameplayAbility> Ability : FavouriteAbilities)
	{
		if (Ability == nullptr)
		{
			continue;
		}

		Results.AddUnique(Ability.LoadSynchronous());
	}

	return Results;
}

TArray<UClass*> UBotaniEditorSettings::GetFavouriteEffects() const
{
	TArray<UClass*> Results;
	for (TSoftClassPtr<UGameplayEffect> Effect : FavouriteEffects)
	{
		if (Effect == nullptr)
		{
			continue;
		}

		Results.AddUnique(Effect.LoadSynchronous());
	}

	return Results;
}

#if WITH_EDITOR
void UBotaniEditorSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ApplySettings();
}

void UBotaniEditorSettings::PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext)
{
	Super::PostSaveRoot(ObjectSaveContext);

	ApplySettings();
}
#endif

void UBotaniEditorSettings::ApplySettings()
{
	// Save the settings to the config file
	SaveConfig();
}
