// Copyright © 2024 MajorT. All rights reserved.


#include "AssetDefinition_InventoryDefinition.h"

#include "GameplayInventoryEditor.h"
#include "Application/ItemEditorApp.h"
#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Definitions/GameplayEquipmentDefinition.h"
#include "Settings/GameplayInventoryDeveloperSettings.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText UAssetDefinition_InventoryDefinition::GetAssetDisplayName() const
{
	return LOCTEXT("AssetDefinition_InventoryDefinition", "Item Definition");
}

FText UAssetDefinition_InventoryDefinition::GetAssetDisplayName(const FAssetData& AssetData) const
{
	if (UClass* Class = AssetData.GetClass())
	{
		FString ClassName = Class->GetName();

		if (UGameplayInventoryDeveloperSettings::Get()->ItemClassFilter.Contains(Class))
		{
			UClass* ParentClass = Class->GetSuperClass();
			if (ParentClass)
			{
				return GetAssetDisplayName(FAssetData(ParentClass->GetDefaultObject()));
			}
		}

		for (const FName& Key : UGameplayInventoryDeveloperSettings::Get()->KeywordFilter)
		{
			ClassName.RemoveFromStart(Key.ToString());
		}

		// Put a space between each word that starts with a capital letter
		for (int32 Index = 1; Index < ClassName.Len(); ++Index)
		{
			if (FChar::IsUpper(ClassName[Index]) && !FChar::IsUpper(ClassName[Index - 1]))
			{
				ClassName.InsertAt(Index, TEXT(" "));
				++Index;
			}
		}
		
		return FText::Format(LOCTEXT("AssetDefinition_InventoryDefinition", "{0}"), FText::FromString(ClassName));
	}
	
	return Super::GetAssetDisplayName();
}

TSoftClassPtr<UObject> UAssetDefinition_InventoryDefinition::GetAssetClass() const
{
	return UGameplayInventoryItemDefinition::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_InventoryDefinition::GetAssetCategories() const
{
	static const FAssetCategoryPath CategoryPaths[] = {
		FAssetCategoryPath(LOCTEXT("AssetCategory_Item", "Inventory"))
	};

	return CategoryPaths;
}

FAssetSupportResponse UAssetDefinition_InventoryDefinition::CanDuplicate(const FAssetData& InAsset) const
{
	return FAssetSupportResponse::Supported();
}

EAssetCommandResult UAssetDefinition_InventoryDefinition::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	const EToolkitMode::Type Mode = OpenArgs.GetToolkitMode();
	IGameplayInventoryEditorModule& InventoryEdModule = IGameplayInventoryEditorModule::Get();

	for (UGameplayInventoryItemDefinition* ItemDef : OpenArgs.LoadObjects<UGameplayInventoryItemDefinition>())
	{
		// Check if we already have an editor open for this item definition
		bool bFoundExistingEditor = false;
		if (ItemDef->EquipmentDefinition != nullptr)
		{
			FItemEditorApp* App = static_cast<FItemEditorApp*>(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(ItemDef->EquipmentDefinition, true));
			if (App != nullptr && App->GetItemDefinition() == nullptr)
			{
				App->InitItemEditorApp(Mode, OpenArgs.ToolkitHost, ItemDef);
				bFoundExistingEditor = true;
			}
		}

		if (!bFoundExistingEditor)
		{
			InventoryEdModule.CreateItemEditorApp(Mode, OpenArgs.ToolkitHost, ItemDef);
		}
	}

	return EAssetCommandResult::Handled;
}

#undef LOCTEXT_NAMESPACE
