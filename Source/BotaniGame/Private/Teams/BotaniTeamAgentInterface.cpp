// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Teams/BotaniTeamAgentInterface.h"

#include "BotaniLogChannels.h"

UBotaniTeamAgentInterface::UBotaniTeamAgentInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void IBotaniTeamAgentInterface::ConditionalBroadcastTeamChanged(TScriptInterface<IBotaniTeamAgentInterface> This, FGenericTeamId OldTeamID, FGenericTeamId NewTeamID)
{
	if (OldTeamID == NewTeamID)
	{
		return;
	}

	const int32 OldTeamIndex = GenericTeamIdToInteger(OldTeamID);
	const int32 NewTeamIndex = GenericTeamIdToInteger(NewTeamID);
	UObject* ThisObj = This.GetObject();

	UE_LOG(LogTemp, Display, TEXT("[%s] %s assigned team %d"), *GetClientServerContextString(ThisObj), *GetPathNameSafe(ThisObj), NewTeamIndex);

	This.GetInterface()->GetTeamChangedDelegateChecked().Broadcast(ThisObj, OldTeamIndex, NewTeamIndex);
}
