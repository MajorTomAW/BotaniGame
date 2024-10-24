// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Engine/DataAsset.h"
#include "BotaniBackingAttributeData.generated.h"


/**
 * FBotaniBackingAttribute
 *
 * Maps an attribute to the attribute that backs it.
 */
USTRUCT(BlueprintType)
struct FBotaniBackingAttributeInfo
{
	GENERATED_BODY()

	FBotaniBackingAttributeInfo()
		: bUsedAsCost(true)
	{
	}

public:
	/** The attribute that is backed by another attribute. */
	UPROPERTY(EditDefaultsOnly, Category = "Attributes", meta=(FilterMetaTag="HideFromModifiers"))
	FGameplayAttribute RelevantAttribute;

	/** The attribute that backs the relevant attribute. */
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	FGameplayAttribute BackingAttribute;

	/** Whether the backing attribute is used to check for cost. */
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	uint8 bUsedAsCost:1;
};

/**
 * UBotaniBackingAttributeData
 *
 * Non-mutable data asset that contains backing attribute data.
 * Used to define that one attribute backs another attribute, for example, damage is backed by health.
 */
UCLASS(BlueprintType, Const, meta = (DisplayName = "Botani Backing Attribute Data", ShortTooltip = "Data asset containing backing attribute data."))
class BOTANIGAME_API UBotaniBackingAttributeData : public UDataAsset
{
	GENERATED_BODY()

public:
	UBotaniBackingAttributeData();

protected:
	/** The list of backing attributes for relevant attributes can be sued to back the Damage attribute with a Health attribute, for example. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes", meta = (TitleProperty = "{RelevantAttribute} -> {BackingAttribute}"))
	TArray<FBotaniBackingAttributeInfo> BackingAttributes;
};
