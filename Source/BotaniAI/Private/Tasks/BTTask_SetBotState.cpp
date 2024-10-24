// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Tasks/BTTask_SetBotState.h"

UBTTask_SetBotState::UBTTask_SetBotState()
{
	NodeName = TEXT("Set Bot State");
	
	NewState = BotState_Suspicious;
	bFailIfSameState = false;
}

EBTNodeResult::Type UBTTask_SetBotState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	ABotaniBotController* BotController = Controller ? Cast<ABotaniBotController>(Controller) : nullptr;
	if (BotController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if (BotController->BotState == NewState)
	{
		return bFailIfSameState ? EBTNodeResult::Failed : EBTNodeResult::Succeeded;
	}

	BotController->BotState = NewState;
	return EBTNodeResult::Succeeded;
}

FString UBTTask_SetBotState::GetStaticDescription() const
{
	return FString::Printf(TEXT("Set Bot State to %s"), *UEnum::GetValueAsString(NewState));
}
