// Copyright © 2024 Botanibots Team. All rights reserved.


#include "IndicatorSystem/Library/BotaniIndicatorStatics.h"

#include "IndicatorSystem/Components/BotaniIndicatorManagerComponent.h"

UBotaniIndicatorStatics::UBotaniIndicatorStatics()
{
}

UBotaniIndicatorManagerComponent* UBotaniIndicatorStatics::GetIndicatorManagerComponent(AController* Controller)
{
	return UBotaniIndicatorManagerComponent::GetComponent(Controller);
}
