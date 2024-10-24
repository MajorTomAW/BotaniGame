// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Input/Component/BotaniInputComponent.h"

UBotaniInputComponent::UBotaniInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBotaniInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
