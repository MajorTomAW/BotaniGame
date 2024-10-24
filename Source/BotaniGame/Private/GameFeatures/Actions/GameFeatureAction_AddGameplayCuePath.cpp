// Copyright © 2024 Botanibots Team. All rights reserved.


#include "GameFeatures/Actions/GameFeatureAction_AddGameplayCuePath.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddGameplayCuePath)

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

UGameFeatureAction_AddGameplayCuePath::UGameFeatureAction_AddGameplayCuePath()
{
	// Add a default path that is commonly used
	DirectoryPathsToAdd.Add(FDirectoryPath(TEXT("/AbilitySystem/GameplayCues")));
}

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddGameplayCuePath::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	for (const auto& Path : DirectoryPathsToAdd)
	{
		if (Path.Path.IsEmpty())
		{
			const FText Message = FText::Format(NSLOCTEXT("GameFeatures", "InvalidCuePathError", "'{0}' is not a valid path!"), FText::FromString(Path.Path));
			Context.AddError(Message);
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
		}
	}

	return Result;
}
#endif