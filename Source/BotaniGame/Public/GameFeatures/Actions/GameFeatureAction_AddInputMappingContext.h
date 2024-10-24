// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddInputMappingContext.generated.h"

struct FComponentRequestHandle;

//////////////////////////////////////////////////////////////////////////
/// FInputMappingContextAndPriority
//////////////////////////////////////////////////////////////////////////

/**
 * FInputMappingContextAndPriority
 *
 * Structure that holds an input mapping context and a priority for that context.
 */
USTRUCT()
struct FInputMappingContextAndPriority
{
	GENERATED_BODY()

	/** The input mapping context to use for this priority */
	UPROPERTY(EditAnywhere, Category = "Input", meta = (AssetBundles = "Client,Server"))
	TSoftObjectPtr<class UInputMappingContext> InputMapping;

	/** Higher priority input mappings will be prioritized over mappings with a lower priority. */
	UPROPERTY(EditAnywhere, Category = "Input")
	int32 Priority = 0;

	/** If true, then this mapping context will be registered with the settings when this game feature action is registered. */
	UPROPERTY(EditAnywhere, Category = "Input")
	uint8 bRegisterWithSettings:1;
};

//////////////////////////////////////////////////////////////////////////
/// UGameFeatureAction_AddInputMappingContext
//////////////////////////////////////////////////////////////////////////

/**
 * UGameFeatureAction_AddInputMappingContext
 *
 * Game feature action that adds input mapping contexts to local players' EnhancedInput system.
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Input Mapping Context"))
class UGameFeatureAction_AddInputMappingContext final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	//~Begin UGameFeatureAction interface
	virtual void OnGameFeatureRegistering() override;
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void OnGameFeatureUnregistering() override;
	//~ End UGameFeatureAction interface

	//~ Begin UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	//~ End UObject interface

protected:
	/** List of input mapping contexts to add to the local players' EnhancedInput system */
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<FInputMappingContextAndPriority> InputMappings;

private:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APlayerController>> ControllersAddedTo;
	};
	typedef TMap<FGameFeatureStateChangeContext, FPerContextData> FContextData;
	FContextData ContextData;

	/** Delegate for when the game instance is changed to register IMC's */
	FDelegateHandle RegisterInputContextMappingsForGameInstanceHandle;

	//~UGameFeatureAction_WorldActionBase interface
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
	//~End of UGameFeatureAction_WorldActionBase interface

private:
	/** Registers owned Input Mapping Contexts to the Input Registry Subsystem. Also binds onto the start of GameInstances and the adding/removal of Local Players. */
	void RegisterInputMappingContexts();
	
	/** Registers owned Input Mapping Contexts to the Input Registry Subsystem for a specified GameInstance. This also gets called by a GameInstance Start. */
	void RegisterInputContextMappingsForGameInstance(UGameInstance* GameInstance);

	/** Registers owned Input Mapping Contexts to the Input Registry Subsystem for a specified Local Player. This also gets called when a Local Player is added. */
	void RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);

	/** Unregisters owned Input Mapping Contexts from the Input Registry Subsystem. Also unbinds from the start of GameInstances and the adding/removal of Local Players. */
	void UnregisterInputMappingContexts();

	/** Unregisters owned Input Mapping Contexts from the Input Registry Subsystem for a specified GameInstance. */
	void UnregisterInputContextMappingsForGameInstance(UGameInstance* GameInstance);

	/** Unregisters owned Input Mapping Contexts from the Input Registry Subsystem for a specified Local Player. This also gets called when a Local Player is removed. */
	void UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);
	
	void Reset(FPerContextData& ActiveData);
	void HandleControllerExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
	void AddInputMappingForPlayer(UPlayer* Player, FPerContextData& ActiveData);
	void RemoveInputMapping(APlayerController* PlayerController, FPerContextData& ActiveData);
};
