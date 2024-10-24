// Copyright © 2024 Botanibots Team. All rights reserved.


#include "System/Tasks/AsyncAction_WaitExperienceReady.h"

#include "Game/Components/BotaniExperienceManagerComponent.h"
#include "Game/Experience/BotaniExperienceDefinition.h"


UAsyncAction_WaitExperienceReady::UAsyncAction_WaitExperienceReady(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAsyncAction_WaitExperienceReady* UAsyncAction_WaitExperienceReady::WaitForExperienceReady(UObject* WorldContextObject)
{
	UAsyncAction_WaitExperienceReady* MyObj = nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		MyObj = NewObject<UAsyncAction_WaitExperienceReady>();
		MyObj->WorldPtr = World;
		MyObj->RegisterWithGameInstance(World);
	}

	return MyObj;
}

void UAsyncAction_WaitExperienceReady::Activate()
{
	if (UWorld* World = WorldPtr.Get())
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			Step2_ListenToExperienceLoading(GameState);
		}
		else
		{
			World->GameStateSetEvent.AddUObject(this, &ThisClass::Step1_HandleGameStateSet);
		}
	}
	else
	{
		SetReadyToDestroy();
	}
}

void UAsyncAction_WaitExperienceReady::Step1_HandleGameStateSet(class AGameStateBase* GameState)
{
	if (UWorld* World = WorldPtr.Get())
	{
		World->GameStateSetEvent.RemoveAll(this);
	}

	Step2_ListenToExperienceLoading(GameState);
}

void UAsyncAction_WaitExperienceReady::Step2_ListenToExperienceLoading(class AGameStateBase* GameState)
{
	check(GameState);

	UBotaniExperienceManagerComponent* ExperienceManager = GameState->FindComponentByClass<UBotaniExperienceManagerComponent>();
	check(ExperienceManager);

	if (ExperienceManager->IsExperienceLoaded())
	{
		UWorld* World = GameState->GetWorld();
		check(World);
		
		World->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::Step4_BroadcastReady), 0.2f, false, 0.3f);
	}
	else
	{
		ExperienceManager->CallOrRegister_OnExperienceLoaded(FOnBotaniExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::Step3_HandleExperienceLoaded));
	}
}

void UAsyncAction_WaitExperienceReady::Step3_HandleExperienceLoaded(const UBotaniExperienceDefinition* CurrentExperience)
{
	TimerHandle.Invalidate();
	
	Step4_BroadcastReady();
}

void UAsyncAction_WaitExperienceReady::Step4_BroadcastReady()
{
	OnReady.Broadcast();
	SetReadyToDestroy();
}
