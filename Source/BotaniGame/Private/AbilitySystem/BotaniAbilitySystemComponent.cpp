// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/BotaniAbilitySystemComponent.h"

#include "BotaniLogChannels.h"
#include "GameplayTags/BotaniGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniAbilitySystemComponent)

#define LOCTEXT_NAMESPACE "BotaniAbilitySystemComponent"



UBotaniAbilitySystemComponent::UBotaniAbilitySystemComponent()
{
	InputHeldSpecHandles.Reset();
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();

	FMemory::Memset(ActivationGroupCounts, 0, sizeof(ActivationGroupCounts));
}

void UBotaniAbilitySystemComponent::CancelAbilitiesByFunc(const TShouldCancelAbilityFunc& ShouldCancelAbilityFunc, bool bReplicateCancelAbility)
{
	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		UBotaniGameplayAbility* AbilityCDO = Cast<UBotaniGameplayAbility>(AbilitySpec.Ability);
		if (AbilityCDO == nullptr)
		{
			continue;
		}

		// Cancel all spawned instances, not the CDO
		if (AbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			for (UGameplayAbility* AbilityInstance : AbilitySpec.GetAbilityInstances())
			{
				UBotaniGameplayAbility* BotaniAbilityInst = Cast<UBotaniGameplayAbility>(AbilityInstance);
				if (BotaniAbilityInst && ShouldCancelAbilityFunc(BotaniAbilityInst, AbilitySpec.Handle))
				{
					if (!BotaniAbilityInst->CanBeCanceled())
					{
						continue;
					}

					BotaniAbilityInst->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), BotaniAbilityInst->GetCurrentActivationInfo(), bReplicateCancelAbility);
				}
			}
		}
		// Cancel the ability CDO for non-instanced abilities
		else
		{
			if (ShouldCancelAbilityFunc(AbilityCDO, AbilitySpec.Handle))
			{
				check(AbilityCDO->CanBeCanceled());
				AbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), AbilityCDO->GetCurrentActivationInfo(), bReplicateCancelAbility);
			}
		}
	}
}

void UBotaniAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability == nullptr)
		{
			continue;
		}

		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			continue;
		}

		UBotaniGameplayAbility* AbilityCDO = Cast<UBotaniGameplayAbility>(AbilitySpec.Ability);
		if (AbilityCDO == nullptr)
		{
			continue;
		}

		if (AbilityCDO->GetActivationPolicy() == EBotaniAbilityActivationPolicy::WhileInputActive)
		{
			InputHeldSpecHandles.Add(AbilitySpec.Handle);
		}
		else // if (AbilityCDO->GetActivationPolicy() == EBotaniAbilityActivationPolicy::OnInputTriggered)
		{
			//TODO: Should count as pressed if input triggered or on spawn policy to make sure both policies work with input?
			InputPressedSpecHandles.Add(AbilitySpec.Handle);
		}
	}
}

void UBotaniAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability == nullptr)
		{
			continue;
		}

		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			continue;
		}

		InputReleasedSpecHandles.Add(AbilitySpec.Handle);
		InputHeldSpecHandles.Remove(AbilitySpec.Handle);
		ActivatedInputAbilities.Remove(AbilitySpec.Handle);
	}
}

void UBotaniAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	if (bGamePaused)
	{
		ClearAbilityInput();
		return;
	}

	if (HasMatchingGameplayTag(BotaniGameplayTags::Gameplay::TAG_Gameplay_AbilityInputBlocked))
	{
		ClearAbilityInput();
		return;
	}

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// Process all abilities that are active when input is help
	for (const FGameplayAbilitySpecHandle& Handle : InputHeldSpecHandles)
	{
		const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
		if (AbilitySpec == nullptr)
		{
			continue;
		}

		if (AbilitySpec->Ability == nullptr)
		{
			continue;
		}

		if (AbilitySpec->IsActive())
		{
			continue;
		}

		const UBotaniGameplayAbility* AbilityCDO = Cast<UBotaniGameplayAbility>(AbilitySpec->Ability);
		
		if (AbilityCDO && AbilityCDO->GetActivationPolicy() == EBotaniAbilityActivationPolicy::WhileInputActive)
		{
			AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
		}
	}

	// Process all abilities that are active when input is pressed
	for (const FGameplayAbilitySpecHandle& Handle : InputPressedSpecHandles)
	{
		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
		if (AbilitySpec == nullptr)
		{
			continue;
		}

		if (AbilitySpec->Ability == nullptr)
		{
			continue;
		}

		if (ActivatedInputAbilities.Contains(AbilitySpec->Handle))
		{
			continue;
		}

		AbilitySpec->InputPressed = true;

		if (AbilitySpec->IsActive())
		{
			AbilitySpecInputPressed(*AbilitySpec);
			continue;
		}

		const UBotaniGameplayAbility* AbilityCDO = Cast<UBotaniGameplayAbility>(AbilitySpec->Ability);
		if (AbilityCDO && AbilityCDO->GetActivationPolicy() == EBotaniAbilityActivationPolicy::OnInputTriggered)
		{
			AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
			ActivatedInputAbilities.AddUnique(AbilitySpec->Handle);
		}
	}

	// Try to activate all the abilities
	for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToActivate)
	{
		TryActivateAbility(Handle);
	}

	// Process all abilities that had their input released this frame
	for (const FGameplayAbilitySpecHandle& Handle : InputReleasedSpecHandles)
	{
		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
		if (AbilitySpec == nullptr)
		{
			continue;
		}

		if (AbilitySpec->Ability == nullptr)
		{
			continue;
		}

		AbilitySpec->InputPressed = false;

		if (AbilitySpec->IsActive())
		{
			AbilitySpecInputReleased(*AbilitySpec);
		}
	}
	
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UBotaniAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UBotaniAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	// Apply a scope lock, this is important because we are going to be modifying the ability system component's state
	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		const UBotaniGameplayAbility* AbilityCDO = CastChecked<UBotaniGameplayAbility>(AbilitySpec.Ability);
		AbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
	}
}

bool UBotaniAbilitySystemComponent::IsActivationGroupBlocked(EBotaniAbilityActivationGroup Group) const
{
	bool bIsBlocked = false;

	switch (Group)
	{
	case EBotaniAbilityActivationGroup::Independent:
		{
			// Independent abilities are never blocked
			bIsBlocked = false;
		}
		break;
	case EBotaniAbilityActivationGroup::Exclusive_Replaceable:
	case EBotaniAbilityActivationGroup::Exclusive_Blocking:
		{
			// Exclusive abilities can activate if nothing else is blocking them
			bIsBlocked = ActivationGroupCounts[static_cast<uint8>(EBotaniAbilityActivationGroup::Exclusive_Blocking)] > 0;
		}
		break;
	default:
		checkf(0, TEXT("Unknown EBotaniAbilityActivationGroup %d! \nHow did we even get there? Did you dare to change the activation group to something that shouldn't be used??"), static_cast<uint8>(Group));
		break;
	}

	return bIsBlocked;
}

