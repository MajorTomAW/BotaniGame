// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "BotaniVerbMessage.generated.h"

/**
 * FBotaniVerMessage
 *
 * Represents a generic message of the form Instigator Verb Target (in Context, with Magnitude)
 */
USTRUCT(BlueprintType)
struct FBotaniVerbMessage
{
	GENERATED_BODY()

public:
	/** Returns a debug string representation of this message */
	FString ToString() const;

public:
	/** The tag representing the verb of the message */
	UPROPERTY(BlueprintReadWrite, Category = "Message")
	FGameplayTag VerbTag;

	/** The instigator of the message */
	UPROPERTY(BlueprintReadWrite, Category = "Message")
	TObjectPtr<UObject> Instigator = nullptr;

	/** The target of the message */
	UPROPERTY(BlueprintReadWrite, Category = "Message")
	TObjectPtr<UObject> Target = nullptr;

	/** The tags associated with the instigator */
	UPROPERTY(BlueprintReadWrite, Category = "Message")
	FGameplayTagContainer InstigatorTags;

	/** The tags associated with the target */
	UPROPERTY(BlueprintReadWrite, Category = "Message")
	FGameplayTagContainer TargetTags;

	/** Optional tags associated with the context of the message */
	UPROPERTY(BlueprintReadWrite, Category = "Message")
	FGameplayTagContainer OptContextTags;

	/** The size of the message */
	UPROPERTY(BlueprintReadWrite, Category = "Message")
	double Magnitude = 1.0;
};
