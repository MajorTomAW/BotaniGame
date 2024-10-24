// Copyright © 2024 Botanibots Team. All rights reserved.


#include "IndicatorSystem/Components/BotaniIndicatorManagerComponent.h"

#include "IndicatorSystem/BotaniIndicatorDescriptor.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniIndicatorManagerComponent)

UBotaniIndicatorManagerComponent::UBotaniIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoRegister = true;
	bAutoActivate = true;
}

UBotaniIndicatorManagerComponent* UBotaniIndicatorManagerComponent::GetComponent(AController* Controller)
{
	if (Controller)
	{
		return Controller->FindComponentByClass<UBotaniIndicatorManagerComponent>();
	}

	return nullptr;
}

void UBotaniIndicatorManagerComponent::AddIndicator(class UBotaniIndicatorDescriptor* IndicatorDescriptor)
{
	IndicatorDescriptor->SetIndicatorManagerComponent(this);

	OnIndicatorAdded.Broadcast(IndicatorDescriptor);
	Indicators.Add(IndicatorDescriptor);
}

void UBotaniIndicatorManagerComponent::RemoveIndicator(class UBotaniIndicatorDescriptor* IndicatorDescriptor)
{
	if (IndicatorDescriptor)
	{
		ensure(IndicatorDescriptor->GetIndicatorManagerComponent() == this);

		OnIndicatorRemoved.Broadcast(IndicatorDescriptor);
		Indicators.Remove(IndicatorDescriptor);
	}
}
