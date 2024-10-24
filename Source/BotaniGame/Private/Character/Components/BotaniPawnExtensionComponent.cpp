// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Character/Components/BotaniPawnExtensionComponent.h"

#include "BotaniLogChannels.h"
#include "AbilitySystem/BotaniAbilitySystemComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameplayTags/BotaniGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniPawnExtensionComponent)


const FName UBotaniPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UBotaniPawnExtensionComponent::UBotaniPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
}

void UBotaniPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PawnData);
}

void UBotaniPawnExtensionComponent::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();

	static const TArray<FGameplayTag> StateChain =
	{
		BotaniGameplayTags::InitState::TAG_InitState_Spawned,
		BotaniGameplayTags::InitState::TAG_InitState_DataAvailable,
		BotaniGameplayTags::InitState::TAG_InitState_DataInitialized,
		BotaniGameplayTags::InitState::TAG_InitState_GameplayReady
	};

	// Try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UBotaniPawnExtensionComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}


void UBotaniPawnExtensionComponent::SetPawnData(const UBotaniPawnData* InPawnData)
{
	check(InPawnData);
	APawn* Pawn = GetPawnChecked<APawn>();

	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		BOTANI_LOG(Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has PawnData [%s]"), *InPawnData->GetName(), *Pawn->GetName(), *PawnData->GetName());
		return;
	}

	PawnData = InPawnData;
	Pawn->ForceNetUpdate();

	CheckDefaultInitialization();
}

void UBotaniPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// if another feature has changed state, we need to check if we can progress
	if (Params.FeatureName == NAME_ActorFeatureName)
	{
		return;
	}

	if (Params.FeatureState == BotaniGameplayTags::InitState::TAG_InitState_DataAvailable)
	{
		CheckDefaultInitialization();
	}
}

bool UBotaniPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);
	APawn* Pawn = GetPawn<APawn>();

	// None -> Spawned
	if (!CurrentState.IsValid() &&
		DesiredState == BotaniGameplayTags::InitState::TAG_InitState_Spawned)
	{
		// As long as we're on a valid pawn, we count as spawned
		if (Pawn)
		{
			return true;
		}
	}

	// Spawned -> DataAvailable
	else if (CurrentState == BotaniGameplayTags::InitState::TAG_InitState_Spawned &&
		DesiredState == BotaniGameplayTags::InitState::TAG_InitState_DataAvailable)
	{
		// Pawn data is required
		if (!PawnData)
		{
			BOTANI_LOG(Log, TEXT("NO PAWN DATA!"));
			return false;
		}

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bHasAuthority || bIsLocallyControlled)
		{
			// Check for being possessed by a controller
			if (!GetController<AController>())
			{
				BOTANI_LOG(Log, TEXT("NO CONTROLLER!"));
				return false;
			}
		}

		BOTANI_LOG(Log, TEXT("NOW CHANGING FROM SPAWNED TO AVAILABLE!"));
		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == BotaniGameplayTags::InitState::TAG_InitState_DataAvailable &&
		DesiredState == BotaniGameplayTags::InitState::TAG_InitState_DataInitialized)
	{
		// Transition to initialize if all features have their data available
		const bool bCanChange = Manager->HaveAllFeaturesReachedInitState(Pawn, BotaniGameplayTags::InitState::TAG_InitState_DataAvailable);

		if (!bCanChange)
		{
			BOTANI_LOG(Warning, TEXT("Bot [%s] is not ready to transition to DataInitialized"), *Pawn->GetName());
		}

		return bCanChange;
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == BotaniGameplayTags::InitState::TAG_InitState_DataInitialized &&
		DesiredState == BotaniGameplayTags::InitState::TAG_InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UBotaniPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	// This is currently all handled by other components listening to this state change
}

void UBotaniPawnExtensionComponent::InitializeAbilitySystem(UBotaniAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		// The ability system is already initialized
		return;
	}

	if (AbilitySystemComponent)
	{
		// Clean up the old ability system component
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	const AActor* ExistingAvatar = InASC->GetAvatarActor();

	BOTANI_LOG(Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s]"),
		*InASC->GetName(), *Pawn->GetName(), *InOwnerActor->GetName(), *ExistingAvatar->GetName());

	if ((ExistingAvatar != nullptr) &&
		(ExistingAvatar != Pawn))
	{
		BOTANI_LOG(Log, TEXT("Existing Avatar (authoriy=%d"), ExistingAvatar->HasAuthority() ? 1 : 0);

		// There is already a pawn acting as the ASC's avatar, so we need to kick it out.
		// This can happen on clients if they're lagged: their new pawn is spawned + possessed before the dead one is removed
		ensure(!ExistingAvatar->HasAuthority());

		if (UBotaniPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	if (ensure(PawnData))
	{
		// InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}

	OnAbilitySystemInitialized.Broadcast();
}

void UBotaniPawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		AbilitySystemComponent = nullptr;
		return;
	}

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(BotaniGameplayTags::Ability::Behavior::TAG_Ability_Behavior_SurvivesDeath);

	AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
	//TODO AbilitySystemComponent->ClearAbilityInput();
	AbilitySystemComponent->RemoveAllGameplayCues();

	if (AbilitySystemComponent->GetOwnerActor() != nullptr)
	{
		AbilitySystemComponent->SetAvatarActor(nullptr);
	}
	else
	{
		// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
		AbilitySystemComponent->ClearActorInfo();
	}

	OnAbilitySystemUninitialized.Broadcast();
	AbilitySystemComponent = nullptr;
}

void UBotaniPawnExtensionComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent &&
		(AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());

		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}
	
	CheckDefaultInitialization();
}

void UBotaniPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UBotaniPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void UBotaniPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UBotaniPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}
}

void UBotaniPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();
	const APawn* Pawn = GetPawn<APawn>();

	ensureAlwaysMsgf(Pawn, TEXT("BotaniPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	// Ensure only one BotaniPawnExtensionComponent exists on the pawn
	TArray<UActorComponent*> AllPawnExtensionComponents;
	Pawn->GetComponents(StaticClass(), AllPawnExtensionComponents);
	ensureAlwaysMsgf(AllPawnExtensionComponents.Num() == 1, TEXT("Only one BotaniPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// Register with the init state system early, this will only work if this is a game world
	RegisterInitStateFeature();
}

void UBotaniPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for changes to all features
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// Notify the state manager that we are ready to be initialized
	ensure(TryToChangeInitState(BotaniGameplayTags::InitState::TAG_InitState_Spawned));
	CheckDefaultInitialization();
}

void UBotaniPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

