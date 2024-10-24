// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Bot/BotaniBotController.h"
#include "BTTask_SetBotState.generated.h"

/**
 * Attempts to change the bot's current state.
 */
UCLASS(meta = (DisplayName = "[BIO] Set Bot State", ShortTooltip = "Sets the bot's current state."))
class UBTTask_SetBotState : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetBotState();
	
	/** The new state to set. */
	UPROPERTY(EditAnywhere, Category = "Bot")
	TEnumAsByte<EBotaniBotState> NewState;

	/** Whether to fail if the new state is the same as the current state. */
	UPROPERTY(EditAnywhere, Category = "Bot")
	uint32 bFailIfSameState : 1;

protected:
	//~ Begin UBTTaskNode Interface
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
	//~ End UBTTaskNode Interface
};
