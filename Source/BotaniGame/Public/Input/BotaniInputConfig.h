// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "BotaniInputConfig.generated.h"

class UInputAction;

/**
 * FBotaniInputActionBinding
 *
 * Struct used to map an input action to a gameplay tag.
 */
USTRUCT(BlueprintType)
struct FBotaniInputActionBinding
{
	GENERATED_BODY()

public:
	/** Input action to bind to the input tag. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<const UInputAction> InputAction = nullptr;

	/** Gameplay tag to bind to the input action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", Meta = (Categories = "Input.InputTag"))
	FGameplayTag InputTag;
};

/**
 * UBotaniInputConfig
 *
 * Non-mutable data asset that contains input configuration data.
 * Used to map certain UInputActions to gameplay tags for use in the ability system.
 */
UCLASS(BlueprintType, Const)
class BOTANIGAME_API UBotaniInputConfig : public UDataAsset
{
	GENERATED_UCLASS_BODY()
	
public:
	/** List of native input actions used by the owner. These input actions are mapped to a gameplay tag and must be manually bound. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", Meta = (TitleProperty = "InputAction"))
	TArray<FBotaniInputActionBinding> NativeInputActions;

	/** List of ability input actions used by the owner. These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", Meta = (TitleProperty = "InputAction"))
	TArray<FBotaniInputActionBinding> AbilityInputActions;

protected:
	friend class UBotaniInputComponent;

	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

protected:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

private:
	const UInputAction* FindInputAction(const FGameplayTag& InputTag, const TArray<FBotaniInputActionBinding>& InputActions, bool bLogNotFound) const;
};
