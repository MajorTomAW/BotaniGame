// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Player/BotaniBotCoreController.h"
#include "BotaniBotController.generated.h"

/**
 * The bot's current state.
 */
UENUM(BlueprintType)
enum EBotaniBotState
{
	/** The bot is unaware of any targets. */
	BotState_Unaware = 1 << 0,

	/** The bot is suspicious and is investigating. */
	BotState_Suspicious = 1 << 1,

	/** The bot has lost its target. */
	BotState_LostTarget = 1 << 2,

	/** The bot is threatened and is attacking. */
	BotState_Threatened = 1 << 3,

	/** The bot is patrolling. */
	BotState_Patrolling = 1 << 4,

	/** The bot is in a custom state. */
	BotState_None = 0
};

/**
 * ABotaniBotController
 *
 * An actor class.
 */
UCLASS()
class BOTANIAI_API ABotaniBotController : public ABotaniBotCoreController
{
	GENERATED_UCLASS_BODY()

public:
	/** The bot's current state. */
	UPROPERTY(BlueprintReadWrite, Category = "Botani|Bot")
	TEnumAsByte<EBotaniBotState> BotState;

	/**
	 * The bot's stress level.
	 * Higher values indicate that much is happening around the bot.
	 * A high stress level could indicate that the bot is currently very aggressive.
	 * A lower stress level most likely indicates that the bot is calm and trying to do other tasks than fighting.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Botani|Bot")
	float StressLevel;
};
