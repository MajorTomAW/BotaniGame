// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "UObject/Interface.h"
#include "BotaniTeamAgentInterface.generated.h"

template <typename InterfaceType> class TScriptInterface;

/** Delegate for when a team index changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBotaniTeamIndexChangedDelegate, UObject*, ObjectChangingTeam, int32, OldTeamID, int32, NewTeamID);

inline int32 GenericTeamIdToInteger(FGenericTeamId ID)
{
	return (ID == FGenericTeamId::NoTeam) ? INDEX_NONE : static_cast<int32>(ID);
}

inline FGenericTeamId IntegerToGenericTeamId(int32 ID)
{
	return (ID == INDEX_NONE) ? FGenericTeamId::NoTeam : FGenericTeamId(static_cast<uint8>(ID));
}

/**
 * UBotaniTeamAgentInterface
 *
 * Interface for actors which can be associated with teams
 */
UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class UBotaniTeamAgentInterface : public UGenericTeamAgentInterface
{
	GENERATED_UINTERFACE_BODY()
};

class BOTANIGAME_API IBotaniTeamAgentInterface : public IGenericTeamAgentInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual FOnBotaniTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() { return nullptr; }

	static void ConditionalBroadcastTeamChanged(TScriptInterface<IBotaniTeamAgentInterface> This, FGenericTeamId OldTeamID, FGenericTeamId NewTeamID);

	FOnBotaniTeamIndexChangedDelegate& GetTeamChangedDelegateChecked()
	{
		FOnBotaniTeamIndexChangedDelegate* Result = GetOnTeamIndexChangedDelegate();
		check(Result);
		return *Result;
	}
};
