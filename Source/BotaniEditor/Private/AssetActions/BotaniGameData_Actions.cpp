// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BotaniGameData_Actions.h"

#include "GameFeatures/Data/BotaniGameData.h"
#define LOCTEXT_NAMESPACE "FBotaniGameData_Actions"

FBotaniGameData_Actions::FBotaniGameData_Actions()
{
}

FText FBotaniGameData_Actions::GetName() const
{
	return LOCTEXT("BotaniGameData", "Game Data");
}

const TArray<FText>& FBotaniGameData_Actions::GetSubMenus() const
{
	static const TArray<FText> SubMenus
		{
			LOCTEXT("GameDataSubmenu", "Advanced")
		};

	return SubMenus;
}

UClass* FBotaniGameData_Actions::GetSupportedClass() const
{
	return UBotaniGameData::StaticClass();
}
#undef LOCTEXT_NAMESPACE