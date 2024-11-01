// Copyright © 2024 Botanibots Team. All rights reserved.


#include "AbilitySystem/Abilities/BotaniGameplayAbility.h"

#include "AbilitySystemLog.h"
#include "BotaniLogChannels.h"
#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/Cost/BotaniAbilityCost.h"
#include "Camera/BotaniCameraMode.h"
#include "Character/BotaniCharacter.h"
#include "Character/Components/BotaniHeroComponent.h"
#include "GameplayTags/BotaniGameplayTags.h"
#include "Player/BotaniPlayerController.h"
#include "Player/BotaniPlayerState.h"

#define ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(FunctionName, ReturnValue)																				\
{																																						\
	if (!ensure(IsInstantiated()))																														\
	{																																					\
		ABILITY_LOG(Error, TEXT("%s: " #FunctionName " cannot be called on a non-instanced ability. Check the instancing policy."), *GetPathName());	\
		return ReturnValue;																																\
	}																																					\
}


UBotaniGameplayAbility::UBotaniGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: UGameplayAbility(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	
	ActivationGroup = EBotaniAbilityActivationGroup::Independent;
	ActivationPolicy = EBotaniAbilityActivationPolicy::OnInputTriggered;
	HUDPolicy = EBotaniAbilityHUDPolicy::None;
}

UBotaniAbilitySystemComponent* UBotaniGameplayAbility::GetBotaniAbilitySystemComponent() const
{
	return CurrentActorInfo ? Cast<UBotaniAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr;
}

AController* UBotaniGameplayAbility::GetControllerFromActorInfo() const
{
	if (CurrentActorInfo == nullptr)
	{
		return nullptr;
	}

	if (AController* Controller = CurrentActorInfo->PlayerController.Get())
	{
		return Controller;
	}

	// Look for a controller on the pawn if we don't have a player controller
	AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
	while (TestActor)
	{
		if (AController* Controller = Cast<AController>(TestActor))
		{
			return Controller;
		}

		if (const APawn* Pawn = Cast<APawn>(TestActor))
		{
			return Pawn->GetController();
		}

		TestActor = TestActor->GetOwner();
	}

	return nullptr;
}

ABotaniPlayerController* UBotaniGameplayAbility::GetBotaniPlayerControllerFromActorInfo() const
{
	return CurrentActorInfo ? Cast<ABotaniPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr;
}

ABotaniPlayerState* UBotaniGameplayAbility::GetBotaniPlayerStateFromActorInfo() const
{
	if (CurrentActorInfo == nullptr)
	{
		return nullptr;
	}

	if (ABotaniPlayerState* PlayerState = Cast<ABotaniPlayerState>(CurrentActorInfo->OwnerActor.Get()))
	{
		return PlayerState;
	}

	AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
	while (TestActor)
	{
		if (const AController* Controller = Cast<AController>(TestActor))
		{
			return Controller->GetPlayerState<ABotaniPlayerState>();
		}

		if (const APawn* Pawn = Cast<APawn>(TestActor))
		{
			return Pawn->GetPlayerState<ABotaniPlayerState>();
		}

		TestActor = TestActor->GetOwner();
	}

	return nullptr;
}

ABotaniCharacter* UBotaniGameplayAbility::GetBotaniCharacterFromActorInfo() const
{
	return CurrentActorInfo ? Cast<ABotaniCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr;
}

UBotaniHeroComponent* UBotaniGameplayAbility::GetBotaniHeroComponentFromActorInfo() const
{
	return CurrentActorInfo ? UBotaniHeroComponent::FindHeroComponent(CurrentActorInfo->AvatarActor.Get()) : nullptr;
}

void UBotaniGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);
	
	if (ActorInfo && !Spec.IsActive() && !bIsPredicting && (ActivationPolicy == EBotaniAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* AbilitySystem = ActorInfo->AbilitySystemComponent.Get();
		const AActor* Avatar = ActorInfo->AvatarActor.Get();

		// If Avatar is torn off or about to die, don't activate the ability
		if (AbilitySystem && Avatar && !Avatar->GetTearOff() && (Avatar->GetLifeSpan() <= 0.f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				AbilitySystem->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

bool UBotaniGameplayAbility::CanChangeActivationGroup(EBotaniAbilityActivationGroup NewGroup) const
{
	if (!IsInstantiated() || !IsActive())
	{
		return false;
	}

	if (ActivationGroup == NewGroup)
	{
		return true;
	}

	UBotaniAbilitySystemComponent* BotaniASC = GetBotaniAbilitySystemComponent();
	check(BotaniASC);

	// The ability can't change the activation group if it is blocked (unless it is the one doing the blocking).
	if ((ActivationGroup != EBotaniAbilityActivationGroup::Exclusive_Blocking) && BotaniASC->IsActivationGroupBlocked(NewGroup))
	{
		return false;
	}

	// The ability can't become replaceable if it can't be canceled.
	if ((NewGroup == EBotaniAbilityActivationGroup::Exclusive_Replaceable) && !CanBeCanceled())
	{
		return false;
	}

	return true;
}

bool UBotaniGameplayAbility::ChangeActivationGroup(EBotaniAbilityActivationGroup NewGroup)
{
	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(ChangeActivationGroup, false);

	if (!CanChangeActivationGroup(NewGroup))
	{
		return false;
	}

	if (ActivationGroup != NewGroup)
	{
		UBotaniAbilitySystemComponent* BotaniASC = GetBotaniAbilitySystemComponent();
		check(BotaniASC);

		BotaniASC->RemoveAbilityFromActivationGroup(ActivationGroup, this);
		BotaniASC->AddAbilityToActivationGroup(NewGroup, this);
		
		ActivationGroup = NewGroup;
	}

	return true;
}

void UBotaniGameplayAbility::SetCameraMode(TSubclassOf<class UBotaniCameraMode> CameraMode)
{
	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(SetCameraMode, );

	if (UBotaniHeroComponent* HeroComponent = GetBotaniHeroComponentFromActorInfo())
	{
		HeroComponent->SetAbilityCameraMode(CameraMode, CurrentSpecHandle);
		ActiveCameraMode = CameraMode;
	}
}

void UBotaniGameplayAbility::ClearCameraMode()
{
	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(ClearCameraMode, );

	if (ActiveCameraMode)
	{
		if (UBotaniHeroComponent* HeroComponent = GetBotaniHeroComponentFromActorInfo())
		{
			HeroComponent->ClearAbilityCameraMode(CurrentSpecHandle);
		}

		ActiveCameraMode = nullptr;
	}
}

bool UBotaniGameplayAbility::K2_CheckCanActivate()
{
	return K2_CheckAbilityCooldown() && K2_CheckAbilityCost();
}

bool UBotaniGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const UBotaniAbilitySystemComponent* BotaniAbilitySystem = CastChecked<UBotaniAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (BotaniAbilitySystem->IsActivationGroupBlocked(ActivationGroup))
	{
		if (OptionalRelevantTags)
		{
			OptionalRelevantTags->AddTag(BotaniGameplayTags::Ability::ActivationFail::TAG_AbilityActivationFail_TagsBlocked);
		}

		return false;
	}

	return true;
}

void UBotaniGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)
{
	// The ability can't block canceling if it's activation group is replaceable
	if (!bCanBeCanceled && (ActivationGroup == EBotaniAbilityActivationGroup::Exclusive_Replaceable))
	{
		BOTANI_GAS_LOG(Error, TEXT("[%hs]: Ability (%s) can not block canceling because it's activation group is replaceable."), __FUNCTION__, *GetName());
		return;
	}
	
	Super::SetCanBeCanceled(bCanBeCanceled);
}

bool UBotaniGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
	{
		return false;
	}

	// Check for additional costs
	for (const UBotaniAbilityCost* Cost : AdditionalCosts)
	{
		if (Cost == nullptr)
		{
			continue;
		}

		if (!Cost->CheckCost(this, Handle, ActorInfo, /*inout*/ OptionalRelevantTags))
		{
			return false;
		}
	}

	return true;
}

void UBotaniGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
	check(ActorInfo);

	//@TODO: Check if we should only apply costs on hit

	for (UBotaniAbilityCost* Cost : AdditionalCosts)
	{
		if (Cost == nullptr)
		{
			continue;
		}

		Cost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
	}
}

void UBotaniGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	K2_OnAbilityAdded();

	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UBotaniGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	K2_OnAbilityRemoved();
	
	Super::OnRemoveAbility(ActorInfo, Spec);
}
