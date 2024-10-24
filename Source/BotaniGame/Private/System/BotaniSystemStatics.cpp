// Copyright © 2024 Botanibots Team. All rights reserved.


#include "System/BotaniSystemStatics.h"

#include "CommonSessionSubsystem.h"
#include "Game/BotaniGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "System/BotaniGameInstance.h"


UBotaniSystemStatics::UBotaniSystemStatics(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UBotaniGameInstance* UBotaniSystemStatics::GetBotaniGameInstance(const UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance ? Cast<UBotaniGameInstance>(GameInstance) : nullptr;
}

ABotaniGameModeBase* UBotaniSystemStatics::GetBotaniGameMode(const UObject* WorldContextObject)
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(WorldContextObject);
	return GameMode ? Cast<ABotaniGameModeBase>(GameMode) : nullptr;
}

UCommonSession_HostSessionRequest* UBotaniSystemStatics::CreateHostingRequest(const UObject* WorldContextObject, FPrimaryAssetId MapId)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	UCommonSession_HostSessionRequest* Result = nullptr;

	if (UCommonSessionSubsystem* SessionSub = GameInstance ? GameInstance->GetSubsystem<UCommonSessionSubsystem>() : nullptr )
	{
		Result = SessionSub->CreateOnlineHostSessionRequest();
	}

	// Failsafe
	if (Result == nullptr)
	{
		Result = NewObject<UCommonSession_HostSessionRequest>();
		Result->OnlineMode = ECommonSessionOnlineMode::Online;
		Result->bUseLobbies = true;
	}

	Result->MapID = MapId;
	Result->ModeNameForAdvertisement = TEXT("My Own Session!");
	Result->MaxPlayerCount = 5;

	return Result;
}
