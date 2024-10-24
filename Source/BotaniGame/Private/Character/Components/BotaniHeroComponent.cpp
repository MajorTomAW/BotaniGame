// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Character/Components/BotaniHeroComponent.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif

#include "BotaniLogChannels.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Character/Components/BotaniPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameplayTags/BotaniGameplayTags.h"
#include "Input/Component/BotaniInputComponent.h"
#include "Player/BotaniLocalPlayer.h"
#include "Player/BotaniPlayerController.h"
#include "Player/BotaniPlayerState.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputActionValue.h"
#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "Camera/BotaniCameraMode.h"
#include "GameFeatures/Actions/GameFeatureAction_AddInputMappingContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniHeroComponent)

namespace BotaniHero_Stick
{
	static constexpr float LookYawRate = 240.0f;
	static constexpr float LookPitchRate = 125.0f;
}

const FName UBotaniHeroComponent::NAME_ActorFeatureName("Hero");
const FName UBotaniHeroComponent::NAME_BindInputsNow("BindInputsNow");

UBotaniHeroComponent::UBotaniHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	bReadyToBindInputs = false;
}

void UBotaniHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		BOTANI_LOG(Error, TEXT("BotaniHeroComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("BotaniHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("BotaniHeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

void UBotaniHeroComponent::SetAbilityCameraMode(TSubclassOf<class UBotaniCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeSpecHandle = OwningSpecHandle;
	}
}

void UBotaniHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeSpecHandle = FGameplayAbilitySpecHandle();
	}
}

bool UBotaniHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);
	APawn* Pawn = GetPawn<APawn>();

	//if (Pawn->IsBotControlled()) BOTANI_LOG(Error, TEXT("		-		Attempting to change InitState from [%s] to [%s] for Bot controlled pawn [%s]"), *CurrentState.ToString(), *DesiredState.ToString(), *GetNameSafe(Pawn));

	// None -> Spawned
	if (!CurrentState.IsValid() &&
		DesiredState == BotaniGameplayTags::InitState::TAG_InitState_Spawned)
	{
		// As long as we have a real pawn, let us count as spawned
		if (Pawn)
		{
			return true;
		}
	}

	// Spawned -> DataAvailable
	else if (CurrentState == BotaniGameplayTags::InitState::TAG_InitState_Spawned &&
		DesiredState == BotaniGameplayTags::InitState::TAG_InitState_DataAvailable)
	{
		// The player state is required
		if (!GetPlayerState<ABotaniPlayerState>())
		{
			return false;
		}

		// If we're authority or locally controlled, we need a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPs = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPs)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			ABotaniPlayerController* BotaniPC = GetController<ABotaniPlayerController>();

			// The input component and local player are required when locally controlled
			if (!Pawn->InputComponent || !BotaniPC || !BotaniPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == BotaniGameplayTags::InitState::TAG_InitState_DataAvailable &&
		DesiredState == BotaniGameplayTags::InitState::TAG_InitState_DataInitialized)
	{
		// Wait for a player state and extension component
		const ABotaniPlayerState* BotaniPS = GetPlayerState<ABotaniPlayerState>();
		const bool bCanChange = BotaniPS && Manager->HasFeatureReachedInitState(Pawn, UBotaniPawnExtensionComponent::NAME_ActorFeatureName, BotaniGameplayTags::InitState::TAG_InitState_DataInitialized);

		if (!bCanChange)
		{
			BOTANI_LOG(Warning, TEXT("		Cannot change init state. PlayerState [%s] and PawnExtensionComponent [%s] must be at DataInitialized. but is at: %s"), *GetNameSafe(BotaniPS), *GetNameSafe(Pawn), *Manager->GetInitStateForFeature(Pawn, UBotaniPawnExtensionComponent::NAME_ActorFeatureName).ToString());
		}

		return bCanChange;
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == BotaniGameplayTags::InitState::TAG_InitState_DataInitialized &&
		DesiredState == BotaniGameplayTags::InitState::TAG_InitState_GameplayReady)
	{
		//@TODO: Maybe add more checks here such as ability initialization
		return true;
	}

	return false;
}

void UBotaniHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	APawn* Pawn = GetPawn<APawn>();
	//BOTANI_LOG(Error, TEXT("				Changing InitState from [%s] to [%s] for [%s]"), *CurrentState.ToString(), *DesiredState.ToString(), *GetNameSafe(Pawn));
	
	if (CurrentState != BotaniGameplayTags::InitState::TAG_InitState_DataAvailable ||
		DesiredState != BotaniGameplayTags::InitState::TAG_InitState_DataInitialized)
	{
		return;
	}
	
	ABotaniPlayerState* BotaniPS = GetPlayerState<ABotaniPlayerState>();
	if (!ensure(Pawn && BotaniPS))
	{
		return;
	}

	const UBotaniPawnData* PawnData = nullptr;

	if (UBotaniPawnExtensionComponent* PawnExtComp = UBotaniPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		PawnData = PawnExtComp->GetPawnData<UBotaniPawnData>();

		PawnExtComp->InitializeAbilitySystem(BotaniPS->GetBotaniAbilitySystemComponent(), BotaniPS);

		BOTANI_LOG(Display, TEXT("PawnData [%s] initialized for [%s]"), *GetNameSafe(PawnData), *GetNameSafe(Pawn));
	}

	if (GetController<ABotaniPlayerController>())
	{
		if (Pawn->InputComponent != nullptr)
		{
			InitializePlayerInput(Pawn->InputComponent);
		}
	}

	// Hookup the delegate for all pawns
	if (PawnData)
	{
		if (UBotaniCameraComponent* CameraComponent = UBotaniCameraComponent::FindCameraComponent(Pawn))
		{
			CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		}
	}
}

void UBotaniHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UBotaniPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == BotaniGameplayTags::InitState::TAG_InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

void UBotaniHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain =
	{
		BotaniGameplayTags::InitState::TAG_InitState_Spawned,
		BotaniGameplayTags::InitState::TAG_InitState_DataAvailable,
		BotaniGameplayTags::InitState::TAG_InitState_DataInitialized,
		BotaniGameplayTags::InitState::TAG_InitState_GameplayReady
	};

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

