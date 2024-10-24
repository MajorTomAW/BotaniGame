// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Messaging/BotaniVerbMessageHelpers.h"

#include "GameFramework/PlayerState.h"
#include "Messaging/BotaniVerbMessage.h"


//////////////////////////////////////////////////////////////////////////
/// BotaniVerbMessage
//////////////////////////////////////////////////////////////////////////

FString FBotaniVerbMessage::ToString() const
{
	FString DebugString;
	StaticStruct()->ExportText(/*out*/ DebugString, this, /*Defaults=*/ nullptr, /*OwnerObject=*/ nullptr, PPF_None, /*ExportRootScope=*/ nullptr);
	return DebugString;
}

//////////////////////////////////////////////////////////////////////////
/// BotaniVerbMessageHelpers
//////////////////////////////////////////////////////////////////////////

APlayerState* UBotaniVerMessageHelpers::GetPlayerStateFromObject(UObject* Object)
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(Object))
	{
		return PlayerController->PlayerState;
	}

	if (APlayerState* PlayerState = Cast<APlayerState>(Object))
	{
		return PlayerState;
	}

	if (const APawn* Pawn = Cast<APawn>(Object))
	{
		if (APlayerState* PlayerState = Pawn->GetPlayerState())
		{
			return PlayerState;
		}
	}

	return nullptr;
}
