// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Teams/Info/BotaniTeamInfoBase.h"

#include "Net/UnrealNetwork.h"
#include "Teams/Subsystem/BotaniTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniTeamInfoBase)


ABotaniTeamInfoBase::ABotaniTeamInfoBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TeamId(INDEX_NONE)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0f;

	SetReplicatingMovement(false);
}

void ABotaniTeamInfoBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TeamTags);
	DOREPLIFETIME_CONDITION(ThisClass, TeamId, COND_InitialOnly);
}

void ABotaniTeamInfoBase::BeginPlay()
{
	Super::BeginPlay();

	TryRegisterWithTeamSubsystem();
}

void ABotaniTeamInfoBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TeamId != INDEX_NONE)
	{
		if (UBotaniTeamSubsystem* TeamSub = GetWorld()->GetSubsystem<UBotaniTeamSubsystem>())
		{
			TeamSub->UnregisterTeamInfo(this);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void ABotaniTeamInfoBase::RegisterWithTeamSubsystem(UBotaniTeamSubsystem* Subsystem)
{
	Subsystem->RegisterTeamInfo(this);
}

void ABotaniTeamInfoBase::TryRegisterWithTeamSubsystem()
{
	if (TeamId == INDEX_NONE)
	{
		return;
	}

	UBotaniTeamSubsystem* TeamSub = GetWorld()->GetSubsystem<UBotaniTeamSubsystem>();
	if (ensure(TeamSub))
	{
		RegisterWithTeamSubsystem(TeamSub);
	}
}

void ABotaniTeamInfoBase::SetTeamId(int32 NewTeamId)
{
	check(HasAuthority());
	check(TeamId == INDEX_NONE);
	check(NewTeamId != INDEX_NONE);

	TeamId = NewTeamId;
	TryRegisterWithTeamSubsystem();
}

void ABotaniTeamInfoBase::OnRep_TeamId()
{
	TryRegisterWithTeamSubsystem();
}
