// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BotaniIndicatorStatics.generated.h"

class UBotaniIndicatorManagerComponent;

UCLASS()
class BOTANIUI_API UBotaniIndicatorStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UBotaniIndicatorStatics();

	/** Returns the indicator manager component associated with the specified controller. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	static UBotaniIndicatorManagerComponent* GetIndicatorManagerComponent(AController* Controller);
};
