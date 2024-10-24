// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActionWidget.h"
#include "BotaniActionWidget.generated.h"

class UEnhancedInputLocalPlayerSubsystem;

/**
 * UBotaniActionWidget
 *
 * An action widget that will get the icon of the input key
 * that is currently assigned to the common input action on this widget.
 */
UCLASS(Blueprintable, BlueprintType)
class UBotaniActionWidget : public UCommonActionWidget
{
	GENERATED_BODY()

public:
	UBotaniActionWidget();
	
	//~ Begin UCommonActionWidget interface
	virtual FSlateBrush GetIcon() const override;
	//~ End UCommonActionWidget interface

public:
	/** The Enhanced Input Action that is associated with this Common Input action. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input")
	const TObjectPtr<class UInputAction> AssociatedInputAction;

private:
	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
};
