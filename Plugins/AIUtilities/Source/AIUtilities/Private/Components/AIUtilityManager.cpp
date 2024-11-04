// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Components/AIUtilityManager.h"
#include "AIUtilityConfigAsset.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(AIUtilityManager)

UAIUtilityManager::UAIUtilityManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bQueuedForUpdate(false)
	, bWasPreventedFromUpdating(false)
	, ConfigAsset(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.SetTickFunctionEnable(false);
}

void UAIUtilityManager::StartLogic()
{
	Super::StartLogic();

	bIsLogicStopped = false;
	LogicStoppedReason = "";

	// If we're not the authority, we don't need to do anything.
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	if (IsValid(ConfigAsset))
	{
		//@TODO: SetBrainConfigFromAsset

		SetAIUtilityConfigFromAsset(ConfigAsset);
	}
	else
	{
		//@TODO: BrainCOnfigChanged()
	}
}

void UAIUtilityManager::RestartLogic()
{
	Super::RestartLogic();
}

void UAIUtilityManager::StopLogic(const FString& Reason)
{
	Super::StopLogic(Reason);
}

void UAIUtilityManager::PauseLogic(const FString& Reason)
{
	Super::PauseLogic(Reason);
}

EAILogicResuming::Type UAIUtilityManager::ResumeLogic(const FString& Reason)
{
	return Super::ResumeLogic(Reason);
}

void UAIUtilityManager::Update()
{
}

void UAIUtilityManager::SetAIUtilityConfigFromAsset(class UAIUtilityConfigAsset* InConfigAsset)
{
	UtilityConfig = InConfigAsset->DefaultConfig;
}
