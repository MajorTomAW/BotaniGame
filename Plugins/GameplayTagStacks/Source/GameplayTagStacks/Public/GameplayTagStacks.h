// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FGameplayTagStacksModule : public IModuleInterface
{
public:

	//~ Begin IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface interface
};
