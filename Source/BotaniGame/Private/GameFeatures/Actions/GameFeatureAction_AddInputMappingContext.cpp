// Copyright © 2024 Botanibots Team. All rights reserved.


#include "GameFeatures/Actions/GameFeatureAction_AddInputMappingContext.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include "BotaniLogChannels.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Character/Components/BotaniHeroComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "System/BotaniAssetManager.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputMappingContext)

#define LOCTEXT_NAMESPACE "GameFeatures"

void UGameFeatureAction_AddInputMappingContext::OnGameFeatureRegistering()
{
	Super::OnGameFeatureRegistering();
	RegisterInputMappingContexts();
}

void UGameFeatureAction_AddInputMappingContext::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);

	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) ||
		!ensure(ActiveData.ControllersAddedTo.IsEmpty()))
	{
		Reset(ActiveData);
	}
	
	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputMappingContext::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	FPerContextData* ActiveData = ContextData.Find(Context);
	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}

void UGameFeatureAction_AddInputMappingContext::OnGameFeatureUnregistering()
{
	Super::OnGameFeatureUnregistering();
	UnregisterInputMappingContexts();
}

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddInputMappingContext::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 Index = 0;

	for (const FInputMappingContextAndPriority& Entry : InputMappings)
	{
		if (Entry.InputMapping.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("NullInputMapping", "Null InputMapping at index {0}."), Index));
		}
		++Index;
	}

	return Result;
}
#endif

void UGameFeatureAction_AddInputMappingContext::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	const UWorld* World = WorldContext.World();
	const UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if ((GameInstance != nullptr) &&
		(World != nullptr) &&
		(World->IsGameWorld()))
	{
		if (UGameFrameworkComponentManager* FrameworkSub = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddInputMappingsDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandleControllerExtension, ChangeContext);

			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle =
				FrameworkSub->AddExtensionHandler(APlayerController::StaticClass(), AddInputMappingsDelegate);

			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

void UGameFeatureAction_AddInputMappingContext::RegisterInputMappingContexts()
{
	RegisterInputContextMappingsForGameInstanceHandle =
		FWorldDelegates::OnStartGameInstance.AddUObject(this, &ThisClass::RegisterInputContextMappingsForGameInstance);

	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();

	for (TIndirectArray<FWorldContext>::TConstIterator WorldContextIt = WorldContexts.CreateConstIterator(); WorldContextIt; ++WorldContextIt)
	{
		RegisterInputContextMappingsForGameInstance(WorldContextIt->OwningGameInstance);
	}
}

void UGameFeatureAction_AddInputMappingContext::RegisterInputContextMappingsForGameInstance(UGameInstance* GameInstance)
{
	if ((GameInstance != nullptr) &&
		(!GameInstance->OnLocalPlayerAddedEvent.IsBoundToObject(this)))
	{
		GameInstance->OnLocalPlayerAddedEvent.AddUObject(this, &ThisClass::RegisterInputMappingContextsForLocalPlayer);
		GameInstance->OnLocalPlayerRemovedEvent.AddUObject(this, &ThisClass::UnregisterInputMappingContextsForLocalPlayer);

		for (TArray<ULocalPlayer*>::TConstIterator LocalPlayerIt = GameInstance->GetLocalPlayerIterator(); LocalPlayerIt; ++LocalPlayerIt)
		{
			RegisterInputMappingContextsForLocalPlayer(*LocalPlayerIt);
		}
	}
}

void UGameFeatureAction_AddInputMappingContext::RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
	if (!ensure(LocalPlayer))
	{
		return;
	}

	UBotaniAssetManager& AssetManager = UBotaniAssetManager::Get();

	if (UEnhancedInputLocalPlayerSubsystem* InputSub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		if (UEnhancedInputUserSettings* Settings = InputSub->GetUserSettings())
		{
			for (const auto& Entry : InputMappings)
			{
				// Skip entries that don't want to be registered
				if (!Entry.bRegisterWithSettings)
				{
					continue;
				}

				// Register the IMC with the settings
				if (UInputMappingContext* IMC = AssetManager.GetAsset(Entry.InputMapping, true))
				{
					Settings->RegisterInputMappingContext(IMC);
					BOTANI_GFP_LOG(Display, TEXT("Registered Input Mapping Context %s for LocalPlayer %s"), *IMC->GetName(), *LocalPlayer->GetName());
				}
			}
		}
	}
}

