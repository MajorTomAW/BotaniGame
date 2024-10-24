// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "BotaniDeathCauseData.generated.h"

USTRUCT(BlueprintType)
struct FBotaniDeathCauseInfo
{
	GENERATED_BODY()

public:
	bool operator==(const FBotaniDeathCauseInfo& Other) const
	{
		return Other.DeathCauseTag.MatchesTagExact(DeathCauseTag);
	}

public:
	/** Tag that represents the cause of death. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	FGameplayTag DeathCauseTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	FText SelfInflictedMessage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death", meta = (DisplayName = "Self Inflicted down-but-not-out Message"))
	FText SelfInflictedDBNOMessage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	FText EliminatedMessage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death", meta = (DisplayName = "Eliminated down-but-not-out Message"))
	FText EliminatedDBNOMessage;
};

/**
 * UBotaniDeathCauseData
 *
 * Non-mutable data asset that holds data for a death causes.
 */
UCLASS(BlueprintType, Const, meta = (DisplayName = "Botani Death Cause Data", ShortTooltip = "Data asset used to define a death cause."))
class BOTANIGAME_API UBotaniDeathCauseData : public UDataAsset
{
	GENERATED_BODY()

public:
	FBotaniDeathCauseInfo GetDeathCauseInfo(const FGameplayTag& DeathCauseTag) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	TArray<FBotaniDeathCauseInfo> DeathCauseInfos;
};
