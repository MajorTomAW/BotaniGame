// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Input/BotaniInputConfig.h"
#include "BotaniLogChannels.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "BotaniGame"

UBotaniInputConfig::UBotaniInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInputAction* UBotaniInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	return FindInputAction(InputTag, NativeInputActions, bLogNotFound);
}

const UInputAction* UBotaniInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	return FindInputAction(InputTag, AbilityInputActions, bLogNotFound);
}

const UInputAction* UBotaniInputConfig::FindInputAction(const FGameplayTag& InputTag, const TArray<FBotaniInputActionBinding>& InputActions, bool bLogNotFound) const
{
	for (const auto& Action : InputActions)
	{
		if (Action.InputAction == nullptr)
		{
			continue;
		}

		if (!Action.InputTag.MatchesTagExact(InputTag))
		{
			continue;
		}

		return Action.InputAction;
	}

	if (bLogNotFound)
	{
		BOTANI_INPUT_LOG(Warning, TEXT("Can't find InputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

#if WITH_EDITOR
EDataValidationResult UBotaniInputConfig::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	TArray<FBotaniInputActionBinding> CombinedInputActions;
	CombinedInputActions.Append(NativeInputActions);
	CombinedInputActions.Append(AbilityInputActions);

	for (const auto& Action : CombinedInputActions)
	{
		if (Action.InputAction == nullptr)
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
			Context.AddError(NSLOCTEXT("BotaniGame", "InputActionNull", "BotaniInputConfig contains an invalid InputAction."));
		}

		if (!Action.InputTag.IsValid())
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
			Context.AddError(NSLOCTEXT("BotaniGame", "InputTagInvalid", "BotaniInputConfig contains an invalid InputTag."));
		}
	}

	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE