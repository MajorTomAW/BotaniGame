// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "BotaniIndicatorLayer.generated.h"

class SActorCanvas;

/**
 * UBotaniIndicatorLayer
 *
 * A single layer on the HUD that can be used to render indicators.
 */
UCLASS(MinimalAPI)
class UBotaniIndicatorLayer : public UWidget
{
	GENERATED_UCLASS_BODY()

public:
	/** Default arrow brush to use if UI is clamped to the screen and needs to show an arrow. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FSlateBrush ArrowBrush;

protected:
	//~ Begin UWidget Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

protected:
	TSharedPtr<SActorCanvas> MyActorCanvas;
};
