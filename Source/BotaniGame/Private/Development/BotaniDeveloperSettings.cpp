// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Development/BotaniDeveloperSettings.h"

//////////////////////////////////////////////////////////////////////////
/// UBotaniDeveloperSettings

UBotaniDeveloperSettings::UBotaniDeveloperSettings()
{
}

UBotaniDeveloperSettings* UBotaniDeveloperSettings::Get()
{
	return GetMutableDefault<UBotaniDeveloperSettings>();
}

FName UBotaniDeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

FName UBotaniDeveloperSettings::GetContainerName() const
{
	return "Editor";
}

