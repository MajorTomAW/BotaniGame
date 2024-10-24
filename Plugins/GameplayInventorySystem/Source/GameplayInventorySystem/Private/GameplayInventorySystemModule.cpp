// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayInventorySystemModule.h"

#include "GameplayDebugger.h"
#include "Components/GameplayInventoryManager.h"
#include "Debugging/GameplayDebuggerCategory_Inventory.h"
#include "GameFramework/HUD.h"

#define LOCTEXT_NAMESPACE "FGameplayInventorySystemModule"

void FGameplayInventorySystemModule::StartupModule()
{
#if WITH_GAMEPLAY_DEBUGGER
	IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
	GameplayDebuggerModule.RegisterCategory("Inventory", IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_Inventory::MakeInstance));
	GameplayDebuggerModule.NotifyCategoriesChanged();
#endif

	if (!IsRunningDedicatedServer())
	{
		AHUD::OnShowDebugInfo.AddStatic(&UGameplayInventoryManager::OnShowDebugInfo);
	}
}

void FGameplayInventorySystemModule::ShutdownModule()
{
#if WITH_GAMEPLAY_DEBUGGER
	if (IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
		GameplayDebuggerModule.UnregisterCategory("Inventory");
		GameplayDebuggerModule.NotifyCategoriesChanged();
	}
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGameplayInventorySystemModule, GameplayInventorySystem)