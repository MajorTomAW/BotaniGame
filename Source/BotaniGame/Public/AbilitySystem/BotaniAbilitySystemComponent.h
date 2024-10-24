// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Abilities/BotaniGameplayAbility.h"
#include "BotaniAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilitiesReplicated, const TArray<FGameplayAbilitySpec>& /** Abilities */);

/**
 * UBotaniAbilitySystemComponent
 *
 * Base ability system component class for the project.
 */
UCLASS()
class BOTANIGAME_API UBotaniAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UBotaniAbilitySystemComponent();

public:
	typedef TFunctionRef<bool(const UBotaniGameplayAbility* UBotaniAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	void CancelAbilitiesByFunc(const TShouldCancelAbilityFunc& ShouldCancelAbilityFunc, bool bReplicateCancelAbility);
	
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	void TryActivateAbilitiesOnSpawn();

	bool IsActivationGroupBlocked(EBotaniAbilityActivationGroup Group) const;
	void AddAbilityToActivationGroup(EBotaniAbilityActivationGroup Group, UBotaniGameplayAbility* BotaniAbility);
	void RemoveAbilityFromActivationGroup(EBotaniAbilityActivationGroup Group, UBotaniGameplayAbility* BotaniAbility);
	void CancelActivationGroupAbilities(EBotaniAbilityActivationGroup Group, UBotaniGameplayAbility* IgnoreBotaniAbility, bool bReplicateCancelAbility);

	void DeferredSetBaseAttributeValueFromReplication(const FGameplayAttribute& Attribute, float NewValue);
	void DeferredSetBaseAttributeValueFromReplication(const FGameplayAttribute& Attribute, FGameplayAttributeData NewValue);

	FOnAbilitiesReplicated OnAbilitiesReplicatedCallbacks;

public:
	//~ Begin UAbilitySystemComponent Interface
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRep_ActivateAbilities() override;
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	//~ End UAbilitySystemComponent Interface

private:
	/** Handle to abilities that had their input tag pressed this frame. */
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	/** Handle to abilities that had their input tag released this frame. */
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	/** Handle to abilities that have their input held down. */
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	/** Handle to store all abilities that have been activated this input action. */
	TArray<FGameplayAbilitySpecHandle> ActivatedInputAbilities;

	/** Number of abilities running in each activation group. */
	int32 ActivationGroupCounts[static_cast<uint8>(EBotaniAbilityActivationGroup::MAX)];
};