TSubclassOf<UBotaniCameraMode> UBotaniHeroComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	const APawn* Pawn = GetPawn<APawn>();
	if (Pawn == nullptr)
	{
		return nullptr;
	}

	if (UBotaniPawnExtensionComponent* PawnExtComp = UBotaniPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UBotaniPawnData* PawnData = PawnExtComp->GetPawnData<UBotaniPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

void UBotaniHeroComponent::AddAdditionalInputConfig(const UBotaniInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (Pawn == nullptr)
	{
		BOTANI_GFP_LOG(Warning, TEXT("Pawn is null, cannot add additional input config."));
		return;
	}

	const APlayerController* PlayerController = GetController<APlayerController>();
	check(PlayerController);

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* InputSub = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(InputSub);

	if (const UBotaniPawnExtensionComponent* PawnExtComp = UBotaniPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		UBotaniInputComponent* BotaniInput = Pawn->FindComponentByClass<UBotaniInputComponent>();

		if (ensureMsgf(BotaniInput, TEXT("Unexpected Input Component class! The Gameplay wont work as expected. Change the input component to UBotaniInputComponent!")))
		{
			BotaniInput->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
	else
	{
		BOTANI_GFP_LOG(Warning, TEXT("Pawn [%s] does not have a PawnExtensionComponent."), *GetNameSafe(Pawn));
	}
}

void UBotaniHeroComponent::RemoveAdditionalInputConfig(const UBotaniInputConfig* InputConfig)
{
	//@TODO: Implement remove additional input config
}

void UBotaniHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UBotaniPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we're done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(BotaniGameplayTags::InitState::TAG_InitState_Spawned));
	CheckDefaultInitialization();
}

void UBotaniHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UBotaniHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	BOTANI_LOG(Log, TEXT("Initializing player input for [%s]"), *GetNameSafe(GetOwner()));

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PlayerController = GetController<APlayerController>();
	check(PlayerController);

	const UBotaniLocalPlayer* LocalPlayer = Cast<UBotaniLocalPlayer>(PlayerController->GetLocalPlayer());
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* InputSub = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(InputSub);

	InputSub->ClearAllMappings();

	if (const UBotaniPawnExtensionComponent* PawnExtComp = UBotaniPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UBotaniPawnData* PawnData = PawnExtComp->GetPawnData<UBotaniPawnData>())
		{
			if (const UBotaniInputConfig* InputConfig = PawnData->InputConfig)
			{
				// Add the default mappings
				for (const auto& Mapping : DefaultInputMappings)
				{
					UInputMappingContext* MappingContext = Mapping.InputMapping.LoadSynchronous(); //@TODO: Load input mappings via asset manager
					if (MappingContext == nullptr)
					{
						BOTANI_LOG(Error, TEXT("InputMappingContextAndPriority [%s] has a null InputMapping."), *Mapping.InputMapping.ToString());
						continue;
					}

					if (!Mapping.bRegisterWithSettings)
					{
						continue;
					}

					if (UEnhancedInputUserSettings* Settings = InputSub->GetUserSettings())
					{
						Settings->RegisterInputMappingContext(MappingContext);
					}

					FModifyContextOptions Options = {};
					Options.bIgnoreAllPressedKeysUntilRelease = false;

					InputSub->AddMappingContext(MappingContext, Mapping.Priority, Options);
				}

				// Add the mappings from the pawn data
				UBotaniInputComponent* BotaniInput = Cast<UBotaniInputComponent>(PlayerInputComponent);
				if (ensureMsgf(BotaniInput, TEXT("Unexpected Input Component class! The Gameplay wont work as expected. Change the input component to UBotaniInputComponent!")))
				{
					// Add the key mappings that may have been set by the player
					// BotaniInput->AddIn//@TODO: Add player key mappings

					// Bind dynamic ability actions
					TArray<uint32> BindHandles;
					BotaniInput->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

					// Bind native actions
					BotaniInput->BindNativeAction(InputConfig, BotaniGameplayTags::Input::TAG_InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, true);
					BotaniInput->BindNativeAction(InputConfig, BotaniGameplayTags::Input::TAG_InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, true);
					BotaniInput->BindNativeAction(InputConfig, BotaniGameplayTags::Input::TAG_InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, true);
				}
			}
			else
			{
				BOTANI_LOG(Error, TEXT("PawnData [%s] does not have an InputConfig."), *GetNameSafe(PawnData));
			}
		}
		else
		{
			BOTANI_LOG(Error, TEXT("Pawn [%s] does not have a PawnData."), *GetNameSafe(Pawn));
		}
	}
	else
	{
		BOTANI_LOG(Error, TEXT("Pawn [%s] does not have a PawnExtensionComponent."), *GetNameSafe(Pawn));
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PlayerController), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UBotaniHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const UBotaniPawnExtensionComponent* PawnExtComp = UBotaniPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (UBotaniAbilitySystemComponent* BotaniASC = PawnExtComp->GetBotaniAbilitySystemComponent())
		{
			BotaniASC->AbilityInputTagPressed(InputTag);
		}
	}
}

void UBotaniHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const UBotaniPawnExtensionComponent* PawnExtComp = UBotaniPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (UBotaniAbilitySystemComponent* BotaniASC = PawnExtComp->GetBotaniAbilitySystemComponent())
		{
			BotaniASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UBotaniHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	const AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller == nullptr)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);

	if (Value.X != 0.f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		Pawn->AddMovementInput(MovementDirection, Value.X);
	}

	if (Value.Y != 0.f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		Pawn->AddMovementInput(MovementDirection, Value.Y);
	}
}

void UBotaniHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	const AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller == nullptr)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UBotaniHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (Pawn == nullptr)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = Pawn->GetWorld();
	check(World);

	if (Value.X != 0.f)
	{
		Pawn->AddControllerYawInput(Value.X * BotaniHero_Stick::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.f)
	{
		Pawn->AddControllerPitchInput(Value.Y * BotaniHero_Stick::LookPitchRate * World->GetDeltaSeconds());
	}
}
