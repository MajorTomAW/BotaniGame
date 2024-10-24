// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Bot/BotaniBotController.h"

#include "Net/UnrealNetwork.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniBotController)

ABotaniBotController::ABotaniBotController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BotState = BotState_Unaware;
	StressLevel = 0.f;
}