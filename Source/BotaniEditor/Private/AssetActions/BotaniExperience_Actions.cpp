// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BotaniExperience_Actions.h"

#include "AbilitySystem/BotaniAbilitySet.h"
#include "Game/Experience/BotaniExperienceDefinition.h"
#include "GameFeatures/Data/BotaniExperienceActionSet.h"

#define LOCTEXT_NAMESPACE "BotaniExperience_Actions"

const TArray<FText> FSharedBotaniEditorData::ExperienceSubMenus
	{
		LOCTEXT("GameDataSubmenu", "Experience")
	};

const TArray<FText> FSharedBotaniEditorData::PawnSubMenus
	{
		LOCTEXT("GameDataSubmenu", "Pawn")
	};

const TArray<FText> FSharedBotaniEditorData::DataSubMenus
	{
		LOCTEXT("GameDataSubmenu", "Data")
	};

FText FBotaniExperience_DataActions::GetDisplayNameFromAssetData(const FAssetData& AssetData) const
{
	const FString Path = AssetData.AssetName.ToString();
	FString Name, Type;
	Path.Split(".", &Type, &Name);
	
	return FText::FromString(Name);
}

//////////////////////////////////////////////////////////////////////////
/// Experience Definition
//////////////////////////////////////////////////////////////////////////

FText FBotaniExperienceDefinition_Actions::GetName() const
{
	return LOCTEXT("BotaniExperienceDefinition", "Experience Definition");
}

UClass* FBotaniExperienceDefinition_Actions::GetSupportedClass() const
{
	return UBotaniExperienceDefinition::StaticClass();
}

//////////////////////////////////////////////////////////////////////////
/// Experience Action Set
//////////////////////////////////////////////////////////////////////////

FText FBotaniExperienceActionSet_Actions::GetName() const
{
	return LOCTEXT("BotaniExperienceActionSet", "Experience Action Set");
}

UClass* FBotaniExperienceActionSet_Actions::GetSupportedClass() const
{
	return UBotaniExperienceActionSet::StaticClass();
}

const TArray<FText>& FBotaniExperienceActionSet_Actions::GetSubMenus() const
{
	return ExperienceSubMenus;
}

//////////////////////////////////////////////////////////////////////////
/// User Experience Playlist
//////////////////////////////////////////////////////////////////////////

FText FBotaniUserExperiencePlaylist_Actions::GetName() const
{
	return LOCTEXT("BotaniUserExperiencePlaylist", "User Experience Playlist");
}

UClass* FBotaniUserExperiencePlaylist_Actions::GetSupportedClass() const
{
	return FBotaniExperience_DataActions::GetSupportedClass();
}

const TArray<FText>& FBotaniUserExperiencePlaylist_Actions::GetSubMenus() const
{
	return ExperienceSubMenus;
}

//////////////////////////////////////////////////////////////////////////
/// Ability Set
//////////////////////////////////////////////////////////////////////////

FText FBotaniAbilitySet_Actions::GetName() const
{
	return LOCTEXT("BotaniAbilitySet", "Ability Set");
}

UClass* FBotaniAbilitySet_Actions::GetSupportedClass() const
{
	return UBotaniAbilitySet::StaticClass();
}

const TArray<FText>& FBotaniAbilitySet_Actions::GetSubMenus() const
{
	return DataSubMenus;
}

uint32 FBotaniAbilitySet_Actions::GetCategories()
{
	return FBotaniEditorModule::GetAbilitySystemCategory();
}

//////////////////////////////////////////////////////////////////////////
/// Gameplay Ability
//////////////////////////////////////////////////////////////////////////

FText FBotaniGameplayAbility_Actions::GetName() const
{
	return LOCTEXT("BotaniGameplayAbility", "Gameplay Ability");
}

UClass* FBotaniGameplayAbility_Actions::GetSupportedClass() const
{
	return UBotaniGameplayAbility::StaticClass();
}

uint32 FBotaniGameplayAbility_Actions::GetCategories()
{
	return FBotaniEditorModule::GetAbilitySystemCategory();
}

//////////////////////////////////////////////////////////////////////////
/// Gameplay Effect
//////////////////////////////////////////////////////////////////////////

FText FBotaniGameplayEffect_Actions::GetName() const
{
	return LOCTEXT("BotaniGameplayEffect", "Gameplay Effect");
}

UClass* FBotaniGameplayEffect_Actions::GetSupportedClass() const
{
	return UGameplayEffect::StaticClass();
}

uint32 FBotaniGameplayEffect_Actions::GetCategories()
{
	return FBotaniEditorModule::GetAbilitySystemCategory();
}

#undef LOCTEXT_NAMESPACE
