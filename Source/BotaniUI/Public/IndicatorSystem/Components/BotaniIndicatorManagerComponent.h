// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "BotaniIndicatorManagerComponent.generated.h"

/**
 * UBotaniIndicatorManagerComponent
 *
 * An indicator manager component that can be added to a controller to manage indicators.
 */
UCLASS(Blueprintable, BlueprintType)
class BOTANIUI_API UBotaniIndicatorManagerComponent : public UControllerComponent
{
	GENERATED_UCLASS_BODY()

public:
	static UBotaniIndicatorManagerComponent* GetComponent(AController* Controller);

	/** Add an indicator to the manager. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void AddIndicator(class UBotaniIndicatorDescriptor* IndicatorDescriptor);

	/** Remove an indicator from the manager. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Indicator")
	void RemoveIndicator(class UBotaniIndicatorDescriptor* IndicatorDescriptor);

	const TArray<UBotaniIndicatorDescriptor*>& GetIndicators() const { return Indicators; }

public:
	DECLARE_EVENT_OneParam(UBotaniIndicatorManagerComponent, FIndicatorEvent, UBotaniIndicatorDescriptor* Descriptor)
	FIndicatorEvent OnIndicatorAdded;
	FIndicatorEvent OnIndicatorRemoved;

private:
	/** Array of indicators managed by this component. */
	UPROPERTY()
	TArray<TObjectPtr<UBotaniIndicatorDescriptor>> Indicators;
};
