// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "BotaniButtonBase.generated.h"

/**
 * UBotaniButtonBase
 *
 * The base class for all buttons related to common ui and input handling.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class UBotaniButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UBotaniButtonBase();
	
	/** Called to set the button text */
	UFUNCTION(BlueprintCallable, Category = "Botani|Button")
	void SetButtonText(const FText InText);

protected:
	//~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	//~ End UUserWidget Interface

	//~ Begin UCommonButtonBase Interface
	virtual void UpdateInputActionWidget() override;
	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;
	//~ End UCommonButtonBase Interface

protected:
	void RefreshButtonText();

	/** Called to update the button text */
	UFUNCTION(BlueprintImplementableEvent, Category = "Botani|Button")
	void UpdateButtonText(const FText& InText);

	/** Called to update the button style */
	UFUNCTION(BlueprintImplementableEvent, Category = "Botani|Button")
	void UpdateButtonStyle();

private:
	/** Whether to override the button text */
	UPROPERTY(EditAnywhere, Category = "Button", meta = (InlineEditConditionToggle))
	uint8 bOverride_ButtonText : 1;

	/** The button text */
	UPROPERTY(EditAnywhere, Category = "Button", meta = (Editcondition = "bOverride_ButtonText"))
	FText ButtonText;
};
