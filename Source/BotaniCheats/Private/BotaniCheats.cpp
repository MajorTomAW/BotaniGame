#include "BotaniCheats.h"

#include "ConsoleSettings.h"
#include "Engine/Console.h"
#include "Extensions/InventoryCheatManagerExtension.h"

#define LOCTEXT_NAMESPACE "FBotaniCheatsModule"

void FBotaniCheatsModule::StartupModule()
{
#if ALLOW_CONSOLE
	UConsole::RegisterConsoleAutoCompleteEntries.AddStatic(&UInventoryCheatManagerExtension::PopulateAutoCompleteEntries);
#endif
}

void FBotaniCheatsModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FBotaniCheatsModule, BotaniCheats)