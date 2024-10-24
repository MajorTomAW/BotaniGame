// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "ModularCharacter.h"
#include "Components/Movement/BotaniMovementTypes.h"
#include "Teams/BotaniTeamAgentInterface.h"
#include "BotaniCharacter.generated.h"

class FOnBotaniTeamDisplayAssetChangedDelegate;
class UBotaniAbilitySystemComponent;
class ABotaniPlayerController;
class ABotaniPlayerState;

/**
 * ABotaniCharacter
 *
 * The very base character class for Botani characters.
 * Responsible for sending events to pawn components.
 * New behavior should be added via pawn components when possible.
 */
UCLASS(Config = Game, meta = (ShortTooltip = "The very base character class for Botani characters."))
class BOTANIGAME_API ABotaniCharacter : public AModularCharacter, public IAbilitySystemInterface/*, public IGameplayTagAssetInterface*/, public IBotaniTeamAgentInterface
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin AActor Interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	//~ End AActor Interface

	//~ Begin APawn Interface
	virtual void NotifyControllerChanged() override;
	//~ End APawn Interface

	//~ Begin ACharacter Interface
	virtual void Restart() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	//~ End ACharacter Interface

	//~ Begin IAbilitySystemInterface Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface Interface

	//~ Begin IGameplayTagAssetInterface Interface
	//~ End IGameplayTagAssetInterface Interface

	//~ Begin IBotaniTeamAgentInterface Interface
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnBotaniTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~ End IBotaniTeamAgentInterface Interface

	/** Delegate for when the team index has changed */
	UPROPERTY()
	FOnBotaniTeamIndexChangedDelegate OnTeamIndexChangedDelegate;

	virtual class UBotaniMovementComponent* GetBotaniMoveComp() const { return BotaniMoveComp; }
	virtual USkeletalMeshComponent* GetFirstPersonArmsMesh() const { return FirstPersonArmsMesh; }

	virtual void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	virtual FCollisionQueryParams GetIgnoreCharacterQueryParams() const;

