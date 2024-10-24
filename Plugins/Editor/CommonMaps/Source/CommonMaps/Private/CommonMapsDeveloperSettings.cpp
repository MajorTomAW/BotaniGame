// Copyright © 2024 MajorT. All rights reserved.


#include "CommonMapsDeveloperSettings.h"

UCommonMapsDeveloperSettings::UCommonMapsDeveloperSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bPromptUserToSaveMap = true;
	CommonMaps.Add(FName(TEXT("Testing")), FCommonMapContainer());
	CommonMaps.Add(FName(TEXT("Production")), FCommonMapContainer());
}

UCommonMapsDeveloperSettings& UCommonMapsDeveloperSettings::Get()
{
	return *GetMutableDefault<UCommonMapsDeveloperSettings>();
}

FName UCommonMapsDeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}
