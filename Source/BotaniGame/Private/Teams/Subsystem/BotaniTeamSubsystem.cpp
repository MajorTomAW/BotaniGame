// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Teams/Subsystem/BotaniTeamSubsystem.h"

#include "AbilitySystemGlobals.h"
#include "BotaniLogChannels.h"
#include "GenericTeamAgentInterface.h"
#include "Player/BotaniPlayerState.h"
#include "Teams/BotaniTeamAgentInterface.h"
#include "Teams/Info/BotaniTeamInfoBase.h"
#include "Teams/Info/BotaniTeamPrivateInfo.h"
#include "Teams/Info/BotaniTeamPublicInfo.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniTeamSubsystem)

//////////////////////////////////////////////////////////////////////////
/// FBotaniTeamTrackingInfo
//////////////////////////////////////////////////////////////////////////

void FBotaniTeamTrackingInfo::SetTeamInfo(ABotaniTeamInfoBase* Info)
{
	if (ABotaniTeamPublicInfo* NewPublicInfo = Cast<ABotaniTeamPublicInfo>(Info))
	{
		ensure((PublicInfo == nullptr) || (PublicInfo == NewPublicInfo));
		PublicInfo = NewPublicInfo;

		UBotaniTeamDisplayAsset* OldDisplayAsset = DisplayAsset;
		DisplayAsset = PublicInfo->GetTeamDisplayAsset();

		if (OldDisplayAsset != DisplayAsset)
		{
			OnTeamDisplayAssetChanged.Broadcast(DisplayAsset);
		}
	}
	else if (ABotaniTeamPrivateInfo* NewPrivateInfo = Cast<ABotaniTeamPrivateInfo>(Info))
	{
		ensure((PrivateInfo == nullptr) || (PrivateInfo == NewPrivateInfo));
		PrivateInfo = NewPrivateInfo;
	}
	else
	{
		checkf(false, TEXT("Unknown team info type: %s"), *GetPathNameSafe(Info));
	}
}

void FBotaniTeamTrackingInfo::RemoveTeamInfo(ABotaniTeamInfoBase* Info)
{
	if (PublicInfo == Info)
	{
		PublicInfo = nullptr;
	}
	else if (PrivateInfo == Info)
	{
		PrivateInfo = nullptr;
	}
	else
	{
		checkf(false, TEXT("Unknown team info type: %s"), *GetPathNameSafe(Info));
	}
}

//////////////////////////////////////////////////////////////////////////
/// BotaniTeamSubsystem
//////////////////////////////////////////////////////////////////////////

UBotaniTeamSubsystem::UBotaniTeamSubsystem()
{
}

void UBotaniTeamSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UBotaniTeamSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UBotaniTeamSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	TArray<UClass*> ChildClasses;
	GetDerivedClasses(GetClass(), ChildClasses, false);

	// Only create an instance if there is not a game-specific subclass
	return ChildClasses.Num() == 0 || true; //@TODO: For now, always create the base class
}

bool UBotaniTeamSubsystem::RegisterTeamInfo(ABotaniTeamInfoBase* TeamInfo)
{
	if (!ensure(TeamInfo))
	{
		return false;
	}

	const int32 TeamId = TeamInfo->GetTeamId();
	if (ensure(TeamId != INDEX_NONE))
	{
		FBotaniTeamTrackingInfo& Entry = TeamMap.FindOrAdd(TeamId);
		Entry.SetTeamInfo(TeamInfo);

		return true;
	}

	return false;
}

bool UBotaniTeamSubsystem::UnregisterTeamInfo(ABotaniTeamInfoBase* TeamInfo)
{
	if (!ensure(TeamInfo))
	{
		return false;
	}

	const int32 TeamId = TeamInfo->GetTeamId();
	if (ensure(TeamId != INDEX_NONE))
	{
		// If we couldn't find the entry, it is probably a leftover actor from a previous world, so we can just ignore it
		if (FBotaniTeamTrackingInfo* Entry = TeamMap.Find(TeamId))
		{
			Entry->RemoveTeamInfo(TeamInfo);
			TeamMap.Remove(TeamId);

			return true;
		}
	}

	return false;
}

bool UBotaniTeamSubsystem::ChangeTeamForActor(AActor* Actor, int32 NewTeamIndex)
{
	const FGenericTeamId NewTeamId = IntegerToGenericTeamId(NewTeamIndex);
	if (ABotaniPlayerState* BotaniPlayerState = const_cast<ABotaniPlayerState*>(FindPlayerStateForActor(Actor)))
	{
		BotaniPlayerState->SetGenericTeamId(NewTeamId);
		return true;
	}

	if (IBotaniTeamAgentInterface* TeamAgent = Cast<IBotaniTeamAgentInterface>(Actor))
	{
		TeamAgent->SetGenericTeamId(NewTeamId);
		return true;
	}

	return false;
}

