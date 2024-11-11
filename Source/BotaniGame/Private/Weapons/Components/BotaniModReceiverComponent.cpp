// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Weapons/Components/BotaniModReceiverComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniModReceiverComponent)

UBotaniModReceiverComponent::UBotaniModReceiverComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UBotaniModReceiverComponent::TickComponent(
	float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
