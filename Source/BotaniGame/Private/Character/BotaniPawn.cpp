// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Character/BotaniPawn.h"

#include "BotaniLogChannels.h"
#include "Net/UnrealNetwork.h"


ABotaniPawn::ABotaniPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ABotaniPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MyTeamID);
}

void ABotaniPawn::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ABotaniPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ABotaniPawn::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;
	
	Super::PossessedBy(NewController);

	// Grab the current team ID and listen for future changes
	if (IBotaniTeamAgentInterface* TeamAgent = Cast<IBotaniTeamAgentInterface>(NewController))
	{
		MyTeamID = TeamAgent->GetGenericTeamId();
		TeamAgent->GetOnTeamIndexChangedDelegate()->AddDynamic(this, &ABotaniPawn::OnControllerChangedTeam);
	}

	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ABotaniPawn::UnPossessed()
{
	AController* const OldController = Controller;

	// Stop listening for changes from the old controller
	const FGenericTeamId OldTeamID = MyTeamID;
	if (IBotaniTeamAgentInterface* TeamAgent = Cast<IBotaniTeamAgentInterface>(OldController))
	{
		TeamAgent->GetOnTeamIndexChangedDelegate()->RemoveAll(this);
	}
	
	Super::UnPossessed();

	// Determine what happens to the team ID when possession ends
	MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ABotaniPawn::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (GetController() == nullptr)
	{
		if (HasAuthority())
		{
			const FGenericTeamId OldTeamID = MyTeamID;
			MyTeamID = NewTeamID;
			ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
		}
		else
		{
			BOTANI_LOG(Error, TEXT("SetGenericTeamId called on a pawn (%s) that doesn't have authority!"), *GetPathNameSafe(this));
		}
	}
	else
	{
		BOTANI_LOG(Error, TEXT("SetGenericTeamId called on a pawn (%s) that is possessed as the team id is driven by the associated controller!"), *GetPathNameSafe(this));
	}
}

FGenericTeamId ABotaniPawn::GetGenericTeamId() const
{
	return MyTeamID;
}

FOnBotaniTeamIndexChangedDelegate* ABotaniPawn::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void ABotaniPawn::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ABotaniPawn::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	const FGenericTeamId MyOldTeamID = MyTeamID;
	MyTeamID = IntegerToGenericTeamId(NewTeam);
	ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
}
