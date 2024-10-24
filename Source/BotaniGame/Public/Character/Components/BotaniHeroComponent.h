// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "BotaniHeroComponent.generated.h"

struct FGameplayAbilitySpecHandle;
struct FInputMappingContextAndPriority;
struct FInputActionValue;



/**
 * UBotaniHeroComponent
 *
 * Component that sets up input and camera handling for player controlled pawns (or bots that simulate players).
 * This depends on the PawnExtensionComponent to coordinate initialization.
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class BOTANIGAME_API UBotaniHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_UCLASS_BODY()

public:
	/** Static getter that returns the hero component if one exists on the specified actor. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Hero")
	static UBotaniHeroComponent* FindHeroComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UBotaniHeroComponent>() : nullptr); }

	/** Overrides the camera from an active gameplay ability. */
	virtual void SetAbilityCameraMode(TSubclassOf<class UBotaniCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle);

	/** Clears the camera mode override from an active gameplay ability. */
	virtual void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);

	/** The name of the extension event sent via UGameFrameworkComponentManager when ability inputs are ready to bind */
	static const FName NAME_BindInputsNow;

	/** The name of this component-implemented feature */
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

	bool IsReadyToBindInputs() const { return bReadyToBindInputs; }

	TSubclassOf<UBotaniCameraMode> DetermineCameraMode() const;

	void AddAdditionalInputConfig(const class UBotaniInputConfig* InputConfig);
	void RemoveAdditionalInputConfig(const class UBotaniInputConfig* InputConfig);

protected:
	//~ Begin UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRegister() override;
	//~ End UActorComponent interface

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);

protected:
	UPROPERTY(EditAnywhere)
	TArray<FInputMappingContextAndPriority> DefaultInputMappings;

	/** The camera mode to use when an ability is active. */
	UPROPERTY()
	TSubclassOf<UBotaniCameraMode> AbilityCameraMode;

	/** Spec handle of the ability that is currently overriding the camera mode. */
	FGameplayAbilitySpecHandle AbilityCameraModeSpecHandle;

private:
	/** True when player input bindings have been applied. Will never be true for AI controlled pawns. */
	uint8 bReadyToBindInputs:1;
};
