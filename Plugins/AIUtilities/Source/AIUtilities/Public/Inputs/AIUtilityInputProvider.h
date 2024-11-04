// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AIUtilityInputProvider.generated.h"

struct FAIUtilityQueryContext;
/**
 * UAIUtilityInputProvider
 *
 * A single input provider used to evaluate a float input value to considerations, which is determined at runtime and identified by a Gameplay Tag.
 */
UCLASS(Blueprintable, Abstract)
class UAIUtilityInputProvider : public UPrimaryDataAsset //@TODO: not sure if this should stay a data asset or an edit inline new object to be able to modify the input provider in the utility directly.
{
	GENERATED_UCLASS_BODY()

public:
	/** Returns the Gameplay Tag used to identify the input provider */
	AIUTILITIES_API virtual FGameplayTag GetInputProviderTag() const { return InputProviderTag; }

	/**
	 * Called to evaluate the input via a given context.
	 * Also used to resolve parameters to queries and other inputs, in which case context is solely the Self reference.
	 */
	UFUNCTION(BlueprintNativeEvent, Blueprintable, Category = "AI Utility")
	AIUTILITIES_API float Evaluate(const class UAIUtilityManager* UtilityManager, const FAIUtilityQueryContext& QueryContext) const;
	
protected:
	/** The Gameplay Tag used to identify the input provider */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "AI.Utiliy.InputProvider"))
	FGameplayTag InputProviderTag;

#if WITH_EDITOR
	//~ Begin UObject Interface
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	//~ End UObject Interface
#endif
};
