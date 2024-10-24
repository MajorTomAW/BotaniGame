// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ConsoleSettings.h"
#include "GameFramework/CheatManager.h"
#include "InventoryCheatManagerExtension.generated.h"

/**
 * UInventoryCheatManagerExtension
 *
 * A cheat manager extension class used to manage inventory cheats.
 */
UCLASS(MinimalAPI)
class UInventoryCheatManagerExtension : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	static void PopulateAutoCompleteEntries(TArray<FAutoCompleteCommand>& AutoCompleteCommands);

protected:
	UFUNCTION(Exec)
	void GiveItem(FString ItemPath);
};
