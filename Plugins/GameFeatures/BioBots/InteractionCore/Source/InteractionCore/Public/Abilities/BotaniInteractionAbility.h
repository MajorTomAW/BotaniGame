// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractionOption.h"
#include "AbilitySystem/Abilities/BotaniGameplayAbility.h"
#include "BotaniInteractionAbility.generated.h"


/**
 * UBotaniInteractionAbility
 *
 * A base class for a gameplay ability used for character interaction.
 */
UCLASS(Abstract)
class INTERACTIONCORE_API UBotaniInteractionAbility : public UBotaniGameplayAbility
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//~ End UGameplayAbility Interface

	/** Update the interaction options */
	UFUNCTION(BlueprintCallable, Category = "Botani|Interaction")
	void UpdateInteraction(const TArray<FBotaniInteractionOption>& InteractiveOptions);

	/** Called to trigger a single interaction */
	UFUNCTION(BlueprintCallable, Category = "Botani|Interaction")
	void TriggerInteraction();

protected:
	/** A list of the current interaction options */
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TArray<FBotaniInteractionOption> CurrentOptions;

	UPROPERTY()
	TArray<TObjectPtr<class UBotaniIndicatorDescriptor>> Indicators;

protected:
	/** The rate (in seconds) at which the ability scans for interactions */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (ClampMin = 0.0f, UIMin = 0.0f, Units = s))
	float InteractionScanRate = 0.1f;

	/** The range (in cm) at which the ability scans for interactions */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (ClampMin = 0.0f, UIMin = 0.0f, Units = cm))
	float InteractionScanRange = 500.f;

	/** The class of the default interaction widget that will be displayed when interacting */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;
};
