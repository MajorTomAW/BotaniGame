// Copyright © 2024 Botanibots Team. All rights reserved.


#include "System/BotaniGameInstance.h"

#include "CommonUserSubsystem.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameplayTags/BotaniGameplayTags.h"

UBotaniGameInstance::UBotaniGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBotaniGameInstance::HandlerUserInitialized(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext)
{
	Super::HandlerUserInitialized(UserInfo, bSuccess, Error, RequestedPrivilege, OnlineContext);

	// If the user was initialized successfully, tell the local player to load their settings
	if (bSuccess && ensure(UserInfo))
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(GetLocalPlayerByIndex(UserInfo->LocalPlayerIndex));

		// There will not be a local player attached to the dedicated server user
		if (LocalPlayer)
		{
			// LocalPlayer->LoadSharedSettingsFromDisk(); TODO: Implement this function
		}
	}
}

void UBotaniGameInstance::Init()
{
	Super::Init();

	// Register our custom init states
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);
	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(BotaniGameplayTags::InitState::TAG_InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(BotaniGameplayTags::InitState::TAG_InitState_DataAvailable, false, BotaniGameplayTags::InitState::TAG_InitState_Spawned);
		ComponentManager->RegisterInitState(BotaniGameplayTags::InitState::TAG_InitState_DataInitialized, false, BotaniGameplayTags::InitState::TAG_InitState_DataAvailable);
		ComponentManager->RegisterInitState(BotaniGameplayTags::InitState::TAG_InitState_GameplayReady, false, BotaniGameplayTags::InitState::TAG_InitState_DataInitialized);
	}
}
