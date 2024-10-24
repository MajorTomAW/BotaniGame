// Copyright © 2024 Botanibots Team. All rights reserved.


#include "ActorFactory_BotaniPlayerStart.h"
#include "Player/BotaniPlayerStart.h"

#define LOCTEXT_NAMESPACE "BotaniEditor"

UActorFactory_BotaniPlayerStart::UActorFactory_BotaniPlayerStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = LOCTEXT("BotaniEditor", "Botani Player Start");
	NewActorClass = ABotaniPlayerStart::StaticClass();
}


#undef LOCTEXT_NAMESPACE