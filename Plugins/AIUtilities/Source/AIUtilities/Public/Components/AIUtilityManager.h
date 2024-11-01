// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIUtilityQueryContext.h"
#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "AIUtilityManager.generated.h"

/**
 * EAIUtilityActionChoiceMethod
 *
 * The method used to choose an action from the list of available actions.
 */
UENUM(BlueprintType)
enum class EAIUtilityActionChoiceMethod : uint8
{
	/** Always pick the highest scoring action. */
	HighestScoring = 0 UMETA(DisplayName = "Single Highest Scoring"),

	/** Picks a weighted random action for all non-zero scoring actions. */
	WeightedRandomAll,

	/** Picks a weighted random action for the top N scoring actions. */
	WeightedRandomTopN,

	/** Picks a weighted random action for the top N percent scoring actions. */
	WeightedRandomTopNPercent UMETA(DisplayName = "Weighted Random Top N Percent"),
};

/**
 * FAIUtilityConfig
 *
 * A struct that holds the configuration for the AI Utility Manager.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "AI Utility Config"))
struct FAIUtilityConfig
{
	GENERATED_BODY()

public:
	/** Determines how an action is chosen by the AI Utility Manager. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Utility Config")
	EAIUtilityActionChoiceMethod ActionChoiceMethod = EAIUtilityActionChoiceMethod::HighestScoring;

	/** When using the "Top N" action choice method, this value determines the value of N. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Utility Config", meta = (ClampMin = "1", UIMin = "1", EditCondition = "ActionChoiceMethod == EAIUtilityActionChoiceMethod::WeightedRandomTopN", EditConditionHides))
	int32 TopN = 5;

	/**
	 * When using the "Top N Percent" action choice method,
	 * this value determines the percentage value of N.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Utility Config", meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Units = "%", EditCondition = "ActionChoiceMethod == EAIUtilityActionChoiceMethod::WeightedRandomTopNPercent", EditConditionHides))
	float TopNPercent = 80.0f;

	/** When any of these gameplay tags are present, the AI Utility Manager will not run. (Useful for blocking certain actions when the AI is stunned, etc) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Utility Config")
	FGameplayTagContainer PreventBrainUpdateIfHasTags;

	/** List of action sets to use for the AI Utility Manager. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Utility Config")
	TArray<TObjectPtr<class UAIUtilityActionSet>> ActionSets;

	/** List of individual actions to use for the AI Utility Manager. */
	UPROPERTY(EditAnywhere, Category = "AI Utility Action Set", Instanced)
	TArray<TObjectPtr<class UAIUtilityActionDefinition>> Actions;
};

/**
 * FAIUtilityActionScoringResult
 *
 * Output result of an action + context pair being evaluated.
 * Only recorded if the score is non-zero.
 */
USTRUCT()
struct FAIUtilityActionScoringResult
{
	GENERATED_BODY()

public:
	int ActionDefIndex = INDEX_NONE;
	FAIUtilityQueryContext Context;
	float Score = 0.0f;
};

/**
 * FAIUtilityActionHistory
 *
 * History of actions that were previously run
 */
USTRUCT()
struct FAIUtilityActionHistory
{
	GENERATED_BODY()

public:
	/** The time at which this action was last started. */
	double LastStartTime = -UE_DOUBLE_BIG_NUMBER;

	/** The time at which this action was last completed or interrupted. */
	double LastEndTime = -UE_DOUBLE_BIG_NUMBER;

	/** The context in which this action was last run. */
	FAIUtilityQueryContext LastContext;

	/** The score of this action when it was last run. */
	float LastScore = 0.0f;

	/** Negative scoring bias value which discourages running this action again after it completes. (bleeds away over time) */
	float RepetitionPenalty = 0.0f;

	/** Positive or negative bias applied manually for a temporary period of time. */
	float TempScoreAdjust = 0.0f;

	/** The speed at which that TempScoreAdjust value bleeds away. */
	float TempScoreAdjustCooldownRate = 0.0f;
};

/**
 * UAIUtilityManager
 *
 * An actor component class.
 */
UCLASS(MinimalAPI, ClassGroup = (AI), meta = (BlueprintSpawnableComponent, DisplayName = "AI Utility Manager"))
class UAIUtilityManager : public UBrainComponent
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UBrainComponent Interface
	AIUTILITIES_API virtual void StartLogic() override;
	AIUTILITIES_API virtual void RestartLogic() override;
	AIUTILITIES_API virtual void StopLogic(const FString& Reason) override;
	AIUTILITIES_API virtual void PauseLogic(const FString& Reason) override;
	AIUTILITIES_API virtual EAILogicResuming::Type ResumeLogic(const FString& Reason) override;
	//~ End UBrainComponent Interface

	bool NeedsUpdate() const { return bQueuedForUpdate; }
	virtual void Update();

	/** Called to set and update the AI Utility Config. */
	UFUNCTION(BlueprintCallable, Category = "AI Utility Manager")
	void SetAIUtilityConfigFromAsset(class UAIUtilityConfigAsset* InConfigAsset);
	
protected:
	/** Determines whether this AI Utility Manager is awaiting an update that has been queued with the subsystem. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "AI Utility Manager")
	uint32 bQueuedForUpdate : 1;

	/** Determines whether this AI Utility Manager wanted to update but was prevented because of an action. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "AI Utility Manager")
	uint32 bWasPreventedFromUpdating : 1;

	/** Current utility configuration. Can be set in CD or from an asset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Utility Manager")
	FAIUtilityConfig UtilityConfig;

	/**
	 * The asset which will be used to provide the configuration for this AI Utility Manager.
	 * @note This will override any existing configurations at startup.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Utility Manager", BlueprintSetter=SetAIUtilityConfigFromAsset)
	TObjectPtr<class UAIUtilityConfigAsset> ConfigAsset;
	
private:
	bool bIsLogicStopped = false;
	FString LogicStoppedReason;
};