void UGameFeatureAction_AddInputMappingContext::UnregisterInputMappingContexts()
{
	FWorldDelegates::OnStartGameInstance.Remove(RegisterInputContextMappingsForGameInstanceHandle);
	RegisterInputContextMappingsForGameInstanceHandle.Reset();

	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (TIndirectArray<FWorldContext>::TConstIterator WorldContextId = WorldContexts.CreateConstIterator(); WorldContextId; ++WorldContextId)
	{
		UnregisterInputContextMappingsForGameInstance(WorldContextId->OwningGameInstance);
	}
}

void UGameFeatureAction_AddInputMappingContext::UnregisterInputContextMappingsForGameInstance(UGameInstance* GameInstance)
{
	if (GameInstance == nullptr)
	{
		return;
	}

	GameInstance->OnLocalPlayerAddedEvent.RemoveAll(this);
	GameInstance->OnLocalPlayerRemovedEvent.RemoveAll(this);

	for (TArray<ULocalPlayer*>::TConstIterator LocalPlayerIt = GameInstance->GetLocalPlayerIterator(); LocalPlayerIt; ++LocalPlayerIt)
	{
		UnregisterInputMappingContextsForLocalPlayer(*LocalPlayerIt);
	}
}

void UGameFeatureAction_AddInputMappingContext::UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
	if (!ensure(LocalPlayer))
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* InputSub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		if (UEnhancedInputUserSettings* Settings = InputSub->GetUserSettings())
		{
			for (const auto& Entry : InputMappings)
			{
				// Skip entries that don't want to be unregistered
				if (!Entry.bRegisterWithSettings)
				{
					continue;
				}

				// Unregister the IMC with the settings
				if (UInputMappingContext* IMC = Entry.InputMapping.Get())
				{
					Settings->UnregisterInputMappingContext(IMC);
				}
			}
		}
	}
}

void UGameFeatureAction_AddInputMappingContext::Reset(FPerContextData& ActiveData)
{
	ActiveData.ExtensionRequestHandles.Empty();

	while (!ActiveData.ControllersAddedTo.IsEmpty())
	{
		TWeakObjectPtr<APlayerController> ControllerPtr = ActiveData.ControllersAddedTo.Top();

		if (ControllerPtr.IsValid())
		{
			RemoveInputMapping(ControllerPtr.Get(), ActiveData);
		}
		else
		{
			ActiveData.ControllersAddedTo.Pop();
		}
	}
}

void UGameFeatureAction_AddInputMappingContext::HandleControllerExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	APlayerController* AsController = CastChecked<APlayerController>(Actor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveInputMapping(AsController, ActiveData);
	}
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UBotaniHeroComponent::NAME_BindInputsNow))
	{
		AddInputMappingForPlayer(AsController->GetLocalPlayer(), ActiveData);
	}
}

void UGameFeatureAction_AddInputMappingContext::AddInputMappingForPlayer(UPlayer* Player, FPerContextData& ActiveData)
{
	const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer == nullptr)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* InputSub = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		for (const auto& Entry : InputMappings)
		{
			if (const UInputMappingContext* IMC = Entry.InputMapping.Get())
			{
				InputSub->AddMappingContext(IMC, Entry.Priority);
			}
		}
	}
	else
	{
		UE_LOG(LogGameFeatures, Error, TEXT("Failed to get EnhancedInputLocalPlayerSubsystem for LocalPlayer %s"), *LocalPlayer->GetName());
	}

	UE_LOG(LogGameFeatures, Display, TEXT("Added Input Mapping Contexts for LocalPlayer %s"), *LocalPlayer->GetName());
}

void UGameFeatureAction_AddInputMappingContext::RemoveInputMapping(APlayerController* PlayerController, FPerContextData& ActiveData)
{
	if (const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSub = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			for (const auto& Entry : InputMappings)
			{
				if (const UInputMappingContext* IMC = Entry.InputMapping.Get())
				{
					InputSub->RemoveMappingContext(IMC);
				}
			}
		}
		else
		{
			UE_LOG(LogGameFeatures, Error, TEXT("Failed to get EnhancedInputLocalPlayerSubsystem for LocalPlayer %s"), *LocalPlayer->GetName());
		}
	}
	
	ActiveData.ControllersAddedTo.Remove(PlayerController);
}

#undef LOCTEXT_NAMESPACE