void UBotaniAbilitySystemComponent::AddAbilityToActivationGroup(EBotaniAbilityActivationGroup Group, UBotaniGameplayAbility* BotaniAbility)
{
	check(BotaniAbility);
	check(ActivationGroupCounts[(uint8)Group] < INT32_MAX);

	ActivationGroupCounts[(uint8)Group]++;

	const bool bReplicateCancelAbility = false;
	
	switch (Group)
	{
	case EBotaniAbilityActivationGroup::Independent:
		{
			break;
		}
	case EBotaniAbilityActivationGroup::Exclusive_Replaceable:
	case EBotaniAbilityActivationGroup::Exclusive_Blocking:
		{
			CancelActivationGroupAbilities(EBotaniAbilityActivationGroup::Exclusive_Replaceable, BotaniAbility, bReplicateCancelAbility);
			break;
		}
	default:
		{
			checkf(false, TEXT("[%hs] Multiple exclusive abilities are running."), __FUNCTION__);
		}
	}

	const int32 ExclusiveCount = ActivationGroupCounts[(uint8)EBotaniAbilityActivationGroup::Exclusive_Replaceable] + ActivationGroupCounts[(uint8)EBotaniAbilityActivationGroup::Exclusive_Blocking];
	if (!ensure(ExclusiveCount <= 1))
	{
		BOTANI_GAS_LOG(Error, TEXT("Multiple exclusive abilities are running."));
	}
}

void UBotaniAbilitySystemComponent::RemoveAbilityFromActivationGroup(EBotaniAbilityActivationGroup Group, UBotaniGameplayAbility* BotaniAbility)
{
	check(BotaniAbility);
	check(ActivationGroupCounts[(uint8)Group] > 0);

	ActivationGroupCounts[(uint8)Group]--;
}

void UBotaniAbilitySystemComponent::CancelActivationGroupAbilities(EBotaniAbilityActivationGroup Group, UBotaniGameplayAbility* IgnoreBotaniAbility, bool bReplicateCancelAbility)
{
	auto ShouldCancelFunc = [this, Group, IgnoreBotaniAbility] (const UBotaniGameplayAbility* BotaniAbility, FGameplayAbilitySpecHandle Handle)
	{
		return ((BotaniAbility->GetActivationGroup() == Group) && (BotaniAbility != IgnoreBotaniAbility));	
	};

	CancelAbilitiesByFunc(ShouldCancelFunc, bReplicateCancelAbility);
}

void UBotaniAbilitySystemComponent::DeferredSetBaseAttributeValueFromReplication(const FGameplayAttribute& Attribute, float NewValue)
{
	const float OldValue = ActiveGameplayEffects.GetAttributeBaseValue(Attribute);
	ActiveGameplayEffects.SetAttributeBaseValue(Attribute, NewValue);
	SetBaseAttributeValueFromReplication(Attribute, OldValue, NewValue);

	//@TODO: Deferred delegates maybe
}

void UBotaniAbilitySystemComponent::DeferredSetBaseAttributeValueFromReplication(const FGameplayAttribute& Attribute, FGameplayAttributeData NewValue)
{
	DeferredSetBaseAttributeValueFromReplication(Attribute, NewValue.GetBaseValue());
}

void UBotaniAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UBotaniAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UBotaniAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	OnAbilitiesReplicatedCallbacks.Broadcast(ActivatableAbilities.Items);
}

void UBotaniAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
	{
		OnAbilitiesReplicatedCallbacks.Broadcast(ActivatableAbilities.Items);
	}));
}

void UBotaniAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	OnAbilitiesReplicatedCallbacks.Broadcast(ActivatableAbilities.Items);
}

void UBotaniAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);
	
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (!bHasNewPawnAvatar)
	{
		return;
	}

	// Notify all abilities that the avatar has changed
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		UBotaniGameplayAbility* BotaniAbilityCDO = Cast<UBotaniGameplayAbility>(Spec.Ability);
		if (!BotaniAbilityCDO)
		{
			continue;
		}

		if (BotaniAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstance : Instances)
			{
				UBotaniGameplayAbility* BotaniAbilityInstance = Cast<UBotaniGameplayAbility>(AbilityInstance);
				if (BotaniAbilityInstance)
				{
					BotaniAbilityInstance->K2_OnPawnAvatarSet(); //@TODO: use the native version of this
				}
			}
		}
		else
		{
			BotaniAbilityCDO->K2_OnPawnAvatarSet();
		}
	}

	TryActivateAbilitiesOnSpawn();
}


#undef LOCTEXT_NAMESPACE
