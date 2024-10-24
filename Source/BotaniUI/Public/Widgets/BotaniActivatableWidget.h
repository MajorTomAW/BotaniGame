// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BotaniActivatableWidget.generated.h"

/**
 * EBotaniWidgetInputMode
 *
 * Describes the desired input mode to use while this UI is activated
 */
UENUM(BlueprintType)
enum class EBotaniWidgetInputMode : uint8
{
    /* Default input mode */
    Default,

    /* Input mode for both game and menu */
    GameAndMenu,

	/** Input mode for game */
    Game,

	/** Input mode for menu */
	Menu
};


/**
 * UBotaniActivatableWidget
 *
 * An activatable widget that automatically drives the desired input config when activated
 */
UCLASS(Abstract, Blueprintable, meta = (ShortTooltip = "An activatable widget that automatically drives the desired input config when activated."))
class BOTANIUI_API UBotaniActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UCommonActivatableWidget interface
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~ End UCommonActivatableWidget interface

#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, IWidgetCompilerLog& CompileLog) const override;
#endif

protected:
	/** The desired input mode to use while this UI is activated */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	EBotaniWidgetInputMode InputConfig = EBotaniWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};

