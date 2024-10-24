// Copyright © 2024 MajorT. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FCommonMapsModule : public IModuleInterface
{
public:
	//~ Begin IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface interface

private:
	void ExtendContextMenu();
	void AddToCommonMapsMenu(FName CategoryName);
	void CreateCategorySelectionSubmenu(FMenuBuilder& MenuBuilder);

private:
	FDelegateHandle ToolsMenuHandle;
};
