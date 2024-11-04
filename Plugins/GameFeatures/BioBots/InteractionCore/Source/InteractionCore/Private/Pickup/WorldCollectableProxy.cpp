// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Pickup/WorldCollectableProxy.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/PickupableComponent.h"
#include "Inventory/Components/BotaniInventoryManager.h"
#include "NativeGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WorldCollectableProxy)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Ability_Interaction_Activate, "Ability.Descriptor.Interaction.Activate");

AWorldCollectableProxy::AWorldCollectableProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PickupableComponent = ObjectInitializer.CreateDefaultSubobject<UPickupableComponent>(this, TEXT("PickupableComponent"));
}

void AWorldCollectableProxy::GatherInteractionOptions(
	const FBotaniInteractionQuery& InteractionQuery, FBotaniInteractionOptionBuilder& OptionBuilder)
{
	OptionBuilder.AddInteractionOption(InteractionOption);
}

void AWorldCollectableProxy::OnPickupCollisionBeginOverlap_Implementation(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || OtherActor == nullptr)
	{
		return;
	}

	UGameplayInventoryItemDefinition* ItemDef = GetItemDefinition();
	UItemFragment_PickupDefinition* PickupFrag = ItemDef ? ItemDef->GetItemFragment<UItemFragment_PickupDefinition>() : nullptr;
	if (PickupFrag == nullptr)
	{
		return;
	}

	// If the item is not set to auto pickup, then return
	if (PickupFrag->PickupData.AutoPickupBehavior == EInventoryItemAutoPickupBehavior::DoNotAutoPickup)
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn == nullptr)
	{
		return;
	}

	// Find the inventory manager
	UBotaniInventoryManager* InventoryManager = UBotaniInventoryManager::GetInventoryManager(Pawn);
	if (InventoryManager == nullptr)
	{
		return;
	}

	// Find the ability system component
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn);
	if (ASC == nullptr)
	{
		return;
	}

	FBotaniInteractionOption TempOption = InteractionOption;
	TempOption.TargetAbilitySystem = ASC;
	TempOption.InteractableTarget = this;

	FGameplayAbilitySpec* InteractionAbilitySpec = nullptr;
	if (TempOption.TargetAbilitySystem && TempOption.TargetInteractionAbilityHandle.IsValid())
	{
		// Find the spec
		InteractionAbilitySpec = TempOption.TargetAbilitySystem->FindAbilitySpecFromHandle(TempOption.TargetInteractionAbilityHandle);
	}
	else if (TempOption.InteractionAbilityToGrant)
	{
		// Find the spec
		InteractionAbilitySpec = ASC->FindAbilitySpecFromClass(TempOption.InteractionAbilityToGrant);

		if (InteractionAbilitySpec)
		{
			// Update the option
			TempOption.TargetAbilitySystem = ASC;
			TempOption.TargetInteractionAbilityHandle = InteractionAbilitySpec->Handle;
		}
	}

	// Send a gameplay event to the ability system component of the pawn
	FGameplayEventData Payload;
	Payload.EventTag = TAG_Ability_Interaction_Activate;
	Payload.Target = this;
	Payload.Instigator = Pawn;

	/**
	 * If needed we allow the interactable target to manipulate the event data so that for example,
	 * a button on the wall may want to specify a door actor to execute the ability on.
	 * So it might choose to override Target to be the door actor.
	 */
	// InteractionOption.InteractableTarget->CustomizeInteractionEventData(TAG_Ability_Interaction_Activate, Payload);

	// Get the actor info needed for the interaction
	FGameplayAbilityActorInfo ActorInfo;
	ActorInfo.InitFromActor(this, Pawn, TempOption.TargetAbilitySystem);

	// Execute the interaction ability
	TempOption.TargetAbilitySystem->TriggerAbilityFromGameplayEvent(
		TempOption.TargetInteractionAbilityHandle,
		&ActorInfo,
		TAG_Ability_Interaction_Activate,
		&Payload,
		*TempOption.TargetAbilitySystem);
}
