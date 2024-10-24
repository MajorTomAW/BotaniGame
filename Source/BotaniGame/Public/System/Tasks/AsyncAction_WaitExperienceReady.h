// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_WaitExperienceReady.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExperienceReadyAsyncDelegate);

/**
 * UAsyncAction_WaitExperienceReady
 *
 * Asynchronously waits for the experience to be ready and valid and then calls the OnReady event.
 * Will call OnReady immediately if the game state is valid already.
 */
UCLASS()
class BOTANIGAME_API UAsyncAction_WaitExperienceReady : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/** Waits for the experience to be determined and loaded */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UAsyncAction_WaitExperienceReady* WaitForExperienceReady(UObject* WorldContextObject);

	//~ Begin UBlueprintAsyncActionBase Interface
	virtual void Activate() override;
	//~ End UBlueprintAsyncActionBase Interface

public:
	/** Called when the experience has been determined and is ready/loaded */
	UPROPERTY(BlueprintAssignable)
	FOnExperienceReadyAsyncDelegate OnReady;

private:
	void Step1_HandleGameStateSet(AGameStateBase* GameState);
	void Step2_ListenToExperienceLoading(AGameStateBase* GameState);
	void Step3_HandleExperienceLoaded(const class UBotaniExperienceDefinition* CurrentExperience);
	void Step4_BroadcastReady();

	TWeakObjectPtr<UWorld> WorldPtr;
	FTimerHandle TimerHandle;
};
