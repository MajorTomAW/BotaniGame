// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BotaniNumPopStyle_Niagara.generated.h"

/**
 * UBotaniNumPopStyle_Niagara
 *
 * Used to define what Niagara asset should be used for the Number Pop System representation
 */
UCLASS()
class UBotaniNumPopStyle_Niagara : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Name of the Niagara Array to set the Number Pop information */
	UPROPERTY(EditDefaultsOnly, Category = "NumberPop")
	FName NiagaraArrayName;

	/** Niagara System used to display the Number Pop */
	UPROPERTY(EditDefaultsOnly, Category = "NumberPop")
	TObjectPtr<class UNiagaraSystem> TextNiagara;
};
