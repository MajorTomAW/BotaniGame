// Copyright © 2024 Botanibots Team. All rights reserved.


#include "GameplayInventoryDeveloperSettings.h"

FName UGameplayInventoryDeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

FText UGameplayInventoryDeveloperSettings::GetSectionText() const
{
	return FText::FromString(TEXT("Inventory System Settings"));
}

FText UGameplayInventoryDeveloperSettings::GetSectionDescription() const
{
	return FText::FromString(TEXT("Settings for the Gameplay Inventory System."));
}

FName UGameplayInventoryDeveloperSettings::GetContainerName() const
{
	return "Editor";
}
