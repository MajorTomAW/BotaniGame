// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "BotaniVerbMessageHelpers.generated.h"


UCLASS()
class BOTANIGAME_API UBotaniVerMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Attempts to get the player state from the given object */
	UFUNCTION(BlueprintCallable, Category = "Botani")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);
};