#pragma region blueprint_helpers
public:
	/** Returns the owning player controller for this character. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Character")
	ABotaniPlayerController* GetBotaniPlayerController() const;

	/** Returns the owning player state for this character. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Character")
	ABotaniPlayerState* GetBotaniPlayerState() const;

	/** Returns the ability system component for this character. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Character")
	UBotaniAbilitySystemComponent* GetBotaniAbilitySystemComponent() const;

	/** Returns the botani movement component for this character. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Botani|Character")
	UBotaniMovementComponent* GetBotaniMovementComponent() const;
#pragma endregion

public:
	virtual bool CanJumpInternal_Implementation() const override;
	
	/************************************************************************
	 * Movement Logic: Sprinting
	 ************************************************************************/
	
	/**
	 * Make the character start sprinting on the next update.
	 * Will call CanSprint() first to check if the character is allowed to sprint.
	 */
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void Sprint();

	/**
	 * Make the character stop sprinting on the next update.
	 * Call this from an input event (such as a button 'up' event) to stop sprinting.
	 * Will revert the character to walking speed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void StopSprinting();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSprinting(bool bNewSprinting);

	/** Check if the character can sprint in the current state. */
	UFUNCTION(BlueprintCallable, Category = "Character")
	bool CanSprint() const;

	UFUNCTION(BlueprintCallable, Category = "Character")
	bool IsSprinting() const;

	/** When true, the player wants to sprint */
	UPROPERTY(Transient, Replicated)
	uint32 bWantsToSprint:1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "Character")
	uint32 bWasSprinting:1;

	/************************************************************************
	 * Movement Logic: Wall Running
	 ************************************************************************/

	/**
	 * Make the character be able to start wall running on the next update.
	 * Will transition automatically into wall running mode if the character is in the correct state.
	 */
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void SetCanWallRun(bool bNewCanWallRun);

	/** Server function to make sure the character can wall run */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCanWallRun(bool bNewCanWallRun);

	/** When true, the player wants to start wall running */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
	uint32 bWantsToWallRun:1;

	/**
	 * Check whether the character can perform wall running.
	 * @note Checks only for walid walls, ignoring the current character state.
	 * 
	 * @param OutHit The hit result of the wall trace. Left unchanged if no wall was found. 
	 * @return true if the character can wall run, false otherwise.
	 */
	virtual bool CanWallRun(FHitResult* OutHit = nullptr) const;

	virtual bool IsAllowedToWallRun() const;

	/**
	 * Checks whether the character is currently wall running.
	 * @return true if the character is wall running, false otherwise.
	 */
	virtual bool IsWallRunning() const;

	/**
	 * Check for a wall that the character can run on.
	 *
	 * @param OutHit The hit result of the wall trace.
	 * @param WallSide The side of the wall to check for.
	 * @param bShowDebug Whether to draw a debug line for the wall trace.
	 *
	 * @return true if a wall was found, false otherwise.
	 */
	virtual bool TraceWallsToRunOn(FHitResult& OutHit, EBotaniWallRunSide WallSide = Wall_Both, bool bShowDebug = false) const;

	/************************************************************************
	* Movement Logic: Sliding
	************************************************************************/

	/** When true, the player wants to start sliding */
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	uint32 bWantsToSlide:1;

	/** Make the character start sliding on the next update. */
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void Slide();

	/** Make the character stop sliding on the next update. */
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void StopSliding();

	/** Check if the character can slide in the current state. */
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual bool CanSlide() const;

	virtual bool IsAllowedToSlide() const;

	virtual bool IsSliding() const;

	virtual bool TraceSurfaceToSlideOn(FHitResult& OutHit, bool bShowDebug = false) const;

	/************************************************************************
	* Movement Logic: Grapple
	************************************************************************/

	/** When true, the player wants to start grappling */
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	uint32 bWantsToGrapple:1;

	virtual bool IsAllowedToGrapple() const;

	virtual bool IsGrappling() const;

	virtual bool CanGrapple(FHitResult& OutHit) const;

	virtual bool TraceForGrappleTarget(FHitResult& OutHit, bool bShowDebug = false) const;

	/** Make the character start grappling on the next update. */
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual bool Grapple();

	/** Make the character stop grappling on the next update. */
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void StopGrapple();

protected:
	/**
	 * Customizable event to check if the character can sprint in the current state.
	 * Default implementation returns true if the character is on the ground
	 * and has a valid CharacterMovementComponent.
	 * Crouching will not prevent sprinting by default unless bAllowSprintingWhenCrouched is false.
	 * 
	 * @return true if the character can sprint, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Character", meta = (DisplayName = "Can Sprint"))
	bool CanSprintInternal() const;
	bool SprintIsAllowedInternal() const;

	/** Marks character as not trying to sprint */
	virtual void ResetSprintState();

protected:
	/** Called when the ability system component has been initialized */
	virtual void OnAbilitySystemInitialized();

	/** Called when the ability system component has been uninitialized */
	virtual void OnAbilitySystemUninitialized();

	void InitializeGameplayTags();

	/** Begins the death sequence for the character (disables collision, movement, ai perception */
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	/** Ends the death sequence for the character (detaches controller, destroys pawn, etc */
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

private:
	/** Pointer to the pawn extension component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBotaniPawnExtensionComponent> PawnExtensionComponent;

	/** Pointer to the health component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBotaniHealthComponent> HealthComponent;
	
	/** Pointer to the Botani Movement Component */
	UPROPERTY()
	TObjectPtr<class UBotaniMovementComponent> BotaniMoveComp;

	/** Pointer to the first person arms mesh component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> FirstPersonArmsMesh;

	/** Pointer to the camera component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBotaniCameraComponent> CameraComponent; //@TODO: Maybe create a generic player character class

private:
	/** Replicated team ID */
	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);

	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);
};
