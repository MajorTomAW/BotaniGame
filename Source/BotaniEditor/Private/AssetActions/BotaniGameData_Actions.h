// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniExperience_Actions.h"

class FBotaniGameData_Actions : public FBotaniExperience_DataActions
{
public:
	FBotaniGameData_Actions();
	
	//~ Begin IAssetTypeActions Interface
	virtual FText GetName() const override;
	virtual const TArray<FText>& GetSubMenus() const override;
	virtual UClass* GetSupportedClass() const override;
	//~ End IAssetTypeActions Interface
};