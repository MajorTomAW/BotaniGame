// Copyright © 2024 Botanibots Team. All rights reserved.


#include "GameFeatures/Data/BotaniGameplayTagData.h"

//////////////////////////////////////////////////////////////////////////
/// Failure Responses

UActivationFailureResponse_DisplayMessage::UActivationFailureResponse_DisplayMessage()
{
#if WITH_EDITORONLY_DATA
	ResponseName = TEXT("Display Message");
#endif
}

UActivationFailureResponse_PlaySound::UActivationFailureResponse_PlaySound()
{
#if WITH_EDITORONLY_DATA
	ResponseName = TEXT("Play Sound");
#endif
}


//////////////////////////////////////////////////////////////////////////
/// Gameplay Tag Data

UBotaniGameplayTagData::UBotaniGameplayTagData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}
