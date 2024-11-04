// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "GameplayTagContainer.h"

#include "AIUtilityQueryContext.generated.h"

/**
 * EAIUtilityParameterType
 *
 * Enum that represents the type of utility parameter.
 */
UENUM(BlueprintType, meta = (DisplayName = "AI Utility Parameter Type"))
enum class EAIUtilityParameterType : uint8
{
	Float = 0,
	Int = 1,
	Bool = 2,
	Name = 3,
	Vector = 4,
	GameplayTag = 5,
	GameplayTagContainer = 6,
};

/**
 * FAIUtilityParameter
 *
 * Struct that holds a single AI Utility parameter.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "AI Utility Parameter"))
struct FAIUtilityParameter
{
	GENERATED_BODY()

public:
	/** The type of this parameter. */
	UPROPERTY(EditDefaultsOnly, Category = "AI Utility")
	EAIUtilityParameterType Type = EAIUtilityParameterType::Float;

	/** Literal float value of this parameter. */
	UPROPERTY(EditDefaultsOnly, Category = "AI Utility", meta = (EditCondition = "Type == EAIUtilityParameterType::Float", EditConditionHides))
	float FloatValue = 0.f;

	/** Literal int32 value of this parameter. */
	UPROPERTY(EditDefaultsOnly, Category = "AI Utility", meta = (EditCondition = "Type == EAIUtilityParameterType::Int", EditConditionHides))
	int32 IntValue = 0;

	/** Literal bool value of this parameter. */
	UPROPERTY(EditDefaultsOnly, Category = "AI Utility", meta = (EditCondition = "Type == EAIUtilityParameterType::Bool", EditConditionHides))
	bool BoolValue = false;

	/** Literal FName value of this parameter. */
	UPROPERTY(EditDefaultsOnly, Category = "AI Utility", meta = (EditCondition = "Type == EAIUtilityParameterType::Name", EditConditionHides))
	FName NameValue;

	/** Literal FVector value of this parameter. */
	UPROPERTY(EditDefaultsOnly, Category = "AI Utility", meta = (EditCondition = "Type == EAIUtilityParameterType::Vector", EditConditionHides))
	FVector VectorValue = FVector::ZeroVector;

	/** Literal FGameplayTag value of this parameter. */
	UPROPERTY(EditDefaultsOnly, Category = "AI Utility", meta = (EditCondition = "Type == EAIUtilityParameterType::GameplayTag", EditConditionHides))
	FGameplayTag GameplayTagValue;

	/** Literal FGameplayTagContainer value of this parameter. */
	UPROPERTY(EditDefaultsOnly, Category = "AI Utility", meta = (EditCondition = "Type == EAIUtilityParameterType::GameplayTagContainer", EditConditionHides))
	FGameplayTagContainer GameplayTagContainerValue;
	
	/** The unique key of this parameter. */
	FName UniqueKey;

public:
	friend bool operator==(const FAIUtilityParameter& Lhs, const FAIUtilityParameter& Rhs)
	{
		if (Lhs.Type != Rhs.Type)
		{
			return false;
		}
		
		switch (Lhs.Type)
		{
		case EAIUtilityParameterType::Float:
			{
				return FMath::IsNearlyEqual(Lhs.FloatValue, Rhs.FloatValue, KINDA_SMALL_NUMBER);
			}
		case EAIUtilityParameterType::Bool:
			{
				return Lhs.BoolValue == Rhs.BoolValue;
			}
		case EAIUtilityParameterType::Int:
			{
				return Lhs.IntValue == Rhs.IntValue;
			}
		case EAIUtilityParameterType::Name:
			{
				return Lhs.NameValue == Rhs.NameValue;
			}
		case EAIUtilityParameterType::Vector:
			{
				return Lhs.VectorValue.Equals(Rhs.VectorValue);
			}
		case EAIUtilityParameterType::GameplayTag:
			{
				return Lhs.GameplayTagValue == Rhs.GameplayTagValue;
			}
		case EAIUtilityParameterType::GameplayTagContainer:
			{
				return Lhs.GameplayTagContainerValue == Rhs.GameplayTagContainerValue;
			}
		}

		return false;
	}
};

/**
 * FAIUtilityParameterContainer
 *
 * Container struct that holds a list of AI Utility parameters.
 */
USTRUCT(BlueprintType)
struct FAIUtilityParameterContainer
{
	GENERATED_BODY()
	friend struct FAIUtilityQueryContext;

public:
	/** Returns a parameter of type float with the given key. */
	float GetFloatValue(const FName Key);

	/** Returns a parameter of type int32 with the given key. */
	int32 GetIntValue(const FName Key);

	/** Returns a parameter of type bool with the given key. */
	bool GetBoolValue(const FName Key);

	/** Returns a parameter of type FName with the given key. */
	FName GetNameValue(const FName Key);

	/** Returns a parameter of type FVector with the given key. */
	FVector GetVectorValue(const FName Key);

	/** Returns a parameter of type FRotator with the given key. */
	FGameplayTag GetGameplayTagValue(const FName Key);

	/** Returns a parameter of type FGameplayTagContainer with the given key. */
	FGameplayTagContainer GetGameplayTagContainerValue(const FName Key);

public:
	bool operator==(const FAIUtilityParameterContainer& Other) const
	{
		bool bIsEqual = NamedValues.Num() == Other.NamedValues.Num();

		if (bIsEqual)
		{
			for (const auto& KVP : NamedValues)
			{
				
			}
		}

		return bIsEqual;
	}

protected:
	/** The list of named values. */
	TArray<FAIUtilityParameter> NamedValues;
};

/**
 * FAIUtilityQueryContext
 *
 * Context utility struct that holds all data required for AI Utility queries.
 */
USTRUCT(BlueprintType)
struct FAIUtilityQueryContext
{
	GENERATED_BODY()
	
public:
	/** The actor representing the pawn with the utility manager. Should never be null. */
	UPROPERTY(BlueprintReadOnly, Category = "AI Utility")
	TObjectPtr<AActor> ControlledActor = nullptr;

	/** The actor representing the target of this context, if any. */
	UPROPERTY(BlueprintReadOnly, Category = "AI Utility")
	TWeakObjectPtr<AActor> TargetActor = nullptr;

	/** Location context for the query. (Could vary per context, determined by query provider.) */
	UPROPERTY(BlueprintReadOnly, Category = "AI Utility")
	FVector Location = FVector::ZeroVector;

	/** Container of parameters for the query. */
	UPROPERTY(BlueprintReadOnly, Category = "AI Utility")
	FAIUtilityParameterContainer Parameters;

public:
	bool operator==(const FAIUtilityQueryContext& Other) const
	{
		return ControlledActor == Other.ControlledActor
			&& TargetActor == Other.TargetActor
			&& Location == Other.Location
			&& Parameters == Other.Parameters;
	}

	FString ToString() const
	{
		TStringBuilder<256> Builder;
		Builder.Append("Context {");
		
		if (TargetActor.IsValid())
		{
			Builder.Append(" Target: ");
			Builder.Append(TargetActor->GetActorNameOrLabel());
		}

		if (!Location.IsNearlyZero())
		{
			Builder.Append(" Location: ");
			Builder.Append(Location.ToString());
		}

		return Builder.ToString();
	}
};