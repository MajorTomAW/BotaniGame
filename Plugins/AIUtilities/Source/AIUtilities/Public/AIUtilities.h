// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Modules/ModuleManager.h"

namespace AIUtilityTags
{
	namespace InputProvider
	{
		AIUTILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputProvider);
		AIUTILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputProvider_Distance);	
	}

	namespace Action
	{
		AIUTILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Action);
	}
}

class FAIUtilitiesModule : public IModuleInterface
{
public:
	//~ Begin IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface Interface
};
