// Copyright © 2024 MajorT. All rights reserved.
#include "GamePlaysetsModule.h"

#define LOCTEXT_NAMESPACE "GamePlaysetsModule"

class FGamePlaysetsModule : public IGamePlaysetsModule
{
public:
	//~ Begin IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface
};

IMPLEMENT_MODULE(FGamePlaysetsModule, GamePlaysets)

void FGamePlaysetsModule::StartupModule()
{
}

void FGamePlaysetsModule::ShutdownModule()
{
}
#undef LOCTEXT_NAMESPACE
