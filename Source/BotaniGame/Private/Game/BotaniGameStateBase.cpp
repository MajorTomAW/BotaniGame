// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Game/BotaniGameStateBase.h"

#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "Game/Components/BotaniExperienceManagerComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniGameStateBase)


extern ENGINE_API float GAverageFPS;

ABotaniGameStateBase::ABotaniGameStateBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UBotaniAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ExperienceManagerComponent = ObjectInitializer.CreateDefaultSubobject<UBotaniExperienceManagerComponent>(this, TEXT("ExperienceManagerComponent"));
	
	ServerFPS = 0.0f;
}

void ABotaniGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ServerFPS);
}

void ABotaniGameStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ABotaniGameStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetLocalRole() == ROLE_Authority)
	{
		ServerFPS = GAverageFPS;
	}
}

UAbilitySystemComponent* ABotaniGameStateBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
