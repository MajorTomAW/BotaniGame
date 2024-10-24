// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniActivatableWidget.h"
#include "BotaniHUDLayout.generated.h"

/**
 * UBotaniHUDLayout
 *
 * Widget used to lay out the player's HUD (typically specified by an Add Widgets action in the experience)
 */
UCLASS(Abstract, Blueprintable, BlueprintType, meta = (DisplayName = "Botani HUD Layout", Category = "Botani|HUD", ShortTooltip = "Widget used to lay out the player's main HUD."))
class BOTANIUI_API UBotaniHUDLayout : public UBotaniActivatableWidget
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UWidget Interface
	virtual void NativeOnInitialized() override;
	//~ End UWidget Interface

protected:
	/** Handle the escape action (e.g. opening the escape menu) */
	void HandleEscapeAction();

protected:
	/** The class of the escape menu widget to display when the player presses the escape key */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;
};
