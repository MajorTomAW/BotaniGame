// Copyright © 2024 Botanibots Team. All rights reserved.


#include "Inputs/AIUtilityInputProvider.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "AIUtilityInputProvider"
#endif

#include "AIUtilities.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AIUtilityInputProvider)

UAIUtilityInputProvider::UAIUtilityInputProvider(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InputProviderTag = AIUtilityTags::InputProvider::TAG_InputProvider;
}

float UAIUtilityInputProvider::Evaluate_Implementation(const UAIUtilityManager* UtilityManager, const FAIUtilityQueryContext& QueryContext) const
{
	return 0.f;
}

#if WITH_EDITOR
EDataValidationResult UAIUtilityInputProvider::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (!InputProviderTag.IsValid())
	{
		Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
		Context.AddError(LOCTEXT("InputProviderTagInvalid", "Input Provider Tag is invalid or missing."));
	}

	return Result;
}
#undef LOCTEXT_NAMESPACE
#endif