// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BotaniSystemStatics.generated.h"


class UCommonSession_HostSessionRequest;

UCLASS(MinimalAPI)
class UBotaniSystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns the botani game instance object */
	UFUNCTION(BlueprintPure, Category = "Game", meta = (WorldContext = "WorldContextObject"))
	static BOTANIGAME_API class UBotaniGameInstance* GetBotaniGameInstance(const UObject* WorldContextObject);

	/** Returns the botani game mode object */
	UFUNCTION(BlueprintPure, Category = "Game", meta = (WorldContext = "WorldContextObject"))
	static BOTANIGAME_API class ABotaniGameModeBase* GetBotaniGameMode(const UObject* WorldContextObject);




	/** Creates a new hosting request for the botani game */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Online", meta = (WorldContext = "WorldContextObject"))
	static UCommonSession_HostSessionRequest* CreateHostingRequest(const UObject* WorldContextObject, FPrimaryAssetId MapId);
};
