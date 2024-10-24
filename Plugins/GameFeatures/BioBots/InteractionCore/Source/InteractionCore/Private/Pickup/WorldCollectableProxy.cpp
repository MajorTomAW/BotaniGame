// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Pickup/WorldCollectableProxy.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(WorldCollectableProxy)

AWorldCollectableProxy::AWorldCollectableProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AWorldCollectableProxy::GatherInteractionOptions(
	const FBotaniInteractionQuery& InteractionQuery, FBotaniInteractionOptionBuilder& OptionBuilder)
{
	OptionBuilder.AddInteractionOption(InteractionOption);
}
