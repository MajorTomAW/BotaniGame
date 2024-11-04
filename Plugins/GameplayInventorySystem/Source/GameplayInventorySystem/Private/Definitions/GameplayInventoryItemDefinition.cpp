// Copyright © 2024 MajorT. All rights reserved.


#include "Definitions/GameplayInventoryItemDefinition.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include "Definitions/GameplayEquipmentDefinition.h"
#include "Fragments/GameplayInventoryItemFragment.h"
#include "Instance/GameplayInventoryItemInstance.h"
#include "Requirements/GameplayInventoryRequirement.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayInventoryItemDefinition)

UGameplayInventoryItemDefinition::UGameplayInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DefaultInstanceClass(UGameplayInventoryItemInstance::StaticClass())
{
#if WITH_EDITOR
	bHasCustomActorFactory = true;
#endif
}

UGameplayInventoryItemDefinition::~UGameplayInventoryItemDefinition()
{
	//LOG_INVENTORY(Warning, TEXT("Item definition %s is being destroyed."), *GetName());
}

const FGameplayTagContainer& UGameplayInventoryItemDefinition::GetGameplayTags() const
{
	return ItemTags;
}

UGameplayInventoryItemFragment* UGameplayInventoryItemDefinition::GetItemFragment(TSubclassOf<UGameplayInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass == nullptr)
	{
		return nullptr;
	}
	
	for (UGameplayInventoryItemFragment* Fragment : ItemFragments)
	{
		if (Fragment && Fragment->IsA(FragmentClass))
		{
			return Fragment;
		}
	}

	return nullptr;
}

UGameplayEquipmentDefinition* UGameplayInventoryItemDefinition::GetEquipmentDefinition(
	TSubclassOf<class UGameplayEquipmentDefinition> EquipmentClass) const
{
	if (EquipmentDefinition->IsA(EquipmentClass))
	{
		return EquipmentDefinition;
	}

	return nullptr;
}

void UGameplayInventoryItemDefinition::GetStackingData(bool& bCanStack, int32& MaxStackSize) const
{
	bCanStack = StackingData.bCanStack;
	MaxStackSize = StackingData.MaxStackSize;
}

UClass* UGameplayInventoryItemDefinition::GetInstanceClass() const
{
	return ItemInstanceClass ? ItemInstanceClass : DefaultInstanceClass;
}

#if WITH_EDITOR
EDataValidationResult UGameplayInventoryItemDefinition::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	for (const UGameplayInventoryRequirement* Requirement : ItemRequirements)
	{
		if (Requirement == nullptr)
		{
			Context.AddError(NSLOCTEXT("ItemDefinitionError", "ItemRequirementNull", "Item requirement is null."));
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
		}
	}

	for (const UGameplayInventoryItemFragment* Fragment : ItemFragments)
	{
		if (Fragment == nullptr)
		{
			Context.AddError(NSLOCTEXT("ItemDefinitionError", "ItemFragmentNull", "Item fragment is null."));
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
		}
	}

	return Result;
}
#endif