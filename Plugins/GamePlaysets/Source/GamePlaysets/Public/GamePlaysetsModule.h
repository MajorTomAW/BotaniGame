// Copyright © 2024 MajorT. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IGamePlaysetsModule : public IModuleInterface
{
public:
	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 * 
	 * @return Singleton instance, loading the module on demand if needed 
	 */
	static IGamePlaysetsModule& Get()
	{
		return FModuleManager::LoadModuleChecked<IGamePlaysetsModule>("GamePlaysets");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 * 
	 * @return True if the module is loaded and ready to use
	 */
	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("GamePlaysets");
	}
};
