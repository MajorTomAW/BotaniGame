// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/BotaniAbilityActor.h"

#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniAbilityActor)

ABotaniAbilityActor::ABotaniAbilityActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, AbilitySystemCreationPolicy(EBotaniAbilitySystemCreationPolicy::Lazy)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	AbilitySystemClass = UBotaniAbilitySystemComponent::StaticClass();

	bReplicates = true;
}

void ABotaniAbilityActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, TransientAbilitySystem, Params);
}

void ABotaniAbilityActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	if ((AbilitySystemCreationPolicy == EBotaniAbilitySystemCreationPolicy::Always) &&
		(GetNetMode() != NM_Client))
	{
		check(!AbilitySystemComponent);
		CreateAbilitySystem();
		InitializeAbilitySystem();
		ForceNetUpdate();
	}
}

void ABotaniAbilityActor::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	if (!TransientAbilitySystem &&
		AbilitySystemComponent &&
		(AbilitySystemCreationPolicy != EBotaniAbilitySystemCreationPolicy::Never))
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, TransientAbilitySystem, this);
		TransientAbilitySystem = AbilitySystemComponent;
	}
}

UAbilitySystemComponent* ABotaniAbilityActor::GetAbilitySystemComponent() const
{
	if (!AbilitySystemComponent &&
		HasAuthority() &&
		(AbilitySystemCreationPolicy == EBotaniAbilitySystemCreationPolicy::Lazy) &&
		GetWorld() &&
		!IsUnreachable())
	{
		ABotaniAbilityActor* MutableThis = const_cast<ABotaniAbilityActor*>(this);
		MutableThis->CreateAbilitySystem();
		MutableThis->InitializeAbilitySystem();
		MutableThis->ForceNetUpdate();
	}

	return AbilitySystemComponent;
}

void ABotaniAbilityActor::InitializeAbilitySystem()
{
	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	OnAbilitySystemInitialized();
}

void ABotaniAbilityActor::SetPendingAttributeFromReplication(const FGameplayAttribute& Attribute, const FGameplayAttributeData& NewValue)
{
	PendingAttributeReplications.Emplace(FPendingAttributeReplication(Attribute, NewValue));
}

void ABotaniAbilityActor::ApplyPendingAttributeReplications()
{
	check(AbilitySystemComponent);

	// If there are no pending attribute replications, do nothing
	if (PendingAttributeReplications.Num() <= 0)
	{
		return;
	}

	for (const auto& Pending : PendingAttributeReplications)
	{
		AbilitySystemComponent->DeferredSetBaseAttributeValueFromReplication(Pending.Attribute, Pending.NewValue);
	}

	PendingAttributeReplications.Empty();
}

void ABotaniAbilityActor::CreateAbilitySystem()
{
	AbilitySystemComponent = NewObject<UBotaniAbilitySystemComponent>(this, AbilitySystemClass);
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AbilitySystemComponent->RegisterComponent();
}

void ABotaniAbilityActor::OnRep_TransientAbilitySystem()
{
	AbilitySystemComponent = TransientAbilitySystem;
	if (AbilitySystemComponent)
	{
		InitializeAbilitySystem();
		ApplyPendingAttributeReplications();
	}
}
