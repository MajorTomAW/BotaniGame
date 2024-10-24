// Copyright © 2024 Botanibots Team. All rights reserved.


#include "GameFeatures/Actions/GameFeatureAction_AddInputConfig.h"

#include "Components/GameFrameworkComponentManager.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFeatures/Actions/GameFeatureAction_WorldActionBase.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include "BotaniLogChannels.h"
#include "Character/Components/BotaniHeroComponent.h"
#include "Input/BotaniInputConfig.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputConfig)

#define LOCTEXT_NAMESPACE "GameFeatures"

void UGameFeatureAction_AddInputConfig::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) ||
		!ensure(ActiveData.PawnsAddedTo.IsEmpty()))
	{
		Reset(ActiveData);
	}
	
	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputConfig::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
	FPerContextData* ActiveData = ContextData.Find(Context);

	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddInputConfig::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 Index = 0;

	for (const TSoftObjectPtr<const UBotaniInputConfig>& Entry : InputConfigs)
	{
		if (Entry.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("NullInputConfig", "Null InputConfig at index {0}."), Index));
		}

		++Index;
	}
	
	return Result;
}
#endif

void UGameFeatureAction_AddInputConfig::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	const UWorld* World = WorldContext.World();
	const UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if ((GameInstance != nullptr) &&
		(World != nullptr) &&
		World->IsGameWorld())
	{
		UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance);

		if (ComponentManager == nullptr)
		{
			return;
		}

		UGameFrameworkComponentManager::FExtensionHandlerDelegate AddInputConfigsDelegate =
			UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandlePawnExtension, ChangeContext);

		TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle =
			ComponentManager->AddExtensionHandler(APawn::StaticClass(), AddInputConfigsDelegate);

		ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
	}
}

void UGameFeatureAction_AddInputConfig::Reset(FPerContextData& ActiveData)
{
	ActiveData.ExtensionRequestHandles.Empty();

	while (!ActiveData.PawnsAddedTo.IsEmpty())
	{
		TWeakObjectPtr<APawn> PawnPtr = ActiveData.PawnsAddedTo.Top();
		if (PawnPtr.IsValid())
		{
			RemoveInputMapping(PawnPtr.Get(), ActiveData);
		}
		else
		{
			ActiveData.PawnsAddedTo.Pop();
		}
	}
}

void UGameFeatureAction_AddInputConfig::HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	APawn* AsPawn = CastChecked<APawn>(Actor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) ||
		(EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveInputMapping(AsPawn, ActiveData);
	}
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) ||
		(EventName == UBotaniHeroComponent::NAME_BindInputsNow))
	{
		AddInputMappingForPlayer(AsPawn, ActiveData);
	}
}

void UGameFeatureAction_AddInputConfig::AddInputMappingForPlayer(APawn* Pawn, FPerContextData& ActiveData)
{
	const APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (PlayerController == nullptr)
	{
		BOTANI_GFP_LOG(Error, TEXT("Failed to find `APlayerController` for pawn [%s]. Input mappings will not be added."), *Pawn->GetName());
		return;
	}

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (LocalPlayer == nullptr)
	{
		BOTANI_GFP_LOG(Error, TEXT("Failed to find `ULocalPlayer` for pawn [%s]. Input mappings will not be added."), *Pawn->GetName());
		return;
	}

	BOTANI_GFP_LOG(Display, TEXT("Adding input mappings for pawn [%s]"), *Pawn->GetName());

	if (UEnhancedInputLocalPlayerSubsystem* InputSub = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		UBotaniHeroComponent* HeroComponent = UBotaniHeroComponent::FindHeroComponent(Pawn);
		if (HeroComponent && HeroComponent->IsReadyToBindInputs())
		{
			for (const TSoftObjectPtr<const UBotaniInputConfig>& Entry : InputConfigs)
			{
				const UBotaniInputConfig* BindSet = Entry.LoadSynchronous();
				if (BindSet)
				{
					HeroComponent->AddAdditionalInputConfig(BindSet);
				}
			}
		}
		ActiveData.PawnsAddedTo.AddUnique(Pawn);
	}
	else
	{
		UE_LOG(LogGameFeatures, Error, TEXT("Failed to find `UEnhancedInputLocalPlayerSubsystem` for local player. Input mappings will not be added. Make sure you're set to use the EnhancedInput system via config file."));
	}
}

void UGameFeatureAction_AddInputConfig::RemoveInputMapping(APawn* Pawn, FPerContextData& ActiveData)
{
	const APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (PlayerController == nullptr)
	{
		ActiveData.PawnsAddedTo.Remove(Pawn);
		return;
	}

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (LocalPlayer == nullptr)
	{
		ActiveData.PawnsAddedTo.Remove(Pawn);
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* InputSub = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		UBotaniHeroComponent* HeroComponent = UBotaniHeroComponent::FindHeroComponent(Pawn);
		if (HeroComponent)
		{
			for (const TSoftObjectPtr<const UBotaniInputConfig>& Entry : InputConfigs)
			{
				const UBotaniInputConfig* InputConfig = Entry.LoadSynchronous();
				if (InputConfig)
				{
					HeroComponent->RemoveAdditionalInputConfig(InputConfig);
				}
			}
		}
	}

	ActiveData.PawnsAddedTo.Remove(Pawn);
}

#undef LOCTEXT_NAMESPACE