int32 UBotaniTeamSubsystem::FindTeamFromObject(const UObject* Object)
{
	// Check if we're already a team agent
	if (const IBotaniTeamAgentInterface* TeamAgent = Cast<IBotaniTeamAgentInterface>(Object))
	{
		return GenericTeamIdToInteger(TeamAgent->GetGenericTeamId());
	}

	if (const AActor* TestActor = Cast<AActor>(Object))
	{
		// Check if the instigator is a team agent
		if (const IBotaniTeamAgentInterface* TeamAgent = Cast<IBotaniTeamAgentInterface>(TestActor->GetInstigator()))
		{
			return GenericTeamIdToInteger(TeamAgent->GetGenericTeamId());
		}

		// TeamInfo actors don't have the team interface, so they need a special case
		if (const ABotaniTeamInfoBase* TeamInfo = Cast<ABotaniTeamInfoBase>(TestActor))
		{
			return TeamInfo->GetTeamId();
		}

		// Fall back to finding the associated player state
		if (const ABotaniPlayerState* PlayerState = FindPlayerStateForActor(TestActor))
		{
			return GenericTeamIdToInteger(PlayerState->GetGenericTeamId());
		}
	}

	return INDEX_NONE;
}

const ABotaniPlayerState* UBotaniTeamSubsystem::FindPlayerStateForActor(const AActor* Actor)
{
	if (Actor == nullptr)
	{
		return nullptr;
	}

	if (const APawn* Pawn = Cast<const APawn>(Actor))
	{
		//@TODO: Consider an interface instead or have team actors register with the subsystem and have it maintain a map? (or LWC style)

		if (ABotaniPlayerState* PlayerState = Pawn->GetPlayerState<ABotaniPlayerState>())
		{
			return PlayerState;
		}
	}
	else if (const AController* Controller = Cast<const AController>(Actor))
	{
		if (ABotaniPlayerState* PlayerState = Controller->PlayerState ? Cast<ABotaniPlayerState>(Controller->PlayerState) : nullptr)
		{
			return PlayerState;
		}
	}
	else if (const ABotaniPlayerState* BotaniPlayerState = Cast<const ABotaniPlayerState>(Actor))
	{
		return BotaniPlayerState;
	}

	return nullptr;
}

void UBotaniTeamSubsystem::NotifyTeamDisplayAssetChanged(const UBotaniTeamDisplayAsset* /*DisplayAsset*/)
{
	// Broadcast to all team observers
	for (const auto& KVP : TeamMap)
	{
		const FBotaniTeamTrackingInfo& Entry = KVP.Value;
		Entry.OnTeamDisplayAssetChanged.Broadcast(Entry.DisplayAsset);
	}
}

FOnBotaniTeamDisplayAssetChangedDelegate& UBotaniTeamSubsystem::GetTeamDisplayAssetChangedDelegate(int32 TeamId)
{
	return TeamMap.FindOrAdd(TeamId).OnTeamDisplayAssetChanged;
}

void UBotaniTeamSubsystem::FindTeamFromActor(const AActor* TestActor, bool& bIsPartOfTeam, int32& TeamId) const
{
	TeamId = FindTeamFromObject(TestActor);
	bIsPartOfTeam = TeamId != INDEX_NONE;
}

EBotaniTeamComparison UBotaniTeamSubsystem::CompareTeams(const AActor* ActorA, const AActor* ActorB, int32& TeamA, int32& TeamB)
{
	TeamA = FindTeamFromObject(ActorA);
	TeamB = FindTeamFromObject(ActorB);

	if ((TeamA == INDEX_NONE) || (TeamB == INDEX_NONE))
	{
		return EBotaniTeamComparison::InvalidArgument;
	}

	return (TeamA == TeamB) ? EBotaniTeamComparison::OnSameTeam : EBotaniTeamComparison::DifferentTeams;
}

EBotaniTeamComparison UBotaniTeamSubsystem::CompareTeams(const AActor* ActorA, const AActor* ActorB)
{
	int32 TeamA, TeamB;
	return CompareTeams(ActorA, ActorB, TeamA, TeamB);
}

bool UBotaniTeamSubsystem::CanCauseDamage(const UObject* Instigator, const UObject* Target, bool bAllowDamageToSelf)
{
	if (bAllowDamageToSelf)
	{
		if ((Instigator == Target) ||
			(FindPlayerStateForActor(Cast<AActor>(Instigator)) == FindPlayerStateForActor(Cast<AActor>(Target))))
		{
			return true;
		}
	}

	int32 InstigatorTeamId, TargetTeamId;
	const EBotaniTeamComparison Comparison = CompareTeams(Cast<AActor>(Instigator), Cast<AActor>(Target), InstigatorTeamId, TargetTeamId);

	if (Comparison == EBotaniTeamComparison::DifferentTeams)
	{
		return true;
	}

	if ((Comparison == EBotaniTeamComparison::InvalidArgument) || (InstigatorTeamId != INDEX_NONE))
	{
		return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Cast<AActor>(Target)) != nullptr;
	}

	//@TODO: Add friendly fire checks
	return false;
}

