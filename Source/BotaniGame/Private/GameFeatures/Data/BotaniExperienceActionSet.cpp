// Copyright © 2024 Botanibots Team. All rights reserved.


#include "GameFeatures/Data/BotaniExperienceActionSet.h"
#include "GameFeatureAction.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(BotaniExperienceActionSet)

#define LOCTEXT_NAMESPACE "BotaniSystem"

UBotaniExperienceActionSet::UBotaniExperienceActionSet()
{
}

#if WITH_EDITOR
EDataValidationResult UBotaniExperienceActionSet::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (const UGameFeatureAction* Action : FeatureActions)
	{
		if (Action)
		{
			EDataValidationResult ChildResult = Action->IsDataValid(Context);
			Result = CombineDataValidationResults(Result, ChildResult);
		}
		else
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("ActionEntryIsNull", "Null entry at index {0} in Actions"), FText::AsNumber(EntryIndex)));
		}

		++EntryIndex;
	}

	return Result;
}
#endif
#if WITH_EDITORONLY_DATA
void UBotaniExperienceActionSet::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();
	
	for (UGameFeatureAction* Action : FeatureActions)
	{
		if (Action)
		{
			Action->AddAdditionalAssetBundleData(AssetBundleData);
		}
	}
}
#endif

#undef LOCTEXT_NAMESPACE