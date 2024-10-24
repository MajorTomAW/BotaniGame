// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Character/Components/BotaniHealthComponent.h"

#include "BotaniLogChannels.h"
#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/BotaniHealthSet.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTags/BotaniGameplayTags.h"
#include "Messaging/BotaniVerbMessage.h"
#include "Messaging/BotaniVerbMessageHelpers.h"

#include "Net/UnrealNetwork.h"
#include "System/BotaniAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniHealthComponent)


UBotaniHealthComponent::UBotaniHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
	DeathState = EBotaniDeathState::NotDead;
}

void UBotaniHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, DeathState);
}

void UBotaniHealthComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();
	
	Super::OnUnregister();
}

void UBotaniHealthComponent::InitializeWithAbilitySystem(UBotaniAbilitySystemComponent* InASC)
{
	const AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		BOTANI_LOG(Error, TEXT("[%hs]: Health component for owner %s is already initialized with an ability system component."), __FUNCTION__, *Owner->GetName());
		return;
	}

	AbilitySystemComponent = InASC;
	if (AbilitySystemComponent == nullptr)
	{
		BOTANI_LOG(Error, TEXT("[%hs]: Attempted to initialize health component for owner %s with a null ability system component."), __FUNCTION__, *Owner->GetName());
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<UBotaniHealthSet>();
	if (HealthSet == nullptr)
	{
		BOTANI_LOG(Error, TEXT("[%hs]: Failed to get health set for owner %s."), __FUNCTION__, *Owner->GetName());
		return;
	}

	// Bind to health set's OnHealthChanged event
	HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
	HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

	ClearGameplayTags();

	// Reset attributes to their default values
	AbilitySystemComponent->SetNumericAttributeBase(UBotaniHealthSet::GetHealthAttribute(), HealthSet->GetMaxHealth());
	BOTANI_LOG(Display, TEXT("[%hs]: Initialized health component for owner %s. New health: %f, max health: %f."), __FUNCTION__, *Owner->GetName(), HealthSet->GetHealth(), HealthSet->GetMaxHealth());

	// Broadcast initial values
	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetMaxHealth(), HealthSet->GetMaxHealth(), nullptr);
}

void UBotaniHealthComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();

	if (HealthSet != nullptr)
	{
		HealthSet->OnHealthChanged.RemoveAll(this);
		HealthSet->OnMaxHealthChanged.RemoveAll(this);
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;
}

void UBotaniHealthComponent::ClearGameplayTags() const
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(BotaniGameplayTags::Status::TAG_Death_Dead, 0);
		AbilitySystemComponent->SetLooseGameplayTagCount(BotaniGameplayTags::Status::TAG_Death_Dying, 0);
	}
}

float UBotaniHealthComponent::GetHealth() const
{
	return (HealthSet != nullptr) ?
		HealthSet->GetHealth() : 0.f;
}

float UBotaniHealthComponent::GetMaxHealth() const
{
	return (HealthSet != nullptr) ?
		HealthSet->GetMaxHealth() : 0.f;
}

float UBotaniHealthComponent::GetHealthNormalized() const
{
	if (HealthSet == nullptr)
	{
		return 0.f;
	}

	const float Health = HealthSet->GetHealth();
	const float MaxHealth = HealthSet->GetMaxHealth();

	return (MaxHealth > 0.f) ? (Health / MaxHealth) : 0.f;
}

void UBotaniHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UBotaniHealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UBotaniHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
#if WITH_SERVER_CODE
	if (AbilitySystemComponent == nullptr || DamageEffectSpec == nullptr)
	{
		return;
	}

	// Send a gameplay event to the ASC to handle the death, can be used to trigger a death gameplay ability
	{
		FGameplayEventData Payload;
		Payload.EventTag = BotaniGameplayTags::Gameplay::Event::TAG_GameplayEvent_Death;
		Payload.Instigator = DamageInstigator;
		Payload.Target = AbilitySystemComponent->GetAvatarActor();
		Payload.OptionalObject = DamageEffectSpec->Def;
		Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
		Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
		Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
		Payload.EventMagnitude = DamageMagnitude;

		FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
		AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
	}

	// Send a standardized verb message that other systems can observe
	{
		FBotaniVerbMessage Message;
		Message.VerbTag = BotaniGameplayTags::Gameplay::Verb::TAG_GameplayVerb_Elimination;
		Message.Instigator = DamageInstigator;
		Message.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
		Message.Target = UBotaniVerMessageHelpers::GetPlayerStateFromObject(AbilitySystemComponent->GetAvatarActor());
		Message.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();

		//@TODO: Gather more information about the elimination event, like team kill, self-elimination, etc.

		UGameplayMessageSubsystem& MessageSub = UGameplayMessageSubsystem::Get(GetWorld());
		MessageSub.BroadcastMessage(Message.VerbTag, Message);
	}
