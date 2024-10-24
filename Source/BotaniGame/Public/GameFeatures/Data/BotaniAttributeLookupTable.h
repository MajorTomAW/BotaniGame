// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "BotaniAttributeLookupTable.generated.h"

/**
 * EBotaniAttributeDisplay
 */
UENUM(BlueprintType)
enum class EBotaniAttributeDisplay : uint8
{
	/** The attribute is displayed as a basic float */
	BasicFloat,

	/** The attribute is displayed as a basic integer */
	BasicInt,

	/** The attribute is displayed as a percentage (0.0-1.0) */
	NormalizedPercentage,
};

/**
 * FBotaniAttributeDetails
 */
USTRUCT(BlueprintType)
struct FBotaniAttributeDetails
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	FSlateBrush Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	FText Description;
};

/**
 * FBotaniAttributeLookupRow
 */
USTRUCT(BlueprintType)
struct FBotaniAttributeLookupTableRow : public FTableRowBase
{
	GENERATED_BODY()

	FBotaniAttributeLookupTableRow()
		: DisplayMethod(EBotaniAttributeDisplay::BasicFloat)
		, DisplayScalingFactor(1.0f)
		, bShowInSummaries(false)
		, bShowInDifferences(false)
		, bShowAsBuffInFE(false)
		, bNegativeValuesShouldBeDisplayedPositively(false)
	{
	}

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	FGameplayAttribute Attribute;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	EBotaniAttributeDisplay DisplayMethod;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	float DisplayScalingFactor;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	FText FormatText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	TArray<FBotaniAttributeDetails> AttributeDetails;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	uint8 bShowInSummaries:1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute")
	uint8 bShowInDifferences:1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute", meta = (DisplayName = "Show As Buff In Frontend"))
	uint8 bShowAsBuffInFE:1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute", meta = (DisplayName = "Always Show As Positive"))
	uint8 bNegativeValuesShouldBeDisplayedPositively:1;
};