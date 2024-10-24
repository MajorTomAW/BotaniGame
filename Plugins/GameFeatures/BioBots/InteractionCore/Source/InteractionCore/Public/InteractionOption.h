// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"

#include "InteractionOption.generated.h"

class IInteractableTarget;
class UUserWidget;

/**
 * FBotaniInteractionOption
 *
 * A struct that represents an interaction option for a botani character.
 * Such options like an interaction text, description, and the ability to grant the avatar when they get near interactable objects.
 */
USTRUCT(BlueprintType)
struct FBotaniInteractionOption
{
	GENERATED_BODY()

public:
	/** The interactable target */
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TScriptInterface<IInteractableTarget> InteractableTarget;

	/** Simple text the interaction might return. Used for display purposes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText Text;

	/** Simple subtext the interaction might return. Used to display a description text. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText SubText;

	/** The widget to show for this kind of interaction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;


	// METHODS OF INTERACTION
	//--------------------------------------------------------------

	// 1) Place an ability on the avatar that they can activate when they perform interaction.

	/** The ability to grant the avatar when they get near interactable objects. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TSubclassOf<UGameplayAbility> InteractionAbilityToGrant;

	// - OR -

	// 2) Allow the object we're interacting with to have its own ability system and interaction ability, that we can activate instead.

	/** The ability system on the target that can be used for the TargetInteractionHandle and sending the event, if needed. */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystem = nullptr;

	/** The ability spec to activate on the object for this option. */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	FGameplayAbilitySpecHandle TargetInteractionAbilityHandle;

public:
	FORCEINLINE bool operator==(const FBotaniInteractionOption& Other) const
	{
		return (InteractableTarget == Other.InteractableTarget) &&
			(InteractionAbilityToGrant == Other.InteractionAbilityToGrant) &&
			(TargetAbilitySystem == Other.TargetAbilitySystem) &&
			(TargetInteractionAbilityHandle == Other.TargetInteractionAbilityHandle) &&
			(InteractionWidgetClass == Other.InteractionWidgetClass) &&
			(Text.IdenticalTo(Other.Text)) &&
			(SubText.IdenticalTo(Other.SubText));
	}

	FORCEINLINE bool operator!=(const FBotaniInteractionOption& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE bool operator<(const FBotaniInteractionOption& Other) const
	{
		return InteractableTarget.GetInterface() < Other.InteractableTarget.GetInterface();
	}
};