// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Player/BotaniPlayerState.h"

#include "BotaniLogChannels.h"
#include "AbilitySystem/BotaniAbilitySet.h"
#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "Character/Components/BotaniPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Game/BotaniGameModeBase.h"
#include "Game/Components/BotaniExperienceManagerComponent.h"
#include "GameFeatures/Data/BotaniPawnData.h"

#include "AbilitySystem/Attributes/BotaniCombatSet.h"
#include "AbilitySystem/Attributes/BotaniDamageSet.h"
#include "AbilitySystem/Attributes/BotaniHealthSet.h"
#include "AbilitySystem/Attributes/BotaniMovementSet.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniPlayerState)

const FName ABotaniPlayerState::NAME_BotaniAbilityReady("BotaniAbilitiesReady");

ABotaniPlayerState::ABotaniPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UBotaniAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	HealthSet = ObjectInitializer.CreateDefaultSubobject<UBotaniHealthSet>(this, TEXT("HealthSet"));
	CombatSet = ObjectInitializer.CreateDefaultSubobject<UBotaniCombatSet>(this, TEXT("CombatSet"));
	MovementSet = ObjectInitializer.CreateDefaultSubobject<UBotaniMovementSet>(this, TEXT("MovementSet"));
	DamageSet = ObjectInitializer.CreateDefaultSubobject<UBotaniDamageSet>(this, TEXT("DamageSet"));
	
	NetUpdateFrequency = 100.0f;
}

void ABotaniPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyTeamId, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MySquadId, SharedParams);

	SharedParams.Condition = COND_SkipOwner;
	DOREPLIFETIME_WITH_PARAMS(ThisClass, ReplicatedViewRotation, SharedParams);
}

UAbilitySystemComponent* ABotaniPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABotaniPlayerState::ClientInitialize(AController* Controller)
{
	Super::ClientInitialize(Controller);

	if (UBotaniPawnExtensionComponent* PawnExtComp = UBotaniPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

void ABotaniPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = World->GetGameState();
		check(GameState);

		UBotaniExperienceManagerComponent* ExperienceManager = UBotaniExperienceManagerComponent::Get(GameState);
		check(ExperienceManager);

		ExperienceManager->CallOrRegister_OnExperienceLoaded(FOnBotaniExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

void ABotaniPlayerState::SetReplicatedViewRotation(const FRotator& NewRotation)
{
	if (NewRotation != ReplicatedViewRotation)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedViewRotation, this);
		ReplicatedViewRotation = NewRotation;
	}
}

void ABotaniPlayerState::SetPawnData(const UBotaniPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		BOTANI_EXP_LOG(Warning, TEXT("SetPawnData called on client for player state [%s]"), *GetName());
		return;
	}

	if (PawnData)
	{
		BOTANI_LOG(Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has a valid PawnData [%s]!"), *InPawnData->GetName(), *GetName(), *PawnData->GetName());
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	BOTANI_EXP_LOG(Display, TEXT("SET PAWN DATA: [%s]"), *InPawnData->GetName());

	for (const UBotaniAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (!ensure(AbilitySet))
		{
			continue;
		}

		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr, this);
		BOTANI_GAS_LOG(Display, TEXT("Gave ability set [%s] to player state [%s]"), *AbilitySet->GetName(), *GetName());
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_BotaniAbilityReady);
	ForceNetUpdate();
}

void ABotaniPlayerState::OnRep_PawnData()
{
}

void ABotaniPlayerState::OnExperienceLoaded(const UBotaniExperienceDefinition* CurrentExperience)
{
	ABotaniGameModeBase* BotaniGameMode = GetWorld()->GetAuthGameMode<ABotaniGameModeBase>();
	if (BotaniGameMode == nullptr)
	{
		return;
	}

	const UBotaniPawnData* FoundPawnData = nullptr;

	if (GetPawn())
	{
		if (UBotaniPawnExtensionComponent* PawnExtComponent = UBotaniPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
		{
			FoundPawnData = PawnExtComponent->GetPawnData<UBotaniPawnData>();
			BOTANI_EXP_LOG(Display, TEXT("[%hs] Found pawn data [%s] for player state [%s]"), __FUNCTION__, FoundPawnData ? *FoundPawnData->GetName() : TEXT("nullptr"), *GetName());
		}
	}
	else
	{
		if (IsABot())
		{
			if (UBotaniPawnExtensionComponent* PawnExtComponent = UBotaniPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
			{
				FoundPawnData = PawnExtComponent->GetPawnData<UBotaniPawnData>();

				// If there is no pawn data yet, request one from game mode
				if (FoundPawnData == nullptr)
				{
					FoundPawnData = BotaniGameMode->GetPawnDataForController(GetOwningController());
				}

				BOTANI_EXP_LOG(Display, TEXT("[%hs] Found pawn data [%s] for player state [%s]"), __FUNCTION__, FoundPawnData ? *FoundPawnData->GetName() : TEXT("nullptr"), *GetName());

				SetPawnData(FoundPawnData);

				return;
			}
		}
	}

	// Fallback to default one
	if (FoundPawnData == nullptr)
	{
		FoundPawnData = BotaniGameMode->GetPawnDataForController(GetOwningController());
	}

	if (FoundPawnData != nullptr)
	{
		SetPawnData(FoundPawnData);
	}
	else
	{
		BOTANI_EXP_LOG(Error, TEXT("[%hs] Unable to find PawnData for player state [%s]"), __FUNCTION__, *GetName());
	}
}

void ABotaniPlayerState::OnRep_MyTeamId(FGenericTeamId OldTeamId)
{
	ConditionalBroadcastTeamChanged(this, OldTeamId, MyTeamId);
}

void ABotaniPlayerState::OnRep_MySquadId()
{
	//@TODO: Implement squad change logic? Maybe even a squad subsystem.
}
