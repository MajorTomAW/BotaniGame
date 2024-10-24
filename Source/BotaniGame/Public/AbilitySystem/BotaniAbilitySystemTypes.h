// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once
#include "GameFeatures/Data/BotaniBackingAttributeData.h"

#include "BotaniAbilitySystemTypes.generated.h"

struct FGameplayAttribute;

/**
 * EBotaniAbilitySystemCreationPolicy
 *
 * Defines how an Ability System should be loaded (if ever) on the client.
 */
UENUM(BlueprintType)
enum class EBotaniAbilitySystemCreationPolicy : uint8
{
	/** The Ability System will always be null on the client */
	Never,

	/** The Ability System will be null in the client until it is used on the server */
	Lazy,

	/** The Ability System is always loaded whenever the owning actor is loaded */
	Always,
};

/** A pending attribute replication */
struct FPendingAttributeReplication
{
public:
	FPendingAttributeReplication()
	{
	}

	FPendingAttributeReplication(const FGameplayAttribute& InAttribute, const FGameplayAttributeData& InNewValue)
		: Attribute(InAttribute)
		, NewValue(InNewValue)
	{
	}


	FGameplayAttribute Attribute;
	FGameplayAttributeData NewValue;
};
