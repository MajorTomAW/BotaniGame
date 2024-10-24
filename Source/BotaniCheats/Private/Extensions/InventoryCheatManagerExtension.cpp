// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Extensions/InventoryCheatManagerExtension.h"

#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Engine/AssetManager.h"

void UInventoryCheatManagerExtension::PopulateAutoCompleteEntries(TArray<FAutoCompleteCommand>& AutoCompleteCommands)
{
#if UE_WITH_CHEAT_MANAGER
	TArray<FAssetData> ItemAssets;
	static const FPrimaryAssetType ItemDefinitionType = TEXT("ItemDefinition");
	UAssetManager::Get().GetPrimaryAssetDataList(ItemDefinitionType, ItemAssets);

	const UConsoleSettings* ConsoleSettings = GetDefault<UConsoleSettings>();
	for (const FAssetData& Item : ItemAssets)
	{
		UGameplayInventoryItemDefinition* ItemDefinition = Cast<UGameplayInventoryItemDefinition>(Item.GetAsset());
		if (!ItemDefinition)
		{
			continue;
		}

		// Chop off all characters up to and including the last slash
		FString PathName = Item.PackageName.ToString();
		PathName = PathName.RightChop(PathName.Find(TEXT("/"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) + 1);

		// Add the command to the list
		FString ItemDesc = TEXT("Cheat the specified item.");

		if (!ItemDefinition->DisplayInfo.ItemDescription.IsEmpty())
		{
			ItemDesc = ItemDefinition->DisplayInfo.ItemDescription.ToString();
		}
		else if (!ItemDefinition->DisplayInfo.ItemShortDescription.IsEmpty())
		{
			ItemDesc = ItemDefinition->DisplayInfo.ItemShortDescription.ToString();
		}
		
		
		FAutoCompleteCommand AutoCompleteCommand;
		AutoCompleteCommand.Command = *FString::Printf(TEXT("GiveItem %s"), *Item.PackageName.ToString());
		AutoCompleteCommand.Desc = FString::Printf(TEXT("%s"), *ItemDesc);
		AutoCompleteCommand.Color = ConsoleSettings->AutoCompleteCommandColor;

		AutoCompleteCommands.Add(MoveTemp(AutoCompleteCommand));
	}
#endif
}

void UInventoryCheatManagerExtension::GiveItem(FString ItemPath)
{
}
