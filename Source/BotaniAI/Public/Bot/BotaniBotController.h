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
	BotState_Unaware = 1 << 0 UMETA(DisplayName = "Unaware"),

	/** The bot is suspicious and is investigating. */
	BotState_Suspicious = 1 << 1 UMETA(DisplayName = "Suspicious"),

	/** The bot has lost its target. */
	BotState_LostTarget = 1 << 2 UMETA(DisplayName = "Lost Target"),

	/** The bot is threatened and is attacking. */
	BotState_Threatened = 1 << 3 UMETA(DisplayName = "Threatened"),

	/** The bot is patrolling. */
	BotState_Patrolling = 1 << 4 UMETA(DisplayName = "Patrolling"),

	/** The bot is in a custom state. */
	BotState_None = 0 UMETA(Hidden),
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
