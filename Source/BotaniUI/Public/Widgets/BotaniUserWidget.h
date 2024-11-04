// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BotaniUserWidget.generated.h"

/**
 * UBotaniUserWidget
 *
 * Common widget class that contains botani-specific functionality. 
 */
UCLASS(Abstract)
class BOTANIUI_API UBotaniUserWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns the botani player controller. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Player")
	class ABotaniPlayerController* GetOwningBotaniPlayer() const;

	/** Returns the botani character. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Player")
	class ABotaniCharacter* GetOwningBotaniCharacter() const;

	/** Returns the botani local player. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Player")
	class UBotaniLocalPlayer* GetOwningBotaniLocalPlayer() const;
};
