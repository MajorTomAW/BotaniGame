// Copyright © 2024 Botanibots Team. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BotaniLogChannels.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Input/BotaniInputConfig.h"
#include "BotaniInputComponent.generated.h"

/**
 * UBotaniInputComponent
 *
 * Component used to manage input mappings and bindings using an input config data asset.
 */
UCLASS(Config = Input)
class UBotaniInputComponent : public UEnhancedInputComponent
{
	GENERATED_UCLASS_BODY()

public:
	template <class UserClass, typename FuncType>
	void BindNativeAction(const UBotaniInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound = true);

	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UBotaniInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& OutBindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);
};

template <class UserClass, typename FuncType>
void UBotaniInputComponent::BindNativeAction(const UBotaniInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	check(InputTag.IsValid());
	
	const UInputAction* InputAction = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound);

	if (InputAction == nullptr)
	{
		return;
	}

	BindAction(InputAction, TriggerEvent, Object, Func);
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UBotaniInputComponent::BindAbilityActions(const UBotaniInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& OutBindHandles)
{
	check(InputConfig);

	for (const FBotaniInputActionBinding& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction == nullptr)
		{
			BOTANI_INPUT_LOG(Error, TEXT("BindAbilityActions called with invalid InputAction! How could this happen? We set the validation result to invalid if the InputConfig contains invalid InputActions."));
			continue;
		}

		if (!Action.InputTag.IsValid())
		{
			BOTANI_INPUT_LOG(Error, TEXT("BindAbilityActions called with invalid InputTag! How could this happen? We set the validation result to invalid if the InputConfig contains invalid InputTags."));
			continue;
		}

		if (PressedFunc)
		{
			OutBindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
		}

		if (ReleasedFunc)
		{
			OutBindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
		}
	}
}
