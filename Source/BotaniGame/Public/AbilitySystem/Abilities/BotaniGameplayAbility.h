// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BotaniGameplayAbility.generated.h"

class UBotaniAbilityCost;

/**
 * EBotaniAbilityHUDPolicy
 *
 * Defines the different ways an ability can be displayed in the HUD.
 */
UENUM(BlueprintType)
enum class EBotaniAbilityHUDPolicy : uint8
{
	/** Don't display this ability in the HUD. */
	None = 0,

	/** Display this ability in the HUD. */
	Info,

	/** Push an individual widget to the HUD. */
	Widget,
};

/**
 * EBotaniAbilityActivationPolicy
 *
 * Defines the different ways an ability can be activated.
 */
UENUM(BlueprintType)
enum class EBotaniAbilityActivationPolicy : uint8
{
	/** Try to activate the ability when the input is pressed. */
	OnInputTriggered,

	/** Try to activate the ability while the input is active. */
	WhileInputActive,

	/** Try to activate the ability when an avatar is assigned. → Boom it is a passive ability */
	OnSpawn,
};

/**
 * EBotaniAbilityActivationGroup
 *
 * Defines how an ability activates in relation to other abilities.
 */
UENUM(BlueprintType)
enum class EBotaniAbilityActivationGroup : uint8
{
	/** Ability runs independently of all other abilities. */
	Independent = 0,

	/** Ability is canceled and replaced by other exclusive abilities */
	Exclusive_Replaceable,

	/** Ability blocks all other exclusive abilities from activating */
	Exclusive_Blocking,

	MAX UMETA(Hidden)
};

/**
 * UBotaniGameplayAbility
 *
 * The base class for all gameplay abilities used in this project.
 */
UCLASS(HideCategories = "Input", meta = (ShortTooltip = "The base class for all gameplay abilities used in this project.", DisplayName = "Botani Ability"))
class BOTANIGAME_API UBotaniGameplayAbility : public UGameplayAbility
{
	GENERATED_UCLASS_BODY()
	friend class UBotaniAbilitySystemComponent;

public:
	/** Returns the BotaniAbilitySystemComponent that owns this ability. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability", meta = (DisplayName = "Get Botani Ability System"))
	class UBotaniAbilitySystemComponent* K2_GetBotaniAbilitySystemComponent() const { return GetBotaniAbilitySystemComponent(); }
	virtual UBotaniAbilitySystemComponent* GetBotaniAbilitySystemComponent() const;

	/** Returns the Controller from the owning actor's ActorInfo. May not work on non-instanced abilities */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability", meta = (DisplayName = "Get Controller"))
	class AController* K2_GetControllerFromActorInfo() const { return GetControllerFromActorInfo(); }
	virtual AController* GetControllerFromActorInfo() const;

	/** Returns the BotaniPlayerController from the owning actor's ActorInfo. May not work on non-instanced abilities */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability", meta = (DisplayName = "Get Botani Player Controller"))
	class ABotaniPlayerController* K2_GetBotaniPlayerControllerFromActorInfo() const { return GetBotaniPlayerControllerFromActorInfo(); }
	virtual ABotaniPlayerController* GetBotaniPlayerControllerFromActorInfo() const;

	/** Returns the BotaniPlayerState from the owning actor's ActorInfo. May not work on non-instanced abilities */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability", meta = (DisplayName = "Get Botani Player State"))
	class ABotaniPlayerState* K2_GetBotaniPlayerStateFromActorInfo() const { return GetBotaniPlayerStateFromActorInfo(); }
	virtual ABotaniPlayerState* GetBotaniPlayerStateFromActorInfo() const;

	/** Returns the BotaniCharacter from the owning actor's ActorInfo. May not work on non-instanced abilities */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability", meta = (DisplayName = "Get Botani Character"))
	class ABotaniCharacter* K2_GetBotaniCharacterFromActorInfo() const { return GetBotaniCharacterFromActorInfo(); }
	virtual ABotaniCharacter* GetBotaniCharacterFromActorInfo() const;

	/** Returns the BotaniHeroComponent from the owning actor's ActorInfo. May not work on non-instanced abilities */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability", meta = (DisplayName = "Get Botani Hero Component"))
	class UBotaniHeroComponent* K2_GetBotaniHeroComponentFromActorInfo() const { return GetBotaniHeroComponentFromActorInfo(); }
	virtual UBotaniHeroComponent* GetBotaniHeroComponentFromActorInfo() const;

	/** Returns the activation group of this ability */
	EBotaniAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

	/** Returns the activation policy of this ability */
	EBotaniAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	/** Returns the HUD policy of this ability */
	EBotaniAbilityHUDPolicy GetHUDPolicy() const { return HUDPolicy; }

	/** Tries to activate the ability when given */
	virtual void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	/** Sets the ability's camera mode. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability", meta = (DisplayName = "Set Camera Mode"))
	void SetCameraMode(TSubclassOf<class UBotaniCameraMode> CameraMode);

	/** Clears the ability's camera mode. Automatically called when the ability is removed. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability", meta = (DisplayName = "Clear Camera Mode"))
	void ClearCameraMode();

	/** Returns true if the ability can afford the cost and is currently not on cooldown. */
	UFUNCTION(BlueprintCallable, Category = "Botani|Ability", meta = (DisplayName = "Check Can Activate"))
	bool K2_CheckCanActivate();

protected:
	/** Defines how this ability is meant to be displayed in the HUD */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD", meta = (DisplayName = "HUD Policy"))
	EBotaniAbilityHUDPolicy HUDPolicy;
	
	/** Defines how this ability is meant to be activated */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Activation")
	EBotaniAbilityActivationPolicy ActivationPolicy;
	
	/** Defines the relationship between this ability activating and other abilities activating. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Activation")
	EBotaniAbilityActivationGroup ActivationGroup;

	/** List of additional costs that this ability has */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Costs")
	TArray<TObjectPtr<UBotaniAbilityCost>> AdditionalCosts;

public:
	//~ Begin UGameplayAbility Interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	//~ End UGameplayAbility Interface

	/** Called when this ability is granted to the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Botani|Ability", DisplayName = "On Ability Added")
	void K2_OnAbilityAdded();

	/** Called when this ability is removed from the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Botani|Ability", DisplayName = "On Ability Removed")
	void K2_OnAbilityRemoved();

	/** Called when the ability system is initialized with a pawn avatar. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Botani|Ability", DisplayName = "On Pawn Avatar Set")
	void K2_OnPawnAvatarSet();

protected:
	/** Current camera mode that this ability is using. */
	TSubclassOf<UBotaniCameraMode> ActiveCameraMode;
};
