// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ControllerComponent.h"
#include "BotaniNumberPopComponent.generated.h"

/**
 * FBotaniNumPopRequest
 *
 * Used as a query to request a number pop to be displayed.
 */
USTRUCT(BlueprintType)
struct BOTANIGAME_API FBotaniNumPopRequest
{
	GENERATED_BODY()

	FBotaniNumPopRequest()
		: WorldLocation(ForceInitToZero)
	{
	}

public:
	/** The world location to create the number pops at */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Number Pop")
	FVector WorldLocation;

	/** Tags related to the source/cause of the number pop (for determining a style) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Number Pop")
	FGameplayTagContainer SourceTags;

	/** Tags related to the target of the number pop (for determining a style) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Number Pop")
	FGameplayTagContainer TargetTags;

	/** The literal number to display */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Number Pop")
	int32 NumberToDisplay = 0;
};

/**
 * UBotaniNumberPopComponent
 *
 * A component that can be added to a controller to allow for the display of number pops.
 * Such as incoming damage, healing, or other numerical feedback.
 */
UCLASS(Abstract, MinimalAPI)
class UBotaniNumberPopComponent : public UControllerComponent
{
	GENERATED_UCLASS_BODY()

public:
	/** Adds a number pop to the number pop list for visualization */
	UFUNCTION(BlueprintCallable, Category = "Botani|Number Pop")
	BOTANIGAME_API virtual void AddNumberPop(const FBotaniNumPopRequest& NewRequest);
};
