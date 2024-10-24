// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BotaniInputConfig_Actions.h"

#include "Input/BotaniInputConfig.h"
#define LOCTEXT_NAMESPACE "FBotaniInputConfig_Actions"


FBotaniInputConfig_Actions::FBotaniInputConfig_Actions()
{
}

FText FBotaniInputConfig_Actions::GetName() const
{
	return LOCTEXT("BotaniInputConfig", "Input Config");
}

bool FBotaniInputConfig_Actions::CanFilter()
{
	return true;
}

UClass* FBotaniInputConfig_Actions::GetSupportedClass() const
{
	return UBotaniInputConfig::StaticClass();
}

FColor FBotaniInputConfig_Actions::GetTypeColor() const
{
	return FColor(255, 255, 127);
}

FText FBotaniInputConfig_Actions::GetDisplayNameFromAssetData(const FAssetData& AssetData) const
{
	return LOCTEXT("BotaniInputConfig", "Input Config");
}
#undef LOCTEXT_NAMESPACE