#endif
}

void UBotaniHealthComponent::OnRep_DeathState(EBotaniDeathState OldDeathState)
{
	const EBotaniDeathState NewDeathState = DeathState;

	// Revert the death state for now as we rely on StartDeath() and FinishDeath() to handle the state changes
	DeathState = OldDeathState;

	if (OldDeathState > NewDeathState)
	{
		BOTANI_LOG(Warning, TEXT("[%hs]: Predicted past server death state [%d] -> [%d] for owner %s."), __FUNCTION__, (uint8)OldDeathState, (uint8)NewDeathState, *GetOwner()->GetName());
		return;
	}

	if (OldDeathState == EBotaniDeathState::NotDead)
	{
		if (NewDeathState == EBotaniDeathState::DeathStarted)
		{
			StartDeath();
		}
		else if (NewDeathState == EBotaniDeathState::DeathFinished)
		{
			StartDeath();
			FinishDeath();
		}
		else
		{
			BOTANI_LOG(Error, TEXT("[%hs]: Invalid death state [%d] for owner %s."), __FUNCTION__, (uint8)NewDeathState, *GetOwner()->GetName());
		}
	}
	else if (OldDeathState == EBotaniDeathState::DeathStarted)
	{
		if (NewDeathState == EBotaniDeathState::DeathFinished)
		{
			FinishDeath();
		}
		else
		{
			BOTANI_LOG(Error, TEXT("[%hs]: Invalid death transition [%d] -> [%d] for owner %s."), __FUNCTION__, (uint8)OldDeathState, (uint8)NewDeathState, *GetOwner()->GetName());
		}
	}

	ensureMsgf((DeathState == NewDeathState), TEXT("[%hs]: Failed to handle death state transition [%d] -> [%d] for owner %s."), __FUNCTION__, static_cast<uint8>(OldDeathState), static_cast<uint8>(NewDeathState), *GetOwner()->GetName());
}

void UBotaniHealthComponent::StartDeath()
{
	if (DeathState != EBotaniDeathState::NotDead)
	{
		return;
	}

	DeathState = EBotaniDeathState::DeathStarted;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(BotaniGameplayTags::Status::TAG_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UBotaniHealthComponent::FinishDeath()
{
	if (DeathState != EBotaniDeathState::DeathStarted)
	{
		return;
	}

	DeathState = EBotaniDeathState::DeathFinished;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(BotaniGameplayTags::Status::TAG_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UBotaniHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	if ((DeathState != EBotaniDeathState::NotDead) || AbilitySystemComponent == nullptr)
	{
		BOTANI_LOG(Warning, TEXT("[%hs]: Owner %s is already dead or missing an ability system component."), __FUNCTION__, *GetOwner()->GetName());
		return;
	}

	const TSubclassOf<UGameplayEffect> DamageGE = UBotaniAssetManager::GetSubclass(UBotaniGameData::Get().DamageGameplayEffect_SetByCaller);
	if (DamageGE == nullptr)
	{
		BOTANI_LOG(Error, TEXT("[%hs]: Failed to get damage gameplay effect for owner %s."), __FUNCTION__, *GetOwner()->GetName());
		return;
	}

	const FGameplayEffectSpecHandle Handle = AbilitySystemComponent->MakeOutgoingSpec(DamageGE, 1.0f, AbilitySystemComponent->MakeEffectContext());
	FGameplayEffectSpec* Spec = Handle.Data.Get();

	if (!Spec)
	{
		BOTANI_LOG(Error, TEXT("[%hs]: Failed to create damage gameplay effect spec for owner %s."), __FUNCTION__, *GetOwner()->GetName());
		return;
	}

	Spec->AddDynamicAssetTag(BotaniGameplayTags::Gameplay::Damage::TAG_GameplayDamage_SelfDestruct);

	if (bFellOutOfWorld)
	{
		Spec->AddDynamicAssetTag(BotaniGameplayTags::Gameplay::Damage::TAG_GameplayDamage_FellOutOfWorld);
	}

	const float DamageMagnitude = HealthSet->GetMaxHealth();

	Spec->SetSetByCallerMagnitude(BotaniGameplayTags::Effect::SetByCaller::TAG_SetByCaller_Damage, DamageMagnitude);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
}
