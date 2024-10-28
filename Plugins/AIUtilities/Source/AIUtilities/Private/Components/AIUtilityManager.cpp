// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Components/AIUtilityManager.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(AIUtilityManager)

UAIUtilityManager::UAIUtilityManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAIUtilityManager::StartLogic()
{
	Super::StartLogic();
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
