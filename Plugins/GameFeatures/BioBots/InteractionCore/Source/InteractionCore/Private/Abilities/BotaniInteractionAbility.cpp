// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Abilities/BotaniInteractionAbility.h"

#include "AbilitySystemComponent.h"
#include "Interfaces/InteractableTarget.h"
#include "Tasks/AbilityTask_GrantNearbyInteraction.h"
#include "InteractionOption.h"
#include "InteractionStatics.h"
#include "NativeGameplayTags.h"
#include "IndicatorSystem/BotaniIndicatorDescriptor.h"
#include "IndicatorSystem/Components/BotaniIndicatorManagerComponent.h"
#include "Player/BotaniPlayerController.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Ability_Interaction_Activate_ABILITY, "Ability.Descriptor.Interaction.Activate");

UBotaniInteractionAbility::UBotaniInteractionAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EBotaniAbilityActivationPolicy::OnSpawn;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UBotaniInteractionAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (!AbilitySystemComponent || AbilitySystemComponent->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	UAbilityTask_GrantNearbyInteraction* Task = UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(this, InteractionScanRange, InteractionScanRate);
	Task->ReadyForActivation();
}

void UBotaniInteractionAbility::UpdateInteraction(const TArray<FBotaniInteractionOption>& InteractiveOptions)
{
	if (ABotaniPlayerController* BotaniPC = GetBotaniPlayerControllerFromActorInfo())
	{
		if (UBotaniIndicatorManagerComponent* IndicatorManager = UBotaniIndicatorManagerComponent::GetComponent(BotaniPC))
		{
			// Remove all existing indicators
			for (UBotaniIndicatorDescriptor* Indicator : Indicators)
			{
				IndicatorManager->RemoveIndicator(Indicator);
			}
			Indicators.Reset();

			for (const FBotaniInteractionOption& Option : InteractiveOptions)
			{
				AActor* Interactable = UInteractionStatics::GetActorFromInteractableTarget(Option.InteractableTarget);
				TSoftClassPtr<UUserWidget> InteractionWidgetClass = Option.InteractionWidgetClass.IsNull() ? DefaultInteractionWidgetClass : Option.InteractionWidgetClass;

				UBotaniIndicatorDescriptor* Indicator = NewObject<UBotaniIndicatorDescriptor>();
				Indicator->SetDataObject(Interactable);
				Indicator->SetSceneComponent(Interactable->GetRootComponent());
				Indicator->SetIndicatorClass(InteractionWidgetClass);

				IndicatorManager->AddIndicator(Indicator);
				Indicators.Add(Indicator);
			}
		}
	}

	CurrentOptions = InteractiveOptions;
}

void UBotaniInteractionAbility::TriggerInteraction()
{
	if (CurrentOptions.Num() <= 0)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (!AbilitySystem)
	{
		return;
	}

	const FBotaniInteractionOption& Option = CurrentOptions[0];

	AActor* Instigator = GetAvatarActorFromActorInfo();
	AActor* Interactable = UInteractionStatics::GetActorFromInteractableTarget(Option.InteractableTarget);

	// Allow the target to customize the event data, in case the ability needs custom data that only the actor knows
	FGameplayEventData Payload;
	Payload.EventTag = TAG_Ability_Interaction_Activate_ABILITY;
	Payload.Target = Interactable;
	Payload.Instigator = Instigator;

	/**
	 * If needed we allow the interactable target to manipulate the event data so that for example,
	 * a button on the wall may want to specify a door actor to execute the ability on.
	 * So it might choose to override Target to be the door actor.
	 */
	Option.InteractableTarget->CustomizeInteractionEventData(TAG_Ability_Interaction_Activate_ABILITY, Payload);

	// Target actor is the 'avatar' and the source Interactable is the owner actor.
	AActor* TargetActor = const_cast<AActor*>(ToRawPtr(Payload.Target));

	// Get the actor info needed for the interaction
	FGameplayAbilityActorInfo ActorInfo;
	ActorInfo.InitFromActor(Interactable, TargetActor, Option.TargetAbilitySystem);

	// Execute the interaction ability
	const bool bSuccess = Option.TargetAbilitySystem->TriggerAbilityFromGameplayEvent(
		Option.TargetInteractionAbilityHandle,
		&ActorInfo,
		TAG_Ability_Interaction_Activate_ABILITY,
		&Payload,
		*Option.TargetAbilitySystem);
}
