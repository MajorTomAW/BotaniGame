// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniEditor.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"
#include "AssetTypeActions/AssetTypeActions_DataAsset.h"

//////////////////////////////////////////////////////////////////////////
/// Experience Parent
//////////////////////////////////////////////////////////////////////////

class FSharedBotaniEditorData
{
public:
	static const TArray<FText> ExperienceSubMenus;
	static const TArray<FText> PawnSubMenus;
	static const TArray<FText> DataSubMenus;
};

class FBotaniExperience_BlueprintActions : public FAssetTypeActions_Blueprint, public FSharedBotaniEditorData
{
public:
	FBotaniExperience_BlueprintActions() {}

	//~ Begin IAssetTypeActions Interface
	virtual uint32 GetCategories() override { return FBotaniEditorModule::GetExperienceAssetsCategory(); }
	//~ End IAssetTypeActions Interface
};

class FBotaniExperience_DataActions : public FAssetTypeActions_DataAsset, public FSharedBotaniEditorData
{
public:
	FBotaniExperience_DataActions() {}

	//~ Begin IAssetTypeActions Interface
	virtual bool CanFilter() override { return true; }
	virtual FText GetDisplayNameFromAssetData(const FAssetData& AssetData) const override;
	virtual uint32 GetCategories() override { return FBotaniEditorModule::GetExperienceAssetsCategory(); }
	//~ End IAssetTypeActions Interface
};

//////////////////////////////////////////////////////////////////////////
/// Experience Definition
//////////////////////////////////////////////////////////////////////////

class FBotaniExperienceDefinition_Actions : public FBotaniExperience_BlueprintActions
{
public:
	FBotaniExperienceDefinition_Actions() {}
	
	//~ Begin IAssetTypeActions Interface
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	//~ End IAssetTypeActions Interface
};

//////////////////////////////////////////////////////////////////////////
/// Experience Action Set
//////////////////////////////////////////////////////////////////////////

class FBotaniExperienceActionSet_Actions final : public FBotaniExperience_DataActions
{
public:
	FBotaniExperienceActionSet_Actions() {}

	//~ Begin IAssetTypeActions Interface
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual const TArray<FText>& GetSubMenus() const override;
	//~ End IAssetTypeActions Interface
};

//////////////////////////////////////////////////////////////////////////
/// User Experience Playlist
//////////////////////////////////////////////////////////////////////////

class FBotaniUserExperiencePlaylist_Actions final : public FBotaniExperience_DataActions
{
public:
	FBotaniUserExperiencePlaylist_Actions() {}

	//~ Begin IAssetTypeActions Interface
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual const TArray<FText>& GetSubMenus() const override;
	//~ End IAssetTypeActions Interface
};

//////////////////////////////////////////////////////////////////////////
/// Ability Set
//////////////////////////////////////////////////////////////////////////

class FBotaniAbilitySet_Actions final : public FBotaniExperience_DataActions
{
public:
	FBotaniAbilitySet_Actions() {}

	//~ Begin IAssetTypeActions Interface
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual const TArray<FText>& GetSubMenus() const override;
	virtual uint32 GetCategories() override;
	//~ End IAssetTypeActions Interface
};

//////////////////////////////////////////////////////////////////////////
/// Gameplay Ability
//////////////////////////////////////////////////////////////////////////

class FBotaniGameplayAbility_Actions final : public FBotaniExperience_BlueprintActions
{
public:
	FBotaniGameplayAbility_Actions() {}

	//~ Begin IAssetTypeActions Interface
	virtual FText GetName() const override;
	virtual bool CanFilter() override { return true; }
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	//~ End IAssetTypeActions Interface
};

//////////////////////////////////////////////////////////////////////////
/// Gameplay Effect
//////////////////////////////////////////////////////////////////////////

class FBotaniGameplayEffect_Actions final : public FBotaniExperience_BlueprintActions
{
public:
	FBotaniGameplayEffect_Actions() {}

	//~ Begin IAssetTypeActions Interface
	virtual FText GetName() const override;
	virtual bool CanFilter() override { return true; }
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	//~ End IAssetTypeActions Interface
};