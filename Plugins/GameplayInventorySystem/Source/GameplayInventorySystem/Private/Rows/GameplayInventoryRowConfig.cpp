// Copyright © 2024 MajorT. All rights reserved.


#include "Rows/GameplayInventoryRowConfig.h"

#include "Definitions/GameplayInventoryItemDefinition.h"
#include "Spec/GameplayInventoryItemSpec.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "GameplayInventorySystem"
#endif

#include "GameplayInventoryLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayInventoryRowConfig)

UGameplayInventoryRowConfig::UGameplayInventoryRowConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Limits = EGameplayInventoryLimits::NoLimit;
	MaxItemCount = 100;
	MaxWeight = 100;
	MaxSlotCount = 10;
}

bool UGameplayInventoryRowConfig::DoesSpecMatch(const FGameplayInventoryItemSpec& Spec) const
{
	if (!Spec.IsValid())
	{
		return false;
	}

	FGameplayTagContainer SpecTags = Spec.GetItemDefinition()->GetGameplayTags();
	if (TagsFilter.HasAnyExact(SpecTags))
	{
		return true;
	}

	if (ItemClassFilter.Contains(Spec.GetItemDefinition()->GetClass()))
	{
		return true;
	}
	else
	{
		LOG_INVENTORY(Warning, TEXT("Row: %s does not contain %s"), *DisplayName.ToString(), *GetPathNameSafe(Spec.GetItemDefinition()->GetClass()));
	}

	return false;
}

#if WITH_EDITOR
EDataValidationResult UGameplayInventoryRowConfig::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (!RowTag.IsValid())
	{
		Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
		Context.AddError(LOCTEXT("DataValidation", "The row config contains an invalid row tag!"));
	}

	return Result;
}

#undef LOCTEXT_NAMESPACE
#endif