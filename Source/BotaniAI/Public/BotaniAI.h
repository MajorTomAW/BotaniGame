#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBotaniAIModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
