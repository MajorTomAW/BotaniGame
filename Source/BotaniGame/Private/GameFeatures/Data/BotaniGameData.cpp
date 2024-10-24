// Copyright © 2024 Botanibots Team. All rights reserved.


#include "GameFeatures/Data/BotaniGameData.h"

#include "System/BotaniAssetManager.h"

UBotaniGameData::UBotaniGameData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UBotaniGameData& UBotaniGameData::Get()
{
	return UBotaniAssetManager::Get().GetGameData();
}
