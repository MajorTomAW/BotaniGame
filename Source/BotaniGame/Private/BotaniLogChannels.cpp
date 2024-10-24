// Copyright © 2024 Botanibots Team. All rights reserved.


#include "BotaniLogChannels.h"

DEFINE_LOG_CATEGORY(LogBotani);
DEFINE_LOG_CATEGORY(LogBotaniMovement);
DEFINE_LOG_CATEGORY(LogBotaniUI);
DEFINE_LOG_CATEGORY(LogBotaniInput);
DEFINE_LOG_CATEGORY(LogBotaniGameFeatures);
DEFINE_LOG_CATEGORY(LogBotaniAbilitySystem);
DEFINE_LOG_CATEGORY(LogBotaniExperience);

FString GetClientServerContextString(UObject* ContextObject)
{
	ENetRole Role = ROLE_None;

	if (const AActor* Actor = Cast<AActor>(ContextObject))
	{
		Role = Actor->GetLocalRole();
	}
	else if (const UActorComponent* Component = CastChecked<UActorComponent>(ContextObject))
	{
		Role = Component->GetOwnerRole();
	}

	if (Role != ROLE_None)
	{
		return (Role == ROLE_Authority) ? TEXT("Server") : TEXT("Client");
	}

#if WITH_EDITOR
	if (GIsEditor)
	{
		extern ENGINE_API FString GPlayInEditorContextString;
		return GPlayInEditorContextString;
	}
#endif

	return TEXT("[]");
}