void UBotaniTeamSubsystem::AddTeamTagStack(int32 TeamId, const FGameplayTag& Tag, int32 StackCount)
{
	auto FailureHandle = [&] (const FString& ErrorMessage)
	{
		UE_LOG(LogBotani, Error, TEXT("AddTeamTagStack(TeamId=%d, Tag=%s, StackCount=%d) failed: %s"), TeamId, *Tag.ToString(), StackCount, *ErrorMessage);	
	};

	if (FBotaniTeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		if (Entry->PublicInfo)
		{
			if (Entry->PublicInfo->HasAuthority())
			{
				Entry->PublicInfo->TeamTags.AddStack(Tag, StackCount);
			}
			else
			{
				FailureHandle(TEXT("failed because the team info is not on the authority"));
			}
		}
		else
		{
			FailureHandle(TEXT("failed because there is no team info spawned yet (called too early, before the experience was ready)"));
		}
	}
	else
	{
		FailureHandle(TEXT("failed because the team does not exist"));
	}
}

void UBotaniTeamSubsystem::RemoveTeamTagStack(int32 TeamId, const FGameplayTag& Tag, int32 StackCount)
{
	auto FailureHandle = [&] (const FString& ErrorMessage)
	{
		UE_LOG(LogBotani, Error, TEXT("RemoveTeamTagStack(TeamId=%d, Tag=%s, StackCount=%d) failed: %s"), TeamId, *Tag.ToString(), StackCount, *ErrorMessage);	
	};

	if (FBotaniTeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		if (Entry->PublicInfo)
		{
			if (Entry->PublicInfo->HasAuthority())
			{
				Entry->PublicInfo->TeamTags.RemoveStack(Tag, StackCount);
			}
			else
			{
				FailureHandle(TEXT("failed because the team info is not on the authority"));
			}
		}
		else
		{
			FailureHandle(TEXT("failed because there is no team info spawned yet (called too early, before the experience was ready)"));
		}
	}
	else
	{
		FailureHandle(TEXT("failed because the team does not exist"));
	}
}

int32 UBotaniTeamSubsystem::GetTeamTagStackCount(int32 TeamId, const FGameplayTag& Tag) const
{
	if (const FBotaniTeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		const int32 PublicStackCount = (Entry->PublicInfo != nullptr) ? Entry->PublicInfo->TeamTags.GetStackCount(Tag) : 0;
		const int32 PrivateStackCount = (Entry->PrivateInfo != nullptr) ? Entry->PrivateInfo->TeamTags.GetStackCount(Tag) : 0;
		return PublicStackCount + PrivateStackCount;
	}

	BOTANI_LOG(Warning, TEXT("GetTeamTagStackCount(TeamId=%d, Tag=%s) failed because the team does not exist"), TeamId, *Tag.ToString());
	return 0;
}

bool UBotaniTeamSubsystem::TeamHasTag(int32 TeamId, const FGameplayTag& Tag) const
{
	return GetTeamTagStackCount(TeamId, Tag) > 0;
}

bool UBotaniTeamSubsystem::DoesTeamExist(int32 TeamId) const
{
	return TeamMap.Contains(TeamId);
}

TArray<int32> UBotaniTeamSubsystem::GetTeamIDs() const
{
	TArray<int32> TeamIDs;
	TeamMap.GenerateKeyArray(TeamIDs);
	TeamIDs.Sort();

	return TeamIDs;
}

UBotaniTeamDisplayAsset* UBotaniTeamSubsystem::GetTeamDisplayAsset(int32 TeamId, int32 ViewerTeamId)
{
	//@TODO: Adapt so that its taking the viewers perspective into account

	if (FBotaniTeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		return Entry->DisplayAsset;
	}

	return nullptr;
}

UBotaniTeamDisplayAsset* UBotaniTeamSubsystem::GetEffectiveTeamDisplayAsset(int32 TeamId, UObject* ViewerTeamAgent)
{
	return GetTeamDisplayAsset(TeamId, FindTeamFromObject(ViewerTeamAgent));
}

bool UBotaniTeamSubsystem::TryChangeTeamForActor(AActor* Actor, const bool bCreateNew, int32 NewTeamId, int32& OldTeamID) const
{
	if (!ensure(Actor))
	{
		return false;
	}

	if (!Actor->HasAuthority())
	{
		return false;
	}

	if (!DoesTeamExist(NewTeamId) && !bCreateNew)
	{
		return false;
	}

	//@TODO: Create a new team if necessary?

	OldTeamID = FindTeamFromObject(Actor);

	return ChangeTeamForActor(Actor, NewTeamId);
}
