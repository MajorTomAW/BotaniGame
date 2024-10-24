// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "IndicatorWidgetInterface.generated.h"

class AActor;
class UBotaniIndicatorDescriptor;

UINTERFACE(BlueprintType)
class BOTANIUI_API UIndicatorWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class IIndicatorWidgetInterface
{
	GENERATED_BODY()
	
public:
	/** Bind the indicator to the widget. */
	UFUNCTION(BlueprintNativeEvent, Category = "Botani|Indicator")
	void BindIndicator(UBotaniIndicatorDescriptor* Indicator);

	/** Unbind the indicator from the widget. */
	UFUNCTION(BlueprintNativeEvent, Category = "Botani|Indicator")
	void UnbindIndicator(const UBotaniIndicatorDescriptor* Indicator);
};